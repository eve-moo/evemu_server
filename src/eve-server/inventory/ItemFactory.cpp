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

#include "character/Character.h"
#include "manufacturing/Blueprint.h"
#include "pos/Structure.h"
#include "ship/Ship.h"
#include "station/Station.h"
#include "system/Container.h"
#include "system/SolarSystem.h"

#include <mutex>

Client *ItemFactory::m_pClient; // pointer to client currently using the ItemFactory, we do not own this
// The items list and mutex.
std::map<uint32, InventoryItemRef> ItemFactory::m_items;
std::unique_lock<std::mutex> getItemLock()
{
    static std::mutex m_itemsMutex;
    return std::unique_lock<std::mutex>(m_itemsMutex);
}

void ItemFactory::Shutdown()
{
    // items
    {
        auto lock = getItemLock();
        std::map<uint32, InventoryItemRef>::const_iterator cur, end;
		uint32 total_item_count = m_items.size();
		uint32 items_saved = 0;
		float current_percent_items_saved = 0.0;
        cur = m_items.begin();
        end = m_items.end();
        for(; cur != end; cur++) {
            // save attributes of item
            if (IsNonStaticItem(cur->second->itemID()))
            {
                cur->second->SaveItem();
            }

			items_saved++;
			if( ((float)items_saved / (float)total_item_count) > (current_percent_items_saved + 0.05) )
			{
				current_percent_items_saved = (float)items_saved / (float)total_item_count;
				SysLog::Log( "Saving Items", " %3.2f%%", (current_percent_items_saved * 100.0) );
			}
        }
		SysLog::Log( "Saving Items", " COMPLETE!" );
    }

    // Set Client pointer to NULL
    m_pClient = NULL;
}

template<class _Ty>
RefPtr<_Ty> ItemFactory::_GetItem(uint32 itemID)
{
    auto lock = getItemLock();
    std::map<uint32, InventoryItemRef>::iterator res = m_items.find( itemID );
    if( res == m_items.end())
    {
        lock.unlock();
        // load the item
        RefPtr<_Ty> item = _Ty::Load( itemID );
        if( !item )
            return RefPtr<_Ty>();

        lock.lock();
        //we keep the original ref.
        res = m_items.insert( std::make_pair( itemID, item ) ).first;
    }
    // return to the user.
    return RefPtr<_Ty>::StaticCast( res->second );
}

InventoryItemRef ItemFactory::GetItem(uint32 itemID)
{
    return _GetItem<InventoryItem>( itemID );
}

BlueprintRef ItemFactory::GetBlueprint(uint32 blueprintID)
{
    return _GetItem<Blueprint>( blueprintID );
}

CharacterRef ItemFactory::GetCharacter(uint32 characterID)
{
    return _GetItem<Character>( characterID );
}

ShipRef ItemFactory::GetShip(uint32 shipID)
{
    return _GetItem<Ship>( shipID );
}

CelestialObjectRef ItemFactory::GetCelestialObject(uint32 celestialID)
{
    return _GetItem<CelestialObject>( celestialID );
}

SolarSystemRef ItemFactory::GetSolarSystem(uint32 solarSystemID)
{
    return _GetItem<SolarSystem>( solarSystemID );
}

StationRef ItemFactory::GetStation(uint32 stationID)
{
    return _GetItem<Station>( stationID );
}

SkillRef ItemFactory::GetSkill(uint32 skillID)
{
    return _GetItem<Skill>( skillID );
}

OwnerRef ItemFactory::GetOwner(uint32 ownerID)
{
    return _GetItem<Owner>( ownerID );
}

StructureRef ItemFactory::GetStructure(uint32 structureID)
{
    return _GetItem<Structure>( structureID );
}

CargoContainerRef ItemFactory::GetCargoContainer(uint32 containerID)
{
    return _GetItem<CargoContainer>( containerID );
}

InventoryItemRef ItemFactory::SpawnItem(ItemData &data) {
    InventoryItemRef i = InventoryItem::Spawn(data);
    if (!i)
    {
        return InventoryItemRef();
    }

    auto lock = getItemLock();
    // spawn successful; store the ref
    m_items.insert( std::make_pair( i->itemID(), i ) );
    return i;
}

BlueprintRef ItemFactory::SpawnBlueprint(ItemData &data, BlueprintData &bpData) {
    BlueprintRef bi = Blueprint::Spawn(data, bpData);
    if (!bi)
    {
        return BlueprintRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( bi->itemID(), bi ) );
    return bi;
}

CharacterRef ItemFactory::SpawnCharacter(ItemData &data, CharacterData &charData, CorpMemberInfo &corpData) {
    CharacterRef c = Character::Spawn(data, charData, corpData);
    if (!c)
    {
        return CharacterRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( c->itemID(), c ) );
    return c;
}

ShipRef ItemFactory::SpawnShip(ItemData &data) {
    ShipRef s = Ship::Spawn(data);
    if (!s)
    {
        return ShipRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( s->itemID(), s ) );
    return s;
}

SkillRef ItemFactory::SpawnSkill(ItemData &data)
{
    SkillRef s = Skill::Spawn( data );
    if (!s)
    {
        return SkillRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( s->itemID(), s ) );
    return s;
}

OwnerRef ItemFactory::SpawnOwner(ItemData &data)
{
    OwnerRef o = Owner::Spawn( data );
    if (!o)
    {
        return OwnerRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( o->itemID(), o ) );
    return o;
}

StructureRef ItemFactory::SpawnStructure(ItemData &data)
{
    StructureRef o = Structure::Spawn( data );
    if (!o)
    {
        return StructureRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( o->itemID(), o ) );
    return o;
}

CargoContainerRef ItemFactory::SpawnCargoContainer(ItemData &data)
{
    CargoContainerRef o = CargoContainer::Spawn( data );
    if (!o)
    {
        return CargoContainerRef();
    }

    auto lock = getItemLock();
    m_items.insert( std::make_pair( o->itemID(), o ) );
    return o;
}

Inventory *ItemFactory::GetInventory(uint32 inventoryID, bool load)
{
    InventoryItemRef item;

    if (load)
    {
        item = GetItem(inventoryID);
    }
    else
    {
        auto lock = getItemLock();
        std::map<uint32, InventoryItemRef>::iterator res = m_items.find( inventoryID );
        if( res != m_items.end() )
            item = res->second;
    }

    return Inventory::Cast( item );
}

void ItemFactory::_DeleteItem(uint32 itemID)
{
    auto lock = getItemLock();
    std::map<uint32, InventoryItemRef>::iterator res = m_items.find( itemID );
    if( res == m_items.end() )
    {
        SysLog::Error("Item Factory", "Item ID %u not found when requesting deletion!", itemID );
    }
    else
    {
        m_items.erase( res );
    }
}

void ItemFactory::SetUsingClient(Client *pClient)
{
    m_pClient = pClient;
}

Client * ItemFactory::GetUsingClient()
{
    return m_pClient;
}

void ItemFactory::UnsetUsingClient()
{
    m_pClient = NULL;
}
