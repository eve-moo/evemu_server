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
    Author:     eve-moo
 */

#include "eve-server.h"

#include "PyServiceCD.h"
#include "character/AchievementTrackerMgrService.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(AchievementTrackerMgrService)

AchievementTrackerMgrService::AchievementTrackerMgrService()
: PyService("achievementTrackerMgr", new Dispatcher(this))
{
    PyCallable_REG_CALL(AchievementTrackerMgrService, GetCompletedAchievementsAndClientEventCount)
    PyCallable_REG_CALL(AchievementTrackerMgrService, UpdateClientAchievmentsAndCounters)
}

AchievementTrackerMgrService::~AchievementTrackerMgrService() { }

PyResult AchievementTrackerMgrService::Handle_GetCompletedAchievementsAndClientEventCount(PyCallArgs &call)
{
    PyDict *dict = new PyDict();
    PyDict *dict1 = new PyDict();
    // TO-DO: populate with completed achievements.
    // Known values:
    std::vector<std::string> achievements = {
        "client_activateAutopilot", "client_lockHostileNPC", "client_orbitHostileNPC",
        "client_spinCamera", "client_chatInLocal", "client_loadBlueprint", "client_openCorpFinder",
        "client_activateSalvager", "client_lockAsteroid", "client_openMap", "client_orbitAsteroid",
        "client_mouseZoom_out", "client_lootItems", "client_launchProbes", "client_moveItemFromCargoToHangar",
        "client_bookmarkWormhole", "client_setDestination", "client_activateShip", "client_lookAtObject",
        "client_mouseZoom_in", "client_activateGun", "client_unfitItem", "client_jumpToNextSystemInRoute",
        "client_lookAtOwnShip", "client_reachPerfectScanResult", "client_approach", "client_doubleClickInSpace"
    };
    bool completed = false;
    for(auto key : achievements)
    {
        dict1->SetItem(new PyString(key), new PyInt(completed ? 1 : 0));
    }
    PyDict *dict2 = new PyDict();
    // TO-DO: populate completed dict with achievementID and completed time.
    // Known possible values: 2, 9, 10, 12, 13, 15, 16, 17, 18, 19, 20, 23, 24,
    // 25, 26, 27, 28, 29, 30, 31, 32, 36, 38, 41, 43, 44, 45, 46, 47, 48, 49,
    // 50, 52, 53, 55, 58, 59, 60, 61, 62, 63, 64, 66, 70, 71, 72, 74, 75, 76, 77, 81
    //completedDict->SetItem(new PyInt(2), new PyLong(Win32TimeNow()));
    dict->SetItem(new PyString("eventDict"), dict1);
    dict->SetItem(new PyString("completedDict"), dict2);
    return dict;
}

PyResult AchievementTrackerMgrService::Handle_UpdateClientAchievmentsAndCounters(PyCallArgs &call)
{
    if(call.tuple->size() != 2)
    {
        codelog(CLIENT__ERROR, "%s: Update achievements tuple wrong size.", call.client->GetName());
        return NULL;
    }
    PyDict *dict1;
    PyDict *dict2;
    if(!pyIsAs(Dict, call.tuple->GetItem(0), dict1) || !pyIsAs(Dict, call.tuple->GetItem(1), dict2))
    {
        codelog(CLIENT__ERROR, "%s: Update achievements item not dict.", call.client->GetName());
        return NULL;
    }
    for(auto pair : dict1->items)
    {
        int key = pyAs(Int, pair.first)->value();
        uint64 time = 0;
        if(pyIs(Int, pair.second))
        {
            time = pyAs(Int, pair.second)->value();
        }
        if(pyIs(Long, pair.second))
        {
            time = pyAs(Long, pair.second)->value();
        }
        // TO-DO: save these values.
    }
    for(auto pair : dict2->items)
    {
        std::string key = pyAs(String, pair.first)->content();
        uint64 value = 0;
        if(pyIs(Int, pair.second))
        {
            value = pyAs(Int, pair.second)->value();
        }
        if(pyIs(Long, pair.second))
        {
            value = pyAs(Long, pair.second)->value();
        }
        // TO-DO: save these values.
    }
    return nullptr;
}