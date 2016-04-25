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
    Author:        ozatomic
*/

#include "eve-server.h"

#include "PyServiceCD.h"
#include "account/InfoGatheringMgr.h"

PyCallable_Make_InnerDispatcher(InfoGatheringMgr)

InfoGatheringMgr::InfoGatheringMgr()
: PyService("infoGatheringMgr", new Dispatcher(this))
{
    PyCallable_REG_CALL(InfoGatheringMgr, GetStateAndConfig);
    PyCallable_REG_CALL(InfoGatheringMgr, LogInfoEventsFromClient);

}

InfoGatheringMgr::~InfoGatheringMgr() { }

PyResult InfoGatheringMgr::Handle_GetStateAndConfig(PyCallArgs &call)
{
    // TO-DO: find the purpose of all these values.
    PyDict *rsp = new PyDict;

    rsp->SetItemString("clientWorkerInterval", new PyInt(600000)); //Default From packetlogs is 600000
    rsp->SetItemString("isEnabled", new PyInt(1)); //0 = Disabled, 1 = Enabled. Set to 0 becuase jsut gettting rid of exception.

    PyDict *aggregates = new PyDict();
    aggregates->SetItem(new PyInt(2), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3}));
    aggregates->SetItem(new PyInt(3), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6}));
    aggregates->SetItem(new PyInt(4), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6}));
    aggregates->SetItem(new PyInt(5), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6}));
    aggregates->SetItem(new PyInt(6), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3, 4}));
    aggregates->SetItem(new PyInt(7), PyObjectEx_Type1::createBuiltinSetList({1}));
    aggregates->SetItem(new PyInt(8), PyObjectEx_Type1::createBuiltinSetList({1, 4}));
    aggregates->SetItem(new PyInt(9), PyObjectEx_Type1::createBuiltinSetList({1, 4}));
    aggregates->SetItem(new PyInt(12), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3}));
    aggregates->SetItem(new PyInt(13), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3}));
    aggregates->SetItem(new PyInt(14), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3}));
    aggregates->SetItem(new PyInt(17), PyObjectEx_Type1::createBuiltinSetList({1}));
    aggregates->SetItem(new PyInt(18), PyObjectEx_Type1::createBuiltinSetList({1, 3}));
    aggregates->SetItem(new PyInt(19), PyObjectEx_Type1::createBuiltinSetList({1, 2}));
    aggregates->SetItem(new PyInt(20), PyObjectEx_Type1::createBuiltinSetList({1, 3}));
    aggregates->SetItem(new PyInt(21), PyObjectEx_Type1::createBuiltinSetList({1, 3}));
    aggregates->SetItem(new PyInt(22), PyObjectEx_Type1::createBuiltinSetList({1, 2, 3, 4, 5}));
    aggregates->SetItem(new PyInt(25), PyObjectEx_Type1::createBuiltinSetList({1, 6}));
    aggregates->SetItem(new PyInt(26), PyObjectEx_Type1::createBuiltinSetList({1}));
    aggregates->SetItem(new PyInt(27), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6, 7}));
    aggregates->SetItem(new PyInt(28), PyObjectEx_Type1::createBuiltinSetList({1, 4}));
    aggregates->SetItem(new PyInt(29), PyObjectEx_Type1::createBuiltinSetList({1, 3, 6}));
    aggregates->SetItem(new PyInt(30), PyObjectEx_Type1::createBuiltinSetList({1}));
    aggregates->SetItem(new PyInt(32), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6, 7}));
    aggregates->SetItem(new PyInt(33), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6}));
    aggregates->SetItem(new PyInt(34), PyObjectEx_Type1::createBuiltinSetList({1, 2, 6}));
    rsp->SetItemString("infoTypeAggregates", aggregates);
    rsp->SetItemString("infoTypesOncePerRun", new PyObjectEx_Type1(new PyToken("__builtin__.set"), new_tuple(new PyList())));
    PyDict *params = new PyDict();
    params->SetItem(new PyInt(2), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4}));
    params->SetItem(new PyInt(3), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    params->SetItem(new PyInt(4), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    params->SetItem(new PyInt(5), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    params->SetItem(new PyInt(6), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5}));
    params->SetItem(new PyInt(7), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3}));
    params->SetItem(new PyInt(8), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(9), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(12), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4}));
    params->SetItem(new PyInt(13), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 9}));
    params->SetItem(new PyInt(14), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 9}));
    params->SetItem(new PyInt(15), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5}));
    params->SetItem(new PyInt(16), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5, 9, 10}));
    params->SetItem(new PyInt(17), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4, 5}));
    params->SetItem(new PyInt(18), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(19), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5}));
    params->SetItem(new PyInt(20), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(21), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(22), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5, 9}));
    params->SetItem(new PyInt(24), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3}));
    params->SetItem(new PyInt(25), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 6, 9}));
    params->SetItem(new PyInt(26), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3}));
    params->SetItem(new PyInt(27), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6, 7, 9}));
    params->SetItem(new PyInt(28), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4}));
    params->SetItem(new PyInt(29), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4, 6}));
    params->SetItem(new PyInt(30), PyObjectEx_Type1::createBuiltinSetList({0, 1, 3, 4, 5, 9}));
    params->SetItem(new PyInt(32), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6, 7}));
    params->SetItem(new PyInt(33), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    params->SetItem(new PyInt(34), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    params->SetItem(new PyInt(35), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5, 9, 10, 11}));
    params->SetItem(new PyInt(36), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5, 9}));
    params->SetItem(new PyInt(37), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 4, 5, 9, 10}));
    params->SetItem(new PyInt(38), PyObjectEx_Type1::createBuiltinSetList({0, 1, 2, 3, 6}));
    rsp->SetItemString("infoTypeParameters", params);

    std::vector<int32> infoTypesVals = {
        2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38
    };

    rsp->SetItemString("infoTypes", PyObjectEx_Type1::createBuiltinSetList(infoTypesVals));

    return new PyObject( "utillib.KeyVal", rsp );
}

PyResult InfoGatheringMgr::Handle_LogInfoEventsFromClient(PyCallArgs &call) {
    return new PyNone;
}
