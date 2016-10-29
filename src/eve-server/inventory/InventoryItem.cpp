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

#include "Client.h"
#include "EntityList.h"
#include "character/Skill.h"
#include "inventory/Owner.h"
#include "manufacturing/Blueprint.h"
#include "ship/Ship.h"
#include "station/Station.h"
#include "system/Celestial.h"
#include "system/Container.h"

const int32 ITEM_DB_SAVE_TIMER_EXPIRY(10);

const std::map<uint32, EVEItemFlags> InventoryItem::m_cargoAttributeFlagMap = {
    {AttrCapacity, flagCargoHold},
    {AttrDroneCapacity, flagDroneBay},
    {AttrSpecialFuelBayCapacity, flagSpecializedFuelBay},
    {AttrSpecialOreHoldCapacity, flagSpecializedOreHold},
    {AttrSpecialGasHoldCapacity, flagSpecializedGasHold},
    {AttrSpecialMineralHoldCapacity, flagSpecializedMineralHold},
    {AttrSpecialSalvageHoldCapacity, flagSpecializedSalvageHold},
    {AttrSpecialShipHoldCapacity, flagSpecializedShipHold},
    {AttrSpecialSmallShipHoldCapacity, flagSpecializedSmallShipHold},
    {AttrSpecialLargeShipHoldCapacity, flagSpecializedLargeShipHold},
    {AttrSpecialIndustrialShipHoldCapacity, flagSpecializedIndustrialShipHold},
    {AttrSpecialAmmoHoldCapacity, flagSpecializedAmmoHold},
    {AttrShipMaintenanceBayCapacity, flagShipHangar},
    {AttrFleetHangarCapacity, flagHangar}
};

const std::map<EVEItemFlags, uint32 > InventoryItem::m_cargoFlagAttributeMap = {
    {flagCargoHold, AttrCapacity},
    {flagDroneBay, AttrDroneCapacity},
    {flagSpecializedFuelBay, AttrSpecialFuelBayCapacity},
    {flagSpecializedOreHold, AttrSpecialOreHoldCapacity},
    {flagSpecializedGasHold, AttrSpecialGasHoldCapacity},
    {flagSpecializedMineralHold, AttrSpecialMineralHoldCapacity},
    {flagSpecializedSalvageHold, AttrSpecialSalvageHoldCapacity},
    {flagSpecializedShipHold, AttrSpecialShipHoldCapacity},
    {flagSpecializedSmallShipHold, AttrSpecialSmallShipHoldCapacity},
    {flagSpecializedLargeShipHold, AttrSpecialLargeShipHoldCapacity},
    {flagSpecializedIndustrialShipHold, AttrSpecialIndustrialShipHoldCapacity},
    {flagSpecializedAmmoHold, AttrSpecialAmmoHoldCapacity},
    {flagShipHangar, AttrShipMaintenanceBayCapacity},
    {flagHangar, AttrFleetHangarCapacity}
};

/*
 * ItemData
 */
ItemData::ItemData(
    const char *_name,
    uint32 _typeID,
    uint32 _ownerID,
    uint32 _locationID,
    EVEItemFlags _flag,
    bool _contraband,
    bool _singleton,
    uint32 _quantity,
    const Vector3D &_position,
    const char *_customInfo)
: name(_name),
  typeID(_typeID),
  ownerID(_ownerID),
  locationID(_locationID),
  flag(_flag),
  contraband(_contraband),
  singleton(_singleton),
  quantity(_quantity),
  position(_position),
  customInfo(_customInfo)
{
}

ItemData::ItemData(
    uint32 _typeID,
    uint32 _ownerID,
    uint32 _locationID,
    EVEItemFlags _flag,
    uint32 _quantity,
    const char *_customInfo,
    bool _contraband)
: name(""),
  typeID(_typeID),
  ownerID(_ownerID),
  locationID(_locationID),
  flag(_flag),
  contraband(_contraband),
  singleton(false),
  quantity(_quantity),
  position(0, 0, 0),
  customInfo(_customInfo)
{
}

ItemData::ItemData(
    uint32 _typeID,
    uint32 _ownerID,
    uint32 _locationID,
    EVEItemFlags _flag,
    const char *_name,
    const Vector3D &_position,
    const char *_customInfo,
    bool _contraband)
: name(_name),
  typeID(_typeID),
  ownerID(_ownerID),
  locationID(_locationID),
  flag(_flag),
  contraband(_contraband),
  singleton(true),
  quantity(1),
  position(_position),
  customInfo(_customInfo)
{
}

/*
 * InventoryItem
 */
InventoryItem::InventoryItem(
    uint32 _itemID,
                             const InvTypeRef _type,
    const ItemData &_data)
: RefObject( 0 ),
  //attributes( *this, true, true),
m_AttributeMap(*this),
m_DefaultAttributeMap(*this, true),
  m_saveTimer(0,true),
  m_itemID(_itemID),
  m_itemName(_data.name),
  m_type(_type),
  m_ownerID(_data.ownerID),
  m_locationID(_data.locationID),
  m_flag(_data.flag),
  m_contraband(_data.contraband),
  m_singleton(_data.singleton),
  m_quantity(_data.quantity),
  m_position(_data.position),
  m_customInfo(_data.customInfo)

{
    // assert for data consistency
    assert(_data.typeID == _type->typeID);

    //m_saveTimerExpiryTime = ITEM_DB_SAVE_TIMER_EXPIRY * 60 * 1000;      // 10 minutes in milliseconds
    //m_saveTimer.SetTimer(m_saveTimerExpiryTime);                        // set timer in milliseconds
    m_saveTimer.Disable();                                              // disable timer by default

    _log(ITEM__TRACE, "Created object %p for item %s (%u).", this, itemName().c_str(), itemID());
}

