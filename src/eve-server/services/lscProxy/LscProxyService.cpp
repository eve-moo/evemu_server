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
    Author:        Cometo
*/


#include "eve-server.h"
#include "LscProxyService.h"
#include "chat/LSCDB.h"
#include "PyBoundObject.h"
#include "PyServiceCD.h"
#include "PyServiceMgr.h"
#include "admin/SlashService.h"

// Set the base (minimum) and maximum numbers for any user-created chat channel.
const uint32 LSCProxyService::BASE_CHANNEL_ID = 200000000;
const uint32 LSCProxyService::MAX_CHANNEL_ID = 0xFFFFFFFF;

PyCallable_Make_InnerDispatcher(LSCProxyService)

LSCProxyService::LSCProxyService(CommandDispatcher* cd)
        : PyService("lscProxy", new Dispatcher(this)),
 m_commandDispatch(cd)
{
    PyCallable_REG_CALL(LSCProxyService, JoinChannels)
    PyCallable_REG_CALL(LSCProxyService, LeaveChannels)
    PyCallable_REG_CALL(LSCProxyService, SendMessage)
}

LSCProxyService::~LSCProxyService()
{
}

PyResult LSCProxyService::ExecuteCommand(Client *from, const char *msg) {
    return(m_commandDispatch->Execute(from, msg));
}

void LSCProxyService::CreateSystemChannel(uint32 systemID) {
    if (getChannel(systemID).get() == nullptr)
    {
        CreateChannel(systemID, "System Channels\\Local", LSCDB::GetSolarSystemName(systemID).c_str(), std::string("solarsystemid2"));
    }
}

void LSCProxyService::CharacterLogout(uint32 charID, OnLSC_SenderInfo* si)
{
    auto lock = std::unique_lock<std::mutex>(sMutex);
    // Get a copy of the channel list in case it's modified.
    std::map<uint32, std::shared_ptr<LSCChannel>> channels(m_channels);
    lock.release();

    for(auto cur : channels)
    {
        if( cur.second->IsJoined( charID ) )
        {
            cur.second->LeaveChannel( charID, new OnLSC_SenderInfo( *si ) );
        }
    }

    SafeDelete( si );
}

//stuck here to be close to related functionality
void LSCProxyService::SendMail(uint32 sender, const std::vector<int32> &recipients, const std::string &subject, const std::string &content) {
    NotifyOnMessage notify;
    std::set<uint32> successful_recipients;

    notify.subject = subject;
    notify.sentTime = Win32TimeNow();
    notify.senderID = sender;

    // there's attachmentID and messageID... does this means a single message can contain multiple attachments?
    // eg. text/plain and text/html? we should be watching for this at reading mails...
    // created should be creation time. But Win32TimeNow returns uint64, and is stored as bigint(20),
    // so change in the db is needed
    std::vector<int32>::const_iterator cur, end;
    cur = recipients.begin();
    end = recipients.end();

    for(; cur != end; cur++)
    {
        uint32 messageID = LSCDB::StoreMail(sender, *cur, subject.c_str(), content.c_str(), notify.sentTime);
        if(messageID == 0) {
            _log(SERVICE__ERROR, "Failed to store message from %u for recipient %u", sender, *cur);
            continue;
        }
        //TODO: supposed to have a different messageID in each notify I suspect..
        notify.messageID = messageID;

        _log(SERVICE__MESSAGE, "Delivered message from %u to recipient %u", sender, *cur);
        //record this person in the 'delivered to' list:
        notify.recipients.push_back(*cur);
        successful_recipients.insert(*cur);
    }

    //now, send a notification to each successful recipient
    PyTuple *answer = notify.Encode();
    EntityList::Multicast(successful_recipients, "OnMessage", "*multicastID", &answer, false);
}

