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
    Author:        Aknor Jaden, Luck
*/

#ifndef MODULE_DEFS_H
#define MODULE_DEFS_H

#include "inventory/AttributeEnum.h"
#include "utils/EvilNumber.h"

// Important constants pertaining to modules and their operation:
#define ONLINE_MODULE_IN_SPACE_CAP_PENALTY					0.75		// 75% of your capacitor will be drained when you online a module while in space, if you have it ;)

//more will go here
//this is to avoid include complications and multiple dependancies etc..
enum ModuleCommand
{
    CMD_ERROR = 1000,
    ONLINE,
    OFFLINE,
    ACTIVATE,
    DEACTIVATE,
    OVERLOAD,       //idk if this is used yet - or what it's called :)
    DEOVERLOAD,     //idk if this is used
    LOAD_CHARGE,
    RELOAD_CHARGE,
    UNLOAD_CHARGE
};

// *** use these values to decode the 'effectAppliedInState' field of the 'dgmEffectsActions' database table
enum ModuleStates
{
    MOD_UNFITTED = 0,
    MOD_OFFLINE = 1,
    MOD_ONLINE = 2,
    MOD_ACTIVATED = 4,
    MOD_OVERLOADED = 8,
    MOD_GANG = 16,
	MOD_FLEET = 32,
	MOD_DEACTIVATING = 64
};

enum ChargeStates
{
    MOD_UNLOADED = 1200,
    MOD_LOADING,
    MOD_RELOADING,
    MOD_LOADED
};

// These are the module states where an effect will, ahem, take 'effect':
// *** use these values to decode the 'effectAppliedBehavior' field of the 'dgmEffectsInfo' database table
enum ModuleEffectAppliedBehaviors
{
    EFFECT_PERSISTENT = 1300,   // means the effect is active AT ALL TIMES; used ONLY for skill, ship, subsystem effects
    EFFECT_ONLINE = 14, // means the effect takes effect on the target (see below) upon entering the ONLINE state
    EFFECT_ACTIVE = 12, // used only for ACTIVE modules operating in non-Overloaded mode
    EFFECT_OVERLOAD = 8, // used only for ACTIVE modules operating in Overloaded mode
    EFFECT_PASSIVE = 2 // used only for ACTIVE modules when not active
};

// These are the target types to which module and other types' effects are applied when activated:
// *** use these values to decode the 'effectTargetEquipmentType' field of the 'dgmEffectsInfo' database table
enum EffectTargetEquipmentTypes
{
    EQUIP_MODULE = 1400,
    EQUIP_CHARGE,
    EQUIP_THIS_SHIP,
    EQUIP_DRONE,
    EQUIP_EXTERNAL_SHIP,
    EQUIP_EXTERNAL_SHIP_MODULE,
    EQUIP_EXTERNAL_SHIP_CHARGE
};

// These are the target types to which module effects are applied when activated:
// *** use these values to decode the 'effectAppliedTo' field of the 'dgmEffectsInfo' database table
enum ModuleEffectAppliedToTargetTypes
{
    EFFECT_TARGET_SELF  = 1500, // 1500: means the target of the effect is the module's own attribute(s)
    EFFECT_TARGET_SHIP,         // 1501: means the target of the effect is the attribute(s) of the ship to which the module is fitted
    EFFECT_TARGET_EXTERNAL,     // 1502: means the target of the effect is the attribute(s) of the current target of the ship to which the module is fitted
	EFFECT_MODULE_ON_SHIP,		// 1503: means a module or modules that are fitted to the current ship, this special case will indicate when the effect is
								// applied to other modules applied to the same ship - the dgmEffectsActions table fields of targetEquipmentType and
								// targetGroupIDs will have additional information for the Module Manager to make use of this effect
	EFFECT_LOADED_CHARGE,		// 1504: means that the effect is from a charge loaded into a weapon module so this will affect the weapon module the charge
								// is loaded into
	EFFECT_CHARGE,				// 1505: means that the effect acts upon a charge loaded into a weapon module so this will affect charges of the specified
								// groupID loaded into any module on the ship
	EFFECT_CHARACTER			// 1506: means that the effect acts upon the character's attribute specific to the effect

};

