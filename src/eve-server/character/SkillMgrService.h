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

#ifndef _SKILLMGR_SERVICE_H
#define _SKILLMGR_SERVICE_H

#include "PyBoundObject.h"
#include "PyService.h"

enum skillEventID
{
    skillEventCharCreation = 33,
    skillEventClonePenalty = 34,
    skillEventGMGive = 39,
    skillEventHaltedAccountLapsed = 260,
    skillEventTaskMaster = 35,
    skillEventTrainingCancelled = 38,
    skillEventTrainingComplete = 37,
    skillEventTrainingStarted = 36,
    skillEventQueueTrainingCompleted = 53,
    skillEventSkillInjected = 56,
    skillEventFreeSkillPointsUsed = 307,
    skillEventGMReverseFreeSkillPointsUsed = 309,
    skillEventSkillRemoved = 177,
    skillEventSkillExtracted = 431,
    skillEventSkillExtractionReverted = 432
};

class SkillMgr2Service : public PyService
{
public:
    SkillMgr2Service();
    virtual ~SkillMgr2Service();

protected:
    class Dispatcher;

    PyCallable_DECL_CALL(GetMySkillHandler)

    //overloaded in order to support bound objects:
    virtual PyBoundObject *_CreateBoundObject(Client *c, const PyRep *bind_args);
};

#endif
