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
    Author:     Zhur
*/

#include "eve-server.h"

#include "character/Character.h"
#include "corporation/CorporationDB.h"
#include "Client.h"

PyObject *CorporationDB::ListCorpStations(uint32 corp_id) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   stationID, stationTypeID AS typeID"
        " FROM staStations"
        " WHERE corporationID=%u",
            corp_id
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToRowset(res);
}

PyObject *CorporationDB::ListStationOffices(uint32 station_id) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   corporationID, itemID, officeFolderID"
        " FROM srvCrpOffices"
        " WHERE officeFolderID=%u",
//TODO: new a new DBSequence for this ID
            station_id + 6000000
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToRowset(res);
}

PyObject *CorporationDB::ListStationCorps(uint32 station_id) {

    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   corporationID,corporationName,description,shares,graphicID,"
        "   memberCount,ceoID,stationID,raceID,corporationType,creatorID,"
        "   hasPlayerPersonnelManager,tickerName,sendCharTerminationMessage,"
        "   shape1,shape2,shape3,color1,color2,color3,typeface,memberLimit,"
        "   allowedMemberRaceIDs,url,taxRate,minimumJoinStanding,division1,"
        "   division2,division3,division4,division5,division6,division7,"
        "   allianceID,deleted,isRecruiting"
        " FROM srvCorporation"
//no idea what the criteria should be here...
        " WHERE stationID=%u",
            station_id
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToRowset(res);
}

PyObject *CorporationDB::ListStationOwners(uint32 station_id) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   itemID AS ownerID, itemName AS ownerName, typeID, NULL AS ownerNameID"
        " FROM srvCorporation"
//no idea what the criteria should be here...
        "   LEFT JOIN eveNames ON (creatorID=itemID OR ceoID=itemID)"
        "WHERE stationID=%u",
            station_id
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToRowset(res);
}

PyDict *CorporationDB::ListAllCorpInfo() {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   corporationName,"
        "   corporationID,"
        "   size,extent,solarSystemID,investorID1,investorShares1,"
        "   investorID2, investorShares2, investorID3,investorShares3,"
        "   investorID4,investorShares4,"
        "   friendID,enemyID,publicShares,initialPrice,"
        "   minSecurity,scattered,fringe,corridor,hub,border,"
        "   factionID,sizeFactor,stationCount,stationSystemCount,"
        "   stationID,ceoID,srvEntity.itemName AS ceoName"
        " FROM crpNPCCorporations"
        " JOIN srvCorporation USING (corporationID)"
        "   LEFT JOIN srvEntity ON ceoID=srvEntity.itemID"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return(DBResultToIntRowDict(res, 1));
}

