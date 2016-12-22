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
    Author:     Zhur, Aknor Jaden
*/


#include "eve-server.h"

#include "PyServiceCD.h"
#include "admin/CommandDispatcher.h"
#include "admin/SlashService.h"
#include "chat/LSCService.h"
#include "PyServiceMgr.h"
#include "services/lscProxy/LscProxyService.h"

PyCallable_Make_InnerDispatcher(LSCService)

LSCService::LSCService(CommandDispatcher* cd)
: PyService("LSC", new Dispatcher(this)),
  m_commandDispatch(cd)
{
    //make sure you edit the header file too
    PyCallable_REG_CALL(LSCService, GetChannels)
    PyCallable_REG_CALL(LSCService, GetRookieHelpChannel)
    PyCallable_REG_CALL(LSCService, CreateChannel)
    PyCallable_REG_CALL(LSCService, Configure)
    PyCallable_REG_CALL(LSCService, DestroyChannel)
    PyCallable_REG_CALL(LSCService, GetMembers)
    PyCallable_REG_CALL(LSCService, GetMember)
    PyCallable_REG_CALL(LSCService, Invite)
    PyCallable_REG_CALL(LSCService, AccessControl)

    PyCallable_REG_CALL(LSCService, GetMyMessages)
    PyCallable_REG_CALL(LSCService, GetMessageDetails)
    PyCallable_REG_CALL(LSCService, Page)
    PyCallable_REG_CALL(LSCService, MarkMessagesRead)
    PyCallable_REG_CALL(LSCService, DeleteMessages)

}


LSCService::~LSCService()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Eve Chat calls
//
///////////////////////////////////////////////////////////////////////////////

const int cmode = 2950;

PyResult LSCService::Handle_GetChannels(PyCallArgs &call)
{
    /*
        Assume this is only called when the char's logging in.
        Next step from the client is to join to all channels that's been sent by this
        So only send back the relevant channelIDs
        These are:
            - Help/Rookie, which is 1
            - character
            - corporation
            - solar system
            - region
            - constellation

            and later on:
            - allianceID
            - gangID

            also there are things like:
            - empireID
    */
    std::vector<std::string> names;

    // Call to LSCDB object to get the following strings in this order (index is to left):
    //    0    Character name
    //    1    Corporation name
    //    2    Solar System name
    //    3    Constellation name
    //    4    Region name
    LSCDB::GetChannelNames(call.client->GetCharacterID(), names);

    uint32 channelID = call.client->GetCharacterID();

    LSCProxyService *proxy = PyServiceMgr::lscProxy_service;
    // Use characterID to join this character's mailing list (channelID == characterID):
    if( proxy->getChannel(channelID).get() == nullptr )
    {
        proxy->CreateChannel(channelID, call.client->GetName(), "", true);
    }

    // Use corporationID to join this character's CORP chat channel:
    channelID = call.client->GetCorporationID();
    if( proxy->getChannel(channelID).get() == nullptr )
    {
        proxy->CreateChannel(channelID, "System Channels\\Corp", names[1].c_str(), std::string("corpid"));
    }

    // Use solarSystemID to join the Local chat of this character's present solar system:
    channelID = call.client->GetSystemID();
    if( proxy->getChannel(channelID).get() == nullptr )
    {
        proxy->CreateChannel(channelID, "System Channels\\Local", names[2].c_str(), std::string("solarsystemid2"));
    }

    // Use constellationID to join the Constellation chat of this character's present constellation:
    channelID = call.client->GetConstellationID();
    if( proxy->getChannel(channelID).get() == nullptr )
    {
        proxy->CreateChannel(channelID, "System Channels\\Constellation", names[3].c_str(), std::string("constellationid"));
    }

    // Use regionID to join the Region chat of this character's present region:
    channelID = call.client->GetRegionID();
    if( proxy->getChannel(channelID).get() == nullptr )
    {
        proxy->CreateChannel(channelID, "System Channels\\Region", names[4].c_str(), std::string("regionid"));
    }

    std::vector<unsigned long> charChannelIDs;
    std::vector<std::string> charChannelNames;
    std::vector<std::string> charChannelMOTDs;
    std::vector<unsigned long> charOwnerIDs;
    std::vector<std::string> charComparisonKeys;
    std::vector<int> charMemberless;
    std::vector<std::string> charPasswords;
    std::vector<int> charMailingLists;
    std::vector<int> charCSPAs;
    std::vector<int> charTemporaries;
    std::vector<int> charModes;
    int channelCount = 0;

    // Get this character's subscribed Private Channel names and IDs:
    LSCDB::GetChannelSubscriptions(call.client->GetCharacterID(), charChannelIDs, charChannelNames, charChannelMOTDs,
        charOwnerIDs, charComparisonKeys, charMemberless, charPasswords, charMailingLists, charCSPAs, charTemporaries,
        charModes, channelCount);

    std::shared_ptr<LSCChannel> channel;
    if( channelCount > 0 )
    {
        // Check each private chat channel listed in the names/IDs just procurred to
        // see if they exist yet and if not, create them:
        for( int i=0; i<channelCount; i++ )
        {
            if(proxy->getChannel(charChannelIDs[i]).get() == nullptr)
            {
                // Create each private chat channel listed in the names/IDs just procurred
                // and grab a pointer to them while we're at it:
                //channel = CreateChannel(charChannelIDs[i], charChannelNames[i].c_str(),
                //    charChannelMOTDs[i].c_str(), LSCChannel::normal);
                channel = proxy->CreateChannel(
                    charChannelIDs[i],
                    charChannelNames[i].c_str(),
                    charChannelMOTDs[i].c_str(),
                    std::string(""),
                    charComparisonKeys[i].c_str(),
                    charOwnerIDs[i],
                    (charMemberless[i] ? true : false),
                    charPasswords[i].c_str(),
                    (charMailingLists[i] ? true : false),
                    charCSPAs[i],
                    charTemporaries[i],
                    charModes[i]
                    );
            }
        }
    }

    ChannelInfo info;
    info.lines = new PyList;

    for(auto cur : proxy->m_channels)
    {
        info.lines->AddItem( cur.second->EncodeChannel( call.client->GetCharacterID() ) );
    }

    return info.Encode();
}


