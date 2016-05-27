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

#ifndef __OWNER__H__INCL__
#define __OWNER__H__INCL__

#include "inventory/InventoryItem.h"

/**
 * Generic class for owners (Character, Corporation).
 */
class Owner
: public InventoryItem
{
    friend class InventoryItem; // to let it construct us
public:
    /**
     * Loads Owner from DB.
     *
     * @param[in] ownerID ID of owner to load.
     * @return Ref to loaded Owner object.
     */
    static OwnerRef Load(uint32 ownerID);
    /**
     * Spawns new Owner.
     *
     * @param[in] data ItemData of new Owner.
     * @return Ref to spawned Owner object.
     */
    static OwnerRef Spawn(ItemData &data);

protected:
    Owner(uint32 _ownerID, const ItemData &_data);

};

#endif /* !__OWNER__H__INCL__ */

