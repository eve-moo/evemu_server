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

#include "PyServiceCD.h"
#include "character/SkillMgrService.h"
#include "character/CharacterDB.h"
#include "PyServiceMgr.h"

class SkillMgr2Bound
: public PyBoundObject
{
public:
    SkillMgr2Bound();
    virtual ~SkillMgr2Bound();

    virtual void Release();

    PyCallable_DECL_CALL(GetSkills)
    PyCallable_DECL_CALL(GetSkillQueueAndFreePoints)
    PyCallable_DECL_CALL(GetSkillHistory)
    PyCallable_DECL_CALL(CheckAndSendNotifications)
    PyCallable_DECL_CALL(GetFreeSkillPoints)
    PyCallable_DECL_CALL(GetBoosters)
    PyCallable_DECL_CALL(GetImplants)
    PyCallable_DECL_CALL(GetAttributes)
    PyCallable_DECL_CALL(SaveNewQueue)
    PyCallable_DECL_CALL(GetRespecInfo)
    PyCallable_DECL_CALL(RespecCharacter)

    //    /**
    //     * CharStartTrainingSkillByTypeID
    //     *
    //     * Starts training a characters skill based on typeID
    //     */
    //    PyCallable_DECL_CALL(CharStartTrainingSkillByTypeID)
    //    PyCallable_DECL_CALL(CharStopTrainingSkill)
    //    PyCallable_DECL_CALL(GetEndOfTraining)
    //    PyCallable_DECL_CALL(CharAddImplant)
    //    PyCallable_DECL_CALL(RemoveImplantFromCharacter)
    //
    //    /**
    //     * SaveSkillQueue
    //     *
    //     * Saves a list of character skills received
    //     * from the client.
    //     */
    //    PyCallable_DECL_CALL(SaveSkillQueue)
    //
    //    /**
    //     * AddToEndOfSkillQueue
    //     *
    //     * Adds a skill to end of a characters skill
    //     * queue.
    //     */
    //    PyCallable_DECL_CALL(AddToEndOfSkillQueue)
    //
    //    PyCallable_DECL_CALL(GetCharacterAttributeModifiers)

protected:
    class Dispatcher;

};

PyCallable_Make_InnerDispatcher(SkillMgr2Service)
PyCallable_Make_InnerDispatcher(SkillMgr2Bound)

SkillMgr2Service::SkillMgr2Service()
: PyService("skillMgr2", new Dispatcher(this))
{
    PyCallable_REG_CALL(SkillMgr2Service, GetMySkillHandler)
}

SkillMgr2Service::~SkillMgr2Service() { }

PyResult SkillMgr2Service::Handle_GetMySkillHandler(PyCallArgs &call)
{
    PyTuple *tuple = new PyTuple(4);
    tuple->SetItem(0, new PyString("skillMgr2"));
    tuple->SetItem(1, new PyInt(PyServiceMgr::GetNodeID()));
    tuple->SetItem(2, new PyInt(call.client->GetCharacterID()));
    tuple->SetItem(3, new PyNone());

    return new PyObject("carbon.common.script.net.moniker.Moniker", tuple);
}

PyBoundObject *SkillMgr2Service::_CreateBoundObject(Client *c, const PyRep *bind_args)
{
    _log(CLIENT__MESSAGE, "SkillMgr2Service bind request for:");
    bind_args->Dump(CLIENT__MESSAGE, "    ");

    return (new SkillMgr2Bound());
}

