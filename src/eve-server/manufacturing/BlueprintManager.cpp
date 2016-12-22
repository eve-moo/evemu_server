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

#include "PyServiceCD.h"
#include "manufacturing/Blueprint.h"
#include "manufacturing/FactoryDB.h"
#include "PyServiceMgr.h"
#include "BlueprintManager.h"

PyCallable_Make_InnerDispatcher(BlueprintManager)

BlueprintManager::BlueprintManager()
: PyService("blueprintManager", new Dispatcher(this))
{
    PyCallable_REG_CALL(BlueprintManager, GetBlueprintDataByOwner);
}

BlueprintManager::~BlueprintManager()
{
}

PyResult BlueprintManager::Handle_GetBlueprintDataByOwner(PyCallArgs &call) {
    if(call.tuple->items.size() != 2)
    {
        codelog(CLIENT__ERROR, "%s: BlueprintManager::GetBlueprintDataByOwner wrong number of args.", call.client->GetName());
        return NULL;
    }

    PyList *bpList = new PyList();
    PyDict *facilities = new PyDict();
    PySubStream *result = new PySubStream(new_tuple(bpList, facilities));

    int ownerID = 0;
    int locationID = 0;
    if(!pyIs(Int, call.tuple->items[0]))
    {
        codelog(CLIENT__ERROR, "%s: BlueprintManager::GetBlueprintDataByOwner invalid characterID.", call.client->GetName());
        return _BuildCachedReturn(&result, CacheCheckTime::check_in_15_minutes);
    }
    PyInt *cID = pyAs(Int, call.tuple->items[0]);
    ownerID = cID->value();
    // Check for location.
    if(pyIs(Int, call.tuple->items[1]))
    {
        PyInt *lID = pyAs(Int, call.tuple->items[1]);
        locationID = lID->value();
    }
    
    // Get blueprint IDs.
    std::vector<int64> bpIDs;
    if(!FactoryDB::getBlueprintIDsForOwner(ownerID, bpIDs))
    {
        codelog(CLIENT__ERROR, "%s: BlueprintManager::GetBlueprintDataByOwner Failed to get blueprint IDs.", call.client->GetName());
        return _BuildCachedReturn(&result, CacheCheckTime::check_in_15_minutes);
    }

    std::map<uint32, uint32> facilityMap;
    for(int64 id : bpIDs)
    {
        BlueprintRef b = ItemFactory::GetBlueprint(id);
        if(b.get() != nullptr)
        {
            // Count the number of BPs in each location.
            // We count ALL BPs even if they are not in our location.
            uint32 locID = b->locationID();
            facilityMap[locID]++;
            if(locationID != 0 && (int)locID != locationID)
            {
                // We are looking for only the BPs in a specific location.
                // This one is not there!
                continue;
            }
            bpList->AddItem(b->getPackedRow());
        }
    }
    // Now populate the dict with the blueprint counts.
    for(auto pair : facilityMap)
    {
        facilities->SetItem(new PyInt(pair.first), new PyInt(pair.second));
    }

    return _BuildCachedReturn(&result, CacheCheckTime::check_in_15_minutes);
}

