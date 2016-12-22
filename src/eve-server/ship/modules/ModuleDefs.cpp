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
    Author:        eve-moo
 */

#include <assert.h>
#include "ModuleDefs.h"
#include "log/SystemLog.h"

EvilNumber Percentage(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    return (attributeToModify * (EvilNumber(1.0) + (modifierValue / EvilNumber(100.0))));
}

EvilNumber Addition(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    return (attributeToModify + modifierValue);
}

EvilNumber Difference(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if (modifierValue <= 0)
    {
        return (((EvilNumber(100.0) - attributeToModify) * (-modifierValue / EvilNumber(100))) + attributeToModify);
    }
    else
    {
        return ((attributeToModify * (-modifierValue / EvilNumber(100.0))) + attributeToModify);
    }
}

EvilNumber Velocity(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    // In this special case, it is expected that modifierValue is actually the thrust/mass ratio multiplied by the module effect source attribute:
    return (attributeToModify + (attributeToModify * modifierValue / EvilNumber(100.0)));
}

//EvilNumber Divide(EvilNumber &val1, EvilNumber &val2)
//{
//    return ( val1 / val2 );
//}

EvilNumber Multiplier(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    return (attributeToModify * modifierValue);
}

EvilNumber AddPositive(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if (modifierValue > 0)
    {
        return (attributeToModify + modifierValue);
    }
    else
    {
        return (attributeToModify);
    }
}

EvilNumber AddNegative(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if (modifierValue < 0)
    {
        return (attributeToModify + modifierValue);
    }
    else
    {
        return (attributeToModify);
    }
}

EvilNumber Subtraction(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    return (attributeToModify - modifierValue);
}

EvilNumber CloakedVelocity(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    return (EvilNumber(-100.0) + ((EvilNumber(100.0) + attributeToModify * (modifierValue / EvilNumber(100.0)))));
}

EvilNumber AbsoluteMax(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if (attributeToModify > modifierValue)
    {
        return attributeToModify;
    }
    else
    {
        return modifierValue;
    }
}

EvilNumber AbsoluteMin(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if (attributeToModify < modifierValue)
    {
        return attributeToModify;
    }
    else
    {
        return modifierValue;
    }
}

EvilNumber CapBoosters(EvilNumber &attributeToModify, EvilNumber &modifierValue)
{
    if ((attributeToModify - modifierValue) < 0)
    {
        return (attributeToModify - modifierValue);
    }
    else
    {
        return EvilNumber(0.0);
    }
}

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

EvilNumber CalculateNewAttributeValue(EvilNumber attrVal, EvilNumber attrMod, EVECalculationType type)
{
    switch (type)
    {
    case CALC_NONE: return attrVal;
    case CALC_PERCENTAGE: return Percentage(attrVal, attrMod);
        break;
    case CALC_ADDITION: return Addition(attrVal, attrMod);
        break;
    case CALC_DIFFERENCE: return Difference(attrVal, attrMod);
        break;
    case CALC_VELOCITY: return Velocity(attrVal, attrMod);
        break;
    case CALC_ABSOLUTE: return attrVal;
        break;
    case CALC_MULTIPLIER: return Multiplier(attrVal, attrMod);
        break;
    case CALC_ADD_POSITIVE: return AddPositive(attrVal, attrMod);
        break;
    case CALC_ADD_NEGATIVE: return AddNegative(attrVal, attrMod);
        break;
    case CALC_SUBTRACTION: return Subtraction(attrVal, attrMod);
        break;
    case CALC_CLOAKED_VELOCITY: return CloakedVelocity(attrVal, attrMod);
        break;
    case CALC_SKILL_LEVEL: return attrVal;
        break; // is this really right for attribute effect per skill level?
    case CALC_SKILL_LEVEL_x_ATT: return attrVal;
        break; // is this really right for attribute effect per skill level?
    case CALC_ABSOLUTE_MAX: return AbsoluteMax(attrVal, attrMod);
        break;
    case CALC_ABSOLUTE_MIN: return AbsoluteMin(attrVal, attrMod);
        break;
    case CALC_CAP_BOOSTERS: return CapBoosters(attrVal, attrMod);
        break;
        //case CALC_AUTO :                            return attrVal; break;                             // AUTO NOT SUPPORTED AT THIS TIME !!!
        //case CALC_ADD :                             return Add(attrVal, attrMod); break;
        //case CALC_SUBTRACT :                        return Subtract(attrVal, attrMod); break;
        //case CALC_DIVIDE :                          return Divide(attrVal, attrMod); break;
        //case CALC_MULTIPLY :                        return Multiply(attrVal, attrMod); break;
        //case CALC_ADD_PERCENT :                     return AddPercent(attrVal, attrMod); break;
        //case CALC_REV_ADD_PERCENT :                 return ReverseAddPercent(attrVal, attrMod); break;
        //case CALC_SUBTRACT_PERCENT :                return SubtractPercent(attrVal, attrMod); break;
        //case CALC_REV_SUBTRACT_PERCENT :            return ReverseSubtractPercent(attrVal, attrMod); break;
        //case CALC_ADD_AS_PERCENT :                  return AddAsPercent(attrVal, attrMod); break;
        //case CALC_SUBTRACT_AS_PERCENT :             return SubtractAsPercent(attrVal, attrMod); break;
        //case CALC_MODIFY_PERCENT_W_PERCENT :        return ModifyPercentWithPercent(attrVal, attrMod); break;
        //case CALC_REV_MODIFY_PERCENT_W_PERCENT :    return ReverseModifyPercentWithPercent(attrVal, attrMod); break;
        //case CALC_REDUCE_BY_PERCENT:				return ReduceByPercent(attrVal, attrMod); break;
        //case CALC_REV_REDUCE_BY_PERCENT :			return ReverseReduceByPercent(attrVal, attrMod); break;
    default: return 0;
        break;
    }

    SysLog::Error("CalculateNewAttributeValue", "Unknown EveCalculationType used");
    assert(false);
    return 0;
}
