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
    Author:     caytchen, Zhur
*/

#include "eve-server.h"

#include "EVEServerConfig.h"
#include "PyServiceCD.h"
#include "cache/ObjCacheService.h"
#include "character/CharUnboundMgrService.h"
#include "imageserver/ImageServer.h"
#include "PyServiceMgr.h"
#include "character/PhotoUploadService.h"

#include "chr/ChrBloodline.h"
#include "SkillMgrService.h"

PyCallable_Make_InnerDispatcher(CharUnboundMgrService)

CharUnboundMgrService::CharUnboundMgrService()
: PyService("charUnboundMgr", new Dispatcher(this))
{
    CharacterDB::Init();

    PyCallable_REG_CALL(CharUnboundMgrService, SelectCharacterID)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharacterToSelect)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharactersToSelect)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharacterInfo)
    PyCallable_REG_CALL(CharUnboundMgrService, IsUserReceivingCharacter)
    PyCallable_REG_CALL(CharUnboundMgrService, DeleteCharacter)
    PyCallable_REG_CALL(CharUnboundMgrService, PrepareCharacterForDelete)
    PyCallable_REG_CALL(CharUnboundMgrService, CancelCharacterDeletePrepare)
    PyCallable_REG_CALL(CharUnboundMgrService, ValidateNameEx)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharCreationInfo)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharNewExtraCreationInfo)
    PyCallable_REG_CALL(CharUnboundMgrService, CreateCharacterWithDoll)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCharacterSelectionData)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCohortsForUser)
    PyCallable_REG_CALL(CharUnboundMgrService, GetCohortsForCharacter)
}

CharUnboundMgrService::~CharUnboundMgrService() {
}

void CharUnboundMgrService::GetCharacterData(uint32 characterID, std::map<std::string, uint32> &characterDataMap)
{
    CharacterDB::GetCharacterData(characterID, characterDataMap);
}

PyResult CharUnboundMgrService::Handle_IsUserReceivingCharacter(PyCallArgs &call) {
    return new PyBool(false);
}

PyResult CharUnboundMgrService::Handle_ValidateNameEx(PyCallArgs &call)
{
    Call_ValidateNameEx arg;
    if (!arg.Decode(&call.tuple))
    {
        codelog(CLIENT__ERROR, "Failed to decode args for ValidateNameEx call");
        return NULL;
    }

    return new PyBool(CharacterDB::ValidateCharName(arg.name.c_str()));
}

PyResult CharUnboundMgrService::Handle_SelectCharacterID(PyCallArgs &call) {
    CallSelectCharacterID arg;
    if (!arg.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Failed to decode args for SelectCharacterID call");
        return NULL;
    }

    call.client->SelectCharacter(arg.charID);
    return NULL;
}

PyResult CharUnboundMgrService::Handle_GetCharactersToSelect(PyCallArgs &call)
{
    return (CharacterDB::GetCharacterList(call.client->GetAccountID()));
}

PyResult CharUnboundMgrService::Handle_GetCharacterToSelect(PyCallArgs &call) {
    Call_SingleIntegerArg args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments");
        return NULL;
    }

    PyRep *result = CharacterDB::GetCharSelectInfo(args.arg);
    if(result == NULL) {
        _log(CLIENT__ERROR, "Failed to load character %d", args.arg);
        return NULL;
    }

    return result;
}

PyResult CharUnboundMgrService::Handle_DeleteCharacter(PyCallArgs &call) {
    Call_SingleIntegerArg args;
    if (!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments for DeleteCharacter call");
        return NULL;
    }

    return CharacterDB::DeleteCharacter(call.client->GetAccountID(), args.arg);
}

PyResult CharUnboundMgrService::Handle_PrepareCharacterForDelete(PyCallArgs &call) {
    Call_SingleIntegerArg args;
    if (!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments for PrepareCharacterForDelete call");
        return NULL;
    }

    return new PyLong((int64) CharacterDB::PrepareCharacterForDelete(call.client->GetAccountID(), args.arg));
}

PyResult CharUnboundMgrService::Handle_CancelCharacterDeletePrepare(PyCallArgs &call) {
    Call_SingleIntegerArg args;
    if (!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments for CancelCharacterDeletePrepare call");
        return NULL;
    }

    CharacterDB::CancelCharacterDeletePrepare(call.client->GetAccountID(), args.arg);

    // the client doesn't care what we return here
    return NULL;
}