//stuck here to be close to related functionality
//theres a lot of duplicated crap in here...
//this could be replaced by the SendNewEveMail if it weren't in the Client
void Client::SelfEveMail( const char* subject, const char* fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    char* str = NULL;
    if(vasprintf( &str, fmt, args ) == -1)
    {
        assert(false);
    }
    assert( str );

    va_end( args );

    PyServiceMgr::lscProxy_service->SendMail(GetCharacterID(), GetCharacterID(), subject, str);
    SafeFree( str );
}

const int cmode = 2950;


std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(uint32 channelID, const char * name, const char * motd, std::string type, const char * compkey,
    uint32 ownerID, bool memberless, const char * password, bool maillist, uint32 cspa, uint32 temporary, uint32 mode)
{
    auto lock = std::unique_lock<std::mutex>(sMutex);
    return m_channels[channelID] = std::make_shared<LSCChannel>(this, channelID, type, ownerID, name, motd, compkey, memberless, password, maillist, cspa, temporary, mode);
}


std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(uint32 channelID, const char * name, const char * motd, std::string type, bool maillist)
{
    auto lock = std::unique_lock<std::mutex>(sMutex);
    //ownerID must be sent in responce to GetEveOwners
    return m_channels[channelID] = std::make_shared<LSCChannel>(this, channelID, type, 1, name, motd, (const char *)NULL, false, "", maillist, true, false, cmode);
}

std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(uint32 channelID, const char * name, std::string type, bool maillist)
{
    auto lock = std::unique_lock<std::mutex>(sMutex);
    return m_channels[channelID] = std::make_shared<LSCChannel>(this, channelID, type, 1, name, (const char *)NULL, (const char *)NULL, false, "", maillist, true, false, cmode);
}

std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(uint32 channelID)
{
    std::string type;
    std::string name;
    std::string motd;
    if (IsRegion(channelID))
    {
        type = "regionid";
        name = "System Channels\\Region";
        motd = LSCDB::GetRegionName(channelID);
    }
    else if (IsConstellation(channelID))
    {
        type = "constellationid";
        name = "System Channels\\Constellation";
        motd = LSCDB::GetConstellationName(channelID);
    }
    else if (IsSolarSystem(channelID))
    {
        type = "solarsystemid2";
        name = "System Channels\\Local";
        motd = LSCDB::GetSolarSystemName(channelID);
    }
    // official corporations
    else if ((channelID >= 1000000) && (channelID < 2000000))
    {
        type = "corpid";
        name = "System Channels\\Corp";
        motd = LSCDB::GetCorporationName(channelID);
    }
    // player-created corporations
    else if ((channelID >= 2000000) && (channelID < 3000000))
    {
        type = "corpid";
        name = "System Channels\\Corp";
        motd = LSCDB::GetCorporationName(channelID);
    }
    // Only the Help\Rookie and Help\Help channels have channelIDs < 1000000:
    else
    {
        type = "";
        LSCDB::GetChannelInfo(channelID, name, motd);
    }

    auto lock = std::unique_lock<std::mutex>(sMutex);
    return m_channels[channelID] = std::make_shared<LSCChannel>(this, channelID, type, 1, name.c_str(), motd.c_str(), (const char *)NULL, false, (const char *)NULL, false, true, false, cmode);
}


std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(uint32 channelID, std::string type) {
    std::string name;
    std::string motd;
    if (IsRegion(channelID))
    {
        name = "System Channels\\Region";
        motd = LSCDB::GetRegionName(channelID);
    }
    else if (IsConstellation(channelID))
    {
        name = "System Channels\\Constellation";
        motd = LSCDB::GetConstellationName(channelID);
    }
    else if (IsSolarSystem(channelID))
    {
        name = "System Channels\\Local";
        motd = LSCDB::GetSolarSystemName(channelID);
    }
    // official corporations
    else if ((channelID >= 1000000) && (channelID < 2000000))
    {
        name = "System Channels\\Corp";
        motd = LSCDB::GetCorporationName(channelID);
    }
    // player-created corporations
    else if ((channelID >= 2000000) && (channelID < 3000000))
    {
        name = "System Channels\\Corp";
        motd = LSCDB::GetCorporationName(channelID);
    }
    // Only the Help\Rookie and Help\Help channels have channelIDs < 1000000:
    else
    {
        LSCDB::GetChannelInfo(channelID, name, motd);
    } //GetCharacterName(channelID); motd = ""; }

    auto lock = std::unique_lock<std::mutex>(sMutex);
    return m_channels[channelID] = std::make_shared<LSCChannel>(this, channelID, type, 1, name.c_str(), motd.c_str(), (const char *)NULL, false, (const char *)NULL, false, true, false, cmode);
}


