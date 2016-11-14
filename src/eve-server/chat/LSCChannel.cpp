/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2016 The EVEmu Team
    For the latest information visit http://evemu.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:        Zhur
*/

#include "eve-server.h"

#include "Client.h"
#include "chat/kenny.h"
#include "chat/LSCChannel.h"
#include "services/lscProxy/LscProxyService.h"

PyRep *LSCChannelChar::Encode() const {
    ChannelJoinChannelCharsLine line;

    line.allianceID = m_allianceID;
    line.charID = m_charID;
    line.corpID = m_corpID;
    line.role = m_role;
    line.warFactionID = m_warFactionID;
    line.extra = EncodeExtra();

    return line.Encode();
}

PyRep *LSCChannelChar::EncodeExtra() const {
    PyList *extra = new PyList();
    extra->items.push_back(new PyInt(m_charID));
    extra->items.push_back(new PyString(m_charName));
    extra->items.push_back(new PyInt(m_typeID));
    // To-DO: find out what this value is (sometimes it's true)
    extra->items.push_back(new PyBool(false));
    extra->items.push_back(new PyNone());

    return extra;
}

PyRep *LSCChannelMod::Encode() {
    ChannelJoinChannelModsLine line;

    line.accessor = m_accessor;
    line.admin = m_admin;
    line.mode = m_mode;
    line.originalMode = m_originalMode;
    line.reason = m_reason;
    line.untilWhen = m_untilWhen;

    return line.Encode();
}

LSCChannel::LSCChannel(
    LSCProxyService *svc,
    uint32 channelID,
    std::string type,
    uint32 ownerID,
    const char *displayName,
    const char *motd,
    const char *comparisonKey,
    bool memberless,
    const char *password,
    bool mailingList,
    uint32 cspa,
    uint32 temporary,
    uint32 mode
)
: m_service(svc),
  m_ownerID(ownerID),
  m_channelID(channelID),
  m_type(type),
  m_displayName(displayName==NULL?"":displayName),
  m_motd(motd==NULL?"":motd),
  m_comparisonKey(comparisonKey==NULL?"":comparisonKey),
  m_memberless(memberless),
  m_password(password==NULL?"":password),
  m_mailingList(mailingList),
  m_cspa(cspa),
  m_temporary(temporary),
  m_mode(mode)
{
    _log(LSC__CHANNELS, "Creating channel \"%s\"", m_displayName.c_str());
}

LSCChannel::~LSCChannel() {
    _log(LSC__CHANNELS, "Destroying channel \"%s\"", m_displayName.c_str());
}

void LSCChannel::GetChannelInfo(uint32 * channelID, uint32 * ownerID, std::string &displayName, std::string &motd, std::string &comparisonKey,
    bool * memberless, std::string &password, bool * mailingList, uint32 * cspa, uint32 * temporary, uint32 * mode)
{
    *channelID = GetChannelID();
    *ownerID = GetOwnerID();
    displayName = GetDisplayName();
    motd = GetMOTD();
    comparisonKey = GetComparisonKey();
    *memberless = GetMemberless();
    password = GetPassword();
    *mailingList = GetMailingList();
    *cspa = GetCSPA();
    *temporary = GetTemporary();
    *mode = GetMode();
}

void LSCChannel::SetChannelInfo(uint32 ownerID, std::string displayName, std::string motd, std::string comparisonKey,
    bool memberless, std::string password, bool mailingList, uint32 cspa, uint32 temporary, uint32 mode)
{
    SetOwnerID(ownerID);
    SetDisplayName(displayName);
    SetMOTD(motd);
    SetComparisonKey(comparisonKey);
    SetMemberless(memberless);
    SetPassword(password);
    SetMailingList(mailingList);
    SetCSPA(cspa);
    SetTemporary(temporary);
    SetMode(mode);
}

