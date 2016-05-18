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
    Author:        Zhur
*/

#include "eve-server.h"

#include "NetService.h"
#include "PyServiceCD.h"
#include "cache/ObjCacheService.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(NetService)

NetService::NetService()
: PyService("machoNet", new Dispatcher(this))
{

    PyCallable_REG_CALL(NetService, GetInitVals)
    PyCallable_REG_CALL(NetService, GetTime)
}

NetService::~NetService() {
}

PyResult NetService::Handle_GetInitVals(PyCallArgs &call) {
    PyString* str = new PyString( "machoNet.serviceInfo" );

    if (!PyServiceMgr::cache_service->IsCacheLoaded(str))
    {
        PyDict *dict = new PyDict;
        /* ServiceCallGPCS.py:197
        where = self.machoNet.serviceInfo[service]
        if where:
            for (k, v,) in self.machoNet.serviceInfo.iteritems():
                if ((k != service) and (v and (v.startswith(where) or where.startswith(v)))):
                    nodeID = self.services.get(k, None)
                    break
         */
        dict->SetItemString("eventLog", new PyNone());
        dict->SetItemString("LSC", new PyString("location"));
        dict->SetItemString("entity", new PyNone());
        dict->SetItemString("DB2", new PyNone());
        dict->SetItemString("conflictManager", new PyNone());
        dict->SetItemString("hackingMgr", new PyString("location"));
        dict->SetItemString("charMgr", new PyString("character"));
        dict->SetItemString("cache", new PyNone());
        dict->SetItemString("beyonce", new PyNone());
        dict->SetItemString("dogmaStaticMgr", new PyNone());
        dict->SetItemString("voiceAdminMgr", new PyNone());
        dict->SetItemString("facWarMgr", new PyString("solarsystem2"));
        dict->SetItemString("corpStationMgr", new PyNone());
        dict->SetItemString("facilityManager", new PyString("corporation"));
        dict->SetItemString("ccpNotificationSingleton", new PyString("clustersingleton"));
        dict->SetItemString("paperDollServer", new PyString("character"));
        dict->SetItemString("shipSkinApplicationMgr", new PyNone());
        dict->SetItemString("corpRegistry", new PyNone());
        dict->SetItemString("brainMaker", new PyNone());
        dict->SetItemString("catmaStaticInfo", new PyString("character"));
        dict->SetItemString("corporationSvc", new PyString("character"));
        dict->SetItemString("industryManager", new PyString("solarsystem2"));
        dict->SetItemString("mailingListsMgr", new PyString("character"));
        dict->SetItemString("lootRights", new PyNone());
        dict->SetItemString("taleDistributionMgr", new PyNone());
        dict->SetItemString("communityService", new PyNone());
        dict->SetItemString("dustCorporation", new PyString("corporation"));
        dict->SetItemString("agentMgr", new PyNone());
        dict->SetItemString("scanMgr", new PyString("solarsystem"));
        dict->SetItemString("dbLog", new PyNone());
        dict->SetItemString("contactSvc", new PyNone());
        dict->SetItemString("sovereigntyBillMgr", new PyString("clustersingleton_3"));
        dict->SetItemString("voiceMgr", new PyString("character"));
        dict->SetItemString("structureAssetSafety", new PyNone());
        dict->SetItemString("facWarZoneMgr", new PyNone());
        dict->SetItemString("voucher", new PyString("character"));
        dict->SetItemString("machoNet", new PyNone());
        dict->SetItemString("blueprintManager", new PyString("character"));
        dict->SetItemString("dungeon", new PyNone());
        dict->SetItemString("communityClient", new PyNone());
        dict->SetItemString("WSGIService", new PyNone());
        dict->SetItemString("settingsSvc", new PyNone());
        dict->SetItemString("battleInitialization", new PyNone());
        dict->SetItemString("loginManagementSvc", new PyNone());
        dict->SetItemString("jumpbeaconsvc", new PyNone());
        dict->SetItemString("SupportDataService", new PyNone());
        dict->SetItemString("slash", new PyString("location"));
        dict->SetItemString("taleMgr", new PyNone());
        dict->SetItemString("industryActivityManager", new PyString("clustersingleton_3"));
        dict->SetItemString("billingMgr", new PyNone());
        dict->SetItemString("posMgr", new PyNone());
        dict->SetItemString("onlineStatus", new PyString("character"));
        dict->SetItemString("MMOS", new PyNone());
        dict->SetItemString("xmlrpcService", new PyNone());
        dict->SetItemString("StructureProfiles", new PyString("character"));
        dict->SetItemString("jumpCloneSvc", new PyNone());
        dict->SetItemString("shipKillCounter", new PyString("solarsystem2"));
        dict->SetItemString("debug", new PyNone());
        dict->SetItemString("EspNotificationService", new PyNone());
        dict->SetItemString("keeper", new PyString("solarsystem"));
        dict->SetItemString("tetheringManager", new PyNone());
        dict->SetItemString("effectCompiler", new PyNone());
        dict->SetItemString("crimewatch", new PyNone());
        dict->SetItemString("crestapiService", new PyNone());
        dict->SetItemString("petitioner", new PyString("character"));
        dict->SetItemString("districtCommander", new PyNone());
        dict->SetItemString("vault", new PyNone());
        dict->SetItemString("calendarMgr", new PyString("character"));
        dict->SetItemString("notificationMgr", new PyString("character"));
        dict->SetItemString("invbroker", new PyNone());
        dict->SetItemString("lien", new PyNone());
        dict->SetItemString("devIndexManager", new PyString("solarsystem2"));
        dict->SetItemString("dogmaIM", new PyNone());
        dict->SetItemString("missionTrackerMgr", new PyString("solarsystem2"));
        dict->SetItemString("xmlrpcClient", new PyNone());
        dict->SetItemString("dustSkill", new PyString("character"));
        dict->SetItemString("npcSvc", new PyNone());
        dict->SetItemString("market", new PyNone());
        dict->SetItemString("processHealth", new PyNone());
        dict->SetItemString("allianceRegistry", new PyNone());
        dict->SetItemString("ProjectDiscovery", new PyString("character"));
        dict->SetItemString("bookmark", new PyString("location"));
        dict->SetItemString("mailNodesMgr", new PyNone());
        dict->SetItemString("station", new PyString("station"));
        dict->SetItemString("insuranceSvc", new PyNone());
        dict->SetItemString("LPSvc", new PyString("character"));
        dict->SetItemString("BSD", new PyNone());
        dict->SetItemString("jumpTimers", new PyNone());
        dict->SetItemString("lookupSvc", new PyString("character"));
        dict->SetItemString("standing2", new PyNone());
        dict->SetItemString("sovMgr", new PyString("solarsystem2"));
        dict->SetItemString("structureServices", new PyString("character"));
        dict->SetItemString("bounty", new PyNone());
        dict->SetItemString("dogma", new PyNone());
        dict->SetItemString("districtManager", new PyNone());
        dict->SetItemString("crestServerConnection", new PyNone());
        dict->SetItemString("clones", new PyNone());
        dict->SetItemString("gagger", new PyNone());
        dict->SetItemString("account", new PyString("character"));
        dict->SetItemString("pathfinderService", new PyNone());
        dict->SetItemString("userSvc", new PyNone());
        dict->SetItemString("charUnboundMgr", new PyNone());
        dict->SetItemString("SP", new PyNone());
        dict->SetItemString("http2", new PyNone());
        dict->SetItemString("dustSync", new PyNone());
        dict->SetItemString("watchdog", new PyNone());
        dict->SetItemString("securityMonitor", new PyNone());
        dict->SetItemString("bountyMgr", new PyString("clustersingleton_2"));
        dict->SetItemString("dungeonExplorationMgr", new PyNone());
        dict->SetItemString("lootSvc", new PyNone());
        dict->SetItemString("damageTracker", new PyNone());
        dict->SetItemString("ship", new PyNone());
        dict->SetItemString("skillMgr2", new PyString("character"));
        dict->SetItemString("aggressionMgr", new PyNone());
        dict->SetItemString("sessionMgr", new PyNone());
        dict->SetItemString("ShieldService", new PyNone());
        dict->SetItemString("achievementTrackerMgr", new PyString("character"));
        dict->SetItemString("infoGatheringMgr", new PyNone());
        dict->SetItemString("bountyClaimMgr", new PyNone());
        dict->SetItemString("oauth2Service", new PyNone());
        dict->SetItemString("authentication", new PyNone());
        dict->SetItemString("stationSvc", new PyNone());
        dict->SetItemString("entosisEventService", new PyNone());
        dict->SetItemString("config", new PyString("character"));
        dict->SetItemString("billMgr", new PyNone());
        dict->SetItemString("map", new PyString("character"));
        dict->SetItemString("emailreader", new PyNone());
        dict->SetItemString("http", new PyNone());
        dict->SetItemString("dataconfig", new PyNone());
        dict->SetItemString("objectCaching", new PyNone());
        dict->SetItemString("dustEveChat", new PyNone());
        dict->SetItemString("wormholeMgr", new PyString("location"));
        dict->SetItemString("alert", new PyNone());
        dict->SetItemString("director", new PyNone());
        dict->SetItemString("tetheringRepair", new PyNone());
        dict->SetItemString("districtLocation", new PyString("solarsystem2"));
        dict->SetItemString("shipStanceMgr", new PyString("location"));
        dict->SetItemString("fleetMgr", new PyString("location"));
        dict->SetItemString("bulkMgr", new PyString("bulk"));
        dict->SetItemString("crestQueueService", new PyNone());
        dict->SetItemString("mailMgr", new PyString("character"));
        dict->SetItemString("districtSatelliteMgr", new PyNone());
        dict->SetItemString("corpmgr", new PyNone());
        dict->SetItemString("counter", new PyNone());
        dict->SetItemString("i2", new PyNone());
        dict->SetItemString("warRegistry", new PyNone());

        //register it
        PyServiceMgr::cache_service->GiveCache(str, (PyRep **) & dict);
    }

    PyRep* serverinfo = PyServiceMgr::cache_service->GetCacheHint(str);
    PyDecRef( str );

    PyDict* initvals = new PyDict();

    PyTuple* result = new PyTuple( 2 );
    result->SetItem( 0, serverinfo );
    result->SetItem( 1, initvals );
    return result;
}

PyResult NetService::Handle_GetTime(PyCallArgs &call) {
    return(new PyLong(Win32TimeNow()));
}