InventoryItem::~InventoryItem()
{
    // Save this item's srvEntity_attributes info to the Database before it is destroyed
    //mAttributeMap.SaveAttributes();

    // Save this item's srvEntity table info to the Database before it is destroyed
    //SaveItem();
}

InventoryItemRef InventoryItem::Load(uint32 itemID)
{
    return InventoryItem::Load<InventoryItem>( itemID );
}

template<class _Ty>
RefPtr<_Ty> InventoryItem::_LoadItem(uint32 itemID,
    // InventoryItem stuff:
                                     const InvTypeRef type, const ItemData &data)
{
    uint32 groupID = type->groupID;
    // See what to do next:
    switch (type->getCategoryID())
    {
        //! TODO not handled.
        case EVEDB::invCategories::_System:
        case EVEDB::invCategories::Station:
        case EVEDB::invCategories::Material:
        case EVEDB::invCategories::Accessories:
        case EVEDB::invCategories::Module:
        case EVEDB::invCategories::Charge:
        case EVEDB::invCategories::Trading:
        case EVEDB::invCategories::Bonus:
        case EVEDB::invCategories::Commodity:
        case EVEDB::invCategories::Drone:
        case EVEDB::invCategories::Implant:
        case EVEDB::invCategories::Deployable:
        case EVEDB::invCategories::Structure:
        case EVEDB::invCategories::Reaction:
        case EVEDB::invCategories::Asteroid:
             break;
        ///////////////////////////////////////
        // Blueprint:
        ///////////////////////////////////////
        case EVEDB::invCategories::Blueprint: {
            return Blueprint::_LoadItem<Blueprint>( itemID, type, data );
        }

        ///////////////////////////////////////
        // Entity:
        ///////////////////////////////////////
        case EVEDB::invCategories::Entity:
        {
            if ((groupID == EVEDB::invGroups::Spawn_Container))
                return CargoContainerRef( new CargoContainer( itemID, type, data ) );
            else
                if ((groupID >= EVEDB::invGroups::Asteroid_Angel_Cartel_Frigate
                    && groupID <= EVEDB::invGroups::Deadspace_Serpentis_Frigate)
                    || (groupID >= 755 /* Asteroid Rogue Drone BattleCruiser */
                    && groupID <= 761 /* Asteroid Rogue Drone Swarm */)
                    || (groupID >= 789 /* Asteroid Angel Cartel Commander Frigate */
                    && groupID <= 814 /* Asteroid Serpentis Commander Frigate */)
                    || (groupID >= 843 /* Asteroid Rogue Drone Commander BattleCruiser */
                    && groupID <= 852 /* Asteroid Serpentis Commander Battleship */)
                    || (groupID >= 959 /* Deadspace Sleeper Sleepless Sentinel */
                    && groupID <= 987 /* Deadspace Sleeper Emergent Patroller */))
					return InventoryItemRef( new InventoryItem(itemID, type, data) );
				else
					return CelestialObjectRef( new CelestialObject( itemID, type, data ) );
        }

        ///////////////////////////////////////
        // Celestial:
        ///////////////////////////////////////
        case EVEDB::invCategories::Celestial:
        {
            if ((groupID == EVEDB::invGroups::Secure_Cargo_Container)
                || (groupID == EVEDB::invGroups::Audit_Log_Secure_Container)
                || (groupID == EVEDB::invGroups::Freight_Container)
                || (groupID == EVEDB::invGroups::Cargo_Container)
                || (groupID == EVEDB::invGroups::Wreck) )
                return CargoContainerRef( new CargoContainer( itemID, type, data ) );
            else
                return CelestialObjectRef( new CelestialObject( itemID, type, data ) );
        }

        ///////////////////////////////////////
        // Ship:
        ///////////////////////////////////////
        case EVEDB::invCategories::Ship: {
            return Ship::_LoadItem<Ship>( itemID, type, data );
        }

        ///////////////////////////////////////
        // Skill:
        ///////////////////////////////////////
        case EVEDB::invCategories::Skill: {
            return Skill::_LoadItem<Skill>( itemID, type, data );
        }

        ///////////////////////////////////////
        // Owner:
        ///////////////////////////////////////
        case EVEDB::invCategories::Owner: {
            return Owner::_LoadItem<Owner>( itemID, type, data );
        }
    }

    // ItemCategory didn't do it, try ItemGroup:
    switch( groupID ) {
        ///////////////////////////////////////
        // Station:
        ///////////////////////////////////////
        case EVEDB::invGroups::Station: {
            return Station::_LoadItem<Station>( itemID, type, data );
        }
    }

    // Generic item, create one:
    return InventoryItemRef( new InventoryItem( itemID, type, data ) );
}

bool InventoryItem::loadState()
{
    // load attributes
    m_AttributeMap.Load();
    m_DefaultAttributeMap.Load();

	// fill basic cargo hold data:
    m_cargoHoldsUsedVolumeByFlag.insert(std::pair<EVEItemFlags, double>(flagCargoHold, m_AttributeMap.GetAttribute(AttrCapacity).get_float()));

    // update inventory
    Inventory *inventory = ItemFactory::GetInventory(locationID(), false);
    if(inventory != NULL)
    {
        inventory->AddItem(InventoryItemRef(this));
    }

    return true;
}

