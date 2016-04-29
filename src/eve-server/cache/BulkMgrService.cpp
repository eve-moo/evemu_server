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
#include "cache/BulkMgrService.h"

PyCallable_Make_InnerDispatcher(BulkMgrService)

BulkMgrService::BulkMgrService()
: PyService("bulkMgr", new Dispatcher(this))
{
    PyCallable_REG_CALL(BulkMgrService, UpdateBulk);
    PyCallable_REG_CALL(BulkMgrService, GetAllBulkIDs);
}

BulkMgrService::~BulkMgrService() {
}

PyResult BulkMgrService::Handle_UpdateBulk(PyCallArgs &call)
{
    Call_UpdateBulk args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments");
	return NULL;
    }

    PyDict* test = new PyDict();
    //if(args.changeID != 116678 || args.hashValue != "2168569871b33fbc686ce664fbe8a2d5")
    //{
    //    // Cache invalid, inform client to update.
    //    test->SetItemString("version", new PyInt(119449));
    //    test->SetItemString("type", new PyInt(updateBulkStatusTooManyRevisions));
    //}
    //else
    //{
        test->SetItemString("type", new PyInt(updateBulkStatusOK));
    //}
    test->SetItemString("allowUnsubmitted", new PyBool(false));

    return test;
}

PyResult BulkMgrService::Handle_GetAllBulkIDs(PyCallArgs &call)
{
    PyList* IDs = new PyList();
    IDs->AddItem(new PyInt(2002600004));
    IDs->AddItem(new PyInt(2002400001));
    IDs->AddItem(new PyInt(2001600002));
    IDs->AddItem(new PyInt(2001600003));
    IDs->AddItem(new PyInt(2001600004));
    IDs->AddItem(new PyInt(2001600005));
    IDs->AddItem(new PyInt(2001600006));
    IDs->AddItem(new PyInt(2001600007));
    IDs->AddItem(new PyInt(2809992));
    IDs->AddItem(new PyInt(800009));
    IDs->AddItem(new PyInt(2000001));
    IDs->AddItem(new PyInt(3200011));
    IDs->AddItem(new PyInt(3200012));
    IDs->AddItem(new PyInt(2002400002));
    IDs->AddItem(new PyInt(3200015));
    IDs->AddItem(new PyInt(3200016));
    IDs->AddItem(new PyInt(800003));
    IDs->AddItem(new PyInt(2002400004));
    IDs->AddItem(new PyInt(800004));
    IDs->AddItem(new PyInt(1800003));
    IDs->AddItem(new PyInt(800005));
    IDs->AddItem(new PyInt(3200002));
    IDs->AddItem(new PyInt(2002500001));
    IDs->AddItem(new PyInt(2002500002));
    IDs->AddItem(new PyInt(7300003));
    IDs->AddItem(new PyInt(7300004));
    IDs->AddItem(new PyInt(2002500005));
    IDs->AddItem(new PyInt(2209987));
    IDs->AddItem(new PyInt(1400002));
    IDs->AddItem(new PyInt(800007));
    IDs->AddItem(new PyInt(600008));
    IDs->AddItem(new PyInt(2002400005));
    IDs->AddItem(new PyInt(1800004));
    IDs->AddItem(new PyInt(100300020));
    IDs->AddItem(new PyInt(2002600010));
    IDs->AddItem(new PyInt(6400004));
    IDs->AddItem(new PyInt(2002600001));
    IDs->AddItem(new PyInt(2002600002));
    IDs->AddItem(new PyInt(2001700035));
    IDs->AddItem(new PyInt(3200001));
    IDs->AddItem(new PyInt(2002600005));
    IDs->AddItem(new PyInt(600006));
    IDs->AddItem(new PyInt(1800007));
    IDs->AddItem(new PyInt(1400008));
    IDs->AddItem(new PyInt(2002200009));
    IDs->AddItem(new PyInt(2002200010));
    IDs->AddItem(new PyInt(2002200011));
    IDs->AddItem(new PyInt(2002600012));
    IDs->AddItem(new PyInt(2003100002));
    IDs->AddItem(new PyInt(2209999));
    IDs->AddItem(new PyInt(1400016));
    IDs->AddItem(new PyInt(2002200001));
    IDs->AddItem(new PyInt(2003100003));
    IDs->AddItem(new PyInt(1800005));
    IDs->AddItem(new PyInt(2800006));
    IDs->AddItem(new PyInt(600007));
    IDs->AddItem(new PyInt(600005));
    IDs->AddItem(new PyInt(1800001));
    IDs->AddItem(new PyInt(7300005));
    IDs->AddItem(new PyInt(2003100001));
    IDs->AddItem(new PyInt(2001900002));
    IDs->AddItem(new PyInt(2001900003));
    IDs->AddItem(new PyInt(2002400003));
    IDs->AddItem(new PyInt(2002200006));
    IDs->AddItem(new PyInt(2002600011));
    IDs->AddItem(new PyInt(100300014));
    IDs->AddItem(new PyInt(100300015));
    IDs->AddItem(new PyInt(2002200002));
    IDs->AddItem(new PyInt(3200010));
    IDs->AddItem(new PyInt(1800006));
    IDs->AddItem(new PyInt(2001800005));
    IDs->AddItem(new PyInt(800006));
    IDs->AddItem(new PyInt(600010));

    return IDs;
}