PyRep *LSCChannel::JoinChannel(Client * c)
{
    uint32 charID = c->GetCharacterID();
    if(this->IsJoined(charID))
    {
        return nullptr;
    }
    _log(LSC__CHANNELS, "Channel %s: Join from %s", m_displayName.c_str(), c->GetName());

    LSCChannelChar cChar( this, c->GetCorporationID(), charID, c->GetCharacterName(), c->Item()->typeID(), c->GetAllianceID(), c->GetWarFactionID(), c->GetAccountRole(), 0 );
    m_chars.insert(
        std::make_pair(c->GetCharacterID(), cChar)
    );
    c->ChannelJoined( this );

    // Don't broadcast presence in wormhole systems.
    bool isWormhole = (m_type == "solarsystemid2" && m_channelID >= 61000000);
    // Don't breadcast presence in help channels.
    if (m_channelID > 2 && !isWormhole)
    {
        OnLSC_JoinChannel join;
        join.sender = _MakeSenderInfo(c);
        join.member_count = m_chars.size();
        join.channelID = EncodeID();

        MulticastTarget mct;

        std::map<uint32, LSCChannelChar>::iterator cur, end;
        cur = m_chars.begin();
        end = m_chars.end();
        for( ; cur != end; cur++ )
        {
            mct.characters.insert( cur->first );
        }

        PyTuple *answer = join.Encode();
        EntityList::Multicast( "OnLSC", m_type.c_str(), &answer, mct );
    }

    // Create response entry.
    ChannelJoinReply rep;
    rep.ChannelID = EncodeID();
    rep.ok = 1;
    rep.ChannelInfo = EncodeChannelSmall(charID);
    rep.ChannelMods = EncodeChannelMods();
    rep.ChannelChars = EncodeChannelChars();

    return rep.Encode();
}

void LSCChannel::LeaveChannel(uint32 charID, OnLSC_SenderInfo * si) {
    _log(LSC__CHANNELS, "Channel %s: Leave from %u", m_displayName.c_str(), charID);

    if (m_chars.find(charID) == m_chars.end())
    {
        return;
    }

    m_chars.erase(charID);

    OnLSC_LeaveChannel leave;
    leave.sender = si;
    leave.member_count = m_chars.size();
    leave.channelID = EncodeID();

    MulticastTarget mct;

    std::map<uint32, LSCChannelChar>::iterator cur, end;
    cur = m_chars.begin();
    end = m_chars.end();
    for(; cur != end; cur++)
    {
        mct.characters.insert( cur->first );
    }

    PyTuple *answer = leave.Encode();
    EntityList::Multicast("OnLSC", m_type.c_str(), &answer, mct);
}

void LSCChannel::LeaveChannel(Client *c, bool self) {
    _log(LSC__CHANNELS, "Channel %s: Leave from %s", m_displayName.c_str(), c->GetName());

    uint32 charID = c->GetCharacterID();

    if (m_chars.find(charID) == m_chars.end())
        return;

    OnLSC_LeaveChannel leave;
    leave.sender = _MakeSenderInfo(c);
    leave.member_count = m_chars.size();
    leave.channelID = EncodeID();

    MulticastTarget mct;

    std::map<uint32, LSCChannelChar>::iterator cur, end;
    cur = m_chars.begin();
    end = m_chars.end();
    for(; cur != end; cur++)
        mct.characters.insert( cur->first );

    PyTuple *answer = leave.Encode();
    EntityList::Multicast("OnLSC", m_type.c_str(), &answer, mct);

    m_chars.erase(charID);
    c->ChannelLeft(this);
}

void LSCChannel::Evacuate(Client * c) {
    OnLSC_DestroyChannel dc;

    dc.channelID = EncodeID();
    dc.member_count = 0;
    dc.sender = _MakeSenderInfo(c);

    MulticastTarget mct;

    std::map<uint32, LSCChannelChar>::iterator cur, end;
    cur = m_chars.begin();
    end = m_chars.end();
    for(; cur != end; cur++)
        mct.characters.insert(cur->first);

    PyTuple *answer = dc.Encode();
    EntityList::Multicast("OnLSC", m_type.c_str(), &answer, mct);
}