PyResult CharUnboundMgrService::Handle_GetCharacterInfo(PyCallArgs &call) {
	_log(CLIENT__MESSAGE, "Called GetCharacterInfo stub");
    return NULL;
}

PyResult CharUnboundMgrService::Handle_GetCharCreationInfo(PyCallArgs &call) {
    PyDict *result = new PyDict();

    //send all the cache hints needed for char creation.
    PyServiceMgr::cache_service->InsertCacheHints(
        ObjCacheService::hCharCreateCachables,
        result);
    _log(CLIENT__MESSAGE, "Sending char creation info reply");

    return result;
}

PyResult CharUnboundMgrService::Handle_GetCharNewExtraCreationInfo(PyCallArgs &call) {
    PyDict *result = new PyDict();
    PyServiceMgr::cache_service->InsertCacheHints(ObjCacheService::hCharCreateNewExtraCachables, result);
    _log(CLIENT__MESSAGE, "Sending char new extra creation info reply");
    return result;
}

PyResult CharUnboundMgrService::Handle_CreateCharacterWithDoll(PyCallArgs &call) {
    CallCreateCharacterWithDoll arg;

    if (!arg.Decode(call.tuple))
    {
        codelog(CLIENT__ERROR, "Failed to decode args for CreateCharacterWithDoll call");
        return NULL;
    }

    _log(CLIENT__MESSAGE, "CreateCharacterWithDoll called for '%s'", arg.name.c_str());
    _log(CLIENT__MESSAGE, "  bloodlineID=%u genderID=%u ancestryID=%u",
            arg.bloodlineID, arg.genderID, arg.ancestryID);

    // obtain character type
    ItemFactory::SetUsingClient(call.client);
    ChrBloodlineRef char_type;
    if (!ChrBloodline::getBloodline(arg.bloodlineID, char_type))
    {
        return nullptr;
    }
    uint32 charTypeID;
    InventoryDB::GetCharacterTypeByBloodline(char_type->bloodlineID, charTypeID);

    // we need to fill these to successfully create character item
    ItemData idata;
    CharacterData cdata;
    CharacterAppearance capp;
    CorpMemberInfo corpData;

    idata.typeID = charTypeID;
    idata.name = arg.name;
    idata.ownerID = 1; // EVE System
    idata.quantity = 1;
    idata.singleton = true;
    idata.flag = flagPilot;
    idata.attributes.insert(std::pair<uint32, EvilNumber>(AttrMass, 0));
    idata.attributes.insert(std::pair<uint32, EvilNumber>(AttrCapacity, 1));
    idata.attributes.insert(std::pair<uint32, EvilNumber>(AttrVolume, 1));
    idata.attributes.insert(std::pair<uint32, EvilNumber>(AttrRadius, 0));

    cdata.accountID = call.client->GetAccountID();
    cdata.gender = arg.genderID != 0;
    cdata.ancestryID = arg.ancestryID;
    cdata.schoolID = arg.schoolID;
    cdata.bloodline = ChrBloodline::getBloodline(arg.bloodlineID);

    //Set the character's career based on the school they picked.
    if (CharacterDB::GetCareerBySchool(cdata.schoolID, cdata.careerID))
    {
        // Right now we don't know what causes the specialization switch, so just make both values the same
        cdata.careerSpecialityID = cdata.careerID;
    } else {
        codelog(SERVICE__WARNING, "Could not find default School ID %u. Using Caldari Military.", cdata.schoolID);
        cdata.careerID = 11;
        cdata.careerSpecialityID = 11;
    }

    corpData.corpRole = 0;
    corpData.rolesAtAll = 0;
    corpData.rolesAtBase = 0;
    corpData.rolesAtHQ = 0;
    corpData.rolesAtOther = 0;

    // Setting character's starting position, and getting it's location...
    if (!CharacterDB::GetLocationCorporationByCareer(cdata)) {
        codelog(CLIENT__ERROR, "Failed to load char create details. Bloodline %u, ancestry %u.", char_type->bloodlineID, cdata.ancestryID);
        return NULL;
    }

    idata.locationID = cdata.stationID; // Just so our starting items end up in the same place.

    // Change starting corperation based on value in XML file.
    if( EVEServerConfig::character.startCorporation )
    {
        if (CharacterDB::DoesCorporationExist(EVEServerConfig::character.startCorporation))
        {
            cdata.corporationID = EVEServerConfig::character.startCorporation;
        } else {
            codelog(SERVICE__WARNING, "Could not find default Corporation ID %u. Using Career Defaults instead.", EVEServerConfig::character.startCorporation);
        }
    }
    else
    {
        uint32 corporationID;
        if (CharacterDB::GetCorporationBySchool(cdata.schoolID, corporationID))
        {
            cdata.corporationID = corporationID;
        }
        else
        {
            codelog(SERVICE__ERROR, "Could not place character in default corporation for school.");
        }
    }

    // Added ability to set starting station in xml config by Pyrii
    if( EVEServerConfig::character.startStation )
    {
        if (!CharacterDB::GetLocationByStation(EVEServerConfig::character.startStation, cdata))
        {
            codelog(SERVICE__WARNING, "Could not find default station ID %u. Using Career Defaults instead.", EVEServerConfig::character.startStation);
        }
    }
    else
    {
        uint32 stationID;
        if (CharacterDB::GetCareerStationByCorporation(cdata.corporationID, stationID))
        {
            if (!CharacterDB::GetLocationByStation(stationID, cdata))
                codelog(SERVICE__WARNING, "Could not find default station ID %u.", stationID);
        }
        else
        {
            codelog(SERVICE__ERROR, "Could not place character in default station for school.");
        }
    }

    cdata.bounty = 0;
    cdata.balance = EVEServerConfig::character.startBalance;
    cdata.aurBalance = 0; // TODO Add aurBalance to the database
    cdata.securityRating = EVEServerConfig::character.startSecRating;
    cdata.logonMinutes = 0;
    cdata.title = "No Title";

    cdata.startDateTime = Win32TimeNow();
    cdata.createDateTime = cdata.startDateTime;
    cdata.corporationDateTime = cdata.startDateTime;

    //load skills
    std::map<uint32, uint32> startingSkills;
    if(!CharacterDB::GetSkillsByRace(char_type->raceID, startingSkills))
    {
        codelog(CLIENT__ERROR, "Failed to load char create skills. RaceID=%u", char_type->raceID);
        return NULL;
    }

    //now we have all the data we need, stick it in the DB
    //create char item
    CharacterRef char_item = ItemFactory::SpawnCharacter(idata, cdata, corpData);
    if( !char_item )
    {
        //a return to the client of 0 seems to be the only means of marking failure
        codelog(CLIENT__ERROR, "Failed to create character '%s'", idata.name.c_str());
        return NULL;
    }

    // build character appearance (body, clothes, accessories)
    capp.Build(char_item->itemID(), arg.avatarInfo);

    // add attribute bonuses
    char_item->setAttribute(AttrIntelligence, 20);
    char_item->setAttribute(AttrCharisma, 19);
    char_item->setAttribute(AttrPerception, 20);
    char_item->setAttribute(AttrMemory, 20);
    char_item->setAttribute(AttrWillpower, 20);

    // register name
    CharacterDB::add_name_validation_set(char_item->itemName().c_str(), char_item->itemID());

    //spawn all the skills
    uint32 charID = char_item->itemID();
    uint64 time = Win32TimeNow();
    uint32 skillLevel;
    std::string values;
    uint32 method = skillEventCharCreation;
    for(auto cur : startingSkills)
    {
        uint32 skillType = cur.first;
        ItemData skillItem(skillType, charID, charID, flagSkill);
        SkillRef skill = ItemFactory::SpawnSkill(skillItem);
        if(!skill)
        {
            _log(CLIENT__ERROR, "Failed to add skill %u to char %s (%u) during char create.", cur.first, char_item->itemName().c_str(), charID);
            continue;
        }

        skillLevel = cur.second;
        double skillPoints = skill->GetSPForLevel(cur.second);
        skill->setAttribute(AttrSkillLevel, skillLevel);
        skill->setAttribute(AttrSkillPoints, skillPoints);
        skill->SaveAttributes();
        // Construct insert group.
        if(values.length() > 0)
        {
            values += ", ";
        }
        char buf[1024];
        snprintf(buf, 1024, "(%u, %u, %u, %f, %u, %" PRId64 ")",
                     charID, skillType, skillLevel, skillPoints, method, time);
        values += buf;
    }
    if(!values.empty())
    {
        // Skill to history
        DBerror err;
        if(!DBcore::RunQuery(err,
                             "INSERT INTO srvChrSkillHistory "
                             "(characterID, typeID, level, points, eventID, eventTime)"
                             " VALUES %s", values.c_str()))
        {
            _log(DATABASE__ERROR, "Failed to save skill creation history for character %u: %s", charID, err.c_str());
        }
    }
    // Start with 
    // All - Civilian miner
    // caldari - Ibis - Civilian Gatling Railgun
    // Minmitar - Reaper - Civilian Gatling Autocannon
    // Amarr - Ibis - Civilian Gatling Pulse Laser.
    // Galentte - Velator - Civilian Light Electron Blaster
    //now set up some initial inventory:
    InventoryItemRef initInvItem;

    // add "Damage Control I"
    ItemData itemDamageControl( 2046, char_item->itemID(), char_item->locationID(), flagHangar, 1 );
    initInvItem = ItemFactory::SpawnItem(itemDamageControl);

    if( !initInvItem )
    {
        codelog(CLIENT__ERROR, "%s: Failed to spawn a starting item", char_item->itemName().c_str());
    }

    // add 1 unit of "Tritanium"
    ItemData itemTritanium( 34, char_item->itemID(), char_item->locationID(), flagHangar, 1 );
    initInvItem = ItemFactory::SpawnItem(itemTritanium);

    // add 1 unit of "Clone Grade Alpha"
    ItemData itemCloneAlpha( 164, char_item->itemID(), char_item->locationID(), flagClone, 1 );
    itemCloneAlpha.customInfo="active";
    initInvItem = ItemFactory::SpawnItem(itemCloneAlpha);

    if( !initInvItem )
    {
        codelog(CLIENT__ERROR, "%s: Failed to spawn a starting item", char_item->itemName().c_str());
    }

    // give the player its ship.
    std::string ship_name = char_item->itemName() + "'s Ship";

    ItemData shipItem( char_type->shipTypeID, char_item->itemID(), char_item->locationID(), flagHangar, ship_name.c_str() );
    ShipRef ship_item = ItemFactory::SpawnShip(shipItem);

    char_item->SetActiveShip( ship_item->itemID() );
    char_item->SaveFullCharacter();
	ship_item->SaveItem();

    if( !ship_item )
    {
        codelog(CLIENT__ERROR, "%s: Failed to spawn a starting item", char_item->itemName().c_str());
    }
    else
    {
        //welcome on board your starting ship
        //char_item->MoveInto( *ship_item, flagPilot, false );
    }
    _log( CLIENT__MESSAGE, "Sending char create ID %u as reply", char_item->itemID() );

    // we need to report the charID to the ImageServer so it can correctly assign a previously received image
    PhotoUploadService::reportNewCharacter(call.client->GetAccountID(), char_item->itemID());

    // Release the item factory now that the character is finished being accessed:
    ItemFactory::UnsetUsingClient();

    return new PyInt( char_item->itemID() );
}

