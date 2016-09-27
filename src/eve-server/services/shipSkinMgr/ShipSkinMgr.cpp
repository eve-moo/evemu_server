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
#include "ShipSkinMgr.h"
#include "PyBoundObject.h"
#include "PyServiceCD.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(ShipSkinMgrService)

ShipSkinMgrService::ShipSkinMgrService()
: PyService("shipSkinMgr", new Dispatcher(this))
{
    PyCallable_REG_CALL(ShipSkinMgrService, GetAppliedSkinMaterialSetID)
    PyCallable_REG_CALL(ShipSkinMgrService, GetAppliedSkin)
    PyCallable_REG_CALL(ShipSkinMgrService, GetLicencedSkinsForType)
}

ShipSkinMgrService::~ShipSkinMgrService()
{
}

PyResult ShipSkinMgrService::Handle_GetAppliedSkinMaterialSetID(PyCallArgs &call)
{
    if (call.tuple->size() != 3)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 3 got %u", call.client->GetName(), call.tuple->size());
        return new PyNone();
    }
    // Three values  (characterID, shipItemID, shipTypeID)
    // This call always has shipTypeID of none?
    uint32 values[2];
    for (int i = 0; i < 2; i++)
    {
        PyRep *val = call.tuple->items[i];
        if (!val->IsInt())
        {
            codelog(CLIENT__ERROR, "%s: Expected int got %s", call.client->GetName(), val->TypeString());
            return nullptr;
        }
        values[i] = (val->AsInt())->value();
    }
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
                          "SELECT materialSetID"
                          " From skinMaterials"
                          " WHERE skinMaterialID IN ("
                          " SELECT skinMaterialID FROM skins WHERE skinID IN ("
                          " SELECT skinID FROM srvShipSkin"
                          " WHERE ItemID=%u))",
                          values[1]))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
    }
    else
    {
        DBResultRow row;
        while (res.GetRow(row))
        {
            if (!row.IsNull(0))
            {
                uint32 materialSetID = row.GetUInt(0);
                // TO-DO: Not sure how this should be returned. Probably PyInt?
                return new PyInt(materialSetID);
            }
        }
    }
    return new PyNone();
}

PyResult ShipSkinMgrService::Handle_GetAppliedSkin(PyCallArgs &call)
{
    if (call.tuple->size() != 3)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 3 got %u", call.client->GetName(), call.tuple->size());
        return new PyNone();
    }
    // Three values  (characterID, shipItemID, shipTypeID)
    // This call may have shipTypeID of none!
    uint32 values[2];
    for (int i = 0; i < 2; i++)
    {
        PyRep *val = call.tuple->items[i];
        if (!val->IsInt())
        {
            codelog(CLIENT__ERROR, "%s: Expected int got %s", call.client->GetName(), val->TypeString());
            return nullptr;
        }
        values[i] = (val->AsInt())->value();
    }
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
                          "SELECT skinID, srvChrSkins.expireTime"
                          " From srvShipSkin"
                          " LEFT JOIN srvChrSkins USING(skinID)"
                          " WHERE characterID=%u AND itemID=%u",
                          values[0], values[1]))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
    }
    else
    {
        DBResultRow row;
        while (res.GetRow(row))
        {
            uint32 skinID = row.GetUInt(0);
            PyToken *token = new PyToken("shipskins.storage.LicensedSkin");
            PyDict *dict = new PyDict();
            dict->SetItemString("_skinID", new PyInt(skinID));
            // Expires for permanent skins is PyNone.
            PyRep *expire;
            if (row.IsNull(1))
            {
                expire = new PyNone();
            }
            else
            {
                // Unknown what the expires value should be for non permanent skins Probably PyLong(Win32Time)?
                expire = new PyLong(row.GetInt64(1));
            }
            dict->SetItemString("_expires", expire);
            PyObjectEx_Type2 *license = new PyObjectEx_Type2(new_tuple(token), dict);
            return license;
        }
    }
    // If no skin applied the response is PyNone.
    return new PyNone();
}

PyResult ShipSkinMgrService::Handle_GetLicencedSkinsForType(PyCallArgs &call)
{
    // Return a List of PyObjectEx_type2 objects.
    // An empty list is no licensed skins.
    PyList *licencedSkins = new PyList();
    if (call.tuple->size() != 1)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 1 got %u", call.client->GetName(), call.tuple->size());
        // Error: Just return the empty list.
        return licencedSkins;
    }
    PyRep *val = call.tuple->items[0];
    if (!val->IsInt())
    {
        codelog(CLIENT__ERROR, "%s: Expected int got %s", call.client->GetName(), val->TypeString());
        // Error: Just return the empty list.
        return licencedSkins;
    }
    uint32 typeID = (val->AsInt())->value();
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
                          "SELECT skinID, expireTime"
                          " From srvChrSkins"
                          " WHERE characterID = %u AND skinID IN (SELECT skinID FROM skinShip WHERE typeID=%u)",
                          call.client->GetCharacterID(), typeID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
    }
    else
    {
        DBResultRow row;
        while (res.GetRow(row))
        {
            uint32 skinID = row.GetUInt(0);
            PyToken *token = new PyToken("shipskins.storage.LicensedSkin");
            PyDict *dict = new PyDict();
            dict->SetItemString("_skinID", new PyInt(skinID));
            // Expires for permanent skins is PyNone.
            PyRep *expire;
            if (row.IsNull(1))
            {
                expire = new PyNone();
            }
            else
            {
                // Unknown what the expires value should be for non permanent skins Probably PyLong(Win32Time)?
                expire = new PyLong(row.GetInt64(1));
            }
            dict->SetItemString("_expires", expire);
            PyObjectEx_Type2 *license = new PyObjectEx_Type2(new_tuple(token), dict);
            // Add the license.
            licencedSkins->AddItem(license);
        }
    }
    return licencedSkins;
}