PyResult LSCService::Handle_GetRookieHelpChannel(PyCallArgs &call) {
    return(new PyInt(1));
}

PyResult LSCService::Handle_CreateChannel( PyCallArgs& call )
{
    // WARNING: This call contains manual packet decoding to handle configuring parameters for
    // user-created chat channels since I didn't want to monkey around with the LSCPkts.xmlp.
    // -- Aknor Jaden (2010-11-26)

    Call_SingleWStringSoftArg name;
    std::shared_ptr<LSCChannel> channel;

    if( !name.Decode( call.tuple ) )
    {
        SysLog::Error( "LSCService", "%s: Invalid arguments", call.client->GetName() );
        return NULL;
    }

    bool create_channel_exists = false;
    bool create_channel = false;
    bool temporary_exists = false;
    bool temporary_channel = false;
    bool joinExisting_exists = false;
    bool joinExisting_channel = false;

    if (call.byname.find("create") != call.byname.end())
    {
        create_channel_exists = true;
        if (pyAs(Bool, call.byname.find("create")->second)->value())
        {
            create_channel = true;
        }
    }

    if (call.byname.find("temporary") != call.byname.end())
    {
        temporary_exists = true;
        if (pyAs(Bool, call.byname.find("temporary")->second)->value())
        {
            temporary_channel = true;
        }
    }

    if (call.byname.find("joinExisting") != call.byname.end())
    {
        joinExisting_exists = true;
        if (pyAs(Bool, call.byname.find("joinExisting")->second)->value())
        {
            joinExisting_channel = true;
        }
    }

    LSCProxyService *proxy = PyServiceMgr::lscProxy_service;

    if (create_channel_exists && create_channel)
    {
        // Query Database to see if a channel with this name does not exist and, if so, create the channel,
        // otherwise, set the channel pointer to NULL
        if (LSCDB::IsChannelNameAvailable(name.arg))
        {
            channel = proxy->CreateChannel( name.arg.c_str() );
        }
        else
        {
            SysLog::Error( "LSCService", "%s: Error creating new chat channel: channel name '%s' already exists.", call.client->GetName(), name.arg.c_str() );
            channel = NULL;
        }

        if (channel == NULL)
        {
            SysLog::Error( "LSCService", "%s: Error creating new chat channel", call.client->GetName() );
            return NULL;
        }

        // Save channel info and channel subscription to the database
        LSCDB::WriteNewChannelToDatabase(channel->GetChannelID(), channel->GetDisplayName(), call.client->GetCharacterID(), 0, cmode);
        LSCDB::WriteNewChannelSubscriptionToDatabase(call.client->GetCharacterID(), channel->GetChannelID(),
            call.client->GetCorporationID(), call.client->GetAllianceID(),
            2, 0 );     // the "extra" field is hard-coded
                                                    // to '0' for now since I don't
                                                    // know what it's used for

        channel->JoinChannel( call.client );

        ChannelCreateReply reply;
        reply.ChannelChars = channel->EncodeChannelChars();
        reply.ChannelInfo = channel->EncodeChannelSmall( call.client->GetCharacterID() );
        reply.ChannelMods = channel->EncodeChannelMods();
        return reply.Encode();
    }


    if (joinExisting_exists && joinExisting_channel)
    {
        std::string channel_name = pyAs(WString, call.tuple->items[0])->content();

        if (!(LSCDB::IsChannelNameAvailable(channel_name)))
        {
            // Channel exists, so get its info from database and create this channel in the cache:
            std::string ch_name, ch_motd, ch_compkey, ch_password;
            uint32 ch_ID, ch_ownerID, ch_cspa, ch_temp, ch_mode;
            bool ch_memberless, ch_maillist;
            std::string ch_type = "";

            LSCDB::GetChannelInformation(channel_name, ch_ID, ch_motd, ch_ownerID, ch_compkey, ch_memberless, ch_password, ch_maillist, ch_cspa, ch_temp, ch_mode);

            channel = proxy->CreateChannel
            (
                ch_ID,
                channel_name.c_str(),
                ch_motd.c_str(),
                ch_type,
                ch_compkey.c_str(),
                ch_ownerID,
                ch_memberless,
                ch_password.c_str(),
                ch_maillist,
                ch_cspa,
                ch_temp,
                ch_mode
            );

            if (channel == NULL)
            {
                SysLog::Error( "LSCService", "%s: Error creating new chat channel", call.client->GetName() );
                return NULL;
            }
        }
        else
        {
            SysLog::Error( "LSCService", "%s: Unable to join channel '%s', this channel does not exist.", call.client->GetName(), channel_name.c_str() );
            return NULL;
        }
    }


    if (temporary_exists && temporary_channel)
    {
        uint32 channel_id;
        channel_id = LSCDB::GetNextAvailableChannelID();

        // This is a temporary private chat channel, so don't look for it in the database, just make a new one:
        channel = proxy->CreateChannel
        (
            channel_id,
            pyAs(String, call.tuple->GetItem(0))->content().c_str(),
            "",
            std::string(""),
            "",
            call.client->GetCharacterID(),
            false,
            "",
            false,
            0,
            1,
            0
        );

        if (channel == NULL)
        {
            SysLog::Error( "LSCService", "%s: Error creating new Temporary chat channel", call.client->GetName() );
            return NULL;
        }

        // Save this channel to the database with the 'temporary' field marked as 1 so that when the last character
        // leaves this channel, the server knows to remove it from the database:
        LSCDB::WriteNewChannelToDatabase(channel_id, pyAs(String, call.tuple->GetItem(0))->content(), call.client->GetCharacterID(), 1, cmode);
    }


    if ((joinExisting_exists && joinExisting_channel) || (temporary_exists && temporary_channel))
    {
        // Now that channel is created, join it:
        if( !channel->IsJoined( call.client->GetCharacterID() ) )
        {
            // Save this subscription to this channel to the database IF it is not temporary:
            if (channel->GetTemporary() == 0)
                LSCDB::WriteNewChannelSubscriptionToDatabase(call.client->GetCharacterID(), channel->GetChannelID(),
                    call.client->GetCorporationID(), call.client->GetAllianceID(),
                    2, 0 );     // the "extra" field is hard-coded
                                                            // to '0' for now since I don't
                                                            // know what it's used for

            channel->JoinChannel( call.client );

            ChannelCreateReply reply;
            reply.ChannelChars = channel->EncodeChannelChars();
            reply.ChannelInfo = channel->EncodeChannelSmall( call.client->GetCharacterID() );
            reply.ChannelMods = channel->EncodeChannelMods();
            return reply.Encode();
        }

        // Somehow execution got here and was not captured in either Creating a new channel, Joining a temporary channel,
        // or Joining an existing channel, so print an error:
        SysLog::Error( "LSCService", "%s: ERROR: Character %u tried to join/create channel '%s'.  The packet format was unexpected.", call.client->GetName(), call.client->GetCharacterID(), channel->GetDisplayName().c_str() );
        return NULL;
    }
    else
    {
        // Malformed packet somehow / no "create" field in byname map
        SysLog::Error( "LSCService", "%s: Malformed packet: 'create' field in byname map is missing.", call.client->GetName() );
        return NULL;
    }
}