std::shared_ptr<LSCChannel> LSCProxyService::CreateChannel(const char * name, bool maillist)
{
    uint32 nextFreeChannelID = LSCDB::GetNextAvailableChannelID();

    if( nextFreeChannelID )
        return CreateChannel(nextFreeChannelID, name, "", maillist);
    else
        return NULL;
}

void LSCProxyService::InitiateStaticChannels() {
    // maybe, but only maybe, this stuff could be replaced by a nice database call
    //CreateChannel(1, "Help\\Rookie Help", "Rookie motd", LSCChannel::normal);
    //CreateChannel(2, "Help\\Help", "Help motd", LSCChannel::normal);
}

std::shared_ptr<LSCChannel> LSCProxyService::getChannel(uint32 channelID)
{
    std::shared_ptr<LSCChannel> channel;
    auto lock = std::unique_lock<std::mutex>(sMutex);
    std::map<uint32, std::shared_ptr<LSCChannel>>::iterator itr = m_channels.find(channelID);
    if(itr != m_channels.end())
    {
        channel = itr->second;
    }
    return channel;
}

void LSCProxyService::removeChannel(uint32 channelID)
{
    auto lock = std::unique_lock<std::mutex>(sMutex);
    auto itr = m_channels.find(channelID);
    if(itr != m_channels.end())
    {
        m_channels.erase(itr);
    }
}