void LSCChannel::SendMessage(Client * c, const char * message, bool self) {
/*
    MulticastTarget mct;

    OnLSC_SendMessage sm;

    if (message[0] == '#') {
        m_service->ExecuteCommand(c, message);
        mct.characters.insert(c->GetCharacterID());
        sm.sender = _MakeSenderInfo(c);
    } else {
        if (self) {
            mct.characters.insert(c->GetCharacterID());
            sm.sender = _FakeSenderInfo();
        } else {
            std::map<uint32, LSCChannelChar>::iterator cur, end;
            cur = m_chars.begin();
            end = m_chars.end();
            for(; cur != end; cur++)
                mct.characters.insert( cur->first );

            sm.sender = _MakeSenderInfo(c);
    }
    }

    sm.channelID = EncodeID();
    sm.message = message;
    sm.member_count = m_chars.size();

    PyTuple *answer = sm.Encode();
    EntityList::Multicast("OnLSC", m_type, &answer, mct);
*/

    // NEW KENNY TRANSLATOR VERSION:
    // execute Multicast() twice: once for all clients where IsKennyTranslatorEnabled() == false and once for all clients where it is true
    MulticastTarget mct_Kennyfied;
    OnLSC_SendMessage sm_Kennyfied;
    uint32 kennyfiedCharListSize = 0;
    MulticastTarget mct_NotKennyfied;
    OnLSC_SendMessage sm_NotKennyfied;
    uint32 notKennyfiedCharListSize = 0;

    if (message[0] == '#') {
        m_service->ExecuteCommand(c, message);
        mct_Kennyfied.characters.insert(c->GetCharacterID());
        sm_Kennyfied.sender = _MakeSenderInfo(c);
    } else {
        if (self) {
            if( c->IsKennyTranslatorEnabled() )
            {
                mct_Kennyfied.characters.insert(c->GetCharacterID());
                sm_Kennyfied.sender = _FakeSenderInfo();
                kennyfiedCharListSize++;
            }
            else
            {
                mct_NotKennyfied.characters.insert(c->GetCharacterID());
                sm_NotKennyfied.sender = _FakeSenderInfo();
                notKennyfiedCharListSize++;
            }
        } else {
            std::map<uint32, LSCChannelChar>::iterator cur, end;
            cur = m_chars.begin();
            end = m_chars.end();
            for(; cur != end; cur++)
            {
                if( (EntityList::FindCharacter(cur->first)->IsKennyTranslatorEnabled()) )
                {
                    mct_Kennyfied.characters.insert( cur->first );
                    kennyfiedCharListSize++;
                }
                else
                {
                    mct_NotKennyfied.characters.insert( cur->first );
                    notKennyfiedCharListSize++;
                }
            }

            //if( c->IsKennyTranslatorEnabled() )
            //{
                sm_Kennyfied.sender = _MakeSenderInfo(c);
                kennyfiedCharListSize++;
            //}
            //else
            //{
                sm_NotKennyfied.sender = _MakeSenderInfo(c);
                notKennyfiedCharListSize++;
            //}
    }
    }

    PyTuple *answerNotKennyfied;
    sm_NotKennyfied.channelID = EncodeID();
    sm_NotKennyfied.message = message;
    sm_NotKennyfied.member_count = notKennyfiedCharListSize;

    // Check if number of non-kennyfied recipients is greater than zero, and if so, send a Multicast() to them
    // with the normal message sent through kenny translator IF the sender has the Kenny Translator enabled
    if( notKennyfiedCharListSize > 0 )
    {
        std::string kennyfied_message;
        if( c->IsKennyTranslatorEnabled() )
        {
            normal_to_kennyspeak(sm_NotKennyfied.message, kennyfied_message);
            sm_NotKennyfied.message = kennyfied_message;
        }
        answerNotKennyfied = sm_NotKennyfied.Encode();
        EntityList::Multicast("OnLSC", m_type.c_str(), &answerNotKennyfied, mct_NotKennyfied);
    }

    sm_Kennyfied.channelID = EncodeID();
    sm_Kennyfied.message = message;
    sm_Kennyfied.member_count = kennyfiedCharListSize;

    PyTuple *answerKennyfied = sm_Kennyfied.Encode();
    EntityList::Multicast("OnLSC", m_type.c_str(), &answerKennyfied, mct_Kennyfied);
}

bool LSCChannel::IsJoined(uint32 charID) {
    return m_chars.find(charID) != m_chars.end();
}

OnLSC_SenderInfo *LSCChannel::_MakeSenderInfo(Client *c) {
    OnLSC_SenderInfo *sender = new OnLSC_SenderInfo;

    sender->senderID = c->GetCharacterID();
    sender->senderName = c->GetName();
    sender->senderType = c->GetChar()->typeID();
    sender->corpID = c->GetCorporationID();
    sender->role = c->GetAccountRole();
    sender->corp_role = c->GetCorpRole();
    sender->gender = c->GetChar()->gender();

    return(sender);
}

OnLSC_SenderInfo *LSCChannel::_FakeSenderInfo() {
    OnLSC_SenderInfo *sender = new OnLSC_SenderInfo();

    sender->senderID = 1;
    sender->senderName = "EVE System";
    sender->senderType = 1;
    sender->corpID = 1;
    sender->role = 1;
    sender->corp_role = 1;
    sender->gender = false;

    return sender;
}

PyRep *LSCChannel::EncodeChannel(uint32 charID) {
    ChannelInfoLine line;

    line.channelID = m_channelID;
    line.comparisonKey = m_comparisonKey;
    line.cspa = m_cspa;
    line.displayName = m_displayName;
    line.estimatedMemberCount = m_chars.size();
    line.mailingList = m_mailingList;
    line.memberless = m_memberless;
    line.mode = m_mode;
    line.motd = m_motd;
    line.ownerID = m_ownerID;
    line.password = m_password;
    line.subscribed = !(m_chars.find(charID) == m_chars.end());
    line.temporary = m_temporary;

    return line.Encode();
}

