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
#include "BountyProxy.h"
#include "PyBoundObject.h"
#include "PyServiceCD.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(BountyProxyService)

BountyProxyService::BountyProxyService()
: PyService("bountyProxy", new Dispatcher(this))
{
    PyCallable_REG_CALL(BountyProxyService, GetMyKillRights)
    PyCallable_REG_CALL(BountyProxyService, GetBountiesAndKillRights)
    PyCallable_REG_CALL(BountyProxyService, GetBounties)
}

BountyProxyService::~BountyProxyService() { }

PyResult BountyProxyService::Handle_GetMyKillRights(PyCallArgs &call)
{
    objectCaching_CachedMethodCallResult_object rights;
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
                         "SELECT"
                         " killRightID,"
                         " fromID,"
                         " toID,"
                         " expiryTime,"
                         " price,"
                         " restrictedTo"
                         " FROM srvKillRights "
                         " WHERE fromID=%u", call.client->GetCharacterID()))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    rights.object = DBResultToCRowset(res);
    return rights.Encode();
}

PyResult BountyProxyService::Handle_GetBountiesAndKillRights(PyCallArgs &call)
{
    if(call.tuple->size() != 2)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 2 got %u", call.client->GetName(), call.tuple->size());
        return nullptr;
    }
    PyObjectEx_Type1 *objEx = (PyObjectEx_Type1 *) call.tuple->GetItem(0)->AsObjectEx();
    PyList *list1 = call.tuple->GetItem(1)->AsList();
    if(objEx == nullptr || list1 == nullptr)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect item type in GetBountiesAndKillRights", call.client->GetName());
        return nullptr;
    }
    if(objEx->GetType()->content() != "__builtin__.set")
    {
        codelog(CLIENT__ERROR, "%s: Incorrect token type in GetBountiesAndKillRights", call.client->GetName());
        return nullptr;
    }
    PyTuple *tuple = objEx->GetArgs();
    if(tuple->size() != 1)
    {
        return nullptr;
    }
    PyList *bountiesList = tuple->GetItem(0)->AsList();
    if(bountiesList == nullptr)
    {
        return nullptr;
    }
    PyList *resBounties = new PyList();
    PyList *resList2 = new PyList();
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("targetID", DBTYPE_I4);
    header->AddColumn("bounty", DBTYPE_CY);
    header->AddColumn("corporationID", DBTYPE_I4);
    header->AddColumn("allianceID", DBTYPE_I4);
    for(int i = 0; i < bountiesList->size(); i++)
    {
        PyRep *itm = bountiesList->GetItem(i);
        if(!itm->IsInt())
        {
            continue;
        }
        PyInt *val = itm->AsInt();
        uint32 id = val->value();
        // TO-DO: populate list with actual results.
        PyPackedRow *row = new PyPackedRow(header);
        PyIncRef(header);
        row->SetField("targetID", new PyInt(id));
        row->SetField("bounty", new PyLong(0));
        row->SetField("corporationID", new PyInt(0));
        row->SetField("allianceID", new PyInt(0));
        resBounties->AddItem(new_tuple(new PyInt(id), row));
    }
    for(int i = 0; i < list1->size(); i++)
    {
        PyRep *itm = list1->GetItem(i);
        if(!itm->IsInt())
        {
            continue;
        }
        PyInt *val = itm->AsInt();
        uint32 id = val->value();
        // TO-DO: populate list with results.
    }
    PyTuple *resTuple = new PyTuple(2);
    resTuple->SetItem(0, resBounties);
    resTuple->SetItem(1, resList2);
    return resTuple;
}

PyResult BountyProxyService::Handle_GetBounties(PyCallArgs &call)
{
    if(call.tuple->size() != 1)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 1 got %u", call.client->GetName(), call.tuple->size());
        return new PyList();
    }
    PyObjectEx_Type1 *objEx = (PyObjectEx_Type1 *) call.tuple->GetItem(0)->AsObjectEx();
    if(objEx == nullptr)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect item type in GetBounties", call.client->GetName());
        return new PyList();
    }
    if(objEx->GetType()->content() != "__builtin__.set")
    {
        codelog(CLIENT__ERROR, "%s: Incorrect token type in GetBounties found %s", call.client->GetName(), objEx->GetType()->TypeString());
        return new PyList();
    }
    PyTuple *tuple = objEx->GetArgs();
    if(tuple->size() != 1)
    {
        return new PyList();
    }
    if(!tuple->GetItem(0)->IsList())
    {
        return new PyList();
    }
    PyList *bountiesList = tuple->GetItem(0)->AsList();
    PyList *resBounties = new PyList();
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("targetID", DBTYPE_I4);
    header->AddColumn("bounty", DBTYPE_CY);
    header->AddColumn("corporationID", DBTYPE_I4);
    header->AddColumn("allianceID", DBTYPE_I4);
    for(int i = 0; i < bountiesList->size(); i++)
    {
        PyRep *itm = bountiesList->GetItem(i);
        if(!itm->IsInt())
        {
            continue;
        }
        PyInt *val = itm->AsInt();
        uint32 id = val->value();
        // TO-DO: populate list with actual results.
        PyPackedRow *row = new PyPackedRow(header);
        PyIncRef(header);
        row->SetField("targetID", new PyInt(id));
        row->SetField("bounty", new PyLong(0));
        row->SetField("corporationID", new PyInt(0));
        row->SetField("allianceID", new PyInt(0));
        resBounties->AddItem(new_tuple(new PyInt(id), row));
    }
    return resBounties;
}