PyResult LSCProxyService::Handle_JoinChannels(PyCallArgs &call)
{
    CallJoinChannels args;
    if (!args.Decode(&call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: LSCProxyService::JoinChannels invalid args.", call.client->GetName());
        return nullptr;
    }
    uint32 charID = call.client->GetCharacterID();
    PyList *ret = new PyList();
    for(PyRep *item : args.channels->items)
    {
        std::shared_ptr<LSCChannel> channel;
        if(pyIs(Tuple, item))
        {
            LSCChannelMultiDesc chanDesc;
            if(!chanDesc.Decode(item))
            {
                codelog(CLIENT__ERROR, "%s: LSCProxyService::JoinChannels invalid channel descriptor.", call.client->GetName());
                continue;
            }
            channel = getChannel(chanDesc.id);
            if(channel.get() == nullptr)
            {
                channel = CreateChannel(chanDesc.id, chanDesc.type);
            }
        }
        else if(pyIs(Int, item))
        {
            PyInt *pyChannel = pyAs(Int, item);
            int32 channelID = pyChannel->value();
            channel = getChannel(channelID);
            if(channel.get() == nullptr)
            {
                channel = CreateChannel(channelID);
            }
        }
        else
        {
            codelog(CLIENT__ERROR, "%s: LSCProxyService::JoinChannels invalid channel type.", call.client->GetName());
        }
        if(channel.get() == nullptr)
        {
            continue;
        }
        PyRep *join = channel->JoinChannel(call.client);
        if(join != nullptr)
        {
            ret->items.push_back(join);
        }
        // Save this subscription to this channel to the database
        if (!(LSCDB::IsChannelSubscribedByThisChar(charID, channel->GetChannelID())))
        {
            // the "extra" field is hard-coded to '0' for now since I don't know what it's used for
            LSCDB::WriteNewChannelSubscriptionToDatabase(charID, channel->GetChannelID(),
                    call.client->GetCorporationID(), call.client->GetAllianceID(), 2, 0 );
        }
    }
    return ret;
}

PyResult LSCProxyService::Handle_LeaveChannels(PyCallArgs &call)
{
    CallLeaveChannels args;

    if(!args.Decode(&call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: LSCProxyService::LeaveChannels invalid args.", call.client->GetName());
        return new PyNone();
    }

    std::set<uint32> toLeave;
    for(auto item : args.channels->items)
    {
        std::shared_ptr<LSCChannel> channel;
        if(pyIs(Tuple, item))
        {
            LSCChannelMultiDesc chanDesc;
            if(!chanDesc.Decode(item))
            {
                codelog(CLIENT__ERROR, "%s: LSCProxyService::JoinChannels invalid channel descriptor.", call.client->GetName());
                continue;
            }
            channel = getChannel(chanDesc.id);
        }
        else if(pyIs(Int, item))
        {
            PyInt *pyChannel = pyAs(Int, item);
            channel = getChannel(pyChannel->value());
        }
        else
        {
            codelog(CLIENT__ERROR, "%s: LSCProxyService::JoinChannels invalid channel type.", call.client->GetName());
        }
        if(channel.get() == nullptr)
        {
            continue;
        }
        uint32 channelID = channel->GetChannelID();
        // Remove channel subscription from database if this character was subscribed to it.
        // NOTE: channel subscriptions are NOT saved to the database for private convo chats
        if (LSCDB::IsChannelSubscribedByThisChar(call.client->GetCharacterID(), channelID))
        {
            LSCDB::RemoveChannelSubscriptionFromDatabase(channelID, call.client->GetCharacterID());
        }

        // Remove channel from database if this character was the last one
        // in the channel to leave and it was a private convo (temporary==1):
        if( (channel->GetMemberCount() == 1)
            && (channel->GetTemporary() != 0)
            && (channelID >= LSCProxyService::BASE_CHANNEL_ID))
        {
            LSCDB::RemoveChannelFromDatabase(channelID);
        }

        channel->LeaveChannel(call.client);
    }

    return new PyNone();
}

PyResult LSCProxyService::Handle_SendMessage(PyCallArgs &call)
{
    Call_SendMessage args;
    if(!args.Decode(&call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: LSCProxyService::SendMessage invalid args.", call.client->GetName());
        return nullptr;
    }
    std::shared_ptr<LSCChannel> channel;
    if(pyIs(Tuple, args.channel))
    {
        LSCChannelMultiDesc chanDesc;
        if(!chanDesc.Decode(args.channel))
        {
            codelog(CLIENT__ERROR, "%s: LSCProxyService::SendMessage invalid channel descriptor.", call.client->GetName());
            return nullptr;
        }
        channel = getChannel(chanDesc.id);
    }
    else if(pyIs(Int, args.channel))
    {
        PyInt *pyChannel = pyAs(Int, args.channel);
        channel = getChannel(pyChannel->value());
    }
    else
    {
        codelog(CLIENT__ERROR, "%s: LSCProxyService::SendMessage invalid channel type.", call.client->GetName());
    }

    std::string message = args.message;
    if(message.at(0) == '.')
    {
        SysLog::Debug( "LSCProxyService::Handle_SendMessage()", "CALL to SlashService->SlashCmd() via LSC Proxy Service, baby!" );

        if (PyServiceMgr::LookupService("slash") != NULL)
        {
            static_cast<SlashService *> (PyServiceMgr::LookupService("slash"))->SlashCommand(call.client, message);
        }
        message = " ";      // Still transmit some message but minimal so that chat window is not "locked" by client for not getting a chat
        return nullptr; // will this be bad?
    }
    channel->SendMessage(call.client, message.c_str());

    return new PyInt(1);
}
