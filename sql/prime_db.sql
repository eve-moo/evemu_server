/*
 * This file is here to deal with some tables needing to contain both
 * static and dynamic information. The solution for ease of release is
 * to keep all the static info in a seperate copy of the tables such
 * that it can be loaded into the non-static table.
 */

/*
 * Truncate all the dynamic tables
 */
TRUNCATE TABLE srvAlliance_ShortNames;
TRUNCATE TABLE srvBillsPayable;
TRUNCATE TABLE srvBillsReceivable;
TRUNCATE TABLE srvBookmarks;
TRUNCATE TABLE srvCacheLocations;
TRUNCATE TABLE srvCacheOwners;
TRUNCATE TABLE srvChannelChars;
TRUNCATE TABLE srvChannelMods;
TRUNCATE TABLE srvCharacter;
TRUNCATE TABLE srvChrApplications;
TRUNCATE TABLE srvChrEmployment;
TRUNCATE TABLE srvChrMissionState;
TRUNCATE TABLE srvChrNotes;
TRUNCATE TABLE srvChrNPCStandings;
TRUNCATE TABLE srvChrOffers;
TRUNCATE TABLE srvChrOwnerNote;
TRUNCATE TABLE srvChrSkillQueue;
TRUNCATE TABLE srvChrStandings;
TRUNCATE TABLE srvCorporation;
TRUNCATE TABLE srvCourierMissions;
TRUNCATE TABLE srvCrpCharShares;
TRUNCATE TABLE srvCrpOffices;
TRUNCATE TABLE srvDroneState;
TRUNCATE TABLE srvEntity;
TRUNCATE TABLE srvEntity_attributes;
TRUNCATE TABLE srvEveMail;
TRUNCATE TABLE srvEveMailDetails;
TRUNCATE TABLE srvInvBlueprints;
TRUNCATE TABLE srvMarket_history_old;
TRUNCATE TABLE srvMarket_journal;
TRUNCATE TABLE srvMarket_orders;
TRUNCATE TABLE srvMarket_transactions;
TRUNCATE TABLE srvRamAssemblyLineStationCostLogs;
TRUNCATE TABLE srvRamJobs;
TRUNCATE TABLE srvRentalInfo;
TRUNCATE TABLE srvStatus;

/*
 * Insert static characters (agents)
 * TODO: Develop a query which will populate srvCharacter using agtAgents and
 * random value generation)
 */
INSERT INTO srvCharacter
 SELECT
  characterID, 
  0 AS accountID, 
  '' AS title, 
  '' AS description, 
  0 AS bounty, 
  0 AS balance,
  0 AS aurBalance, 
  0 AS securityRating, 
  '' AS petitionMessage, 
  0 AS logonMinutes, 
  0 AS skillPoints, 
  0 AS skillQueueEndTime,
  corporationID, 
  0 AS corpRole, 
  0 AS rolesAtAll, 
  0 AS rolesAtBase, 
  0 AS rolesAtHQ, 
  0 AS rolesAtOther,
  0 AS corporationDateTime, 
  0 AS startDateTime, 
  0 AS createDateTime,
  ancestryID, 
  careerID, 
  schoolID, 
  careerSpecialityID, 
  gender,
  stationID, 
  solarSystemID, 
  constellationID, 
  regionID,
  0 AS online, 
  2 AS freeRespecs, 
  0 AS lastRespecDateTime, 
  0 AS nextRespecDateTime, 
  0 AS deletePrepareDateTime, 
  0 AS shipID,
  0 AS freeSkillPoints
 FROM blkCharacterStatic
LEFT JOIN mapDenormalize ON mapDenormalize.itemID = blkCharacterStatic.stationID;

/*
 * Copy over the static entities:
 * Static record of EVE System
 */
INSERT INTO srvEntity (itemID, itemName, singleton, quantity)
 VALUES (1, 'EVE System', 1, 1);
/*
 * Insert solar systems
 */
INSERT INTO srvEntity (itemID, itemName, typeID, ownerID, singleton, quantity, x, y, z)
 SELECT solarSystemID, solarSystemName, 5, 1, 1, 1, x, y, z
 FROM mapSolarSystems;
/*
 * Insert stations
 */
INSERT INTO srvEntity (itemID, itemName, typeID, ownerID, locationID, singleton, quantity, x, y, z)
 SELECT stationID, stationName, stationTypeID, corporationID, solarSystemID, 1, 1, x, y, z
 FROM staStations;
/*
 * Insert characters
 */
INSERT INTO srvEntity (itemID, itemName, typeID, ownerID, locationID, singleton, quantity)
 SELECT characterID, characterName, typeID, 1, stationID, 1, 1
  FROM blkCharacterStatic;
/*
 * Insert factions
 */
INSERT INTO srvEntity (itemID, itemName, typeID)
 SELECT factionID, factionName, 30 AS typeID
 FROM chrFactions;
/*
 * Insert corporations
 */
INSERT INTO srvEntity (itemID, itemName, typeID)
 SELECT corporationID, corporationName, 2 AS typeID
 FROM blkCorporationStatic;
/*
 * Set the auto-increment lower bound for critical tables
 */
ALTER TABLE srvEntity AUTO_INCREMENT=140000000;

/*
 * Copy over the static corporation info
 */
INSERT INTO srvCorporation
 SELECT * FROM blkCorporationStatic;
/*
 * Set the auto-increment lower bound
 */
ALTER TABLE srvCorporation AUTO_INCREMENT=2000001;

/*
 * Set the auto-increment lower bound for other critical tables
 */
ALTER TABLE srvAccount AUTO_INCREMENT=1;
ALTER TABLE srvAccountApi AUTO_INCREMENT=1000000;