PyResult CharUnboundMgrService::Handle_GetCharacterSelectionData(PyCallArgs &call)
{
    uint32 accountID = call.client->GetAccountID();

    // items are - userDetails, trainingDetails, characterDetails
    PyTuple *rtn = new PyTuple(3);

    uint64 now = Win32TimeNow();
    // userDetails
    DBQueryResult res;
    if (!DBcore::RunQuery(res, "SELECT\n"
            "    accountName AS userName,\n"
            "    3 AS characterSlots,\n"
            "    CAST(3 AS CHAR) AS maxCharacterSlots,\n"
            "    %lu AS subscriptionEndTime,\n"
            "    %lu AS creationDate\n"
            "FROM `srvAccount`\n"
            "WHERE accountID = %u", (now + Win32Time_Year), (now - Win32Time_Month), accountID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }
    rtn->SetItem(0, DBResultToCRowset(res));

    // trainingEnds
    // T0-DO: Create table for multi character training times.
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("trainingEnds", DBTYPE::DBTYPE_FILETIME);
    CRowSet *rowset = new CRowSet( &header );
    rtn->SetItem(1, rowset);

    res.Reset();
    if (!DBcore::RunQuery(res,"SELECT \n"
            "    characterID,\n"
            "    0 AS logoffDate,\n"
            "    skillPoints,\n"
            "    0 AS paperdollState,\n"
            "    srvEntity.itemName AS characterName,\n"
            "    srvEntity.typeID,\n"
            "    gender,\n"
            "    bloodlineID,\n"
            "    NULLIF(deletePrepareDateTime, 0) AS deletePrepareDateTime,\n"
            "    srvCharacter.balance,\n"
            "    0 AS balanceChange,\n"            // 0.0 never NULL
            "    corporationID,\n"
            "    NULLIF(allianceID, 0) AS allianceID,\n"
            "    0 AS unreadMailCount,\n"          // 0 never NULL
            "    0 AS unprocessedNotifications,\n" // 0 never NULL
            "    ship.typeID AS shipTypeID,\n"
            "    solarSystemID,\n"
            "    srvCharacter.stationID,\n"                                    // NULL if in space
            "    security AS locationSecurity,\n"
            "    0 AS petitionMessage,\n"          // Bool never NULL
            "    0 AS finishedSkills,\n"           // 0 never NULL
            "    0 AS skillsInQueue,\n"            // 0 never NULL
            "    NULLIF(0, 0) AS skillTypeID,\n"           // Can be NULL, Null if no skill in training
            "    NULLIF(0, 0) AS toLevel,\n"               // Can be NULL, ^
            "    NULLIF(0, 0) AS trainingStartTime,\n"     // Can be NULL, ^
            "    NULLIF(0, 0) AS trainingEndTime,\n"       // Can be NULL, ^
            "    NULLIF(skillQueueEndTime, 0) AS queueEndTime,\n"
            "    NULLIF(0, 0) AS finishSP,\n"              // Can be NULL, ^
            "    NULLIF(0, 0) AS trainedSP,\n"             // Can be NULL, ^
            "    NULLIF(0, 0) AS fromSP\n"                 // Can be NULL, ^
            "FROM `srvCharacter`\n"
            "    LEFT JOIN srvEntity ON characterID = itemID\n"
            "    LEFT JOIN chrAncestries USING(ancestryID)\n"
            "    LEFT JOIN srvCorporation USING(corporationID)\n"
            "    LEFT JOIN srvEntity AS ship ON ship.itemID = shipID\n"
            "    LEFT JOIN mapSolarSystems USING(solarSystemID)\n"
            "WHERE accountID = %u", accountID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }
    // TO-DO: This should return as a CRowset but for some reason it's causing client exceptions.
    //rtn->SetItem(2, DBResultToCRowset(res));
    rtn->SetItem(2, DBResultToTupleKeyVal(res));
    return rtn;
}

/*
Tuple: 3 elements
  [ 0] Tuple: 1 elements
  [ 0]   [ 0] Object:
  [ 0]   [ 0]   Type: String: 'utillib.KeyVal'
  [ 0]   [ 0]   Args: Dictionary: 5 entries
  [ 0]   [ 0]   Args:   [ 0] Key: String: 'maxCharacterSlots'
  [ 0]   [ 0]   Args:   [ 0] Value: WString: '3'
  [ 0]   [ 0]   Args:   [ 1] Key: String: 'subscriptionEndTime'
  [ 0]   [ 0]   Args:   [ 1] Value: Integer field: 131370906800000000
  [ 0]   [ 0]   Args:   [ 2] Key: String: 'characterSlots'
  [ 0]   [ 0]   Args:   [ 2] Value: Integer field: 3
  [ 0]   [ 0]   Args:   [ 3] Key: String: 'creationDate'
  [ 0]   [ 0]   Args:   [ 3] Value: Integer field: 131033946800000000
  [ 0]   [ 0]   Args:   [ 4] Key: String: 'userName'
  [ 0]   [ 0]   Args:   [ 4] Value: WString: 'avianrr'
  [ 1] List: Empty
  [ 2] Tuple: 1 elements
  [ 2]   [ 0] Object:
  [ 2]   [ 0]   Type: String: 'utillib.KeyVal'
  [ 2]   [ 0]   Args: Dictionary: 30 entries
  [ 2]   [ 0]   Args:   [ 0] Key: String: 'fromSP'
  [ 2]   [ 0]   Args:   [ 0] Value: (None)
  [ 2]   [ 0]   Args:   [ 1] Key: String: 'finishSP'
  [ 2]   [ 0]   Args:   [ 1] Value: (None)
  [ 2]   [ 0]   Args:   [ 2] Key: String: 'trainingEndTime'
  [ 2]   [ 0]   Args:   [ 2] Value: (None)
  [ 2]   [ 0]   Args:   [ 3] Key: String: 'trainingStartTime'
  [ 2]   [ 0]   Args:   [ 3] Value: (None)
  [ 2]   [ 0]   Args:   [ 4] Key: String: 'skillsInQueue'
  [ 2]   [ 0]   Args:   [ 4] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [ 5] Key: String: 'trainedSP'
  [ 2]   [ 0]   Args:   [ 5] Value: (None)
  [ 2]   [ 0]   Args:   [ 6] Key: String: 'shipTypeID'
  [ 2]   [ 0]   Args:   [ 6] Value: Integer field: 596
  [ 2]   [ 0]   Args:   [ 7] Key: String: 'allianceID'
  [ 2]   [ 0]   Args:   [ 7] Value: (None)
  [ 2]   [ 0]   Args:   [ 8] Key: String: 'corporationID'
  [ 2]   [ 0]   Args:   [ 8] Value: Integer field: 1000166
  [ 2]   [ 0]   Args:   [ 9] Key: String: 'gender'
  [ 2]   [ 0]   Args:   [ 9] Value: Integer field: 1
  [ 2]   [ 0]   Args:   [10] Key: String: 'typeID'
  [ 2]   [ 0]   Args:   [10] Value: Integer field: 1385
  [ 2]   [ 0]   Args:   [11] Key: String: 'queueEndTime'
  [ 2]   [ 0]   Args:   [11] Value: (None)
  [ 2]   [ 0]   Args:   [12] Key: String: 'skillPoints'
  [ 2]   [ 0]   Args:   [12] Value: Real field: 56484.000000
  [ 2]   [ 0]   Args:   [13] Key: String: 'characterID'
  [ 2]   [ 0]   Args:   [13] Value: Integer field: 140000000
  [ 2]   [ 0]   Args:   [14] Key: String: 'characterName'
  [ 2]   [ 0]   Args:   [14] Value: WString: 'Keldren Kobain'
  [ 2]   [ 0]   Args:   [15] Key: String: 'bloodlineID'
  [ 2]   [ 0]   Args:   [15] Value: Integer field: 13
  [ 2]   [ 0]   Args:   [16] Key: String: 'stationID'
  [ 2]   [ 0]   Args:   [16] Value: Integer field: 60014629
  [ 2]   [ 0]   Args:   [17] Key: String: 'toLevel'
  [ 2]   [ 0]   Args:   [17] Value: (None)
  [ 2]   [ 0]   Args:   [18] Key: String: 'solarSystemID'
  [ 2]   [ 0]   Args:   [18] Value: Integer field: 30003489
  [ 2]   [ 0]   Args:   [19] Key: String: 'balance'
  [ 2]   [ 0]   Args:   [19] Value: Real field: 6665999872.000000
  [ 2]   [ 0]   Args:   [20] Key: String: 'locationSecurity'
  [ 2]   [ 0]   Args:   [20] Value: Real field: 1.000000
  [ 2]   [ 0]   Args:   [21] Key: String: 'finishedSkills'
  [ 2]   [ 0]   Args:   [21] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [22] Key: String: 'logoffDate'
  [ 2]   [ 0]   Args:   [22] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [23] Key: String: 'deletePrepareDateTime'
  [ 2]   [ 0]   Args:   [23] Value: (None)
  [ 2]   [ 0]   Args:   [24] Key: String: 'balanceChange'
  [ 2]   [ 0]   Args:   [24] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [25] Key: String: 'paperdollState'
  [ 2]   [ 0]   Args:   [25] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [26] Key: String: 'unreadMailCount'
  [ 2]   [ 0]   Args:   [26] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [27] Key: String: 'unprocessedNotifications'
  [ 2]   [ 0]   Args:   [27] Value: Integer field: 0
  [ 2]   [ 0]   Args:   [28] Key: String: 'skillTypeID'
  [ 2]   [ 0]   Args:   [28] Value: (None)
  [ 2]   [ 0]   Args:   [29] Key: String: 'petitionMessage'
  [ 2]   [ 0]   Args:   [29] Value: Integer field: 0
 */

PyResult CharUnboundMgrService::Handle_GetCohortsForUser(PyCallArgs &call)
{
    // Find what this should contain.
    return new PyList();
}

PyResult CharUnboundMgrService::Handle_GetCohortsForCharacter(PyCallArgs &call)
{
    // Find what this should contain.
    return new PyList();
}