SkillMgr2Bound::SkillMgr2Bound()
: PyBoundObject(new Dispatcher(this))
{
    m_strBoundObjectName = "SkillMgr2Bound";

    PyCallable_REG_CALL(SkillMgr2Bound, GetSkills)
    PyCallable_REG_CALL(SkillMgr2Bound, GetSkillQueueAndFreePoints)
    PyCallable_REG_CALL(SkillMgr2Bound, GetSkillHistory)
    PyCallable_REG_CALL(SkillMgr2Bound, CheckAndSendNotifications)
    PyCallable_REG_CALL(SkillMgr2Bound, GetFreeSkillPoints)
    PyCallable_REG_CALL(SkillMgr2Bound, GetBoosters)
    PyCallable_REG_CALL(SkillMgr2Bound, GetImplants)
    PyCallable_REG_CALL(SkillMgr2Bound, GetAttributes)
    PyCallable_REG_CALL(SkillMgr2Bound, SaveNewQueue)
    PyCallable_REG_CALL(SkillMgr2Bound, GetRespecInfo)
    PyCallable_REG_CALL(SkillMgr2Bound, RespecCharacter)
            
    //    PyCallable_REG_CALL(SkillMgr2Bound, CharStartTrainingSkillByTypeID)
    //    PyCallable_REG_CALL(SkillMgr2Bound, CharStopTrainingSkill)
    //    PyCallable_REG_CALL(SkillMgr2Bound, GetEndOfTraining)
    //    PyCallable_REG_CALL(SkillMgr2Bound, CharAddImplant)
    //    PyCallable_REG_CALL(SkillMgr2Bound, RemoveImplantFromCharacter)
    //    PyCallable_REG_CALL(SkillMgr2Bound, SaveSkillQueue)
    //    PyCallable_REG_CALL(SkillMgr2Bound, AddToEndOfSkillQueue)
    //
    //    PyCallable_REG_CALL(SkillMgr2Bound, GetCharacterAttributeModifiers)
}

SkillMgr2Bound::~SkillMgr2Bound()
{
}

// TODO: redesign this so this is not needed
void SkillMgr2Bound::Release()
{
    delete this;
}

PyResult SkillMgr2Bound::Handle_GetSkills(PyCallArgs &call)
{
    PyDict *skills = new PyDict();
    CharacterRef chr = call.client->GetChar();
    std::vector<InventoryItemRef> skillList;
    chr->GetSkillsList(skillList);
    for(auto skillItem : skillList)
    {
        SkillRef skill = chr->GetSkill(skillItem->typeID());
        if(skill.get() != nullptr)
        {
            skills->SetItem(new PyInt(skill->typeID()), skill->getKeyValDict());
        }
    }
    return skills;
}

PyResult SkillMgr2Bound::Handle_GetSkillQueueAndFreePoints(PyCallArgs &call)
{
    CharacterRef chr = call.client->GetChar();
    PyList *skillList = chr->GetSkillQueue();
    PyInt *freePts = new PyInt(0);
    return new_tuple(skillList, freePts);
}

PyResult SkillMgr2Bound::Handle_GetSkillHistory(PyCallArgs& call)
{
    Call_SingleIntegerArg args;
    if(!args.Decode(&call.tuple))
    {
        codelog(CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName());
        return NULL;
    }

    CharacterRef chr = call.client->GetChar();
    DBQueryResult result;
    // Temporarily get level in relativePoints column.
    if(!DBcore::RunQuery(result,
                         "SELECT eventTime AS logDate, eventID AS eventTypeID,"
                         " typeID AS skillTypeID, level as relativePoints, points as absolutePoints"
                         " FROM srvChrSkillHistory "
                         " WHERE characterID=%u AND eventID IN (37, 53) ORDER BY eventTime LIMIT %u",
                         chr->itemID(), args.arg))
    {
        _log(DATABASE__ERROR, "Failed to get skill history for character %u: %s", chr->itemID(), result.error.c_str());
    }
    DBRowDescriptor *header = new DBRowDescriptor(result);
    CRowSet *rowset = new CRowSet(&header);

    DBResultRow row;
    while(result.GetRow(row))
    {
        PyPackedRow* into = rowset->NewRow();
        uint32 cc = row.ColumnCount();
        for(uint32 i = 0; i < cc; i++)
        {
            into->SetField(i, DBColumnToPyRep(row, i));
        }
        // Now update the relativePoints column to the actual value.
        int level = row.GetInt(3);
        SkillRef skill = chr->GetSkill(row.GetInt(2));
        int relative = 0;
        if(skill.get() != nullptr)
        {
            relative = skill->GetSPForLevel(level);
            relative -= skill->GetSPForLevel(level - 1);
        }
        into->SetField(3, new PyInt(relative));
    }

    return rowset;
}

