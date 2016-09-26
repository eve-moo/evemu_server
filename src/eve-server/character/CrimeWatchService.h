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

#ifndef __CRIMEWATCH_SVC_H__
#define __CRIMEWATCH_SVC_H__

#include "PyService.h"

#define weaponsTimerStateIdle 100
#define weaponsTimerStateActive 101
#define weaponsTimerStateTimer 102
#define weaponsTimerStateInherited 103
#define pvpTimerStateIdle 200
#define pvpTimerStateActive 201
#define pvpTimerStateTimer 202
#define pvpTimerStateInherited 203
#define criminalTimerStateIdle 300
#define criminalTimerStateActiveCriminal 301
#define criminalTimerStateActiveSuspect 302
#define criminalTimerStateTimerCriminal 303
#define criminalTimerStateTimerSuspect 304
#define criminalTimerStateInheritedCriminal 305
#define criminalTimerStateInheritedSuspect 306
#define npcTimerStateIdle 400
#define npcTimerStateActive 401
#define npcTimerStateTimer 402
#define npcTimerStateInherited 403
// Safety levels
#define shipSafetyLevelNone 0
#define shipSafetyLevelPartial 1
#define shipSafetyLevelFull 2

// Security status change event types.
// Found in eve/common/script/mgt/appLogConst.py
#define eventSecStatusGmRollback 386
#define eventSecStatusGmModification 380
#define eventSecStatusHandInTags 385
#define eventSecStatusIllegalAggression 379
#define eventSecStatusKillPirateNpc 378

/*
crimewatchOutcomeNone = 0
crimewatchOutcomeSuspect = 1
crimewatchOutcomeCriminal = 2
crimewatchOutcomeEngagement = 3
 */
class CrimeWatchService
: public PyService
{
public:
    CrimeWatchService();
    virtual ~CrimeWatchService();

protected:
    class Dispatcher;

    virtual PyBoundObject *_CreateBoundObject(Client *c, const PyRep *bind_args);
};


#endif