PyRep *LSCChannel::EncodeID() {
    if (m_type == "")
    {
        return (new PyInt(m_channelID));
    }

    LSCChannelMultiDesc desc;
    desc.id = m_channelID;
    desc.type = m_type;

    return desc.Encode();
}

PyRep *LSCChannel::EncodeChannelSmall(uint32 charID) {
    if(m_type == "corpid")
    {
        PyDict *dict = new PyDict();
        dict->SetItem(new PyString("mailingList"), new PyBool(false));
        if(m_motd.empty())
        {
            dict->SetItem(new PyString("motd"), new PyNone());
        }
        else
        {
            dict->SetItem(new PyString("motd"), new PyWString(m_motd));
        }
        PyObject *obj = new PyObject("utillib.KeyVal", dict);
        return obj;
    }
    if(m_type == "solarsystemid2")
    {
        return new PyNone();
    }
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("channelID", DBTYPE_I4);
    header->AddColumn("ownerID", DBTYPE_I4);
    header->AddColumn("displayName", DBTYPE_WSTR);
    header->AddColumn("motd", DBTYPE_WSTR);
    header->AddColumn("comparisonKey", DBTYPE_WSTR);
    header->AddColumn("memberless", DBTYPE_BOOL);
    header->AddColumn("password", DBTYPE_WSTR);
    header->AddColumn("mailingList", DBTYPE_BOOL);
    header->AddColumn("cspa", DBTYPE_I4);
    header->AddColumn("temporary", DBTYPE_BOOL);
    header->AddColumn("languageRestriction", DBTYPE_BOOL);
    header->AddColumn("groupMessageID", DBTYPE_I4);
    header->AddColumn("channelMessageID", DBTYPE_I4);
    header->AddColumn("subscribed", DBTYPE_I4);

    PyPackedRow *row = new PyPackedRow(header);
    row->SetField("channelID", new PyInt(m_channelID));
    row->SetField("ownerID", new PyInt(m_ownerID));
    row->SetField("displayName", new PyWString(m_displayName));
    row->SetField("motd", new PyWString(m_motd));
    row->SetField("comparisonKey", new PyWString(m_comparisonKey));
    row->SetField("memberless", new PyBool(m_memberless));
    row->SetField("password", new PyWString(m_password));
    row->SetField("mailingList", new PyBool(m_mailingList));
    row->SetField("cspa", new PyInt(m_cspa));
    row->SetField("temporary", new PyBool((m_temporary == 0) ? false : true));
    row->SetField("languageRestriction", new PyBool(0));
    row->SetField("groupMessageID", new PyInt(0));
    row->SetField("channelMessageID", new PyInt(0));
    row->SetField("subscribed", new PyInt(!(m_chars.find(charID) == m_chars.end())));

    return row;
}

PyRep *LSCChannel::EncodeChannelMods()
{
    if(m_mods.size() == 0)
    {
        return new PyNone();
    }
    // TO-DO: This should use a CRowset but it uses a Rowset.
    ChannelJoinChannelMods info;
    info.lines = new PyList();

    for( uint32 i = 0; i < m_mods.size(); i++ )
    {
        info.lines->AddItem( m_mods[i].Encode() );
    }

    return info.Encode();
}

PyRep *LSCChannel::EncodeChannelChars() {
    ChannelJoinChannelChars info;
    info.lines = new PyList;

    bool populate = true;
    if(m_type == "corpid")
    {
        if(m_channelID < EVEMU_MINIMUM_ID)
        {
            // NPC corporation.
            // DO NOT populate member list.
            populate = false;
        }
    }
    if(m_channelID == 1 || m_channelID == 2)
    {
        // Help channel.
        // DO NOT populate help member lists.
        populate = false;
    }
    if(m_type == "")
    {
        // Normal channel.
        // DO NOT populate list.
        populate = false;
    }
    if(populate)
    {
        std::map<uint32, LSCChannelChar>::iterator cur, end;
        cur = m_chars.begin();
        end = m_chars.end();
        for(; cur != end; cur++)
        {
            std::map<uint32, LSCChannelChar>::const_iterator res = m_chars.find( cur->first );
            if( res != m_chars.end() )
            {
                info.lines->AddItem( res->second.Encode() );
            }
        }
    }

    return info.Encode();
}

PyRep *LSCChannel::EncodeEmptyChannelChars() {
    ChannelJoinChannelChars info;
    info.lines = new PyList;
    return info.Encode();
}