PyResult SkillMgr2Bound::Handle_CheckAndSendNotifications(PyCallArgs &call)
{
    return new PyNone();
}

PyResult SkillMgr2Bound::Handle_GetFreeSkillPoints(PyCallArgs &call)
{
    // return the number of free skill points.
    return new PyInt(0);
}

PyResult SkillMgr2Bound::Handle_GetBoosters(PyCallArgs &call)
{
    // Unknown what this is supposed to contain.
    return new PyDict();
}

PyResult SkillMgr2Bound::Handle_GetImplants(PyCallArgs &call)
{
    PyDict *implants = new PyDict();
    CharacterRef chr = call.client->GetChar();
    for(int slot = 1; slot <= 10; slot++)
    {
        InventoryItemRef implant = chr->GetImplant(slot);
        if(implant.get() != nullptr)
        {
            PyDict *implantDict = new PyDict();
            implantDict->SetItem(new PyString("itemID"), new PyLong(implant->itemID()));
            implantDict->SetItem(new PyString("typeID"), new PyInt(implant->typeID()));
            implantDict->SetItem(new PyString("implantness"), new PyInt(implant->getAttribute(AttrImplantness).get_int()));
            implants->SetItem(new PyInt(slot), new PyObject("utillib.KeyVal", implantDict));
        }
    }
    return implants;
}

PyResult SkillMgr2Bound::Handle_GetAttributes(PyCallArgs &call)
{
    CharacterRef chr = call.client->GetChar();
    PyDict *attribs = new PyDict();
    for(int attribID = 164; attribID <= 168; attribID++)
    {
        float attr = chr->getAttribute(attribID).get_float();
        attribs->SetItem(new PyInt(attribID), new PyFloat(attr));
    }
    return attribs;
}

PyResult SkillMgr2Bound::Handle_SaveNewQueue(PyCallArgs &call)
{
    if(call.tuple->size() != 2)
    {
        codelog(CLIENT__ERROR, "%s: Save queue tuple wrong size.", call.client->GetName());
        return NULL;
    }
    if(!call.tuple->GetItem(0)->IsDict())
    {
        codelog(CLIENT__ERROR, "%s: Save queue not a dict.", call.client->GetName());
        return NULL;
    }
    PyDict *skills = call.tuple->GetItem(0)->AsDict();

    CharacterRef chr = call.client->GetChar();
    chr->ClearSkillQueue();
    for(auto skill : skills->items)
    {
        if(skill.first->IsInt())
        {
            PyInt *index = skill.first->AsInt();
            if(skill.second->IsTuple())
            {
                PyTuple *tup = skill.second->AsTuple();
                int32 typeID = tup->GetItem(0)->AsInt()->value();
                int32 level = tup->GetItem(1)->AsInt()->value();
                chr->AddToSkillQueue(typeID, level);
            }
        }
    }

    chr->UpdateSkillQueue();

    // TO-DO: find out if tuple->GetItem(1) = true means start training?
    SkillRef first = chr->GetSkillInQueue(0);
    if (first.get() != nullptr)
    {
        // Make sure first skill is training.
        chr->StartTraining(first->typeID());
    }

    return NULL;
}