InventoryItemRef InventoryItem::Spawn(ItemData &data)
{
    // obtain type of new item
    const InvTypeRef type = InvType::getType(data.typeID);
    if(type.get() == nullptr)
    {
        return InventoryItemRef();
    }

    // See what to do next:
    switch(type->getCategoryID())
    {
        //! TODO not handled.
        case EVEDB::invCategories::_System:
        case EVEDB::invCategories::Station:
        case EVEDB::invCategories::Material:
        case EVEDB::invCategories::Accessories:
        case EVEDB::invCategories::Trading:
        case EVEDB::invCategories::Bonus:
        case EVEDB::invCategories::Commodity:
        case EVEDB::invCategories::Implant:
        case EVEDB::invCategories::Reaction:
             break;

		///////////////////////////////////////
        // Entity:
        ///////////////////////////////////////
        case EVEDB::invCategories::Entity: {
			uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }
			InventoryItemRef itemRef = InventoryItem::Load( itemID );
			return itemRef;
		}

		///////////////////////////////////////
        // Blueprint:
        ///////////////////////////////////////
        case EVEDB::invCategories::Blueprint: {
            BlueprintData bdata; // use default blueprint attributes
            return Blueprint::Spawn(data, bdata);
        }

        ///////////////////////////////////////
        // Celestial:
        // (used for Cargo Containers, Rings, and Biomasses, Wrecks, Large Collidable Objects, Clouds,
        //  Cosmic Signatures, Mobile Sentry Guns, Global Warp Disruptors, Agents in Space, Cosmic Anomaly, Beacons, Wormholes,
        //  and other celestial static objects such as NPC stations, stars, moons, planets, and stargates)
        ///////////////////////////////////////
        case EVEDB::invCategories::Celestial:
        {
            if((type->groupID == EVEDB::invGroups::Secure_Cargo_Container)
               || (type->groupID == EVEDB::invGroups::Cargo_Container)
               || (type->groupID == EVEDB::invGroups::Freight_Container)
               || (type->groupID == EVEDB::invGroups::Audit_Log_Secure_Container)
               || (type->groupID == EVEDB::invGroups::Spawn_Container)
               || (type->groupID == EVEDB::invGroups::Wreck))
            {
                // Spawn new Cargo Container
                return CargoContainer::Spawn(data);
            }
            else
            {
                // Spawn new Celestial Object
                return CelestialObject::Spawn(data);
            }
        }

        ///////////////////////////////////////
        // Ship:
        ///////////////////////////////////////
        case EVEDB::invCategories::Ship:
        {
            return Ship::Spawn(data);
        }

        ///////////////////////////////////////
        // Skill:
        ///////////////////////////////////////
        case EVEDB::invCategories::Skill: {
            return Skill::Spawn( data );
        }

        ///////////////////////////////////////
        // Owner:
        ///////////////////////////////////////
        case EVEDB::invCategories::Owner:
        {
            return Owner::Spawn( data );
        }

        ///////////////////////////////////////
        // Charge:
        ///////////////////////////////////////
        case EVEDB::invCategories::Charge:
        {
            // THESE SHOULD BE MOVED INTO A Charge::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
            data.attributes[AttrMass] = type->mass; // Mass
            data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrCapacity] = type->capacity; // Capacity
            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
		}

		///////////////////////////////////////
        // Module:
        ///////////////////////////////////////
        case EVEDB::invCategories::Module:
        {
            // THESE SHOULD BE MOVED INTO A Module::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrIsOnline] = EvilNumber(1); // Is Online
            data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
            data.attributes[AttrMass] = type->mass; // Mass
            data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrCapacity] = type->capacity; // Capacity
            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
        }

        ///////////////////////////////////////
        // Drone:
        ///////////////////////////////////////
        case EVEDB::invCategories::Drone:
        {
            // THESE SHOULD BE MOVED INTO A Drone::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
            data.attributes[AttrShieldCharge] = type->getAttribute(AttrShieldCapacity); // Shield Charge
            data.attributes[AttrArmorDamage] = EvilNumber(0.0); // Armor Damage
            data.attributes[AttrMass] = type->mass; // Mass
            data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrCapacity] = type->capacity; // Capacity

            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
        }

        ///////////////////////////////////////
        // Deployable:
        ///////////////////////////////////////
        case EVEDB::invCategories::Deployable:
        {
            // THESE SHOULD BE MOVED INTO A Deployable::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrIsOnline] = EvilNumber(1); // Is Online
            data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
            //itemRef->SetAttribute(AttrShieldCharge,   itemRef->GetAttribute(AttrShieldCapacity));       // Shield Charge
            //itemRef->SetAttribute(AttrArmorDamage,    0.0);                                        // Armor Damage
            data.attributes[AttrMass] = type->mass; // Mass
            data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrCapacity] = type->capacity; // Capacity

            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
        }

        ///////////////////////////////////////
        // Asteroid:
        ///////////////////////////////////////
        case EVEDB::invCategories::Asteroid:
        {
            // THESE SHOULD BE MOVED INTO AN Asteroid::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrRadius] = EvilNumber(500.0); // Radius
            data.attributes[AttrMass] = EvilNumber(1000000.0); // Mass
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrQuantity] = EvilNumber(5000.0); // Quantity

            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
        }

        ///////////////////////////////////////
        // Structure:
        ///////////////////////////////////////
        case EVEDB::invCategories::Structure:
        {
            // THESE SHOULD BE MOVED INTO A Structure::Spawn() function that does not exist yet
            // Create default dynamic attributes in the AttributeMap:
            data.attributes[AttrIsOnline] = EvilNumber(1); // Is Online
            data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
            data.attributes[AttrShieldCharge] = type->getAttribute(AttrShieldCapacity); // Shield Charge
            data.attributes[AttrArmorDamage] = EvilNumber(0.0); // Armor Damage
            data.attributes[AttrMass] = type->mass; // Mass
            data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
            data.attributes[AttrVolume] = type->volume; // Volume
            data.attributes[AttrCapacity] = type->capacity; // Capacity

            // Spawn generic item:
            uint32 itemID = InventoryItem::_Spawn( data );
            if(itemID == 0)
            {
                return InventoryItemRef();
            }

            return InventoryItem::Load(itemID);
        }
    }

    switch(type->groupID)
    {
        ///////////////////////////////////////
        // Station:
        ///////////////////////////////////////
        case EVEDB::invGroups::Station:
        {
            return Station::Spawn(data);
        }
    }

    // Create some basic attributes that are NOT found in dgmTypeAttributes for most items, yet most items DO need:
    data.attributes[AttrDamage] = EvilNumber(0.0); // Structure Damage
    data.attributes[AttrMass] = type->mass; // Mass
    data.attributes[AttrRadius] = type->getAttribute(AttrRadius); // Radius
    data.attributes[AttrVolume] = type->volume; // Volume
    data.attributes[AttrCapacity] = type->capacity; // Capacity

    // Spawn generic item:
    uint32 itemID = InventoryItem::_Spawn( data );
    if(itemID == 0)
    {
        return InventoryItemRef();
    }
    return InventoryItem::Load(itemID);
}

