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

#include "python/PyRep.h"
#include "PyBoundObject.h"
#include "PyServiceCD.h"
#include "character/CrimeWatchService.h"
#include "PyServiceMgr.h"

class CrimeWatchServiceBound
: public PyBoundObject
{
public:
    CrimeWatchServiceBound(uint32 locationID, uint32 groupID);
    virtual ~CrimeWatchServiceBound();

    PyCallable_DECL_CALL(GetClientStates)
    //CanAttackFreely
    //CheckCanTakeItems
    //CheckUnsafe
    //GetBoosterEffects
    //GetCharacterSecurityStatus
    //GetCorpAggressionSettings
    //GetCriminalTimer
    //GetJumpTimers
    //GetMyBoosters
    //GetMyEngagements
    PyCallable_DECL_CALL(GetMySecurityStatus)
    //GetNpcTimer
    //GetPvpTimer
    //GetRequiredSafetyLevelForEffect
    //GetRequiredSafetyLevelForEngagingDrones
    //GetSafetyLevel
    //GetSafetyLevelRestrictionForAttackingTarget
    PyCallable_DECL_CALL(GetSecurityStatusTransactions)
    //GetSlimItemDataForCharID
    //GetWeaponsTimer
    //HasLimitedEngagmentWith
    //IsCriminal
    //IsSuspect
    //SafetyActivated
    //SetSafetyLevel
    //StartDuel

    bool Load();
    virtual void Release();

protected:
    class Dispatcher;

    uint32 m_locationID;
};

PyCallable_Make_InnerDispatcher(CrimeWatchServiceBound)

PyCallable_Make_InnerDispatcher(CrimeWatchService)

CrimeWatchService::CrimeWatchService()
: PyService("crimewatch", new Dispatcher(this)) { }

CrimeWatchService::~CrimeWatchService() { }

PyBoundObject *CrimeWatchService::_CreateBoundObject(Client *c, const PyRep *bind_args)
{
    if(!bind_args->IsTuple())
    {
        codelog(CLIENT__ERROR, "%s: Non-tuple bind argument '%s'", c->GetName(), bind_args->TypeString());
        return NULL;
    }
    const PyTuple *tup = bind_args->AsTuple();
    if(tup->size() != 2)
    {
        codelog(CLIENT__ERROR, "%s: Incorrect tuple size expected 2 got %u", c->GetName(), tup->size());
        return NULL;
    }
    if(!tup->items[0]->IsInt())
    {
        codelog(CLIENT__ERROR, "%s: Expected int got %s", c->GetName(), tup->items[0]->TypeString());
        return NULL;
    }

    uint32 locationID = tup->items[0]->AsInt()->value();
    // groupID 15 = station, 5 = solar system, 935 = world space
    uint32 groupID = tup->items[1]->AsInt()->value();

    CrimeWatchServiceBound *obj = new CrimeWatchServiceBound(locationID, groupID);
    if(!obj->Load())
    {
        _log(SERVICE__ERROR, "Failed to load static info for location %u.", locationID);
        delete obj;
        return NULL;
    }
    else
    {
        return (obj);
    }
}

//******************************************************************************

CrimeWatchServiceBound::CrimeWatchServiceBound(uint32 locationID, uint32 groupID)
: PyBoundObject(new Dispatcher(this)),
m_locationID(locationID)
{
    m_strBoundObjectName = "CrimeWatchServiceBound";

    PyCallable_REG_CALL(CrimeWatchServiceBound, GetClientStates)
    PyCallable_REG_CALL(CrimeWatchServiceBound, GetMySecurityStatus)
    PyCallable_REG_CALL(CrimeWatchServiceBound, GetSecurityStatusTransactions)
}

CrimeWatchServiceBound::~CrimeWatchServiceBound() { }

void CrimeWatchServiceBound::Release()
{
    //I hate this statement
    delete this;
}

bool CrimeWatchServiceBound::Load()
{
    return true;
}