PyResult LSCService::Handle_Configure( PyCallArgs& call )
{
    // WARNING: This call contains manual packet decoding to handle configuring parameters for
    // user-created chat channels since I didn't want to monkey around with the LSCPkts.xmlp.
    // -- Aknor Jaden (2010-11-26)

    std::shared_ptr<LSCChannel> channel;
    int32 channel_id = 0;

    //ChannelInfo args;
    //if (!args.Decode( call.tuple )) {
    //    codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
    //    return NULL;
    //}

    // Get Tuple which contains channel number to modify:
    if (pyIs(Int, pyAs(Tuple, call.tuple)->GetItem(0)))
        channel_id = pyAs(Int, pyAs(Tuple, call.tuple)->GetItem(0))->value();
    else
    {
        SysLog::Error( "LSCService", "%s: Tuple contained wrong type: '%s'", call.client->GetName(), call.tuple->TypeString() );
        return NULL;
    }

    // Get count of parameters or just loop through the std::map until you've reached the end
    if (call.byname.size() == 0)
    {
        SysLog::Error( "LSCService", "%s: byname std::map contained zero elements, expected at least one.", call.client->GetName() );
        return NULL;
    }

    // Find channel in existing channels:
    channel = PyServiceMgr::lscProxy_service->getChannel(channel_id);
    if(channel.get() == nullptr)
    {
        SysLog::Error( "LSCService", "%s: Handle_Configure Couldn't find channel %u", call.client->GetName(), channel_id );
        return NULL;
    }

    std::string str_NEW_displayName;
    int32 int_NEW_memberless;
    std::string str_NEW_motd;
    std::string str_newPassword;
    std::string str_oldPassword;

    // For each entry in the map, check its first value against one of these strings, then call appropriate set() function:
    //        "displayName"
    if (!(call.byname.find("displayName") == call.byname.end()))
    {
        if (pyIs(WString, call.byname.find("displayName")->second))
        {
            str_NEW_displayName = pyAs(WString, call.byname.find("displayName")->second)->content();
            channel->SetDisplayName(str_NEW_displayName);
        }
        else
        {
            SysLog::Error( "LSCService", "%s: displayName contained wrong type: '%s'", call.client->GetName(), call.byname.find("displayName")->second->TypeString() );
            return NULL;
        }
    }

    //        "memberless"
    if (!(call.byname.find("memberless") == call.byname.end()))
    {
        if (pyIs(Int, call.byname.find("memberless")->second))
        {
            int_NEW_memberless = pyAs(Int, call.byname.find("memberless")->second)->value();
            channel->SetMemberless(int_NEW_memberless ? true : false);
        }
        else
        {
            SysLog::Error( "LSCService", "%s: memberless contained wrong type: '%s'", call.client->GetName(), call.byname.find("memberless")->second->TypeString() );
            return NULL;
        }
    }

    //        "motd"
    if (!(call.byname.find("motd") == call.byname.end()))
    {
        if (pyIs(WString, call.byname.find("motd")->second))
        {
            str_NEW_motd = pyAs(WString, call.byname.find("motd")->second)->content();
            channel->SetMOTD(str_NEW_motd);
        }
        else
        {
            SysLog::Error( "LSCService", "%s: motd contained wrong type: '%s'", call.client->GetName(), call.byname.find("motd")->second->TypeString() );
            return NULL;
        }
    }

    //        "oldPassword"
    if (!(call.byname.find("oldPassword") == call.byname.end()))
    {
        if (pyIs(WString, call.byname.find("oldPassword")->second))
        {
            str_oldPassword = pyAs(WString, call.byname.find("oldPassword")->second)->content();
            if (channel->GetPassword() == str_oldPassword)
            {
                //        "newPassword"
                if (!(call.byname.find("newPassword") == call.byname.end()))
                {
                    if (pyIs(WString, call.byname.find("newPassword")->second))
                    {
                        str_newPassword = pyAs(WString, call.byname.find("newPassword")->second)->content();
                        channel->SetPassword(str_newPassword);
                    }
                    else
                    {
                        SysLog::Error( "LSCService", "%s: newPassword contained wrong type: '%s'", call.client->GetName(), call.byname.find("newPassword")->second->TypeString() );
                        return NULL;
                    }
                }
            }
            else
            {
                SysLog::Error( "LSCService", "%s: incorrect oldPassword supplied. Password NOT changed.", call.client->GetName() );
                return NULL;
            }
        }
        else if (pyIs(None, call.byname.find("oldPassword")->second))
        {
            //        "newPassword"
            if (!(call.byname.find("newPassword") == call.byname.end()))
            {
                if (pyIs(WString, call.byname.find("newPassword")->second))
                {
                    str_newPassword = pyAs(WString, call.byname.find("newPassword")->second)->content();
                    channel->SetPassword(str_newPassword);
                }
                else
                {
                    SysLog::Error( "LSCService", "%s: newPassword contained wrong type: '%s'", call.client->GetName(), call.byname.find("newPassword")->second->TypeString() );
                    return NULL;
                }
            }
        }
        else
        {
            SysLog::Error( "LSCService", "%s: oldPassword is of an unexpected type: '%s'", call.client->GetName(), call.byname.find("newPassword")->second->TypeString() );
            return NULL;
        }
    }

    // Save the new channel parameters to the database 'channels' table:
    LSCDB::UpdateChannelConfigureInfo(channel.get());

    // ********** TODO **********
    // Figure out how to send a packet to all clients subscribed to this channel that contains all channel parameters
    // so that their clients can update everything that has changed in this channel's configuration.
    // **************************

    // This packet sent back to the client configuring the channel parameters is insufficient to update itself or
    // any other client attached to this channel.
    ChannelCreateReply reply;
    reply.ChannelChars = channel->EncodeChannelChars();
    reply.ChannelInfo = channel->EncodeChannelSmall( call.client->GetCharacterID() );
    reply.ChannelMods = channel->EncodeChannelMods();
    return reply.Encode();
}