uint32 InventoryItem::_Spawn(
    // InventoryItem stuff:
    ItemData &data
) {
    // obtain type of new item
    // this also checks that the type is valid
    const InvTypeRef t = InvType::getType(data.typeID);
    if (t.get() == nullptr)
    {
        return 0;
    }

    // fix the name (if empty)
    if(data.name.empty())
    {
        data.name = t->typeName;
    }

    // Insert new entry into DB
    uint32 itemID = InventoryDB::NewItem(data);
    if(itemID == 0)
    {
        return itemID;
    }
    // Create attributes.
    if(data.attributes.size() > 0)
    {
        auto itr = data.attributes.begin();
        auto itr_end = data.attributes.end();
        std::string values;
        for(; itr != itr_end; itr++)
        {
            char buf[1024];
            if(itr->second.get_type() == evil_number_int)
            {
                std::sprintf(buf, "(%u, %u, %" PRId64 ", NULL)",
                             itemID, itr->first, itr->second.get_int());
            }
            else if(itr->second.get_type() == evil_number_float)
            {
                std::sprintf(buf, "(%u, %u, NULL, %f)",
                             itemID, itr->first, itr->second.get_float());
            }
            if(values.length() > 0)
            {
                values += ", ";
            }
            values += buf;
        }
        if(values.length() > 0)
        {
            DBerror err;
            bool success = DBcore::RunQuery(err,
                                            "INSERT INTO srvEntity_default_attributes (itemID, attributeID, valueInt, valueFloat) VALUES %s",
                                            values.c_str());
            if(!success)
            {
                SysLog::Error("InventoryItem", "Unable to create default attributes for item %u", itemID);
            }
            success = DBcore::RunQuery(err,
                                       "INSERT INTO srvEntity_attributes (itemID, attributeID, valueInt, valueFloat) VALUES %s",
                                       values.c_str());
            if(!success)
            {
            SysLog::Error("InventoryItem", "Unable to create attributes for item %u", itemID);
            }
        }
    }
    return itemID;
}

void InventoryItem::Delete(bool notify)
{
    // Remove us from our inventory.
    Inventory *inventory = ItemFactory::GetInventory(m_locationID, false);
    if (inventory != nullptr)
    {
        inventory->RemoveItem(InventoryItemRef(this));
    }
    //first, get out of client's sight.
    uint32 ownerID = m_ownerID;
    // Set new owner and location.
    m_ownerID = 2;
    m_locationID = 6;
    if(notify)
    {
        // Issue changes notice to client.
        sendItemChangeNotice(EntityList::FindCharacter(ownerID));
    }

    //take ourself out of the DB
    DBerror err;
    if (!DBcore::RunQueryMulti(err, "DELETE FROM srvEntity WHERE itemID=%u;"
                          "DELETE FROM srvEntity_default_attributes WHERE itemID=%u;"
                          "DELETE FROM srvEntity_attributes WHERE itemID=%u;",
                          m_itemID, m_itemID, m_itemID))
    {
        codelog(DATABASE__ERROR, "Failed to delete item %u: %s", m_itemID, err.c_str());
    }

    //delete ourselves from factory cache
    ItemFactory::_DeleteItem(itemID());
}

PyPackedRow* InventoryItem::GetItemStatusRow() const
{
    DBRowDescriptor* header = new DBRowDescriptor;
    header->AddColumn( "instanceID",    DBTYPE_I8 );
    header->AddColumn( "online",        DBTYPE_BOOL );
    header->AddColumn( "damage",        DBTYPE_R8 );
    header->AddColumn( "charge",        DBTYPE_R8 );
    header->AddColumn( "skillPoints",   DBTYPE_I4 );
    header->AddColumn( "armorDamage",   DBTYPE_R8 );
    header->AddColumn( "shieldCharge",  DBTYPE_R8 );
    header->AddColumn( "incapacitated", DBTYPE_BOOL );

    PyPackedRow* row = new PyPackedRow( header );
    GetItemStatusRow( row );

    return row;
}

void InventoryItem::GetItemStatusRow( PyPackedRow* into ) const
{
    EvilNumber value;
    into->SetField( "instanceID",    new PyLong( itemID() ) );
    into->SetField("online", new PyBool((m_AttributeMap.fetchAttribute(AttrIsOnline, value) ? value.get_int() != 0 : false)));
    into->SetField("damage", new PyFloat((m_AttributeMap.fetchAttribute(AttrDamage, value) ? value.get_float() : 0)));
    into->SetField("charge", new PyFloat((m_AttributeMap.fetchAttribute(AttrCharge, value) ? value.get_float() : 0)));
    into->SetField("skillPoints", new PyInt((m_AttributeMap.fetchAttribute(AttrSkillPoints, value) ? value.get_int() : 0)));
    into->SetField("armorDamage", new PyFloat((m_AttributeMap.fetchAttribute(AttrArmorDamageAmount, value) ? value.get_float() : 0.0)));
    into->SetField("shieldCharge", new PyFloat((m_AttributeMap.fetchAttribute(AttrShieldCharge, value) ? value.get_float() : 0.0)));
    into->SetField("incapacitated", new PyBool((m_AttributeMap.fetchAttribute(AttrIsIncapacitated, value) ? value.get_int() != 0 : false)));
}