PyResult SkillMgr2Bound::Handle_GetRespecInfo(PyCallArgs& call)
{
    uint32 freeRespecs = 0;
    uint64 lastRespec = 0;
    uint64 nextRespec = 0;
    PyDict* result = new PyDict;
    if(!CharacterDB::GetRespecInfo(call.client->GetCharacterID(), freeRespecs, lastRespec, nextRespec))
    {
        result->SetItemString("lastRespecDate", new PyNone());
        result->SetItemString("freeRespecs", new PyInt(freeRespecs));
        result->SetItemString("nextTimedRespec", new PyNone());
    }
    else
    {
        result->SetItemString("lastRespecDate", new PyInt(lastRespec));
        result->SetItemString("freeRespecs", new PyInt(freeRespecs));
        result->SetItemString("nextTimedRespec", new PyLong(nextRespec));
    }

    return result;
}

PyResult SkillMgr2Bound::Handle_RespecCharacter(PyCallArgs &call)
{
    if(call.tuple->size() != 1)
    {
        codelog(CLIENT__ERROR, "%s: Respec character tuple wrong size.", call.client->GetName());
        return NULL;
    }
    if(!call.tuple->GetItem(0)->IsDict())
    {
        codelog(CLIENT__ERROR, "%s: Respec character not a dict.", call.client->GetName());
        return NULL;
    }

    CharacterRef chr = call.client->GetChar();
    // Do we have any respecs left? And reduce by one if yes.
    if(!CharacterDB::ReportRespec(chr->itemID()))
    {
        // No, were done here.
        // TO-DO: propper response for failed respec.
        return NULL;
    }
    // Get skill in training.
    SkillRef training = chr->GetSkillInTraining();
    // Stop the training for the respec.
    chr->StopTraining(false);
    // Do the respec.
    PyDict *attribs = call.tuple->GetItem(0)->AsDict();
    // TODO: validate these values (and their sum)
    for(auto attrib : attribs->items)
    {
        uint32 attribID = 0;
        if(attrib.first->IsInt())
        {
            attribID = attrib.first->AsInt()->value();
            if(attrib.second->IsInt())
            {
                uint32 value = attrib.second->AsInt()->value();
                chr->setAttribute(attribID, value);
            }
        }
    }
    chr->SaveAttributes();
    // Were we training a skill?
    if(training.get() != nullptr)
    {
        // Yes, restart it.
        chr->StartTraining(training->typeID());
    }

    // Send a notice to the client that the queue changed.
    chr->SendSkillQueueChangedNotice(call.client);

    // Send a notice to the client that the respec was completed.
    PyTuple *tuple = new_tuple001(new PyTuple(0));
    call.client->SendNotification("OnRespecInfoChanged", "charid", &tuple, false);
    return nullptr;
}