// These are the methods by which module effects are applied to the designated target:
// *** use these values to decode the 'effectApplicationType' field of the 'dgmEffectsInfo' database table
enum ModuleApplicationTypes
{
    EFFECT_ONLINE_MAINTAIN = 1600,  // applied by PASSIVE or ACTIVE modules where an effect is maintained; means the effect takes effect on the
                                    // target (see below) upon entering the ONLINE state, then reversed when going out of ONLINE state
    EFFECT_ACTIVE_MAINTAIN,         // applied by ACTIVE modules where an effect is maintained; means the effect takes effect on the
                                    // target (see below) upon entering the ACTIVATE state, then reversed when going out of ACTIVATE state
    EFFECT_ACTIVE_ACCUMULATE        // applied by ACTIVE modules where an effect is applied cumulatively on each cycle; means the effect takes
};                                  // effect on the target (see below) one extra time when in ACTIVATE state after each CYCLE duration expires

// These are the methods by which module effects are applied to the designated target:
// *** use these values to decode the 'stackingPenaltyApplied' field of the 'dgmEffectsInfo' database table
enum ModuleStackingPenaltyState
{
    NO_STACKING_PENALTY = 1700,
    STACKING_PENALTY_APPLIES
};

//this may or may not be redundant...idk
enum ModulePowerLevel
{
    MODULE_BANK_RIG = 1800,
    MODULE_BANK_LOW_POWER,
    MODULE_BANK_MEDIUM_POWER,
    MODULE_BANK_HIGH_POWER,
    MODULE_BANK_SUBSYSTEM
};

//calculation types
// *** use these values to decode the 'calculationTypeID' and the 'reverseCalculationTypeID' fields of the 'dgmEffectsInfo' database table
enum EVECalculationType
{
    CALC_NONE = -1,
    CALC_PERCENTAGE = 0,
    CALC_ADDITION = 1,
    CALC_DIFFERENCE = 2,
	CALC_VELOCITY = 3,
    CALC_ABSOLUTE = 4,
	CALC_MULTIPLIER = 5,
    CALC_ADD_POSITIVE = 6,
    CALC_ADD_NEGATIVE = 7,
    CALC_SUBTRACTION = 8,
    CALC_CLOAKED_VELOCITY = 9,
    CALC_SKILL_LEVEL = 10,
    CALC_SKILL_LEVEL_x_ATT = 11,
    CALC_ABSOLUTE_MAX = 12,
    CALC_ABSOLUTE_MIN = 13,
    CALC_CAP_BOOSTERS = 14
    //more will show up, im sure
};


EvilNumber Percentage(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber Addition(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber Difference(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber Velocity(EvilNumber &attributeToModify, EvilNumber &modifierValue);

//EvilNumber Divide(EvilNumber &val1, EvilNumber &val2)
//{
//    return ( val1 / val2 );
//}

EvilNumber Multiplier(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber AddPositive(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber AddNegative(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber Subtraction(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber CloakedVelocity(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber AbsoluteMax(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber AbsoluteMin(EvilNumber &attributeToModify, EvilNumber &modifierValue);

EvilNumber CapBoosters(EvilNumber &attributeToModify, EvilNumber &modifierValue);

/*
EvilNumber AddPercent(EvilNumber &val1, EvilNumber &val2)
{
    return val1 + ( val1 * val2 );
}

EvilNumber ReverseAddPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 1;
    return val1 / ( val3 + val2 );
}

EvilNumber SubtractPercent(EvilNumber &val1, EvilNumber &val2)
{
    return val1 - ( val1 * val2 );
}

EvilNumber ReverseSubtractPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 1;
    return val1 / ( val3 - val2 );
}

EvilNumber AddAsPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 100;
    return val1 + ( val1 * (val2 / val3) );
}

EvilNumber SubtractAsPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 1;
    EvilNumber val4 = 100;

    return val1 / ( val3 + (val2 / val4) );
}

EvilNumber ModifyPercentWithPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 1;
    EvilNumber val4 = 100;

    return val1 * (val3 + (val2 / val4) );
}

EvilNumber ReverseModifyPercentWithPercent(EvilNumber &val1, EvilNumber &val2)
{
    EvilNumber val3 = 1;
    EvilNumber val4 = 100;

    return val4 * ( (val1 / val2) - 1 );
}

EvilNumber ReduceByPercent(EvilNumber &val1, EvilNumber &val2)
{
	EvilNumber val3 = 1;
	EvilNumber val4 = 100;

	return val1 * ( val3 - (val2 / val4) );
}

EvilNumber ReverseReduceByPercent(EvilNumber &val1, EvilNumber &val2)
{
	EvilNumber val3 = 1;
	EvilNumber val4 = 100;

	return val1 / ( val3 - (val2 / val4) );
}
*/

EvilNumber CalculateNewAttributeValue(EvilNumber attrVal, EvilNumber attrMod, EVECalculationType type);

#endif