bool InventoryItem::Populate(Rsp_CommonGetInfo_Entry& result)
{
    //itemID:
    result.itemID = itemID();

    //invItem:
    PySafeDecRef( result.invItem );
    result.invItem = getPackedRow();

    //hacky, but it doesn't really hurt anything.
    if (getAttribute(AttrIsOnline).get_int() != 0)
    {
        //there is an effect that goes along with this. We should
        //probably be properly tracking the effect due to some
        // timer things, but for now, were hacking it.
        EntityEffectState es;
        es.env_itemID = itemID();
        es.env_charID = ownerID();  //may not be quite right...
        es.env_shipID = locationID();
        es.env_target = new PyNone(); //this is what they do.
        es.env_other = new PyNone();
        es.env_area = new PyList();
        es.env_effectID = effectOnline;
        es.startTime = Win32TimeNow() - Win32Time_Second;
        es.duration = -1;
        es.repeat = 0;

        result.activeEffects[es.env_effectID] = es.Encode();
    }
    // TO-DO: add other effects.

    //activeEffects:
    //result..activeEffects[id] = List[11];

    //attributes:
    AttributeMap::AttrMapItr itr = m_AttributeMap.begin();
    AttributeMap::AttrMapItr itr_end = m_AttributeMap.end();
    for (; itr != itr_end; itr++) {
        result.attributes[(*itr).first] = (*itr).second.GetPyObject();
    }

    //no idea what time this is supposed to be
    result.time = Win32TimeNow();
    result.wallclockTime = Win32TimeNow();

    return true;
}

PyDict *InventoryItem::getEffectDict()
{
    PyDict *dict = new PyDict();
    if (getAttribute(AttrIsOnline).get_int() != 0)
    {
        EntityEffectState es;
        es.env_itemID = itemID();
        es.env_charID = ownerID();  //may not be quite right...
        es.env_shipID = locationID();
        es.env_target = new PyNone(); //this is what they do.
        es.env_other = new PyNone();
        es.env_area = new PyList();
        es.env_effectID = effectOnline;
        es.startTime = Win32TimeNow() - Win32Time_Second;
        es.duration = -1;
        es.repeat = 1;

        dict->SetItem(new PyInt(es.env_effectID), es.Encode());
    }
    return dict;
}

PyObject * InventoryItem::ItemGetInfo()
{
    Rsp_ItemGetInfo result;

    if(!Populate(result.entry))
        return NULL;    //print already done.

    return(result.Encode());
}

void InventoryItem::Rename(const char *to)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return;
    }

    m_itemName = to;
    std::string nameEsc;
    DBcore::DoEscapeString(nameEsc, m_itemName);

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET itemName = '%s' WHERE itemID = %u",
                          nameEsc.c_str(), m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return;
    }
}

void InventoryItem::MoveInto(Inventory &new_home, EVEItemFlags _flag, bool notify) {
    Move( new_home.inventoryID(), _flag, notify );
}

void InventoryItem::Move(uint32 new_location, EVEItemFlags new_flag, bool notify)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return;
    }

    uint32 old_location = locationID();
    EVEItemFlags old_flag = flag();

    if( new_location == old_location && new_flag == old_flag )
        return; //nothing to do...

    //first, take myself out of my old inventory, if its loaded.
    Inventory *old_inventory = ItemFactory::GetInventory(old_location, false);
    if(old_inventory != NULL)
        old_inventory->RemoveItem( InventoryItemRef( this ) );  //releases its ref

    m_locationID = new_location;
    m_flag = new_flag;

    //then make sure that my new inventory is updated, if its loaded.
    Inventory *new_inventory = ItemFactory::GetInventory(new_location, false);
    if( new_inventory != NULL )
        new_inventory->AddItem( InventoryItemRef( this ) ); //makes a new ref

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET locationID = %u, flag = %u WHERE itemID = %u",
                          m_locationID, uint32(m_flag), m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return;
    }

    //notify about the changes.
    if( notify )
    {
        sendItemChangeNotice(EntityList::FindCharacter(m_ownerID));
    }
}

bool InventoryItem::AlterQuantity(int32 qty_change, bool notify) {
    if(qty_change == 0)
        return true;

    int32 new_qty = m_quantity + qty_change;

    if(new_qty < 0) {
        codelog(ITEM__ERROR, "%s (%u): Tried to remove %d quantity from stack of %u", m_itemName.c_str(), m_itemID, -qty_change, m_quantity);
        return false;
    }

    return(SetQuantity(new_qty, notify));
}

bool InventoryItem::SetQuantity(uint32 qty_new, bool notify)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return false;
    }

    //if an object has its singleton set then it shouldn't be able to add/remove qty
    if(m_singleton) {
        //Print error
        codelog(ITEM__ERROR, "%s (%u): Failed to set quantity %u , the items singleton bit is set", m_itemName.c_str(), m_itemID, qty_new);
        //return false
        return false;
    }

    uint32 old_qty = m_quantity;

    m_quantity = qty_new;

    if(m_quantity == 0)
    {
        Delete(false);
    }
    else
    {
        DBerror err;
        if(!DBcore::RunQuery(err,
                             "UPDATE srvEntity SET quantity = %u WHERE itemID = %u",
                             m_quantity, m_itemID))
        {
            _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
            return false;
        }
    }

    //notify about the changes.
    if(notify)
    {
        sendItemChangeNotice(EntityList::FindCharacter(m_ownerID));
    }

    return true;
}
bool InventoryItem::SetFlag(EVEItemFlags new_flag, bool notify)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return false;
    }

    EVEItemFlags old_flag = m_flag;
    m_flag = new_flag;

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET flag = %u WHERE itemID = %u",
                          uint32(m_flag), m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return false;
    }

    if(notify)
    {
        sendItemChangeNotice(EntityList::FindCharacter(m_ownerID));
    }
    return true;
}

