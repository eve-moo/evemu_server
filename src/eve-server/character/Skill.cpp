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
    Author:        Bloody.Rabbit
*/

#include "eve-server.h"

#include "../Client.h"
#include "character/Character.h"
#include "character/Skill.h"
#include "inventory/AttributeEnum.h"

#define SKILL_BASE_POINTS 250

/*
 * Skill
 */
Skill::Skill(uint32 _skillID, const ItemData &_data )
: InventoryItem(_skillID, _data)
{
}

SkillRef Skill::Load(uint32 skillID)
{
    return InventoryItem::Load<Skill>( skillID );
}

SkillRef Skill::Spawn(ItemData &data)
{
    uint32 skillID = 0;
    // check it's a skill
    if(data.type.get() != nullptr && data.type->getCategoryID() == EVEDB::invCategories::Skill)
    {
        data.attributes[AttrIsOnline] = EvilNumber((int) 1);
        data.attributes[AttrSkillPoints] = EvilNumber((int) 0);
        data.attributes[AttrSkillLevel] = EvilNumber((int) 0);
        skillID = InventoryItem::_Spawn(data);
    }
    if(skillID == 0)
    {
        return SkillRef();
    }
    return Skill::Load(skillID);
}

double Skill::GetSPForLevel(int level)
{
    if(level <= 0)
    {
        return 0;
    }
    double timeConst = getAttribute(AttrSkillTimeConstant).get_float();
    return (SKILL_BASE_POINTS * timeConst * std::pow(2, (2.5 * (level - 1))));
}

uint8 Skill::GetSkillLevel()
{
    return getAttribute(AttrSkillLevel).get_int();
}

bool Skill::SetSkillLevel(uint8 level)
{
    if (level > 5 || level < 0)
    {
        return false;
    }
    return setAttribute(AttrSkillLevel, level);
}

double Skill::GetSkillPoints()
{
    return getAttribute(AttrSkillPoints).get_float();
}

bool Skill::SetSkillPoints(double points)
{
    if (points > GetSPForLevel(5) || points < 0)
    {
        return false;
    }
    return setAttribute(AttrSkillPoints, points);
}

PyObject *Skill::getKeyValDict()
{
    PyDict *skillDict = new PyDict();
    skillDict->SetItem(new PyString("skillPoints"), new PyInt((int) GetSkillPoints()));
    skillDict->SetItem(new PyString("skillRank"), new PyFloat(getAttribute(AttrSkillTimeConstant).get_float()));
    skillDict->SetItem(new PyString("skillLevel"), new PyInt(GetSkillLevel()));
    return new PyObject("utillib.KeyVal", skillDict);
}

void Skill::sendSkillChangeNotice(Client *client, std::string eventName)
{
    if(client != NULL)
    {
        PyDict *skillInfos = new PyDict();
        skillInfos->SetItem(new PyInt(typeID()), getKeyValDict());
        PyRep *event = new PyNone();
        // TO-DO: find out if the can be false.
        // i.e. if skill is level 5 or char injects skill with another char already training.
        PyRep *canTrain = new PyBool(true);
        if(!eventName.empty())
        {
            event = new PyString(eventName);
        }
        PyTuple *tuple = new_tuple(skillInfos, event, canTrain);
        PyTuple *newQueue = new_tuple(new PyInt(0), new_tuple(new PyInt(0), new_tuple(new PyInt(1), tuple)));
        client->SendNotification("OnServerSkillsChanged", "charid", &newQueue, false);
    }
}
