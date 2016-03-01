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
    Author:        Reve
*/

#include "eve-server.h"

#include "PyServiceCD.h"
#include "ship/RepairService.h"

PyCallable_Make_InnerDispatcher(RepairService)

RepairService::RepairService()
: PyService("repairSvc", new Dispatcher(this))
{
    PyCallable_REG_CALL(RepairService, UnasembleItems);
}

RepairService::~RepairService() {
}

PyResult RepairService::Handle_UnasembleItems(PyCallArgs &call) {

    SysLog::Log("RepairService", "Called UnasembleItems stub.");
    if (call.tuple->size() == 2)
    {
        bool repackDamaged = false;
        // Call contains dictionary and empty list, get the dictionary.
        PyDict *dict = call.tuple->GetItem(0)->AsDict();
        PyDict::const_iterator cur = dict->begin();
        for (; cur != dict->end(); cur++)
        {
            // Dictionary is of Int as a locationID and list of item entries.
            //PyInt *pInt = cur->first->AsInt();
            // Location is irrelevant so get list.
            PyList *pList = cur->second->AsList();
            if (pList != nullptr)
            {
                //uint32 locationID = pInt->value();
                // Iterate through list.
                PyList::const_iterator itemItr = pList->begin();
                for (; itemItr != pList->end(); itemItr++)
                {
                    // List is tuples of itemID, LocationID pairs.
                    PyTuple *tuple = (*itemItr)->AsTuple();
                    if (tuple != nullptr)
                    {
                        // Get the itemID.
                        PyInt *itemInt = tuple->GetItem(0)->AsInt();
                        //PyInt *itemLocation = tuple->GetItem(1)->AsInt();
                        if (itemInt != nullptr)
                        {
                            // Get the item itself.
                            uint32 itemID = itemInt->value();
                            InventoryItemRef item = ItemFactory::GetItem(itemID);
                            if (item.get() != nullptr)
                            {
                                // Add type exceptions here.
                                if (item->type().categoryID() == EVEItemCategories::Blueprint ||
                                        item->type().categoryID() == EVEItemCategories::Skill ||
                                        item->type().categoryID() == EVEItemCategories::Material)
                                {
                                    // Item cannot be repackaged once used!
                                    continue;
                                }
                                if (item->GetAttribute(AttrDamage) == 0)
                                {
                                    item->ChangeSingleton(false);
                                }
                                else
                                {
                                    repackDamaged = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (repackDamaged)
        {
            throw PyException(MakeCustomError("Cannot repackage damaged items."));
        }
    }
    return NULL;
}

/*
17:54:04 [Debug]   Call Arguments:
17:54:04 [Debug]       Tuple: 2 elements
17:54:04 [Debug]         [ 0] Dictionary: 1 entries
17:54:04 [Debug]         [ 0]   [ 0] Key: Integer field: 60014779
17:54:04 [Debug]         [ 0]   [ 0] Value: List: 1 elements
17:54:04 [Debug]         [ 0]   [ 0] Value:   [ 0] Tuple: 2 elements
17:54:04 [Debug]         [ 0]   [ 0] Value:   [ 0]   [ 0] Integer field: 140000118
17:54:04 [Debug]         [ 0]   [ 0] Value:   [ 0]   [ 1] Integer field: 60014779
17:54:04 [Debug]         [ 1] List: Empty
17:54:04 [Debug]   Call Named Arguments:
17:54:04 [Debug]     Argument 'machoVersion':
17:54:04 [Debug]         Integer field: 1
 *
17:54:27 [Debug]   Call Arguments:
17:54:27 [Debug]       Tuple: 2 elements
17:54:27 [Debug]         [ 0] Dictionary: 1 entries
17:54:27 [Debug]         [ 0]   [ 0] Key: Integer field: 60014779
17:54:27 [Debug]         [ 0]   [ 0] Value: List: 2 elements
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 0] Tuple: 2 elements
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 0]   [ 0] Integer field: 140000117
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 0]   [ 1] Integer field: 60014779
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 1] Tuple: 2 elements
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 1]   [ 0] Integer field: 140000118
17:54:27 [Debug]         [ 0]   [ 0] Value:   [ 1]   [ 1] Integer field: 60014779
17:54:27 [Debug]         [ 1] List: Empty
17:54:27 [Debug]   Call Named Arguments:
17:54:27 [Debug]     Argument 'machoVersion':
17:54:27 [Debug]         Integer field: 1

 */