InventoryItemRef InventoryItem::Split(int32 qty_to_take, bool notify) {
    if(qty_to_take <= 0) {
        _log(ITEM__ERROR, "%s (%u): Asked to split into a chunk of %d", itemName().c_str(), itemID(), qty_to_take);
        return InventoryItemRef();
    }
    if(!AlterQuantity(-qty_to_take, notify)) {
        _log(ITEM__ERROR, "%s (%u): Failed to remove quantity %d during split.", itemName().c_str(), itemID(), qty_to_take);
        return InventoryItemRef();
    }

    ItemData idata(
        typeID(),
        ownerID(),
        (notify ? 1 : locationID()), //temp location to cause the spawn via update
        flag(),
        qty_to_take
    );

    InventoryItemRef res = ItemFactory::SpawnItem(idata);
    if(notify)
        res->Move( locationID(), flag() );

    return( res );
}

bool InventoryItem::Merge(InventoryItemRef to_merge, int32 qty, bool notify) {
    if(typeID() != to_merge->typeID()) {
        _log(ITEM__ERROR, "%s (%u): Asked to merge with %s (%u).", itemName().c_str(), itemID(), to_merge->itemName().c_str(), to_merge->itemID());
        return false;
    }
    if(locationID() != to_merge->locationID() || flag() != to_merge->flag()) {
        _log(ITEM__ERROR, "%s (%u) in location %u, flag %u: Asked to merge with item %u in location %u, flag %u.", itemName().c_str(), itemID(), locationID(), flag(), to_merge->itemID(), to_merge->locationID(), to_merge->flag());
        return false;
    }
    if(qty == 0)
        qty = to_merge->quantity();
    if(qty <= 0) {
        _log(ITEM__ERROR, "%s (%u): Asked to merge with %d units of item %u.", itemName().c_str(), itemID(), qty, to_merge->itemID());
        return false;
    }
    if(!AlterQuantity(qty, notify)) {
        _log(ITEM__ERROR, "%s (%u): Failed to add quantity %d.", itemName().c_str(), itemID(), qty);
        return false;
    }

    if(qty == to_merge->quantity()) {
        to_merge->Delete();
    } else if(!to_merge->AlterQuantity(-qty, notify)) {
        _log(ITEM__ERROR, "%s (%u): Failed to remove quantity %d.", to_merge->itemName().c_str(), to_merge->itemID(), qty);
        return false;
    }

    return true;
}

bool InventoryItem::ChangeSingleton(bool new_singleton, bool notify)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return false;
    }

    bool old_singleton = m_singleton;

    if(new_singleton == old_singleton)
        return true;    //nothing to do...

    m_singleton = new_singleton;

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET singleton = %u WHERE itemID = %u",
                          uint32(m_singleton), m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return false;
    }

    //notify about the changes.
    if(notify)
    {
        sendItemChangeNotice(EntityList::FindCharacter(m_ownerID));
    }

    return true;
}

void InventoryItem::ChangeOwner(uint32 new_owner, bool notify)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return;
    }

    uint32 old_owner = m_ownerID;

    if(new_owner == old_owner)
        return; //nothing to do...

    m_ownerID = new_owner;

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET ownerID = %u WHERE itemID = %u",
                          m_ownerID, m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return;
    }

    //notify about the changes.
    if(notify)
    {
        sendItemChangeNotice(EntityList::FindCharacter(m_ownerID));
    }
}

PyPackedRow *InventoryItem::getPackedRow()
{
    PyList *keywords = new PyList();
    keywords->AddItem(new_tuple(new PyString("stacksize"), new PyToken("eve.common.script.sys.eveCfg.StackSize")));
    keywords->AddItem(new_tuple(new PyString("singleton"), new PyToken("eve.common.script.sys.eveCfg.Singleton")));

    DBRowDescriptor* header = new DBRowDescriptor(keywords);
    header->AddColumn("itemID", DBTYPE_I8);
    header->AddColumn("typeID", DBTYPE_I4);
    header->AddColumn("ownerID", DBTYPE_I4);
    header->AddColumn("locationID", DBTYPE_I8);
    header->AddColumn("flagID", DBTYPE_I2);
    header->AddColumn("quantity", DBTYPE_I4);
    header->AddColumn("groupID", DBTYPE_I4);
    header->AddColumn("categoryID", DBTYPE_I4);
    header->AddColumn("customInfo", DBTYPE_STR);

    // Create the packed row.
    PyPackedRow* row = new PyPackedRow(header);
    getPackedRow(row);

    return row;
}

void InventoryItem::getPackedRow(PyPackedRow* into) const
{
    into->SetField("itemID", new PyLong(m_itemID));
    into->SetField("typeID", new PyInt(m_type->typeID));
    into->SetField("ownerID", new PyInt(m_ownerID));
    into->SetField("locationID", new PyLong(m_locationID));
    into->SetField("flagID", new PyInt(m_flag));
    uint32 qty = m_singleton ? -1 : m_quantity;
    if(m_type->groupID == EVEDB::invCategories::Blueprint)
    {
        BlueprintRef bp = ItemFactory::GetBlueprint(m_itemID);
        if(bp.get() != nullptr)
        {
            qty = bp->copy() ? -2 : qty;
        }
    }
    into->SetField("quantity", new PyInt(qty));
    into->SetField("groupID", new PyInt(m_type->groupID));
    into->SetField("categoryID", new PyInt(m_type->getCategoryID()));
    into->SetField("customInfo", new PyString(m_customInfo));
}