PyResult LSCService::Handle_DestroyChannel( PyCallArgs& call )
{
    Call_SingleIntegerArg arg;
    if( !arg.Decode( call.tuple ) )
    {
        SysLog::Error( "LSCService", "%s: Invalid arguments", call.client->GetName() );
        return NULL;
    }

    LSCProxyService *proxy = PyServiceMgr::lscProxy_service;
    std::shared_ptr<LSCChannel> res = proxy->getChannel(arg.arg);
    if( res.get() == nullptr )
    {
        SysLog::Error( "LSCService", "%s: Couldn't find channel %u", call.client->GetName(), arg.arg );
        return NULL;
    }

    // ********** TODO **********
    // Figure out how to validate whether this character (call.client->GetCharacterID()) is allowed
    // to destroy this chat channel, and proceed if they are, otherwise, do not.  And, is there an error
    // packet sent back to the client?
    // **************************

    // Remove the channel from the database:
    LSCDB::RemoveChannelFromDatabase(res->GetChannelID());

    // Finally, remove the channel from the server dynamic objects:
    res->Evacuate( call.client );
    proxy->removeChannel(res->GetChannelID());

    // Now, remove the channel from the database:
    LSCDB::RemoveChannelFromDatabase(res->GetChannelID());

    return new PyNone;
}