PyResult CrimeWatchServiceBound::Handle_GetClientStates(PyCallArgs &call)
{
    // Set first as tuple
    PyTuple *myCombatTimers = new PyTuple(4);
    // If active the PyNone should be a PyInt with the expire timestamp.
    PyTuple *weaponTimerState = new_tuple(new PyInt(weaponsTimerStateIdle), new PyNone());
    PyTuple *pvpTimerState = new_tuple(new PyInt(pvpTimerStateIdle), new PyNone());
    PyTuple *npcTimerState = new_tuple(new PyInt(npcTimerStateIdle), new PyNone());
    PyTuple *criminalTimerState = new_tuple(new PyInt(criminalTimerStateIdle), new PyNone());
    myCombatTimers->SetItem(0, weaponTimerState);
    myCombatTimers->SetItem(1, pvpTimerState);
    myCombatTimers->SetItem(2, npcTimerState);
    myCombatTimers->SetItem(3, criminalTimerState);

    // Create engagements.
    PyDict *myEngagements = new PyDict();
    // Unconfirmed: the dict is key = charID, value = ??

    // Setup flagged characters.
    PyTuple *flaggedCharacters = new PyTuple(2);
    // TO-DO: populate criminal list with characterIDs of characters with criminal flag.
    PyList *criminalList = new PyList();
    PyObjectEx_Type1 *criminals = new PyObjectEx_Type1(new PyToken("__builtin__.set"), new_tuple(criminalList));
    // TO-DO: populate suspect list with characterIDs of characters with suspect flag.
    PyList *suspectList = new PyList();
    PyObjectEx_Type1 *suspects = new PyObjectEx_Type1(new PyToken("__builtin__.set"), new_tuple(suspectList));
    flaggedCharacters->SetItem(0, criminals);
    flaggedCharacters->SetItem(1, suspects);

    // Setup safety level.    
    PyInt *safetyLevel = new PyInt(shipSafetyLevelFull);

    // Setup result.
    PyTuple *result = new PyTuple(4);
    result->SetItem(0, myCombatTimers);
    result->SetItem(1, myEngagements);
    result->SetItem(2, flaggedCharacters);
    result->SetItem(3, safetyLevel);

    return result;
}

PyResult CrimeWatchServiceBound::Handle_GetMySecurityStatus(PyCallArgs &call)
{
    CharacterRef chr = call.client->GetChar();
    if(chr.get() != nullptr)
    {
        return new PyFloat(chr->securityRating());
    }
    return new PyFloat(0);
}

PyResult CrimeWatchServiceBound::Handle_GetSecurityStatusTransactions(PyCallArgs &call)
{
    DBQueryResult result;
    CharacterRef chr = call.client->GetChar();
    if(chr.get() != nullptr)
    {
        // TO-DO: The sample returned 8 results, need to find limit of number of results or time span?
/*
INSERT INTO srvChrSecurityTransactions
( characterID, eventDate, eventTypeID, referenceID, newValue, modification, locationID, otherID, otherOwnerID, otherTypeID)
VALUES
( characterID,
 Win32TimeNow(), eventSecStatusKillPirateNpc, factionID,
 newValue, changePercent, solarSystemID,
 factionID, targetID, targetTypeID
)
; */
        // referenceID appears to always equal otherOwnerID? FactionID for NPC, need to see vs player result.
        // modification is the percentage of change (1.0 = 100% ;0.01=1%)in the following formula.
        // securityStatus += (10-oldstanding) * modification
        if (!DBcore::RunQuery(result, "SELECT "
                " eventDate, eventTypeID, referenceID,"
                " newValue, modification, locationID,"
                " otherID, otherOwnerID, otherTypeID"
                " FROM srvChrSecurityTransactions"
                " WHERE characterID=%u"
                " ORDER BY eventDate DESC"
                " LIMIT 50"
                , chr->itemID()))
        {
            SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
            return nullptr;
        }
    }
    return DBResultToCRowset(result);
}