PyPackedRow *InventoryItem::getStateRow()
{
    DBRowDescriptor* header = new DBRowDescriptor();
    header->AddColumn("instanceID", DBTYPE_I8);
    header->AddColumn("online", DBTYPE_BOOL);
    header->AddColumn("damage", DBTYPE_R8);
    header->AddColumn("charge", DBTYPE_R8);
    header->AddColumn("skillPoints", DBTYPE_I4);
    header->AddColumn("armorDamage", DBTYPE_R8);
    header->AddColumn("shieldCharge", DBTYPE_R8);
    header->AddColumn("incapacitated", DBTYPE_BOOL);

    bool online = false;
    double damage = 0;
    double charge = 0;
    int skillPoints = 0;
    double armorDamage = 0;
    double shieldCharge = 0;
    int incapacitated = false;
    fetchAttribute(AttrDamage, damage);
    fetchAttribute(AttrCharge, charge);
    fetchAttribute(AttrArmorDamage, armorDamage);
    fetchAttribute(AttrSkillPoints, skillPoints);
    fetchAttribute(AttrShieldCharge, shieldCharge);
    fetchAttribute(AttrIsIncapacitated, incapacitated);

    // Create the packed row.
    PyPackedRow* row = new PyPackedRow(header);
    row->SetField("instanceID", new PyLong(m_itemID));
    row->SetField("online", new PyBool(online));
    row->SetField("damage", new PyFloat(damage));
    row->SetField("charge", new PyFloat(charge));
    row->SetField("skillPoints", new PyInt(skillPoints));
    row->SetField("armorDamage", new PyFloat(armorDamage));
    row->SetField("shieldCharge", new PyFloat(shieldCharge));
    row->SetField("incapacitated", new PyBool(incapacitated != 0));

    return row;
}

void InventoryItem::sendItemChangeNotice(Client *client)
{
    if(client != NULL)
    {
        PyList *items = new PyList();
        items->AddItem(getPackedRow());
        PyDict *dict = new PyDict();
        dict->SetItem(new PyInt(9), new PyInt(1));
        PyTuple *tuple = new_tuple(items, dict);
        PyTuple *newQueue = new_tuple(new PyInt(0), new_tuple(new PyInt(0), new_tuple(new PyInt(1), tuple)));
        client->SendNotification("OnItemsChanged", "charid", &newQueue, false);
    }
}

void InventoryItem::SaveItem()
{
    //_log( ITEM__TRACE, "Saving item %u.", itemID() );

    SaveAttributes();

    InventoryDB::SaveItem(
        itemID(),
        ItemData(
            itemName().c_str(),
            typeID(),
            ownerID(),
            locationID(),
            flag(),
            contraband(),
            singleton(),
            quantity(),
            position(),
            customInfo().c_str()
        )
    );
}

/*typedef enum {
    dgmEffPassive = 0,
    dgmEffActivation = 1,
    dgmEffTarget = 2,
    dgmEffArea = 3,
    dgmEffOnline = 4,
    dgmEffOverload = 5,
} EffectCategories;*/


void InventoryItem::SetOnline(bool online)
{

    setAttribute(AttrIsOnline, int(online));

    Client *c = EntityList::FindCharacter(m_ownerID);
    if(c == NULL)
    {
        SysLog::Error("InventoryItem", "unable to set ourselfs online//offline because we can't find the client");
        return;
    }

    Notify_OnGodmaShipEffect ogf;
    ogf.itemID = m_itemID;
    ogf.effectID = effectOnline;
    ogf.when = Win32TimeNow();
    ogf.start = online?1:0;
    ogf.active = online?1:0;

	PyList *environment = new PyList;
	environment->AddItem(new PyInt(ogf.itemID));
	environment->AddItem(new PyInt(m_ownerID));
	environment->AddItem(new PyInt(m_locationID));
	environment->AddItem(new PyNone);
	environment->AddItem(new PyNone);
	environment->AddItem(new PyNone);
	environment->AddItem(new PyInt(ogf.effectID));

	ogf.environment = environment;
	ogf.startTime = ogf.when;
	ogf.duration = 10000;
	ogf.repeat = online?new PyInt(1000):new PyInt(0);
    ogf.randomSeed = new PyNone();
    ogf.error = new PyNone();

    Notify_OnMultiEvent multi;
    multi.events = new PyList;
    multi.events->AddItem( ogf.Encode() );

    PyTuple* tmp = multi.Encode();   //this is consumed below
    c->SendNotification("OnMultiEvent", "clientID", &tmp);
}

void InventoryItem::SetActive(bool active, uint32 effectID, double duration, bool repeat)
{
	Client* c = EntityList::FindCharacter(m_ownerID);
    if(c == NULL)
    {
        SysLog::Error("InventoryItem", "unable to set ourselfs online//offline because we can't find the client");
        return;
    }

	Notify_OnGodmaShipEffect shipEffect;

	shipEffect.itemID = m_itemID;
	shipEffect.effectID = effectID;
	shipEffect.when = Win32TimeNow();
	shipEffect.start = active?1:0;
	shipEffect.active = active?1:0;

	PyList* env = new PyList;
	env->AddItem(new PyInt(m_itemID));
	env->AddItem(new PyInt(ownerID()));
	env->AddItem(new PyInt(m_locationID));
	env->AddItem(new PyNone);				//targetID
	env->AddItem(new PyNone);				//otherID
	env->AddItem(new PyNone);				//area
	env->AddItem(new PyInt(effectID));

	shipEffect.environment = env;
	shipEffect.startTime = shipEffect.when;
	shipEffect.duration = duration;
	shipEffect.repeat = repeat?new PyInt(1000):new PyInt(0);
	shipEffect.randomSeed = new PyNone;
	shipEffect.error = new PyNone;

	Notify_OnMultiEvent multi;
    multi.events = new PyList;
    multi.events->AddItem(shipEffect.Encode());

    PyTuple* tmp = multi.Encode();   //this is consumed below
    c->SendNotification("OnMultiEvent", "clientID", &tmp);
}

