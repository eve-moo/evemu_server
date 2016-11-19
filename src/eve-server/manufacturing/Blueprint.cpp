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

#include "manufacturing/Blueprint.h"

/*
 * BlueprintData
 */
BlueprintData::BlueprintData(
    bool _copy,
    uint32 _materialLevel,
    uint32 _productivityLevel,
    int32 _licensedProductionRunsRemaining)
: copy(_copy),
  materialLevel(_materialLevel),
  productivityLevel(_productivityLevel),
  licensedProductionRunsRemaining(_licensedProductionRunsRemaining)
{
}

/*
 * Blueprint
 */
Blueprint::Blueprint(
    uint32 _blueprintID,
    // InventoryItem stuff:
                     const InvTypeRef _bpType,
    const ItemData &_data,
    // Blueprint stuff:
    const BlueprintData &_bpData)
: InventoryItem(_blueprintID, _bpType, _data),
  m_copy(_bpData.copy),
  m_materialLevel(_bpData.materialLevel),
  m_productivityLevel(_bpData.productivityLevel),
  m_licensedProductionRunsRemaining(_bpData.licensedProductionRunsRemaining)
{
    // data consistency asserts
    assert(_bpType->getCategoryID() == EVEDB::invCategories::Blueprint);
    m_blueprintType = InvBlueprintType::getBlueprintType(m_type->typeID);
}

BlueprintRef Blueprint::Load(uint32 blueprintID)
{
    return InventoryItem::Load<Blueprint>( blueprintID );
}

BlueprintRef Blueprint::Spawn(ItemData &data, BlueprintData &bpData) {
    uint32 blueprintID = Blueprint::_Spawn(data, bpData);
    if(blueprintID == 0)
    {
        return BlueprintRef();
    }
    return Blueprint::Load(blueprintID);
}

uint32 Blueprint::_Spawn(ItemData &data, BlueprintData &bpData)
{
    // make sure it's a blueprint type
    const InvTypeRef bt = InvType::getType(data.typeID);
    if (bt.get() == nullptr)
    {
        return 0;
    }

    // get the blueprintID
    uint32 blueprintID = InventoryItem::_Spawn(data);
    if(blueprintID == 0)
    {
        return 0;
    }

    // insert blueprint entry into DB
    if(!InventoryDB::NewBlueprint(blueprintID, bpData))
    {
        // delete item
        InventoryDB::DeleteItem(blueprintID);

        return 0;
    }

    return blueprintID;
}

void Blueprint::Delete() {
    // delete our blueprint record
    InventoryDB::DeleteBlueprint(m_itemID);
    // redirect to parent
    InventoryItem::Delete();
}

BlueprintRef Blueprint::SplitBlueprint(int32 qty_to_take, bool notify) {
    // split item
    BlueprintRef res = BlueprintRef::StaticCast( InventoryItem::Split( qty_to_take, notify ) );
    if( !res )
        return BlueprintRef();

    // copy our attributes
    res->SetCopy(m_copy);
    res->SetMaterialLevel(m_materialLevel);
    res->SetProductivityLevel(m_productivityLevel);
    res->SetLicensedProductionRunsRemaining(m_licensedProductionRunsRemaining);

    return res;
}

bool Blueprint::Merge(InventoryItemRef to_merge, int32 qty, bool notify) {
    if( !InventoryItem::Merge( to_merge, qty, notify ) )
        return false;
    // do something special? merge material level etc.?
    return true;
}

void Blueprint::SetCopy(bool copy) {
    m_copy = copy;

    SaveBlueprint();
}

void Blueprint::SetMaterialLevel(uint32 materialLevel) {
    m_materialLevel = materialLevel;

    SaveBlueprint();
}

bool Blueprint::AlterMaterialLevel(int32 materialLevelChange) {
    int32 new_material_level = m_materialLevel + materialLevelChange;

    if(new_material_level < 0) {
        _log(ITEM__ERROR, "%s (%u): Tried to remove %u material levels while having %u levels.", m_itemName.c_str(), m_itemID, -materialLevelChange, m_materialLevel);
        return false;
    }

    SetMaterialLevel(new_material_level);
    return true;
}

void Blueprint::SetProductivityLevel(uint32 productivityLevel) {
    m_productivityLevel = productivityLevel;

    SaveBlueprint();
}

bool Blueprint::AlterProductivityLevel(int32 producitvityLevelChange) {
    int32 new_productivity_level = m_productivityLevel + producitvityLevelChange;

    if(new_productivity_level < 0) {
        _log(ITEM__ERROR, "%s (%u): Tried to remove %u productivity levels while having %u levels.", m_itemName.c_str(), m_itemID, -producitvityLevelChange, m_productivityLevel);
        return false;
    }

    SetProductivityLevel(new_productivity_level);
    return true;
}