bool CorporationDB::ListAllCorpFactions(std::map<uint32, uint32> &into) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   corporationID,factionID"
        " FROM crpNPCCorporations"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToUIntUIntDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionStationCounts(std::map<uint32, uint32> &into) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID, COUNT(DISTINCT staStations.stationID) "
        " FROM crpNPCCorporations"
        " LEFT JOIN staStations USING (corporationID)"
        " GROUP BY factionID"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToUIntUIntDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionSystemCounts(std::map<uint32, uint32> &into) {
    DBQueryResult res;

    //this is not quite right, but its good enough.
    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID, COUNT(solarSystemID) "
        " FROM mapSolarSystems"
        " GROUP BY factionID"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToUIntUIntDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionRegions(std::map<int32, PyRep *> &into) {
    DBQueryResult res;

    //this is not quite right, but its good enough.
    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID,regionID "
        " FROM mapRegions"
        " WHERE factionID IS NOT NULL"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToIntIntlistDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionConstellations(std::map<int32, PyRep *> &into) {
    DBQueryResult res;

    //this is not quite right, but its good enough.
    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID,constellationID "
        " FROM mapConstellations"
        " WHERE factionID IS NOT NULL"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToIntIntlistDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionSolarSystems(std::map<int32, PyRep *> &into) {
    DBQueryResult res;

    //this is not quite right, but its good enough.
    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID,solarSystemID "
        " FROM mapSolarSystems"
        " WHERE factionID IS NOT NULL"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToIntIntlistDict(res, into);
    return true;
}

bool CorporationDB::ListAllFactionRaces(std::map<int32, PyRep *> &into) {
    DBQueryResult res;

    //this is not quite right, but its good enough.
    if(!DBcore::RunQuery(res,
        "SELECT "
        "   factionID,raceID "
        " FROM blkFactionRaces"
        " WHERE factionID IS NOT NULL"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultToIntIntlistDict(res, into);
    return true;
}

PyObject *CorporationDB::ListNPCDivisions() {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
                         "SELECT "
                         "crpNPCDivisions.divisionID, crpNPCDivisions.divisionName, divisionNameID, description, leaderType, leaderTypeID "
                         "FROM crpNPCDivisions LEFT JOIN extCrpNPCDivisions ON crpNPCDivisions.divisionID = extCrpNPCDivisions.divisionID"
    ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToRowset(res);
}

PyObject *CorporationDB::GetEmploymentRecord(uint32 charID) {
    DBQueryResult res;

    //do we really need this order by??
    if (!DBcore::RunQuery(res,
        "SELECT startDate, corporationID, deleted "
        "   FROM srvChrEmployment "
        "   WHERE characterID = %u "
        "   ORDER BY startDate DESC", charID
        ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return (DBResultToRowset(res));
}

PyObject* CorporationDB::GetMedalsReceived( uint32 charID )
{
    SysLog::Debug( "CorporationDB", "Called GetMedalsReceived stub." );

    util_Rowset rs;

    rs.header.push_back( "medalID" );
    rs.header.push_back( "title" );
    rs.header.push_back( "description" );
    rs.header.push_back( "ownerID" );
    rs.header.push_back( "" );    //wtf??
    rs.header.push_back( "issuerID" );
    rs.header.push_back( "date" );
    rs.header.push_back( "reason" );
    rs.header.push_back( "status" );

    return rs.Encode();
}

static std::string _IoN( PyRep* r )
{
    if( !r->IsInt() )
        return "NULL";
    return itoa( r->AsInt()->value() );
}

bool CorporationDB::AddCorporation(Call_AddCorporation & corpInfo, Client *client, uint32 & corpID) {
    uint32 charID = client->GetCharacterID();
    uint32 stationID = client->GetStationID();
    uint32 raceID = client->GetChar()->race();
    DBerror err;
    corpID = 0;

    std::string cName, cDesc, cTick, cURL;
    DBcore::DoEscapeString(cName, corpInfo.corpName);
    DBcore::DoEscapeString(cDesc, corpInfo.description);
    DBcore::DoEscapeString(cTick, corpInfo.corpTicker);
    DBcore::DoEscapeString(cURL, corpInfo.url);

    // First create the entity item.
    if (!DBcore::RunQueryLID(err, corpID,
            "INSERT INTO srvEntity (itemName, typeID)"
            " VALUES ('%s', 2)",
            cName.c_str()
            ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        return false;
    }

    if (!DBcore::RunQuery(err,
        " INSERT INTO srvCorporation ( "
        "   corporationID, corporationName, description, tickerName, url, "
        "   taxRate, minimumJoinStanding, corporationType, hasPlayerPersonnelManager, sendCharTerminationMessage, "
        "   creatorID, ceoID, stationID, raceID, allianceID, shares, memberCount, memberLimit, "
        "   allowedMemberRaceIDs, graphicID, color1, color2, color3, shape1, shape2, shape3, "
        "   typeface, isRecruiting "
        "   ) "
        " VALUES ( "
        "       %u, '%s', '%s', '%s', '%s', "
        "       %lf, 0, 2, 0, 1, "
        "       %u, %u, %u, %u, 0, 1000, 0, 10, "
        "       %u, 0, %s, %s, %s, %s, %s, %s, "
        "       NULL, %u )",
        corpID, cName.c_str(), cDesc.c_str(), cTick.c_str(), cURL.c_str(),
        corpInfo.taxRate,
        charID, charID, stationID, raceID, raceID,
	_IoN(corpInfo.color1).c_str(),
	_IoN(corpInfo.color2).c_str(),
	_IoN(corpInfo.color3).c_str(),
	_IoN(corpInfo.shape1).c_str(),
	_IoN(corpInfo.shape2).c_str(),
	_IoN(corpInfo.shape3).c_str(),
	corpInfo.applicationEnabled
            ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        return false;
    }

    // And create a channel too
    if (!DBcore::RunQuery(err,
                          " INSERT INTO srvChannels ("
        "   channelID, ownerID, displayName, motd, comparisonKey, "
        "   memberless, password, mailingList, cspa, temporary, "
        "   mode, subscribed, estimatedMemberCount"
        " ) VALUES ("
        "   %u, %u, '%s', '%s MOTD', '%s', "
        "   1, NULL, 0, 127, 0, "
        "   1, 1, 0"
        " )",
        corpID, corpID, cName.c_str(), cName.c_str(), cTick.c_str()
        ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        // This is not a serious problem either, but would be good if the channel
        // were working...
    }

    return true;
}

#define _NI(a, b) if (row.IsNull(b)) { cc.a = new PyNone(); } else { cc.a = new PyInt(row.GetUInt(b)); }

bool CorporationDB::CreateCorporationChangePacket(Notify_OnCorporaionChanged & cc, uint32 oldCorpID, uint32 newCorpID) {
    DBQueryResult res;
    DBResultRow row;

    if (!DBcore::RunQuery(res,
        " SELECT "
        "   corporationID,corporationName,description,tickerName,url,"
        "   taxRate,minimumJoinStanding,corporationType,hasPlayerPersonnelManager,"
        "   sendCharTerminationMessage,creatorID,ceoID,stationID,raceID,"
        "   allianceID,shares,memberCount,memberLimit,allowedMemberRaceIDs,"
        "   graphicID,shape1,shape2,shape3,color1,color2,color3,typeface,"
        "   division1,division2,division3,division4,division5,division6,"
        "   division7,deleted,isRecruiting"
        " FROM srvCorporation "
        " WHERE corporationID = %u ", newCorpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in retrieving new corporation's data (%u)", newCorpID);
        return false;
    }

    if(!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find new corporation's data (%u)", newCorpID);
        return false;
    }

    cc.corporationIDNew = row.GetUInt(0);
    cc.corporationNameNew = row.GetText(1);
    cc.descriptionNew = row.GetText(2);
    cc.tickerNameNew = row.GetText(3);
    cc.urlNew = row.GetText(4);
    cc.taxRateNew = row.GetDouble(5);
    cc.minimumJoinStandingNew = row.GetDouble(6);
    cc.corporationTypeNew = row.GetUInt(7);
    cc.hasPlayerPersonnelManagerNew = row.GetUInt(8);
    cc.sendCharTerminationMessageNew = row.GetUInt(9);
    cc.creatorIDNew = row.GetUInt(10);
    cc.ceoIDNew = row.GetUInt(11);
    cc.stationIDNew = row.GetUInt(12);
    _NI(raceIDNew, 13);
    _NI(allianceIDNew, 14);
    cc.sharesNew = row.GetUInt64(15);
    cc.memberCountNew = row.GetUInt(16);
    cc.memberLimitNew = row.GetUInt(17);
    cc.allowedMemberRaceIDsNew = row.GetUInt(18);
    cc.graphicIDNew = row.GetUInt(19);
    _NI(shape1New, 20);
    _NI(shape2New, 21);
    _NI(shape3New, 22);
    _NI(color1New, 23);
    _NI(color2New, 24);
    _NI(color3New, 25);
    _NI(typefaceNew, 26);
    _NI(division1New, 27);
    _NI(division2New, 28);
    _NI(division3New, 29);
    _NI(division4New, 30);
    _NI(division5New, 31);
    _NI(division6New, 32);
    _NI(division7New, 33);
    cc.deletedNew = row.GetUInt(34);

    if (!DBcore::RunQuery(res,
        " SELECT "
        "   corporationID,corporationName,description,tickerName,url,"
        "   taxRate,minimumJoinStanding,corporationType,hasPlayerPersonnelManager,"
        "   sendCharTerminationMessage,creatorID,ceoID,stationID,raceID,"
        "   allianceID,shares,memberCount,memberLimit,allowedMemberRaceIDs,"
        "   graphicID,shape1,shape2,shape3,color1,color2,color3,typeface,"
        "   division1,division2,division3,division4,division5,division6,"
        "   division7,deleted,isRecruiting"
        " FROM srvCorporation "
        " WHERE corporationID = %u ", oldCorpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in retrieving old corporation's data (%u)", oldCorpID);
        return false;
    }

    if(!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find old corporation's data (%u)", oldCorpID);
        return false;
    }

    cc.corporationIDOld = new PyInt(row.GetUInt(0));
    cc.corporationNameOld = new PyString(row.GetText(1));
    cc.descriptionOld = new PyString(row.GetText(2));
    cc.tickerNameOld = new PyString(row.GetText(3));
    cc.urlOld = new PyString(row.GetText(4));
    cc.taxRateOld = new PyFloat(row.GetDouble(5));
    cc.minimumJoinStandingOld = new PyFloat(row.GetDouble(6));
    cc.corporationTypeOld = new PyInt(row.GetUInt(7));
    cc.hasPlayerPersonnelManagerOld = new PyInt(row.GetUInt(8));
    cc.sendCharTerminationMessageOld = new PyInt(row.GetUInt(9));
    cc.creatorIDOld = new PyInt(row.GetUInt(10));
    cc.ceoIDOld = new PyInt(row.GetUInt(11));
    cc.stationIDOld = new PyInt(row.GetUInt(12));
    _NI(raceIDOld, 13);
    _NI(allianceIDOld, 14);
    cc.sharesOld = new PyLong(row.GetUInt64(15));
    cc.memberCountOld = new PyInt(row.GetUInt(16));
    cc.memberLimitOld = new PyInt(row.GetUInt(17));
    cc.allowedMemberRaceIDsOld = new PyInt(row.GetUInt(18));
    cc.graphicIDOld = new PyInt(row.GetUInt(19));
    _NI(shape1Old, 20);
    _NI(shape2Old, 21);
    _NI(shape3Old, 22);
    _NI(color1Old, 23);
    _NI(color2Old, 24);
    _NI(color3Old, 25);
    _NI(typefaceOld, 26);
    _NI(division1Old, 27);
    _NI(division2Old, 28);
    _NI(division3Old, 29);
    _NI(division4Old, 30);
    _NI(division5Old, 31);
    _NI(division6Old, 32);
    _NI(division7Old, 33);
    cc.deletedOld = new PyInt(row.GetUInt(34));

    return true;
}


bool CorporationDB::JoinCorporation(uint32 charID, uint32 corpID, uint32 oldCorpID, const CorpMemberInfo &roles) {
    // TODO: check for free member place

    DBerror err;
    // Decrease previous corp's member count
    if (!DBcore::RunQuery(err,
        "UPDATE srvCorporation "
        "   SET srvCorporation.memberCount = srvCorporation.memberCount-1"
        "   WHERE srvCorporation.corporationID = %u",
            oldCorpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in prev corp member decrease query: %s", err.c_str());
        return false;
    }

    // Set new corp
    if (!DBcore::RunQuery(err,
        "UPDATE srvCharacter SET "
        "   corporationID = %u, corporationDateTime = %" PRIu64 ", "
        "   corpRole = %" PRIu64 ", rolesAtAll = %" PRIu64 ", rolesAtBase = %" PRIu64 ", rolesAtHQ = %" PRIu64 ", rolesAtOther = %" PRIu64 " "
        "   WHERE characterID = %u",
            corpID, Win32TimeNow(),
            roles.corpRole, roles.rolesAtAll, roles.rolesAtBase, roles.rolesAtHQ, roles.rolesAtOther,
            charID
        ))
    {
        codelog(SERVICE__ERROR, "Error in char update query: %s", err.c_str());
        //TODO: undo previous member count decrement.
        return false;
    }

    // Increase new corp's member number...
    if (!DBcore::RunQuery(err,
        "UPDATE srvCorporation "
        "   SET memberCount = memberCount+1"
        "   WHERE corporationID = %u",
            corpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in new corp member decrease query: %s", err.c_str());
        //dont stop now, we are already moved... else we need to undo everything we just did.
    }

    // Add new employment history record
    if (!DBcore::RunQuery(err,
        "INSERT INTO srvChrEmployment VALUES (%u, %u, %" PRIu64 ", 0)",
        charID, corpID, Win32TimeNow()
        ))
    {
        codelog(SERVICE__ERROR, "Error in employment insert query: %s", err.c_str());
        //dont stop now, we are already moved... else we need to undo everything we just did.
    }

    return true;
}

bool CorporationDB::CreateCorporationCreatePacket(Notify_OnCorporaionChanged & cc, uint32 oldCorpID, uint32 newCorpID) {
    DBQueryResult res;
    DBResultRow row;

    if (!DBcore::RunQuery(res,
        " SELECT "
        "   corporationID,corporationName,description,tickerName,url,"
        "   taxRate,minimumJoinStanding,corporationType,hasPlayerPersonnelManager,"
        "   sendCharTerminationMessage,creatorID,ceoID,stationID,raceID,"
        "   allianceID,shares,memberCount,memberLimit,allowedMemberRaceIDs,"
        "   graphicID,shape1,shape2,shape3,color1,color2,color3,typeface,"
        "   division1,division2,division3,division4,division5,division6,"
        "   division7,deleted,isRecruiting"
        " FROM srvCorporation "
        " WHERE corporationID = %u ", newCorpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in retrieving new corporation's data (%u)", newCorpID);
        return false;
    }

    if(!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find corporation's data (%u)", newCorpID);
        return false;
    }

    cc.allianceIDOld = new PyNone();
    cc.allowedMemberRaceIDsOld = new PyNone();
    cc.ceoIDOld = new PyNone();
    cc.color1Old = new PyNone();
    cc.color2Old = new PyNone();
    cc.color3Old = new PyNone();
    cc.corporationIDOld = new PyNone();
    cc.corporationNameOld = new PyNone();
    cc.corporationTypeOld = new PyNone();
    cc.creatorIDOld = new PyNone();
    cc.deletedOld = new PyNone();
    cc.descriptionOld = new PyNone();
    cc.division1Old = new PyNone();
    cc.division2Old = new PyNone();
    cc.division3Old = new PyNone();
    cc.division4Old = new PyNone();
    cc.division5Old = new PyNone();
    cc.division6Old = new PyNone();
    cc.division7Old = new PyNone();
    cc.graphicIDOld = new PyNone();
    cc.hasPlayerPersonnelManagerOld = new PyNone();
    cc.memberCountOld = new PyNone();
    cc.memberLimitOld = new PyNone();
    cc.minimumJoinStandingOld = new PyNone();
    cc.raceIDOld = new PyNone();
    cc.sendCharTerminationMessageOld = new PyNone();
    cc.shape1Old = new PyNone();
    cc.shape2Old = new PyNone();
    cc.shape3Old = new PyNone();
    cc.sharesOld = new PyNone();
    cc.stationIDOld = new PyNone();
    cc.taxRateOld = new PyNone();
    cc.tickerNameOld = new PyNone();
    cc.typefaceOld = new PyNone();
    cc.urlOld = new PyNone();

    cc.corporationIDNew = row.GetUInt(0);
    cc.corporationNameNew = row.GetText(1);
    cc.descriptionNew = row.GetText(2);
    cc.tickerNameNew = row.GetText(3);
    cc.urlNew = row.GetText(4);
    cc.taxRateNew = row.GetDouble(5);
    cc.minimumJoinStandingNew = row.GetDouble(6);
    cc.corporationTypeNew = row.GetUInt(7);
    cc.hasPlayerPersonnelManagerNew = row.GetUInt(8);
    cc.sendCharTerminationMessageNew = row.GetUInt(9);
    cc.creatorIDNew = row.GetUInt(10);
    cc.ceoIDNew = row.GetUInt(11);
    cc.stationIDNew = row.GetUInt(12);
    _NI(raceIDNew, 13);
    _NI(allianceIDNew, 14);
    cc.sharesNew = row.GetUInt64(15);
    cc.memberCountNew = row.GetUInt(16);
    cc.memberLimitNew = row.GetUInt(17);
    cc.allowedMemberRaceIDsNew = row.GetUInt(18);
    cc.graphicIDNew = row.GetUInt(19);
    _NI(shape1New, 20);
    _NI(shape2New, 21);
    _NI(shape3New, 22);
    _NI(color1New, 23);
    _NI(color2New, 24);
    _NI(color3New, 25);
    _NI(typefaceNew, 26);
    _NI(division1New, 27);
    _NI(division2New, 28);
    _NI(division3New, 29);
    _NI(division4New, 30);
    _NI(division5New, 31);
    _NI(division6New, 32);
    _NI(division7New, 33);
    cc.deletedNew = row.GetUInt(34);

    return true;
}

PyObject *CorporationDB::GetCorporation(uint32 corpID) {
    DBQueryResult res;
    DBResultRow row;

    if (!DBcore::RunQuery(res,
        " SELECT "
        "   corporationID,corporationName,description,tickerName,url,"
        "   taxRate,minimumJoinStanding,corporationType,hasPlayerPersonnelManager,"
        "   sendCharTerminationMessage,creatorID,ceoID,stationID,raceID,"
        "   allianceID,shares,memberCount,memberLimit,allowedMemberRaceIDs,"
		"   shape1,shape2,shape3,color1,color2,color3,typeface,"
        "   division1,division2,division3,division4,division5,division6,"
        "   division7,deleted,isRecruiting,warFactionID,walletDivision1,"
		"   walletDivision2,walletDivision3,walletDivision4,walletDivision5,"
		"   walletDivision6,walletDivision7"
        " FROM srvCorporation "
        " WHERE corporationID = %u", corpID))
    {
        codelog(SERVICE__ERROR, "Error in retrieving corporation's data (%u)", corpID);
        return NULL;
    }

    if(!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find corporation's data (%u)", corpID);
        return NULL;
    }

    return DBRowToRow(row);
    //return DBResultToRowset(res);
}

PyObjectEx *CorporationDB::GetEveOwners(uint32 corpID)
{
    // This appears to return a list of members. for player corporations.
    // This appears to return a specific list for NPC corporations.
    DBQueryResult res;
    // TO-DO: get the owners for all npc corporations.
    //Convert corpRegistry->GetEveOwners PackedRows to blkCharactersStatic insert values
    // replace corpID with the actual corporationID then remove entries that already exist.
    //search:.+\[PyPackedRow.+\n.+=> <(\d+).+\n.+=> <(.+)>.+\n.+=> <(\d+).+\n.+=> <(\d).+
    //replace:($1, '$2', corpID, $3, 0, 0, 0, 0, $4, 0),
    std::map<uint32, std::vector < int32>> owners = {
        {1000006, // Deep Core Mining Inc.
            {3004089, 3010971, 3010972, 3010973, 3010974, 3010975, 3010976, 3010977, 3010978, 3010979, 3010980, 3010981, 3010982, 3013505, 3013506, 3013507, 3013508, 3016197, 3016198, 3016199, 3016200, 3016201, 3016202, 3016203, 3016204, 3016205, 3016206, 3016207, 3016208, 3016209, 3016210, 3016211, 3016212, 3018348, 3018398, 3018726, 3018727, 3018728}},
        {1000009, // Caldari Provisions
            {3004073, 3011053, 3011054, 3011055, 3011056, 3011057, 3011058, 3011059, 3011060, 3011061, 3011062, 3011063, 3011064, 3011065, 3011066, 3011067, 3011068, 3011069, 3011070, 3011071, 3011072, 3011073, 3011074, 3011075, 3011076, 3011077, 3011078, 3011079, 3011080, 3011081, 3011082, 3011083, 3011084, 3011085, 3011086, 3011087, 3011088, 3011089, 3011090, 3011091, 3011092, 3013519, 3013520, 3013521, 3013522, 3016261, 3016262, 3016263, 3016264, 3016265, 3016266, 3016267, 3016268, 3016269, 3016270, 3016271, 3016272, 3016273, 3016274, 3016275, 3016276, 3016277, 3016278, 3016279, 3016280, 3016281, 3016282, 3016283, 3016284, 3016285, 3016286, 3016287, 3016288, 3016289, 3016290, 3016291, 3017216, 3018373, 3018378, 3018383}},
        {1000014, // Perkone
            {3004161, 3011276, 3011277, 3011278, 3011279, 3011280, 3011281, 3011282, 3011283, 3011284, 3011285, 3011286, 3011287, 3011288, 3011289, 3011290, 3011291, 3011292, 3011293, 3011294, 3011295, 3011296, 3011297, 3011298, 3011299, 3011300, 3011301, 3011302, 3011303, 3011304, 3011305, 3011306, 3011307, 3011308, 3011309, 3011310, 3011311, 3011312, 3011313, 3011314, 3011315, 3013540, 3013541, 3013542, 3013543, 3013544, 3016402, 3016403, 3016404, 3016405, 3016406, 3016407, 3016408, 3016409, 3016410, 3016411, 3016412, 3016413, 3016414, 3016415, 3016416, 3016417, 3016418, 3016419, 3016420, 3016421, 3016422, 3016423, 3016424, 3016425, 3016426, 3016427, 3016428, 3016429, 3016430, 3016431, 3016432, 3017142, 3017145, 3017476}},
        {1000044, // School of Applied Knowledge
            {3004181, 3012239, 3012240, 3012241, 3012242, 3012243, 3012244, 3012245, 3012246, 3012247, 3012248, 3013661, 3013662, 3013663, 3013664, 3013665, 3018580, 3018610, 3018645, 3018672, 3018801, 3018802, 3018803, 3018804, 3018937, 3018938, 3019370, 3019371, 3019448, 3019471, 3019494}},
        {1000045, // Science and Trade Institute
            {3004185, 3012249, 3012250, 3012251, 3012252, 3012253, 3012254, 3012255, 3012256, 3012257, 3012258, 3013666, 3013667, 3013668, 3013669, 3017009, 3017159, 3018391, 3018399, 3018581, 3018611, 3018646, 3018675, 3018805, 3018806, 3018807, 3018808, 3018929, 3019354, 3019355, 3019450, 3019473, 3019493}},
        {1000046, // Sebiestor tribe
            {3004477, 3012259, 3012260, 3012261, 3012262, 3012263, 3012264, 3012265, 3012266, 3012267, 3012268, 3012269, 3012270, 3013670, 3013671, 3013672, 3013673, 3013674, 3017010, 3017011, 3017012, 3017013, 3017014, 3017015, 3017016, 3017017, 3017018, 3017019, 3017020, 3017021, 3017022, 3017023, 3017024, 3017496, 3017678, 3018421, 3018422, 3018423, 3018479, 3018558, 3018668, 3018669, 3018670, 3018671, 3019403, 3019404, 3019483}},
        {1000049, // Brutor tribe
            {3004421, 3012295, 3012296, 3012297, 3012298, 3012299, 3012300, 3012301, 3012302, 3012303, 3012304, 3012305, 3012306, 3013685, 3013686, 3013687, 3013688, 3013689, 3017055, 3017056, 3017057, 3017058, 3017059, 3017060, 3017061, 3017062, 3017063, 3017064, 3017065, 3017066, 3017067, 3017068, 3017440, 3017683, 3018434, 3018437, 3018440, 3018442, 3018957, 3018958, 3019362, 3019392, 3019393, 3019395, 3019398, 3019411}},
        {1000060, // Native Freshfood
            {3004445, 3012685, 3012686, 3012687, 3013469, 3013470, 3014284, 3014285, 3014286, 3014287, 3014288, 3014289, 3014290, 3014291, 3014292, 3014293, 3014294, 3014295, 3014296, 3014297, 3014298, 3014299, 3014300, 3014301, 3018623}},
        {1000066, // Viziam
            {3003993, 3012958, 3012959, 3012960, 3012961, 3012962, 3012963, 3012964, 3012965, 3012966, 3012967, 3012968, 3012969, 3013413, 3013414, 3013415, 3013416, 3014461, 3014462, 3014463, 3014464, 3014465, 3014466, 3014467, 3014468, 3014469, 3014470, 3014471, 3014472, 3014473, 3014474, 3014475, 3014476}},
        {1000072, // Imperial Shipment
            {3003937, 3013099, 3013100, 3013101, 3013102, 3013103, 3013104, 3013105, 3013106, 3013107, 3013108, 3013109, 3013110, 3013111, 3013112, 3013113, 3013114, 3013115, 3013116, 3013117, 3013118, 3013119, 3013120, 3013121, 3013122, 3013123, 3013124, 3013125, 3013126, 3013127, 3013128, 3013130, 3013131, 3013132, 3013133, 3013134, 3013135, 3013136, 3013137, 3013138, 3013139, 3013140, 3013141, 3013142, 3013143, 3013144, 3013145, 3013146, 3013147, 3013148, 3013149, 3013150, 3013151, 3013152, 3013153, 3013154, 3013155, 3013156, 3013157, 3013158, 3013440, 3013441, 3013442, 3013443, 3013444, 3014561, 3014562, 3014563, 3014564, 3014565, 3014566, 3014567, 3014568, 3014569, 3014570, 3014571, 3014572, 3014573, 3014574, 3014575, 3014576, 3014577, 3014578, 3014579, 3014580, 3014581, 3014582, 3014583, 3014584, 3014585, 3014586, 3014587, 3014588, 3014589, 3014590, 3014591, 3014592, 3014593, 3014594, 3014595, 3014596, 3014597, 3014598, 3014599, 3014600, 3014601, 3014602, 3014603, 3014604, 3014605, 3017146, 3017248, 3017258, 3017296, 3017381, 3017400, 3017540, 3018592}},
        {1000077, // Royal Amarr Institute
            {3003977, 3013332, 3013333, 3013334, 3013335, 3013336, 3013337, 3013338, 3013339, 3013340, 3013341, 3013381, 3013382, 3013383, 3013384, 3013385, 3017328, 3017357, 3018628, 3018643, 3018682, 3018809, 3018810, 3018811, 3018812, 3018924, 3019336, 3019337, 3019447, 3019470, 3019486}},
        {1000080, // Ministry of War
            {3003965, 3008570, 3008571, 3008572, 3008573, 3008574, 3008575, 3008576, 3008577, 3008578, 3008579, 3008580, 3008581, 3008582, 3008583, 3008584, 3008585, 3008586, 3008587, 3008588, 3008589, 3008590, 3008591, 3008592, 3008593, 3008594, 3008595, 3008596, 3008597, 3008598, 3008599, 3008600, 3008601, 3008602, 3008603, 3008604, 3008605, 3008606, 3008607, 3008608, 3008609, 3013386, 3013387, 3013388, 3013389, 3014753, 3014754, 3014755, 3014756, 3014757, 3014758, 3014759, 3014760, 3014761, 3014762, 3014763, 3014764, 3014765, 3014766, 3014767, 3014768, 3014769, 3014770, 3014771, 3014772, 3014773, 3014774, 3014775, 3014776, 3014777, 3014778, 3014779, 3014780, 3014781, 3014782, 3014783, 3014784, 3017411, 3017417, 3017463, 3017798, 3017799, 3017835, 3017836, 3017962, 3017963, 3017964, 3017974, 3017975, 3018100, 3018101, 3018102, 3018154, 3018155, 3018281, 3018282, 3018283, 3018631, 3018888, 3018889, 3018890, 3018891, 3018892, 3018893, 3018894, 3018895, 3018896, 3018965, 3018974}},
        {1000107, // The Scope
            {3004341, 3009483, 3009484, 3009485, 3009486, 3009487, 3009488, 3009489, 3009490, 3009491, 3009492, 3009493, 3009494, 3009495, 3009496, 3009497, 3009498, 3009499, 3009500, 3009501, 3009502, 3009503, 3009504, 3009505, 3009506, 3009507, 3009508, 3009509, 3009510, 3009511, 3009512, 3013728, 3013729, 3013730, 3013731, 3013732, 3015396, 3015397, 3015398, 3015399, 3015400, 3017208, 3017273, 3017402, 3017438, 3017460, 3017536, 3017575, 3017595, 3017601, 3017693, 3017822, 3017859, 3017998, 3018146, 3018178, 3018331, 3018554, 3018858, 3018859, 3018860, 3018861, 3018862, 3019413}},
        {1000111, // Aliastra
            {3004229, 3009648, 3009649, 3009650, 3009651, 3009652, 3009653, 3009654, 3009655, 3009656, 3009657, 3009658, 3009659, 3009660, 3009661, 3009662, 3009663, 3009664, 3009665, 3009666, 3009667, 3009668, 3009669, 3009670, 3009671, 3009672, 3009673, 3009674, 3009675, 3009676, 3009677, 3009678, 3009679, 3009680, 3009681, 3009682, 3009683, 3009684, 3009685, 3009686, 3009687, 3009688, 3009689, 3009690, 3009691, 3009692, 3009693, 3009694, 3009695, 3009696, 3009697, 3009698, 3009699, 3009700, 3009701, 3009702, 3009703, 3009704, 3009705, 3009706, 3009707, 3013899, 3013900, 3013901, 3013902, 3013903, 3015478, 3015479, 3015480, 3015481, 3015482, 3015483, 3015484, 3015485, 3015486, 3015487, 3015488, 3015489, 3015490, 3015491, 3015492, 3015493, 3015494, 3015495, 3015496, 3015497, 3015498, 3015499, 3015500, 3015501, 3015502, 3015503, 3015504, 3015505, 3015506, 3015507, 3015508, 3015509, 3017173, 3017196, 3017323, 3017384, 3017389, 3017449, 3017450, 3017500, 3017579, 3017592, 3017629, 3017642, 3017752}},
        {1000114, // Garoun Investment Bank
            {3004297, 3009773, 3009774, 3009775, 3009776, 3009777, 3009778, 3009779, 3009780, 3009781, 3009782, 3009783, 3009784, 3009785, 3009786, 3009787, 3009788, 3009789, 3009790, 3009791, 3009792, 3013914, 3013915, 3013916, 3013917, 3013918, 3015531, 3015532, 3015533, 3015534, 3015535, 3017211, 3017226, 3017714, 3017715, 3017718, 3017720, 3017722, 3017724, 3017727, 3018443}},
        {1000115, // University of Caille
            {3004349, 3009793, 3009794, 3009795, 3009796, 3009797, 3009798, 3009799, 3009800, 3009801, 3009802, 3013919, 3013920, 3013921, 3013922, 3013923, 3017514, 3018540, 3018541, 3018542, 3018552, 3018553, 3018603, 3018642, 3018685, 3018813, 3018814, 3018815, 3018816, 3018931, 3019340, 3019341, 3019451, 3019474, 3019495}},
        {1000165, // Hedion Univirsity
            {3003921, 3010791, 3010792, 3010793, 3010794, 3010795, 3010796, 3010797, 3010798, 3010799, 3010800, 3013849, 3013850, 3013851, 3013852, 3016055, 3017697, 3018626, 3018680, 3018817, 3018818, 3018819, 3018820, 3018930, 3019348, 3019349, 3019445, 3019468, 3019489}},
        {1000166, // Imperial Academy
            {3003925, 3010801, 3010802, 3010803, 3010804, 3010805, 3010806, 3010807, 3010808, 3010809, 3010810, 3013857, 3013858, 3013859, 3013860, 3016056, 3017470, 3017698, 3017925, 3017926, 3017950, 3018063, 3018064, 3018088, 3018145, 3018153, 3018243, 3018244, 3018268, 3018329, 3018337, 3018627, 3018681, 3018821, 3018822, 3018823, 3018824, 3018921, 3019338, 3019339, 3019446, 3019469, 3019490}},
        {1000167, // State War Academy
            {3004193, 3010811, 3010812, 3010813, 3010814, 3010815, 3010816, 3010817, 3010818, 3010819, 3010820, 3013853, 3013854, 3013855, 3013856, 3016057, 3018582, 3018612, 3018647, 3018676, 3018825, 3018826, 3018827, 3018828, 3018935, 3019352, 3019353, 3019449, 3019472, 3019492}},
        {1000168, // Federal Navy Academy
            {3004285, 3010821, 3010822, 3010823, 3010824, 3010825, 3010826, 3010827, 3010828, 3010829, 3010830, 3013844, 3013845, 3013846, 3013847, 3013848, 3017634, 3018602, 3018684, 3018829, 3018830, 3018831, 3018832, 3018923, 3019334, 3019335, 3019453, 3019476, 3019496}},
        {1000169, // Center for Advanced Studies
            {3004245, 3010832, 3010833, 3010834, 3010835, 3010836, 3010837, 3010838, 3010839, 3010840, 3013861, 3013862, 3013863, 3013864, 3013865, 3017128, 3017654, 3018601, 3018683, 3018833, 3018834, 3018835, 3018836, 3018926, 3019332, 3019333, 3019452, 3019475, 3019497}},
        {1000170, // Republic Military School
            {3004461, 3010841, 3010842, 3010843, 3010844, 3010845, 3010846, 3010847, 3010848, 3010849, 3010850, 3013866, 3013867, 3013868, 3013869, 3013870, 3017174, 3017195, 3017637, 3018615, 3018616, 3018678, 3018837, 3018838, 3018839, 3018840, 3018919, 3018920, 3019344, 3019345, 3019454, 3019477, 3019499}},
        {1000171, // Republic University
            {3004473, 3010851, 3010852, 3010853, 3010854, 3010855, 3010856, 3010857, 3010858, 3010859, 3010860, 3013871, 3013872, 3013873, 3013874, 3013875, 3017648, 3018617, 3018679, 3018841, 3018842, 3018843, 3018844, 3018927, 3019342, 3019343, 3019455, 3019478, 3019484, 3019498}},
        {1000172, // Pator Tech School - Joins 1000049 after leaving player corp
            {3004449, 3010861, 3010862, 3010863, 3010864, 3010865, 3010866, 3010867, 3010868, 3010869, 3010870, 3013876, 3013877, 3013878, 3013879, 3013880, 3018614, 3018677, 3018845, 3018846, 3018847, 3018848, 3018932, 3019346, 3019347, 3019456, 3019479, 3019500}}
    };

    std::vector<int32> ownerList;
    auto itr = owners.find(corpID);
    if(itr != owners.end())
    {
        ownerList = itr->second;
    }
    else
    {
        if(corpID < EVEMU_MINIMUM_ID)
        {
            SysLog::Debug("CorpOwners", "Unable to find owners for corpID=%u", corpID);
        }
        // At the very least list the CEO.
        uint32 ceoID = GetCorporationCEO(corpID);
        ownerList.push_back(ceoID);
    }
    std::string inList;
    ListToINString(ownerList, inList, "0");
    if( !DBcore::RunQuery( res,
                          "SELECT characterID as ownerID, itemName as ownerName, typeID, gender FROM srvEntity "
                          "LEFT JOIN srvCharacter ON srvEntity.itemID = srvCharacter.characterID "
                          "where itemID in (%s) "
                          "ORDER BY ownerID", inList.c_str()))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToCRowset(res);
}

PyObject *CorporationDB::GetStations(uint32 corpID) {
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
        " SELECT "
        " stationID, stationTypeID as typeID "
        " FROM staStations "
        " WHERE corporationID = %u ", corpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }
    return DBResultToRowset(res);
}

uint32 CorporationDB::GetOffices(uint32 corpID) {
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
        " SELECT "
        " COUNT(1) AS OfficeNumber "
        " FROM srvCrpOffices "
        " WHERE corporationID = %u ", corpID
        ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return(0);
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find corporation's data (%u)", corpID);
        return 0;
    }
    return row.GetUInt(0);
}

PyRep *CorporationDB::Fetch(uint32 corpID, uint32 from, uint32 count) {
    DBQueryResult res;
    DBResultRow rr;

    if (!DBcore::RunQuery(res,
        " SELECT stationID, typeID, itemID, officeFolderID "
        " FROM srvCrpOffices "
        " WHERE corporationID = %u "
        " LIMIT %u, %u ", corpID, from, count
        ))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    res.GetRow(rr);

    // Have to send back a list that contains a tuple that contains an int and a list...
    // params probably needs the following stuff: stationID, typeID, officeID, officeFolderID
    Reply_FetchOffice reply;
    reply.params = new PyList;

    reply.params->AddItemInt( rr.GetInt(0) );
    reply.params->AddItemInt( rr.GetInt(1) );
    reply.officeID = rr.GetInt(2);
    reply.params->AddItemInt( reply.officeID );
    reply.params->AddItemInt( rr.GetInt(3) );

    return reply.Encode();
}
uint32 CorporationDB::GetQuoteForRentingAnOffice(uint32 stationID) {
    DBQueryResult res;
    DBResultRow row;

    if (!DBcore::RunQuery(res,
        " SELECT "
        " officeRentalCost "
        " FROM staStations "
        " WHERE staStations.stationID = %u ", stationID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        // Try to look more clever than we actually are...
        return 10000;
    }

    if (!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Unable to find station data, stationID: %u", stationID);
        // Try to look more clever than we actually are...
        return 10000;
    }

    return row.GetUInt(0);
}
// Need to find out wether there is any kind of limit regarding the offices
uint32 CorporationDB::ReserveOffice(const OfficeInfo & oInfo) {
    // oInfo should at this point contain the station, officeFolder and corporation infos

    // First check if we have a free office at this station at all...
    // Instead, assume that there is, and add one for this corporation
    DBerror err;

    // First add it into the srvEntity table
    uint32 officeID = 0;
    if (!DBcore::RunQueryLID(err, officeID,
        " INSERT INTO srvEntity ("
        " itemName, typeID, ownerID, locationID, flag, contraband, singleton, "
        " quantity, x, y, z, customInfo "
        " ) VALUES ("
        // office name should be more descriptive
        // corporation owns the office, station locates the office
        // x, y, z should be coords of the station?
        // no extra info
        " 'office', 27, %u, %u, 0, 0, 1, 1, 0, 0, 0, '' "
        " ); ", oInfo.corporationID, oInfo.stationID ))
    {
        codelog(SERVICE__ERROR, "Error in query at ReserveOffice: %s", err.c_str());
        return(0);
    }

    // inserts with the id gotten previously
    if (!DBcore::RunQuery(err,
        " INSERT INTO srvCrpOffices "
        " (corporationID, stationID, itemID, typeID, officeFolderID) "
        " VALUES "
        " (%u, %u, %u, %u, %u) ",
        oInfo.corporationID, oInfo.stationID, officeID, oInfo.typeID, oInfo.officeFolderID))
    {
        codelog(SERVICE__ERROR, "Error in query at ReserveOffice: %s", err.c_str());
        // Ensure that officeID stays 0, whatever the RunQueryLID done...
        return(0);
    }

    // If insert is successful, oInfo.officeID now contains the rented office's ID
    // Nothing else to do...
    return(officeID);
}

//NOTE: it makes sense to push this up to ServiceDB, since others will likely need this too.
uint32 CorporationDB::GetStationOwner(uint32 stationID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT corporationID "
        " FROM staStations "
        " WHERE stationID = %u ", stationID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return 0;
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Missing stationID: %u", stationID);
        return 0;
    }
    return row.GetUInt(0);
}

PyRep *CorporationDB::GetMyApplications(uint32 charID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT corporationID, characterID, applicationText, roles, grantableRoles, "
        " status, applicationDateTime, deleted, lastCorpUpdaterID "
        " FROM srvChrApplications "
        " WHERE characterID = %u ", charID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }
    return DBResultToRowset(res);
}
bool CorporationDB::InsertApplication(const ApplicationInfo & aInfo) {
    if (!aInfo.valid) {
        codelog(SERVICE__ERROR, "aInfo contains invalid data");
        return false;
    }

    DBerror err;
    std::string safeMessage;
    DBcore::DoEscapeString(safeMessage, aInfo.appText);
    if (!DBcore::RunQuery(err,
        " INSERT INTO srvChrApplications ("
        " corporationID, characterID, applicationText, roles, grantableRoles, status, "
        " applicationDateTime, deleted, lastCorpUpdaterID "
        " ) VALUES ( "
        " %u, %u, '%s', %" PRIu64 ", %" PRIu64 ", %u, %" PRIu64 ", %u, %u "
        " ) ", aInfo.corpID, aInfo.charID, safeMessage.c_str(), aInfo.role,
               aInfo.grantRole, aInfo.status, aInfo.appTime, aInfo.deleted, aInfo.lastCID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        return false;
    }

    return true;
}

PyRep *CorporationDB::GetApplications(uint32 corpID)
{
    // TO-DO: Add applicationID column.
    
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT "
        " corporationID, characterID, applicationText, roles, grantableRoles, status, "
        " applicationDateTime, deleted, lastCorpUpdaterID "
        " FROM srvChrApplications "
        " WHERE corporationID = %u ", corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }

    return DBResultToIndexRowset(res, "characterID");
}

uint32 CorporationDB::GetStationCorporationCEO(uint32 stationID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT srvCorporation.ceoID "
        " FROM srvCorporation "
        " LEFT JOIN staStations "
        " ON staStations.corporationID = srvCorporation.corporationID "
        " WHERE staStations.stationID = %u ", stationID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return 0;
    }
    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "There's either no such station or the station has no corp owner or the corporation has no ceo. Probably there's no such corporation.");
        return 0;
    }
    return row.GetUInt(0);
}

uint32 CorporationDB::GetCorporationCEO(uint32 corpID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT ceoID "
        " FROM srvCorporation "
        " WHERE srvCorporation.corporationID = %u ", corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return 0;
    }
    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "There's either no such corp owner or the corporation has no ceo. Probably a buggy db.");
        return 0;
    }
    return row.GetUInt(0);
}

uint32 CorporationDB::GetCloneTypeCostByID(uint32 cloneTypeID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT basePrice "
        " FROM invTypes "
        " WHERE typeID = %u ", cloneTypeID))
    {
        SysLog::Error("CorporationDB","Failed to retrieve basePrice of typeID = %u",cloneTypeID);
    }
    DBResultRow row;
    if (!res.GetRow(row)) {
        SysLog::Error("CorporationDB","Query returned no results");
        return 0;
    }
    return row.GetDouble(0);
}

bool CorporationDB::GetCurrentApplicationInfo(uint32 charID, uint32 corpID, ApplicationInfo & aInfo) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT "
        " status, applicationText, applicationDateTime, roles, grantableRoles, lastCorpUpdaterID, deleted "
        " FROM srvChrApplications "
        " WHERE characterID = %u AND corporationID = %u ",
        charID, corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        aInfo.valid = false;
        return false;
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "There's no previous application.");
        aInfo.valid = false;
        return false;
    }

    aInfo.charID = charID;
    aInfo.corpID = corpID;
    aInfo.status = row.GetUInt(0);
    aInfo.appText = row.GetText(1);
    aInfo.appTime = row.GetUInt64(2);
    aInfo.role = row.GetUInt64(3);
    aInfo.grantRole = row.GetUInt64(4);
    aInfo.lastCID = row.GetUInt(5);
    aInfo.deleted = row.GetUInt(6);
    aInfo.valid = true;
    return true;
}

bool CorporationDB::UpdateApplication(const ApplicationInfo & info) {
    if (!info.valid) {
        codelog(SERVICE__ERROR, "info contains invalid data");
        return false;
    }

    DBerror err;
    std::string clear;
    DBcore::DoEscapeString(clear, info.appText);
    if (!DBcore::RunQuery(err,
        " UPDATE srvChrApplications "
        " SET status = %u, lastCorpUpdaterID = %u, applicationText = '%s' "
        " WHERE corporationID = %u AND characterID = %u ", info.status, info.lastCID, clear.c_str(), info.corpID, info.charID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        return false;
    }
    return true;
}

bool CorporationDB::DeleteApplication(const ApplicationInfo & info) {
    DBerror err;
    if (!DBcore::RunQuery(err,
        " DELETE FROM srvChrApplications "
        " WHERE corporationID = %u AND characterID = %u ", info.corpID, info.charID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", err.c_str());
        return false;
    }
    return true;
}

bool CorporationDB::CreateMemberAttributeUpdate(MemberAttributeUpdate & attrib, uint32 newCorpID, uint32 charID) {
    // What are we doing here exactly?
    // Corporation gets a new member
    // it's new to it

    DBQueryResult res;
    DBResultRow row;
    if (!DBcore::RunQuery(res,
        " SELECT "
        "   title, corporationDateTime, corporationID, "
        "   corpRole, rolesAtAll, rolesAtBase, "
        "   rolesAtHQ, rolesAtOther "
        " FROM srvCharacter "
        " WHERE srvCharacter.characterID = %u ", charID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    if (!res.GetRow(row)) {
        codelog(SERVICE__ERROR, "Cannot find character in database");
        return false;
    }

    // this could be stored in the db
#define PRN new PyNone()
#define PRI(i) new PyInt(i)
#define PRL(i) new PyLong(i)
#define PRS(s) new PyString(s)
#define PRNI(i) (row.IsNull(i) ? PRL(0) : PRL(row.GetUInt64(i)))
#define F(name, o, n) \
    attrib.name##Old = o; \
    attrib.name##New = n

    //element                   Old Value               New Value
    F(accountKey,               PRN,                    PRN);
    // i don't even know what this could refer to
    F(baseID,                   PRN,                    PRN);
    F(characterID,              PRN,                    PRI(charID));
    F(corporationID,            PRI(row.GetUInt(2)),    PRI(newCorpID));
    // these also have to be queried from the db
    F(divisionID,               PRN,                    PRN);
    F(roles,                    PRNI(3),                PRI(0));
    F(grantableRoles,           PRNI(4),                PRI(0));
    F(grantableRolesAtBase,     PRNI(5),                PRI(0));
    F(grantableRolesAtHQ,       PRNI(6),                PRI(0));
    F(grantableRolesAtOther,    PRNI(7),                PRI(0));
    F(squadronID,               PRN,                    PRN);
    F(startDateTime,            PRL(row.GetUInt64(1)),  PRL(Win32TimeNow()));
    // another one i have no idea
    F(titleMask,                PRN,                    PRI(0));
    F(baseID,                   PRS(row.GetText(0)),    PRS(""));
#undef F
#undef PRN
#undef PRI
#undef PRS
#undef PRNI

    return true;
}
bool CorporationDB::UpdateDivisionNames(uint32 corpID, const Call_UpdateDivisionNames & divs, PyDict * notif) {
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
        " SELECT "
        " division1, division2, division3, division4, division5, division6, division7 "
        " FROM srvCorporation "
        " WHERE corporationID = %u ", corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "Corporation %u doesn't exists.", corpID);
        return false;
    }

    // We are here, so something must have changed...
    std::vector<std::string> dbQ;
    ProcessStringChange("division1", row.GetText(0), divs.div1, notif, dbQ);
    ProcessStringChange("division2", row.GetText(1), divs.div2, notif, dbQ);
    ProcessStringChange("division3", row.GetText(2), divs.div3, notif, dbQ);
    ProcessStringChange("division4", row.GetText(3), divs.div4, notif, dbQ);
    ProcessStringChange("division5", row.GetText(4), divs.div5, notif, dbQ);
    ProcessStringChange("division6", row.GetText(5), divs.div6, notif, dbQ);
    ProcessStringChange("division7", row.GetText(6), divs.div7, notif, dbQ);

    std::string query = " UPDATE srvCorporation SET ";

    int N = dbQ.size();
    for (int i = 0; i < N; i++) {
        query = dbQ[i];
        if (i < N - 1) query += ", ";
    }

    query += " WHERE corporationID = %u";

    if ((N > 0) && (!DBcore::RunQuery(res.error, query.c_str(), corpID))) {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    return true;
}

bool CorporationDB::UpdateCorporation(uint32 corpID, const Call_UpdateCorporation & upd, PyDict * notif) {
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
        " SELECT description, url, taxRate "
        " FROM srvCorporation "
        " WHERE corporationID = %u ", corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "Corporation %u doesn't exists.", corpID);
        return false;
    }

    std::vector<std::string> dbQ;
    ProcessStringChange("description", row.GetText(0), upd.description, notif, dbQ);
    ProcessStringChange("url", row.GetText(1), upd.address, notif, dbQ);
    ProcessRealChange("taxRate", row.GetDouble(2), upd.tax, notif, dbQ);

    std::string query = " UPDATE srvCorporation SET ";

    int N = dbQ.size();
    for (int i = 0; i < N; i++) {
        query += dbQ[i];
        if (i < N - 1) query += ", ";
    }

    query += " WHERE corporationID = %u";

    // only update if there is anything to update
    if ((N > 0) && (!DBcore::RunQuery(res.error, query.c_str(), corpID))) {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    return true;

}
#define NI(i) row.IsNull(i) ? 0 : row.GetInt(i)
bool CorporationDB::UpdateLogo(uint32 corpID, const Call_UpdateLogo & upd, PyDict * notif) {
    DBQueryResult res;

    if (!DBcore::RunQuery(res,
        " SELECT shape1, shape2, shape3, color1, color2, color3, typeface "
        " FROM srvCorporation "
        " WHERE corporationID = %u ", corpID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    DBResultRow row;
    if (!res.GetRow(row)) {
        _log(DATABASE__ERROR, "Corporation %u doesn't exists.", corpID);
        return false;
    }

    std::vector<std::string> dbQ;
    ProcessIntChange("shape1", NI(0), upd.shape1, notif, dbQ);
    ProcessIntChange("shape2", NI(1), upd.shape2, notif, dbQ);
    ProcessIntChange("shape3", NI(2), upd.shape3, notif, dbQ);

    ProcessIntChange("color1", NI(3), upd.color1, notif, dbQ);
    ProcessIntChange("color2", NI(4), upd.color2, notif, dbQ);
    ProcessIntChange("color3", NI(5), upd.color3, notif, dbQ);

    std::string query = " UPDATE srvCorporation SET ";

    int N = dbQ.size();
    for (int i = 0; i < N; i++) {
        query += dbQ[i];
        if (i < N - 1) query += ", ";
    }

    query += " WHERE corporationID = %u ";
    if ((N > 0) && (!DBcore::RunQuery(res.error, query.c_str(), corpID))) {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return false;
    }

    return true;
}
#undef NI

//replace all the typeID of the character's clones
bool CorporationDB::ChangeCloneType(uint32 characterID, uint32 typeID) {
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
                         "SELECT typeID, typeName FROM invTypes WHERE typeID = %u", typeID))
    {
        _log(DATABASE__ERROR, "Failed to change clone type of char %u: %s.", characterID, res.error.c_str());
        return false;
    }

    DBResultRow row;
    if( !(res.GetRow(row)) )
    {
        SysLog::Error( "CorporationDB::ChangeCloneType()", "Could not find Clone typeID = %u in invTypes table.", typeID );
        return false;
    }
    std::string typeNameString = row.GetText(1);

    if(!DBcore::RunQuery(res.error,
        "UPDATE "
        "srvEntity "
        "SET typeID=%u, itemName='%s' "
        "WHERE ownerID=%u "
        "AND flag=400",
        typeID,
        typeNameString.c_str(),
        characterID))
    {
        _log(DATABASE__ERROR, "Failed to change clone type of char %u: %s.", characterID, res.error.c_str());
        return false;
    }
    SysLog::Debug( "CorporationDB", "Clone upgrade successful" );
    return true;
}

PyDict* CorporationDB::GetBookmarks(uint32 corporationID)
{
    DBQueryResult res;

    if(!DBcore::RunQuery(res,
        "SELECT "
        " bookmarkID,"
        " ownerID,"
        " itemID,"
        " typeID,"
        " memo,"
        " created,"
        " x,"
        " y,"
        " z,"
        " locationID,"
        " note,"
        " creatorID,"
        " folderID"
        " FROM srvBookmarks"
        " WHERE ownerID = %u",
        corporationID))
    {
        SysLog::Error("CorporationDB::GetBookmarks()", "Failed to query corporation bookmarks for corporation %u: %s", corporationID, res.error.c_str());
        return NULL;
    }
    return DBResultToPackedRowDict(res, "bookmarkID");
    //return DBResultToCRowset(res);
}
PyRep *CorporationDB::GetMyShares(uint32 charID) {
    DBQueryResult res;
    if (!DBcore::RunQuery(res,
        " SELECT corporationID, shares "
        " FROM crpcharshares "
        " WHERE characterID = %u ", charID))
    {
        codelog(SERVICE__ERROR, "Error in query: %s", res.error.c_str());
        return NULL;
    }
    return DBResultToRowset(res);
}
