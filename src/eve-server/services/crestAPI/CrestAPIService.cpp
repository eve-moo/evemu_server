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
    Author:        eve-moo
 */


#include "eve-server.h"
#include "CrestAPIService.h"
#include "PyBoundObject.h"
#include "PyServiceCD.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(CrestAPIService)

CrestAPIService::CrestAPIService()
: PyService("crestapiService", new Dispatcher(this))
{
    PyCallable_REG_CALL(CrestAPIService, GetExternalAddress)
}

CrestAPIService::~CrestAPIService() { }

PyResult CrestAPIService::Handle_GetExternalAddress(PyCallArgs &call)
{
    std::stringstream urlBuilder;
    urlBuilder << "https://" << call.client->m_networkConfig.crestServer;
    if(call.client->m_networkConfig.crestServerPort != 0)
    {
        urlBuilder << ":" << (call.client->m_networkConfig.crestServerPort);
    }
    // TO-DO: implement our own crest server.
    return new PyString(urlBuilder.str());
}