PyResult LSCService::Handle_AccessControl( PyCallArgs& call )
{
    // WARNING: This call contains manual packet decoding to handle Access Control since I didn't want to monkey around with the LSCPkts.xmlp.
    // -- Aknor Jaden (2010-11-26)

    //int32 channel_id = 0;

    // BIG TODO:  The whole reason why normal players cannot post chats in other channels has to do with the Access Mode
    // in the channel settings dialog in the client.  Now, I don't know why chatting in the Help/Rookie Help is not allowed
    // for ANY normal players, however, at the very least, implementing the change in Access Mode to 3 = Allowed may fix
    // the issue.  The only thing to figure out is how to format a packet to be sent to the client(s) to inform of the change
    // in access mode.  Is this really needed though, since the owner will change the mode and the owner's client will know
    // immediately, and anyone wanting to join will get that mode value when the JoinChannel has been called.

    // pyAs(Int, call.tuple->GetItem(0))->value() = channel ID
    // pyIs(None, call.tuple->GetItem(1)) == true  <---- change made to "" field
    // pyAs(Int, call.tuple->GetItem(2))->value() =
    //     0 = ??
    //     1 = Moderated
    //     2 = ??
    //     3 = Allowed

    // pyIs(Int, call.tuple->GetItem(1)) == true  <---- character ID for character add to one of the lists specified by GetItem(2):
    // pyAs(Int call.tuple->GetItem(2))->value() =
    //     3 = Add to Allowed List
    //     -2 = Add to Blocked List
    //     7 = Add to Moderators List


    // ********** TODO **********
    // Figure out how to send a packet to all clients subscribed to this channel that contains all channel parameters
    // so that their clients can update everything that has changed in this channel's access control.
    // **************************

    return new PyInt( 1 );
}