void InventoryItem::SetCustomInfo(const char *ci)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return;
    }

    if (ci == NULL)
        m_customInfo = "";
    else
        m_customInfo = ci;

    std::string customInfoEsc;
    DBcore::DoEscapeString(customInfoEsc, m_customInfo);

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET customInfo = '%s' WHERE itemID = %u",
                          customInfoEsc.c_str(), m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return;
    }
}

void InventoryItem::Relocate(const Vector3D &pos)
{
    if (IsStaticMapItem(m_itemID))
    {
        _log(DATABASE__ERROR, "Refusing to modify static map object %u.", m_itemID);
        return;
    }

    if (m_position == pos)
        return;

    m_position = pos;

    DBerror err;
    if (!DBcore::RunQuery(err,
                          "UPDATE srvEntity SET x = %f, y = %f, z = %f WHERE itemID = %u",
                          m_position.x, m_position.y, m_position.z, m_itemID))
    {
        _log(DATABASE__ERROR, "Error in query: %s.", err.c_str());
        return;
    }
}

bool InventoryItem::setAttribute(uint32 attributeID, int64 num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    EvilNumber devil_number(num);
    bool status = m_AttributeMap.SetAttribute(attributeID, devil_number, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, devil_number, notify);

	return status;
}

bool InventoryItem::setAttribute(uint32 attributeID, double num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    EvilNumber devil_number(num);
    bool status = m_AttributeMap.SetAttribute(attributeID, devil_number, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, devil_number, notify);

	return status;
}

bool InventoryItem::setAttribute(uint32 attributeID, EvilNumber num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    bool status = m_AttributeMap.SetAttribute(attributeID, num, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, num, notify);

	return status;
}

bool InventoryItem::setAttribute(uint32 attributeID, int num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    EvilNumber devil_number(num);
    bool status = m_AttributeMap.SetAttribute(attributeID, devil_number, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, devil_number, notify);

	return status;
}

bool InventoryItem::setAttribute(uint32 attributeID, uint64 num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    EvilNumber devil_number(*((int64*)&num));
    bool status = m_AttributeMap.SetAttribute(attributeID, devil_number, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, devil_number, notify);

	return status;
}

bool InventoryItem::setAttribute(uint32 attributeID, uint32 num, bool notify /* true */, bool shadow_copy_to_default_set /* false */)
{
    EvilNumber devil_number((int64)num);
    bool status = m_AttributeMap.SetAttribute(attributeID, devil_number, notify);
	if(shadow_copy_to_default_set)
        status = status && m_DefaultAttributeMap.SetAttribute(attributeID, devil_number, notify);

	return status;
}

EvilNumber InventoryItem::getDefaultAttribute(const uint32 attributeID) const
{
    return m_DefaultAttributeMap.GetAttribute(attributeID);
}

EvilNumber InventoryItem::getAttribute(const uint32 attributeID) const
{
    return m_AttributeMap.GetAttribute(attributeID);
}

PyDict *InventoryItem::getAttributeDict()
{
    PyDict *dict = new PyDict();
    AttributeMap::AttrMapItr begin = m_AttributeMap.begin();
    AttributeMap::AttrMapItr end = m_AttributeMap.end();
    while(begin != end)
    {
        uint32 id = begin->first;
        EvilNumber num = begin->second;
        dict->SetItem(new PyInt(id), num.GetPyObject());
        begin++;
    }
    return dict;
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, EvilNumber &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, double &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, float &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, int32 &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, uint32 &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, int64 &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::fetchAttribute(const uint32 attributeID, uint64 &value) const
{
    return m_AttributeMap.fetchAttribute(attributeID, value);
}

bool InventoryItem::hasAttribute(const uint32 attributeID) const
{
    return m_AttributeMap.HasAttribute(attributeID);
}

bool InventoryItem::SaveAttributes()
{
    return (m_AttributeMap.SaveAttributes() && m_DefaultAttributeMap.SaveAttributes());
}

bool InventoryItem::ResetAttribute(uint32 attrID, bool notify)
{
    if (attrID >= 10000)
    {
        // These are out of range attributes that the client does not like!
        return true;
    }
    bool success = m_AttributeMap.ResetAttribute(attrID, true);
    EvilNumber nVal = m_AttributeMap.GetAttribute(attrID);
    // modify the value by attribute modifiers applied by modules and enemy's.
    double amount = 0;
    AttributeModifierSource::FactorList factors;
    AttributeModifierSource::FactorList stackedfactors;
    for (auto src : m_attributeModifiers)
    {
        if (src.get() == NULL)
        {
            continue;
        }
        src->getModification(attrID, amount, factors, stackedfactors);
    }
    double value = AttributeModifierSource::finalizeModification(nVal.get_float(), amount, factors, stackedfactors);
    if (nVal.get_type() == EVIL_NUMBER_TYPE::evil_number_int)
    {
        nVal = EvilNumber((int64) value);
    }
    else
    {
        nVal = EvilNumber((double) value);
    }
    return m_AttributeMap.SetAttribute(attrID, nVal, notify);
}

void InventoryItem::AddAttributeModifier(AttributeModifierSourceRef modifier)
{
    if (modifier.get() == NULL)
        return;
    if (std::find(m_attributeModifiers.begin(), m_attributeModifiers.end(), modifier) == m_attributeModifiers.end())
    {
        m_attributeModifiers.push_back(modifier);
        modifier->updateModifiers(this, true);
    }
}

void InventoryItem::RemoveAttributeModifier(AttributeModifierSourceRef modifier)
{
    std::vector<AttributeModifierSourceRef>::iterator itr = std::find(m_attributeModifiers.begin(), m_attributeModifiers.end(), modifier);
    if (itr != m_attributeModifiers.end())
    {
        AttributeModifierSourceRef src = *itr;
        m_attributeModifiers.erase(itr);
        src->updateModifiers(this, true);
    }
}
