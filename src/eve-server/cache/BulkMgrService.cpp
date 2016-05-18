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

/*
cacheEosNpcToNpcStandings = 109998
cacheAutAffiliates = 109997
cacheAutCdkeyTypes = 109996
cacheEveWarnings = 109995
cacheTutCategories = 200006
cacheTutCriterias = 200003
cacheTutTutorials = 200001
cacheTutActions = 200009
cacheDungeonArchetypes = 300001
cacheDungeonDungeons = 300005
cacheDungeonEntityGroupTypes = 300004
cacheDungeonEventMessageTypes = 300017
cacheDungeonEventTypes = 300015
cacheDungeonSpawnpoints = 300012
cacheDungeonTriggerTypes = 300013
cacheInvContrabandTypes = 600008
cacheInvTypeMaterials = 600005
cacheInvTypeReactions = 600010
cacheInvWreckUsage = 600009
cacheInvMetaGroups = 600006
cacheInvMetaTypes = 600007
cacheDogmaAttributes = 800004
cacheDogmaEffects = 800005
cacheDogmaExpressionCategories = 800001
cacheDogmaExpressions = 800003
cacheDogmaOperands = 800002
cacheDogmaTypeAttributes = 800006
cacheDogmaTypeEffects = 800007
cacheDogmaUnits = 800009
cacheEveMessages = 1000001
cacheMapRegions = 1409999
cacheMapConstellations = 1409998
cacheMapSolarSystems = 1409997
cacheMapSolarSystemLoadRatios = 1409996
cacheLocationWormholeClasses = 1409994
cacheMapPlanets = 1409993
cacheMapSolarSystemJumpIDs = 1409992
cacheMapTypeBalls = 1400001
cacheMapWormholeClasses = 1400003
cacheMapCelestialDescriptions = 1400008
cacheMapNebulas = 1400016
cacheMapLocationWormholeClasses = 1400002
cacheMapPlanetsTable = 1400020
cacheMapMoonsTable = 1400021
cacheMapStargateTable = 1400022
cacheMapStarsTable = 1400023
cacheMapBeltsTable = 1400024
cacheMapCelestialStatistics = 1400025
cacheNpcCommandLocations = 1600009
cacheNpcCommands = 1600005
cacheNpcDirectorCommandParameters = 1600007
cacheNpcDirectorCommands = 1600006
cacheNpcLootTableFrequencies = 1600004
cacheNpcCommandParameters = 1600008
cacheNpcTypeGroupingClassSettings = 1600016
cacheNpcTypeGroupingClasses = 1600015
cacheNpcTypeGroupingTypes = 1600017
cacheNpcTypeGroupings = 1600014
cacheNpcTypeLoots = 1600001
cacheRamSkillInfo = 1809999
cacheRamActivities = 1800003
cacheRamAssemblyLineTypes = 1800006
cacheRamAssemblyLineTypesCategory = 1800004
cacheRamAssemblyLineTypesGroup = 1800005
cacheRamCompletedStatuses = 1800007
cacheRamInstallationTypes = 1800002
cacheRamTypeRequirements = 1800001
cacheShipInsurancePrices = 2000007
cacheShipTypes = 2000001
cacheStaOperations = 2209999
cacheStaServices = 2209998
cacheStaOperationServices = 2209997
cacheStaSIDAssemblyLineQuantity = 2209996
cacheStaSIDAssemblyLineType = 2209995
cacheStaSIDAssemblyLineTypeQuantity = 2209994
cacheStaSIDOfficeSlots = 2209993
cacheOutpostReprocessingEfficiency_Get = 2209992
cacheStaStationImprovementTypes = 2209990
cacheStaStationUpgradeTypes = 2209989
cacheStaStations = 2209988
cacheStaStationsStatic = 2209987
cacheMktAveragePriceHistorySelect = 2409996
cacheMktOrderStates = 2409999
cacheMktNpcMarketData = 2400001
cacheCrpRoles = 2809999
cacheCrpActivities = 2809998
cacheCrpNpcDivisions = 2809997
cacheCrpCorporations = 2809996
cacheCrpNpcMembers = 2809994
cacheCrpPlayerCorporationIDs = 2809993
cacheCrpTickerNamesStatic = 2809992
cacheNpcSupplyDemand = 2800001
cacheCrpRegistryGroups = 2800002
cacheCrpRegistryTypes = 2800003
cacheCrpNpcCorporations = 2800006
cacheAgentAgents = 3009999
cacheAgentCorporationActivities = 3009998
cacheAgentCorporations = 3009997
cacheAgentEpicMissionMessages = 3009996
cacheAgentEpicMissionsBranching = 3009995
cacheAgentEpicMissionsNonEnd = 3009994
cacheAgtContentAgentInteractionMissions = 3009992
cacheAgtContentFlowControl = 3009991
cacheAgtContentTalkToAgentMissions = 3009990
cacheAgtPrices = 3009989
cacheAgtContentTemplates = 3000001
cacheAgentMissionsKill = 3000006
cacheAgtStorylineMissions = 3000008
cacheAgtContentCourierMissions = 3000003
cacheAgtContentExchangeOffers = 3000005
cacheAgentEpicArcConnections = 3000013
cacheAgentEpicArcMissions = 3000015
cacheAgentEpicArcs = 3000012
cacheAgtContentMissionExtraStandings = 3000020
cacheAgtContentMissionTutorials = 3000018
cacheAgtContentMissionLocationFilters = 3000021
cacheAgtOfferDetails = 3000004
cacheAgtOfferTableContents = 3000010
cacheChrSchools = 3209997
cacheChrRaces = 3200001
cacheChrBloodlines = 3200002
cacheChrAncestries = 3200003
cacheChrCareers = 3200004
cacheChrSpecialities = 3200005
cacheChrBloodlineNames = 3200010
cacheChrAttributes = 3200014
cacheChrFactions = 3200015
cacheChrDefaultOverviews = 3200011
cacheChrDefaultOverviewGroups = 3200012
cacheChrNpcCharacters = 3200016
cacheFacWarCombatZoneSystems = 4500006
cacheFacWarCombatZones = 4500005
cacheRedeemWhitelist = 6300001
cacheActBillTypes = 6400004
cachePetCategories = 8109999
cachePetQueues = 8109998
cachePetCategoriesVisible = 8109997
cachePetGMQueueOrder = 8109996
cachePetOsTypes = 8109995
cacheCertificates = 5100001
cacheCertificateRelationships = 5100004
cachePlanetBlacklist = 7309999
cachePlanetSchematics = 7300004
cachePlanetSchematicsTypeMap = 7300005
cachePlanetSchematicsPinMap = 7300003
cacheMapDistrictCelestials = 100309999
cacheMapDistricts = 100300014
cacheMapBattlefields = 100300015
cacheMapLevels = 100300020
cacheMapOutposts = 100300022
cacheMapLandmarks = 100300023
cacheEspCorporations = 1
cacheEspAlliances = 2
cacheEspSolarSystems = 3
cacheSolarSystemObjects = 4
cacheCargoContainers = 5
cachePriceHistory = 6
cacheTutorialVersions = 7
cacheSolarSystemOffices = 8
 */
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
