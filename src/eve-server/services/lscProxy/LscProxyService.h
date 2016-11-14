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

#ifndef __EVEMOO_SERVICES_LSCPROXY_H_INCL__
#define __EVEMOO_SERVICES_LSCPROXY_H_INCL__

#include "PyService.h"
#include "chat/LSCService.h"
#include "chat/LSCChannel.h"
#include "admin/CommandDispatcher.h"
#include <memory>
#include <mutex>

class PyRep;
class LSCService;

class LSCProxyService: public PyService
{
    friend class LSCService;
public:
    // All user-created chat channels are created with IDs that are in this set:
    //     [baseChannelID,maxChannelID]  (note the inclusivity in that set)
    static const uint32 BASE_CHANNEL_ID;
    static const uint32 MAX_CHANNEL_ID;

    LSCProxyService(CommandDispatcher *cd);
    virtual ~LSCProxyService();

    PyResult ExecuteCommand(Client *from, const char *msg);
    void CreateSystemChannel(uint32 systemID);
    void CharacterLogout(uint32 charID, OnLSC_SenderInfo * si);

    void SendMail(uint32 sender, uint32 recipient, const std::string &subject, const std::string &content) {
        std::vector<int32> recs(1, recipient);
        SendMail(sender, recs, subject, content);
    }
    void SendMail(uint32 sender, const std::vector<int32> &recipients, const std::string &subject, const std::string &content);

protected:
    class Dispatcher;

    PyCallable_DECL_CALL(JoinChannels)
    PyCallable_DECL_CALL(LeaveChannels)
    PyCallable_DECL_CALL(SendMessage)

    CommandDispatcher *const m_commandDispatch;

    std::mutex sMutex;
    std::map<uint32, std::shared_ptr<LSCChannel>> m_channels;  //we own these pointers

private:
    std::shared_ptr<LSCChannel> CreateChannel(uint32 channelID, const char * name, const char * motd,
                              std::string type, const char * compkey,
                              uint32 ownerID, bool memberless, const char * password,
                              bool maillist, uint32 cspa, uint32 temporary, uint32 mode);
    std::shared_ptr<LSCChannel> CreateChannel(uint32 channelID, const char * name, const char * motd,
                              std::string type, bool maillist = false);
    std::shared_ptr<LSCChannel> CreateChannel(uint32 channelID, const char * name, std::string type, bool maillist = false);
    std::shared_ptr<LSCChannel> CreateChannel(uint32 channelID, std::string type);
    std::shared_ptr<LSCChannel> CreateChannel(uint32 channelID);
    std::shared_ptr<LSCChannel> CreateChannel(const char * name, bool maillist = false);
    void InitiateStaticChannels();
    
    std::shared_ptr<LSCChannel> getChannel(uint32 channelID);
    void removeChannel(uint32 channelID);
};

#endif