PyResult LSCService::Handle_Invite(PyCallArgs &call)
{
    // WARNING: This call contains manual packet decoding to handle chat messages sent inside user-created
    // chat channels since I didn't want to monkey around with the LSCPkts.xmlp.
    // -- Aknor Jaden (2010-11-19)

    std::shared_ptr<LSCChannel> channel;

    uint32 channel_ID;
    //uint32 char_ID = call.client->GetCharacterID();
    uint32 invited_char_ID;

    // Decode the call:
    if (pyIs(Tuple, call.tuple))
    {
        if (pyIs(Int, call.tuple->GetItem(1)))
        {
            channel_ID = pyAs(Int, call.tuple->GetItem(1))->value();
        }
        else
        {
            SysLog::Error( "LSCService", "%s: call.tuple->GetItem(1) is of the wrong type: '%s'.  Expected PyInt type.", call.client->GetName(), call.tuple->TypeString() );
            return NULL;
        }

        if (pyIs(Int, call.tuple->GetItem(0)))
        {
            invited_char_ID = pyAs(Int, call.tuple->GetItem(0))->value();
        }
        else
        {
            SysLog::Error( "LSCService", "%s: call.tuple->GetItem(0) is of the wrong type: '%s'.  Expected PyInt type.", call.client->GetName(), call.tuple->TypeString() );
            return NULL;
        }
    }
    else
    {
        SysLog::Error( "LSCService", "%s: call.tuple is of the wrong type: '%s'.  Expected PyTuple type.", call.client->GetName(), call.tuple->TypeString() );
        return NULL;
    }

    LSCProxyService *proxy = PyServiceMgr::lscProxy_service;
    channel = proxy->getChannel(channel_ID);
    // Now that the packet is known to be good, find the channel to join and join it:
    if(channel.get() != nullptr)
    {
        if(!channel->IsJoined(invited_char_ID))
        {
            // SOMEHOW SEND A JOIN COMMAND/REQUEST TO THE TARGET CLIENT FOR invited_char_ID
        /*    OnLSC_JoinChannel join;
            join.sender = channel->_MakeSenderInfo(call.client);
            join.member_count = 1;
            join.channelID = channel->EncodeID();
            PyTuple *answer = join.Encode();
            MulticastTarget mct;
            //LSCChannelChar *invitor;
            //LSCChannelChar *invitee;
            if ( !channel->IsJoined(char_ID) )
            {
                //invitor = new LSCChannelChar(channel,0,char_ID,call.client->GetCharacterName(),0,0,0,0);
                mct.characters.insert(char_ID);
            }
            //invitee = new LSCChannelChar(channel,0,invited_char_ID,EntityList::FindCharacter(invited_char_ID)->GetCharacterName(),0,0,0,0);
            mct.characters.insert(invited_char_ID);
            EntityList::Multicast( "OnLSC", channel->GetTypeString(), &answer, mct );
            //EntityList::Unicast(invited_char_ID,"OnLSC",channel->GetTypeString(),&answer,false);
        */

            // ********** TODO **********
            // Figure out how to send the ChatInvite packet to the client running the character with id = 'invited_char_ID'
            // in order for that character's client to then issue the JoinChannels call to the server with the chat channel
            // ID equal to that of this channel, be it either a private convo (temporary==1) or an existing user-created chat.
            // **************************

            //ChatInvite chatInvitePacket;
            //chatInvitePacket.integer1 = 1;
            //chatInvitePacket.integer2 = invited_char_ID;
            //chatInvitePacket.boolean = true;
            //chatInvitePacket.displayName = call.tuple->GetItem(2)->AsString()->content();
            //chatInvitePacket.integer3 = 1;
            //chatInvitePacket.integer4 = 0;
            //chatInvitePacket.integer5 = 1;
            //PyTuple *tuple = chatInvitePacket.Encode();
            //EntityList::Unicast(invited_char_ID, "", "", &tuple, false);
        }
        else
        {
            SysLog::Error( "LSCService", "%s: Character %u is already joined to channel %u.", call.client->GetName(), invited_char_ID, channel_ID );
            return NULL;
        }
    }
    else
    {
        SysLog::Error( "LSCService", "%s: Cannot find channel %u.", call.client->GetName(), channel_ID );
        return NULL;
    }

    return new PyInt( 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
// EveMail calls:
//
///////////////////////////////////////////////////////////////////////////////

PyResult LSCService::Handle_GetMyMessages(PyCallArgs &call)
{
    return (LSCDB::GetMailHeaders(call.client->GetCharacterID()));
}


PyResult LSCService::Handle_GetMessageDetails(PyCallArgs &call) {
    Call_TwoIntegerArgs args;
    if(!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    //TODO: verify ability to read this message...

    return (LSCDB::GetMailDetails(args.arg2, args.arg1));
}


PyResult LSCService::Handle_Page(PyCallArgs &call) {
    Call_Page args;
    if(!args.Decode(&call.tuple))
    {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    _log(SERVICE__MESSAGE, "%s: Received evemail msg with subject '%s': %s", call.client->GetName(), args.subject.c_str(), args.body.c_str());

    PyServiceMgr::lscProxy_service->SendMail(call.client->GetCharacterID(), args.recipients, args.subject, args.body);

    return NULL;
}

PyResult LSCService::Handle_MarkMessagesRead(PyCallArgs &call) {
    Call_SingleIntList args;
    if(!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    std::vector<int32>::iterator cur, end;
    cur = args.ints.begin();
    end = args.ints.end();
    for(; cur != end; cur++)
    {
        LSCDB::MarkMessageRead(*cur);
    }
    return NULL;
}


PyResult LSCService::Handle_DeleteMessages(PyCallArgs &call) {
    Call_DeleteMessages args;
    if(!args.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    if(args.channelID != (int32)call.client->GetCharacterID()) {
        _log(SERVICE__ERROR, "%s (%d) tried to delete messages in channel %u. Denied.", call.client->GetName(), call.client->GetCharacterID(), args.channelID);
        return NULL;
    }

    std::vector<int32>::iterator cur, end;
    cur = args.messages.begin();
    end = args.messages.end();
    for(; cur != end; cur++)
    {
        LSCDB::DeleteMessage(*cur, args.channelID);
    }

    return NULL;
}


PyResult LSCService::Handle_GetMembers(PyCallArgs &call) {
    CallGetMembers arg;
    if (!arg.Decode(&call.tuple)) {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    uint32 channelID;
    if( pyIs(Int, arg.channel) )
        channelID = pyAs(Int, arg.channel)->value();
    else if( pyIs(Tuple, arg.channel) )
    {
        PyTuple* prt = pyAs(Tuple, arg.channel);

        if( pyIs(Int, prt->GetItem( 0 )) )
            channelID = pyAs(Int, prt->GetItem( 0 ))->value();
        else if( pyIs(Tuple, prt->GetItem( 0 )) )
        {
            prt = pyAs(Tuple, prt->GetItem( 0 ));

            if( prt->items.size() != 2 || !pyAs(Int, prt->GetItem( 1 )) )
            {
                codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
                return NULL;
            }

            channelID = pyAs(Int, prt->GetItem( 1 ))->value();
        }
        else
        {
            codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
            return NULL;
        }
    }
    else
    {
        codelog(SERVICE__ERROR, "%s: Bad arguments", call.client->GetName());
        return NULL;
    }

    std::shared_ptr<LSCChannel> channel = PyServiceMgr::lscProxy_service->getChannel(channelID);
    if(channel.get() != nullptr)
    {
        return channel->EncodeChannelChars();
    }

    return NULL;
}


PyResult LSCService::Handle_GetMember(PyCallArgs &call) {
    return NULL;
}