//PyResult SkillMgr2Bound::Handle_GetCharacterAttributeModifiers(PyCallArgs &call)
//{
//    // Called for Attribute re-mapping.
//    // expected data: (itemID, typeID, operation, value) in modifiers:
//    Call_SingleIntegerArg args;
//    if (!args.Decode(&call.tuple))
//    {
//        codelog(CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName());
//        return NULL;
//    }
//    uint32 bonusAttr = args.arg - AttrCharisma + AttrCharismaBonus;
//    PyList *list = new PyList;
//    // Check each slot.
//    for (uint32 slot = 1; slot <= 10; slot++)
//    {
//        // Get the implant for the slot.
//        InventoryItemRef item = call.client->GetChar()->GetImplant(slot);
//        if (item.get() == nullptr)
//        {
//            // No implant in this slot.
//            continue;
//        }
//        // Does this implant have the bonus we are looking for?
//        if (!item->hasAttribute(bonusAttr))
//        {
//            // No, continue.
//            continue;
//        }
//        // Add the implant to the list.
//        PyTuple *tuple = new PyTuple(4);
//        tuple->SetItem(0, new PyInt(item->itemID()));
//        tuple->SetItem(1, new PyInt(item->typeID()));
//        tuple->SetItem(2, new PyInt(EVECalculationType::CALC_ADDITION)); // operation?
//        tuple->SetItem(3, new PyInt(item->getAttribute(bonusAttr).get_int()));
//        list->AddItem(tuple);
//    }
//    return list;
//}
//
//PyResult SkillMgr2Bound::Handle_CharStopTrainingSkill(PyCallArgs &call) {
//    CharacterRef ch = call.client->GetChar();
//
//    // clear & update ...
//    ch->StopTraining();
//    ch->UpdateSkillQueue();
//
//    return NULL;
// }
//
//PyResult SkillMgr2Bound::Handle_GetEndOfTraining(PyCallArgs &call) {
//    CharacterRef ch = call.client->GetChar();
//
//    return new PyLong( ch->GetEndOfTraining().get_int() );
//}
//
//PyResult SkillMgr2Bound::Handle_CharAddImplant( PyCallArgs& call )
//{
//    //takes itemid
//    Call_SingleIntegerArg args;
//    if( !args.Decode( &call.tuple ) )
//    {
//        codelog( CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName() );
//        return NULL;
//    }
//
//    CharacterRef charRef = call.client->GetChar();
//    if (charRef->GetSkillInTraining().get() != nullptr)
//    {
//        // TO-DO: throw proper error.
//        throw (PyException(MakeUserError("RespecSkillInTraining")));
//    }
//    InventoryItemRef item = ItemFactory::GetItem(args.arg);
//    if (item.get() != nullptr)
//    {
//        if (!charRef->canUse(item))
//        {
//            // Do not have necessary skills.
//            return NULL;
//        }
//        if (item->quantity() > 1)
//        {
//            item = item->Split(1);
//        }
//        InventoryItemRef existing = charRef->GetImplant(item->getAttribute(AttrImplantness).get_int());
//        if (existing.get() != nullptr && existing != item)
//        {
//            // We are replacing an existing implant!
//            // Delete Item,  Unplugged implants are destroyed!
//            item->Delete();
//        }
//        item->Move(charRef->itemID(), flagImplant);
//    }
//
//    return NULL;
//}
//
//PyResult SkillMgr2Bound::Handle_RemoveImplantFromCharacter( PyCallArgs& call )
//{
//    //takes itemid
//    Call_SingleIntegerArg args;
//    if( !args.Decode( &call.tuple ) )
//    {
//        codelog( CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName() );
//        return NULL;
//    }
//
//    CharacterRef charRef = call.client->GetChar();
//    InventoryItemRef item = ItemFactory::GetItem(args.arg);
//    if (item.get() != nullptr)
//    {
//        // Delete Item,  Unplugged implants are destroyed!
//        item->Delete();
//    }
//
//    return NULL;
//}
//
//
//PyResult SkillMgr2Bound::Handle_AddToEndOfSkillQueue(PyCallArgs &call) {
//    Call_TwoIntegerArgs args;
//    if(!args.Decode(&call.tuple)) {
//        codelog(CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName());
//        return NULL;
//    }
//
//    CharacterRef ch = call.client->GetChar();
//
//    ch->AddToSkillQueue(args.arg1, args.arg2);
//    ch->UpdateSkillQueue();
//
//    return NULL;
//}
//
//PyResult SkillMgr2Bound::Handle_CharStartTrainingSkillByTypeID( PyCallArgs& call )
//{
//    Call_SingleIntegerArg args;
//    if( !args.Decode( &call.tuple ) )
//    {
//        codelog( CLIENT__ERROR, "%s: failed to decode arguments", call.client->GetName() );
//        return NULL;
//    }
//
//    CharacterRef charRef = call.client->GetChar();
//    charRef->StartTraining(args.arg);
//    //Log::Error("SkillMgr2Bound::Handle_CharStartTrainingSkillByTypeID()", "TODO: This is used on resuming skill queue, so should be implemented");
//    //Log::Debug( "SkillMgr2Bound", "Called CharStartTrainingSkillByTypeID stub." );
//
//    return NULL;
//}