void Blueprint::SetLicensedProductionRunsRemaining(int32 licensedProductionRunsRemaining) {
    m_licensedProductionRunsRemaining = licensedProductionRunsRemaining;

    SaveBlueprint();
}

void Blueprint::AlterLicensedProductionRunsRemaining(int32 licensedProductionRunsRemainingChange) {
    int32 new_licensed_production_runs_remaining = m_licensedProductionRunsRemaining + licensedProductionRunsRemainingChange;

    SetLicensedProductionRunsRemaining(new_licensed_production_runs_remaining);
}

PyDict *Blueprint::GetBlueprintAttributes() const {
    Rsp_GetBlueprintAttributes rsp;

    // fill in our attribute info
    rsp.blueprintID = itemID();
    rsp.copy = copy() ? 1 : 0;
    rsp.productivityLevel = productivityLevel();
    rsp.materialLevel = materialLevel();
    rsp.licensedProductionRunsRemaining = licensedProductionRunsRemaining();
    rsp.wastageFactor = wasteFactor();

    rsp.productTypeID = m_blueprintType->productTypeID;
    rsp.manufacturingTime = m_blueprintType->productionTime;
    rsp.maxProductionLimit = m_blueprintType->maxProductionLimit;
    rsp.researchMaterialTime = m_blueprintType->researchMaterialTime;
    rsp.researchTechTime = m_blueprintType->researchTechTime;
    rsp.researchProductivityTime = m_blueprintType->researchProductivityTime;
    rsp.researchCopyTime = m_blueprintType->researchCopyTime;

    return(rsp.Encode());
}

void Blueprint::SaveBlueprint() const
{
    _log( ITEM__TRACE, "Saving blueprint %u.", itemID() );

    InventoryDB::SaveBlueprint(
        itemID(),
        BlueprintData(
            copy(),
            materialLevel(),
            productivityLevel(),
            licensedProductionRunsRemaining()
        )
    );
}

PyPackedRow *Blueprint::getBlueprintRow() const
{
    DBRowDescriptor* header = new DBRowDescriptor();
    header->AddColumn("itemID", DBTYPE_I8);
    header->AddColumn("typeID", DBTYPE_I4);
    header->AddColumn("ownerID", DBTYPE_I4);
    header->AddColumn("locationID", DBTYPE_I8);
    header->AddColumn("locationTypeID", DBTYPE_I4);
    header->AddColumn("locationFlagID", DBTYPE_I4);
    header->AddColumn("flagID", DBTYPE_I2);
    header->AddColumn("quantity", DBTYPE_I4);
    header->AddColumn("timeEfficiency", DBTYPE_I4);
    header->AddColumn("materialEfficiency", DBTYPE_I4);
    header->AddColumn("runs", DBTYPE_I4);
    header->AddColumn("productTypeID", DBTYPE_I4);
    header->AddColumn("facilityID", DBTYPE_I8);
    header->AddColumn("jobID", DBTYPE_I4);

    // Create the packed row.
    PyPackedRow* row = new PyPackedRow(header);
    getBlueprintRow(row);

    return row;
}

void Blueprint::getBlueprintRow(PyPackedRow* into) const
{
    int32 locationType = 0;
    InventoryItemRef loc = ItemFactory::GetItem(m_locationID);
    if(loc.get() != nullptr)
    {
        locationType = loc->typeID();
    }
    into->SetField("itemID", new PyLong(m_itemID));
    into->SetField("typeID", new PyInt(m_type->typeID));
    into->SetField("ownerID", new PyInt(m_ownerID));
    into->SetField("locationID", new PyLong(m_locationID));
    // For corporate blueprints:
    // locationID will be The itemID of the corporations station office.
    // locationTypeID will be 27 ("Office") possibly others?
    // locationFlagID will be 4, 116-121 for the hanger it's in.
    into->SetField("locationTypeID", new PyInt(locationType));
    into->SetField("locationFlagID", new PyInt(m_flag));
    into->SetField("flagID", new PyInt(m_flag));
    uint32 qty = m_singleton ? -1 : m_quantity;
    qty = m_copy ? -2 : qty;
    into->SetField("quantity", new PyInt(qty));
    into->SetField("timeEfficiency", new PyInt(m_productivityLevel));
    into->SetField("materialEfficiency", new PyInt(m_materialLevel));
    int runs = m_copy ? m_licensedProductionRunsRemaining : -1;
    into->SetField("runs", new PyInt(runs));
    into->SetField("facilityID", new PyLong(m_locationID));
    int jobID = 0;
    int productType = 0;
    // TO-DO: if the blueprint is being used set it's job id  and product typeID here.
    into->SetField("productTypeID", new PyInt(productType));
    into->SetField("jobID", new PyInt(jobID));
}
