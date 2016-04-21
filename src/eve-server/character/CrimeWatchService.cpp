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
    CrimeWatchServiceBound(uint32 locationID);
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
    //GetMySecurityStatus
    //GetNpcTimer
    //GetPvpTimer
    //GetRequiredSafetyLevelForEffect
    //GetRequiredSafetyLevelForEngagingDrones
    //GetSafetyLevel
    //GetSafetyLevelRestrictionForAttackingTarget
    //GetSecurityStatusTransactions
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

    CrimeWatchServiceBound *obj = new CrimeWatchServiceBound(locationID);
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

CrimeWatchServiceBound::CrimeWatchServiceBound(uint32 locationID)
: PyBoundObject(new Dispatcher(this)),
m_locationID(locationID)
{
    m_strBoundObjectName = "CrimeWatchServiceBound";

    PyCallable_REG_CALL(CrimeWatchServiceBound, GetClientStates)
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
    PyTuple *result = new PyTuple(4);

    // T0-DO: Figure out what all this means!

    // Set first as tuple
    PyTuple *tup0 = new PyTuple(4);
    tup0->SetItem(0, new_tuple(new PyInt(100), new PyNone()));
    tup0->SetItem(1, new_tuple(new PyInt(200), new PyNone()));
    tup0->SetItem(2, new_tuple(new PyInt(400), new PyNone()));
    tup0->SetItem(3, new_tuple(new PyInt(300), new PyNone()));
    result->SetItem(0, tup0);
    // Set second as dictionary
    result->SetItem(1, new PyDict());
    // Set third as tuple
    PyTuple *tup3 = new PyTuple(2);
    PyObjectEx_Type1 *obj1 = new PyObjectEx_Type1(new PyToken("__builtin__.set"), new_tuple(new PyList()));
    PyObjectEx_Type1 *obj2 = new PyObjectEx_Type1(new PyToken("__builtin__.set"), new_tuple(new PyList()));
    tup3->SetItem(0, obj1);
    tup3->SetItem(1, obj2);
    result->SetItem(2, tup3);
    // Set fourth as Int
    result->SetItem(3, new PyInt(2));

    return result;
}
