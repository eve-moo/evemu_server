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
#include "FacilityManager.h"
#include "station/Station.h"
#include "map/MapDenormalize.h"

PyCallable_Make_InnerDispatcher(FacilityManager)

FacilityManager::FacilityManager()
: PyService("facilityManager", new Dispatcher(this))
{
    PyCallable_REG_CALL(FacilityManager, GetFacility);
    PyCallable_REG_CALL(FacilityManager, GetFacilities);
    PyCallable_REG_CALL(FacilityManager, GetFacilityLocations);
    PyCallable_REG_CALL(FacilityManager, GetMaxActivityModifiers);
}

FacilityManager::~FacilityManager()
{
}

PyResult FacilityManager::Handle_GetFacility(PyCallArgs &call)
{
    Call_SingleLongArg arg;
    if(arg.Decode(call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: FacilityManager::GetFacility failed to decode args.", call.client->GetName());
        return NULL;
    }
    StationRef sta = ItemFactory::GetStation(arg.arg);
    if(sta.get() != nullptr)
    {
        return sta->getFacilityDict();
    }
    // TO-DO: check citadels.

    return nullptr;
}

PyResult FacilityManager::Handle_GetFacilities(PyCallArgs &call)
{
    Call_SingleIntegerArg arg;
    if(arg.Decode(call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: FacilityManager::GetFacilities failed to decode args.", call.client->GetName());
        return NULL;
    }
    // This is a range from player location.
    uint32 range = arg.arg;
    PyList *facilities = new PyList();

    MapSolarSystemRef system = MapSolarSystem::getSystem(call.client->GetSystemID());
    std::vector<int> systems;
    system->getSystemsInRange(range, systems);
    for(int systemID : systems)
    {
        system = MapSolarSystem::getSystem(systemID);
        if(system.get() == nullptr)
        {
            continue;
        }
        std::vector<uint32> stations;
        system->getStations(stations);
        for(uint32 stationID : stations)
        {
            StationRef station = ItemFactory::GetStation(stationID);
            if(station.get() != nullptr)
            {
                facilities->AddItem(station->getFacilityDict());
            }
        }
    }

    return facilities;
}

PyResult FacilityManager::Handle_GetFacilityLocations(PyCallArgs &call)
{
    Call_TwoLongArgs arg;
    if(arg.Decode(call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: FacilityManager::GetFacilityLocations failed to decode args.", call.client->GetName());
        return NULL;
    }
    uint32 facilityID = arg.arg1; // may be PyLong
    //uint32 ownerID = arg.arg2;

    PyList *locations = new PyList();
    InventoryItemRef item = ItemFactory::GetItem(facilityID);
    if(item.get() != nullptr)
    {
        PyDict *dict = new PyDict();
        PyTuple *token = new_tuple(new PyToken("industry.job.Location"));
        dict->SetItem("itemID", new PyLong(item->itemID()));
        dict->SetItem("typeID", new PyLong(item->typeID()));
        // May be other values for corp?
        dict->SetItem("flagID", new PyInt(4));
        dict->SetItem("canTake", new PyBool(true));
        dict->SetItem("ownerID", new PyLong(item->ownerID()));
        dict->SetItem("canView", new PyBool(true));
        PyObjectEx_Type2 *facility = new PyObjectEx_Type2(token, dict);
        locations->AddItem(facility);
    }
    
    return locations;
}

PyResult FacilityManager::Handle_GetMaxActivityModifiers(PyCallArgs &call)
{
    SysLog::Debug( "FacilityManager", "Called GetMaxActivityModifiers stub." );
    PyDict *mods = new PyDict();
    //double modifier = 0.1;
    //int activityID = 1;
    //mods->SetItem(new PyInt(activityID), new PyFloat(modifier));

    return mods;
}
