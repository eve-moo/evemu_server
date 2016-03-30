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

#include "eve-server.h"

#include "ship/modules/ModuleEffects.h"
#include "inv/InvType.h"

std::map<uint32, std::shared_ptr<MEffect>> DGM_Effects_Table::m_EffectsMap;

// ////////////////// MEffect Class ///////////////////////////
MEffect::MEffect(uint32 effectID)
{
    m_EffectID = effectID;
    m_EffectID = 0;
    m_EffectName = "";
    m_EffectCategory = 0;
    m_PreExpression = 0;
    m_PostExpression = 0;
    m_Description = "";
    m_Guid = "";
    m_IconID = 0;
    m_IsOffensive = 0;
    m_IsAssistance = 0;
    m_DurationAttributeID = 0;
    m_TrackingSpeedAttributeID = 0;
    m_DischargeAttributeID = 0;
    m_RangeAttributeID = 0;
    m_FalloffAttributeID = 0;
    m_DisallowAutoRepeat = 0;
    m_Published = 0;
    m_DisplayName = "";
    m_IsWarpSafe = 0;
    m_RangeChance = 0;
    m_ElectronicChance = 0;
    m_PropulsionChance = 0;
    m_Distribution = 0;
    m_SfxName = "";
    m_NpcUsageChanceAttributeID = 0;
    m_NpcActivationChanceAttributeID = 0;
    m_FittingUsageChanceAttributeID = 0;

    m_numOfIDs = 0;
	m_SourceAttributeIDs = NULL;
	m_TargetAttributeIDs = NULL;
	m_CalculationTypeIDs = NULL;
	m_ReverseCalculationTypeIDs = NULL;
	m_StackingPenaltyAppliedIDs = NULL;
	m_EffectAppliedInStateIDs = NULL;
	m_AffectingIDs = NULL;
	m_AffectingTypes = NULL;
	m_AffectedTypes = NULL;

	m_EffectLoaded = false;
	m_EffectsInfoLoaded = false;

	_Populate(effectID);
}

MEffect::~MEffect()
{
    if( m_numOfIDs > 0 )
	{
		delete m_SourceAttributeIDs;
		delete m_TargetAttributeIDs;
		delete m_CalculationTypeIDs;
		delete m_ReverseCalculationTypeIDs;
		delete m_StackingPenaltyAppliedIDs;
		delete m_EffectAppliedInStateIDs;
		delete m_AffectingIDs;
		delete m_AffectingTypes;
		delete m_AffectedTypes;
	}
}

void MEffect::_Populate(uint32 effectID)
{
    DBQueryResult *res = new DBQueryResult();
    ModuleDB::GetDgmEffects(effectID, *res);

    // First, get all general info on this effectID from the dgmEffects table:
    DBResultRow row1;
    if( !res->GetRow(row1))
        SysLog::Error("MEffect", "Could not populate effect information for effectID: %u from the 'dgmEffects' table", effectID);
    else
    {
        //get all the data from the query
        m_EffectID = effectID;
        m_EffectName = row1.GetText(0);
		m_EffectCategory = row1.GetUInt(1);
        m_PreExpression = row1.GetUInt(2);
        m_PostExpression = row1.GetUInt(3);
        if( !row1.IsNull(4) )
            m_Description = row1.GetText(4);
        if( !row1.IsNull(5) )
            m_Guid = row1.GetText(5);
        if( !row1.IsNull(6) )
            m_IconID = row1.GetUInt(6);
        m_IsOffensive = row1.GetUInt(7);
        m_IsAssistance = row1.GetUInt(8);
        if( !row1.IsNull(9) )
            m_DurationAttributeID = row1.GetUInt(9);
        if( !row1.IsNull(10) )
            m_TrackingSpeedAttributeID = row1.GetUInt(10);
        if( !row1.IsNull(11) )
            m_DischargeAttributeID = row1.GetUInt(11);
        if( !row1.IsNull(12) )
            m_RangeAttributeID = row1.GetUInt(12);
        if( !row1.IsNull(13) )
            m_FalloffAttributeID = row1.GetUInt(13);
        if( !row1.IsNull(14) )
            m_DisallowAutoRepeat = row1.GetUInt(14);
        m_Published = row1.GetUInt(15);
        if( !row1.IsNull(16) )
            m_DisplayName = row1.GetText(16);
        m_IsWarpSafe = row1.GetUInt(17);
        m_RangeChance = row1.GetUInt(18);
        m_ElectronicChance = row1.GetUInt(19);
        m_PropulsionChance = row1.GetUInt(20);
        if( !row1.IsNull(21) )
            m_Distribution = row1.GetUInt(21);
        if( !row1.IsNull(22) )
            m_SfxName = row1.GetText(22);
        if( !row1.IsNull(23) )
            m_NpcUsageChanceAttributeID = row1.GetUInt(23);
        if( !row1.IsNull(24) )
            m_NpcActivationChanceAttributeID = row1.GetUInt(24);
        if( !row1.IsNull(25) )
            m_FittingUsageChanceAttributeID = row1.GetUInt(25);
    }

    // Next, get the info from the dgmEffectsInfo table:
    ModuleDB::GetDgmEffectsInfo(effectID, *res);

    DBResultRow row2;

    // Initialize the new tables
	if( res->GetRowCount() > 0 )
	{
		m_SourceAttributeIDs = new uint32[res->GetRowCount()];
		m_TargetAttributeIDs = new uint32[res->GetRowCount()];
		m_CalculationTypeIDs = new uint32[res->GetRowCount()];
		m_ReverseCalculationTypeIDs = new uint32[res->GetRowCount()];
		m_StackingPenaltyAppliedIDs = new uint32[res->GetRowCount()];
		m_EffectAppliedInStateIDs = new uint32[res->GetRowCount()];
		m_AffectingIDs = new uint32[res->GetRowCount()];
		m_AffectingTypes = new uint32[res->GetRowCount()];
		m_AffectedTypes = new uint32[res->GetRowCount()];

		int count = 0;
		std::string targetGroupIDs;
		typeTargetGroupIDlist * TargetGroupIDs;

		while( res->GetRow(row2) )
		{
			m_SourceAttributeIDs[count] = row2.GetUInt(0);
			m_TargetAttributeIDs[count] = row2.GetUInt(1);
			m_CalculationTypeIDs[count] = row2.GetUInt(2);
			m_Descriptions.insert(std::pair<uint32,std::string>(count,row2.GetText(3)));
			m_ReverseCalculationTypeIDs[count] = row2.GetUInt(4);
			targetGroupIDs = row2.GetText(5);
			m_StackingPenaltyAppliedIDs[count] = row2.GetUInt(6);
			m_EffectAppliedInStateIDs[count] = row2.GetUInt(7);
			m_AffectingIDs[count] = row2.GetUInt(8);
			m_AffectingTypes[count] = row2.GetUInt(9);
			m_AffectedTypes[count] = row2.GetUInt(10);

			TargetGroupIDs = new typeTargetGroupIDlist;
			if( !(targetGroupIDs.empty()) )
			{
				// targetGroupIDs string is not empty, so extract one number at a time until it is empty
				int pos = 0;
				std::string tempString = "";

				pos = targetGroupIDs.find_first_of(';');
				if( pos < 0 )
					pos = targetGroupIDs.length()-1;	// we did not find any ';' characters, so targetGroupIDs contains only one number
				tempString = targetGroupIDs.substr(0,pos);

				while( (pos = targetGroupIDs.find_first_of(';')) > 0 )
				{
					tempString = targetGroupIDs.substr(0,pos);
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(tempString.c_str())));
					targetGroupIDs = targetGroupIDs.substr(pos+1,targetGroupIDs.length()-1);
				}

				// Get final number now that there are no more separators to find:
				if( !(targetGroupIDs.empty()) )
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(targetGroupIDs.c_str())));

				m_TargetGroupIDlists.insert(std::pair<uint32, typeTargetGroupIDlist *>(count, TargetGroupIDs));
			}

			count++;
		}

		if( count == 0 )
		{
			;//Log::Error("MEffect","Could not populate effect information for effectID: %u from the 'dgmEffectsInfo' table as the SQL query returned ZERO rows", effectID);
			m_EffectsInfoLoaded = false;
		}
		else
		{
			m_numOfIDs = count;
			m_EffectsInfoLoaded = true;
		}
	}
	else
		m_EffectsInfoLoaded = false;

	m_EffectLoaded = true;
    delete res;
    res = NULL;
}


// ////////////////////// SkillBonusModifier Class ////////////////////////////
SkillBonusModifier::SkillBonusModifier(uint32 skillID)
{
	m_SkillID = skillID;
    m_numOfIDs = 0;
	m_EffectIDs = NULL;
    m_SourceAttributeIDs = NULL;
    m_TargetAttributeIDs = NULL;
    m_CalculationTypeIDs = NULL;
    m_ReverseCalculationTypeIDs = NULL;
    m_TargetChargeSizes = NULL;
	m_AppliedPerLevelList = NULL;
	m_AffectingTypes = NULL;
	m_AffectedTypes = NULL;

	m_ModifierLoaded = false;

	_Populate(skillID);
}

SkillBonusModifier::~SkillBonusModifier()
{
    if( m_numOfIDs > 0 )
	{
		delete m_SourceAttributeIDs;
		delete m_TargetAttributeIDs;
		delete m_CalculationTypeIDs;
		delete m_ReverseCalculationTypeIDs;
		delete m_TargetChargeSizes;
		delete m_AppliedPerLevelList;
		delete m_AffectingTypes;
		delete m_AffectedTypes;
	}
}

void SkillBonusModifier::_Populate(uint32 skillID)
{
    DBQueryResult *res = new DBQueryResult();
    ModuleDB::GetDgmSkillBonusModifiers(skillID, *res);

    DBResultRow row1;
	if( res->GetRowCount() == 0 )
    {
        SysLog::Error("SkillBonusModifier", "Could not populate skill bonus modifier information for skillID: %u from the 'dgmSkillBonusModifiers' table", skillID);
		m_ModifierLoaded = false;
	}
    else
    {
		m_EffectIDs = new uint32[res->GetRowCount()];
		m_SourceAttributeIDs = new uint32[res->GetRowCount()];
		m_TargetAttributeIDs = new uint32[res->GetRowCount()];
		m_CalculationTypeIDs = new uint32[res->GetRowCount()];
		m_ReverseCalculationTypeIDs = new uint32[res->GetRowCount()];
		m_TargetChargeSizes = new uint32[res->GetRowCount()];
		m_AppliedPerLevelList = new uint32[res->GetRowCount()];
		m_AffectingTypes = new uint32[res->GetRowCount()];
		m_AffectedTypes = new uint32[res->GetRowCount()];

		int count = 0;
		std::string targetGroupIDs;
		typeTargetGroupIDlist * TargetGroupIDs;

		while( res->GetRow(row1) )
		{
			m_EffectIDs[count] = row1.GetUInt(0);
			m_SourceAttributeIDs[count] = row1.GetUInt(1);
			m_TargetAttributeIDs[count] = row1.GetUInt(2);
			m_CalculationTypeIDs[count] = row1.GetUInt(3);
			m_Descriptions.insert(std::pair<uint32,std::string>(count,row1.GetText(4)));
			m_ReverseCalculationTypeIDs[count] = row1.GetUInt(5);
			targetGroupIDs = row1.GetText(6);
			m_TargetChargeSizes[count] = row1.GetUInt(7);
			m_AppliedPerLevelList[count] = row1.GetUInt(8);
			m_AffectingTypes[count] = row1.GetUInt(9);
			m_AffectedTypes[count] = row1.GetUInt(10);

			TargetGroupIDs = new typeTargetGroupIDlist;
			if( !(targetGroupIDs.empty()) )
			{
				// targetGroupIDs string is not empty, so extract one number at a time until it is empty
				int pos = 0;
				std::string tempString = "";

				pos = targetGroupIDs.find_first_of(';');
				if( pos < 0 )
					pos = targetGroupIDs.length()-1;	// we did not find any ';' characters, so targetGroupIDs contains only one number
				tempString = targetGroupIDs.substr(0,pos);

				while( (pos = targetGroupIDs.find_first_of(';')) > 0 )
				{
					tempString = targetGroupIDs.substr(0,pos);
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(tempString.c_str())));
					targetGroupIDs = targetGroupIDs.substr(pos+1,targetGroupIDs.length()-1);
				}

				// Get final number now that there are no more separators to find:
				if( !(targetGroupIDs.empty()) )
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(targetGroupIDs.c_str())));

				m_TargetGroupIDlists.insert(std::pair<uint32, typeTargetGroupIDlist *>(count, TargetGroupIDs));
			}

			count++;
		}

		if( count == 0 )
		{
			;//Log::Error("SkillBonusModifier","Could not populate bonus modifier information for skillID: %u from the 'dgmSkillBonusModifiers' table as the SQL query returned ZERO rows", skillID);
			m_ModifierLoaded = false;
		}
		else
		{
			m_numOfIDs = count;
			m_ModifierLoaded = true;
		}
	}

    delete res;
    res = NULL;
}


// ////////////////////// ShipBonusModifier Class ////////////////////////////
ShipBonusModifier::ShipBonusModifier(uint32 shipID)
{
	m_ShipID = shipID;
    m_numOfIDs = 0;
	m_EffectIDs = NULL;
	m_AttributeSkillIDs = NULL;
    m_SourceAttributeIDs = NULL;
    m_TargetAttributeIDs = NULL;
    m_CalculationTypeIDs = NULL;
    m_ReverseCalculationTypeIDs = NULL;
	m_AppliedPerLevelList = NULL;
	m_AffectingTypes = NULL;
	m_AffectedTypes = NULL;

	m_ModifierLoaded = false;

	_Populate(shipID);
}

ShipBonusModifier::~ShipBonusModifier()
{
    if( m_numOfIDs > 0 )
	{
		delete m_AttributeSkillIDs;
		delete m_SourceAttributeIDs;
		delete m_TargetAttributeIDs;
		delete m_CalculationTypeIDs;
		delete m_ReverseCalculationTypeIDs;
		delete m_AppliedPerLevelList;
		delete m_AffectingTypes;
		delete m_AffectedTypes;
	}
}

void ShipBonusModifier::_Populate(uint32 shipID)
{
    DBQueryResult *res = new DBQueryResult();
    ModuleDB::GetDgmShipBonusModifiers(shipID, *res);

    DBResultRow row1;
	if( res->GetRowCount() == 0 )
    {
        SysLog::Error("ShipBonusModifier", "Could not populate ship bonus modifier information for shipID: %u from the 'dgmShipBonusModifiers' table", shipID);
		m_ModifierLoaded = false;
	}
    else
    {
		m_EffectIDs = new uint32[res->GetRowCount()];
		m_AttributeSkillIDs = new uint32[res->GetRowCount()];
		m_SourceAttributeIDs = new uint32[res->GetRowCount()];
		m_TargetAttributeIDs = new uint32[res->GetRowCount()];
		m_CalculationTypeIDs = new uint32[res->GetRowCount()];
		m_ReverseCalculationTypeIDs = new uint32[res->GetRowCount()];
		m_AppliedPerLevelList = new uint32[res->GetRowCount()];
		m_AffectingTypes = new uint32[res->GetRowCount()];
		m_AffectedTypes = new uint32[res->GetRowCount()];

		int count = 0;
		std::string targetGroupIDs;
		typeTargetGroupIDlist * TargetGroupIDs;

		while( res->GetRow(row1) )
		{
			m_EffectIDs[count] = row1.GetUInt(0);
			m_AttributeSkillIDs[count] = row1.GetUInt(1);
			m_SourceAttributeIDs[count] = row1.GetUInt(2);
			m_TargetAttributeIDs[count] = row1.GetUInt(3);
			m_CalculationTypeIDs[count] = row1.GetUInt(4);
			m_Descriptions.insert(std::pair<uint32,std::string>(count,row1.GetText(5)));
			m_ReverseCalculationTypeIDs[count] = row1.GetUInt(6);
			targetGroupIDs = row1.GetText(7);
			m_AppliedPerLevelList[count] = row1.GetUInt(8);
			m_AffectingTypes[count] = row1.GetUInt(9);
			m_AffectedTypes[count] = row1.GetUInt(10);

			TargetGroupIDs = new typeTargetGroupIDlist;
			if( !(targetGroupIDs.empty()) )
			{
				// targetGroupIDs string is not empty, so extract one number at a time until it is empty
				int pos = 0;
				std::string tempString = "";

				pos = targetGroupIDs.find_first_of(';');
				if( pos < 0 )
					pos = targetGroupIDs.length()-1;	// we did not find any ';' characters, so targetGroupIDs contains only one number
				tempString = targetGroupIDs.substr(0,pos);

				while( (pos = targetGroupIDs.find_first_of(';')) > 0 )
				{
					tempString = targetGroupIDs.substr(0,pos);
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(tempString.c_str())));
					targetGroupIDs = targetGroupIDs.substr(pos+1,targetGroupIDs.length()-1);
				}

				// Get final number now that there are no more separators to find:
				if( !(targetGroupIDs.empty()) )
					TargetGroupIDs->insert(TargetGroupIDs->begin(), (atoi(targetGroupIDs.c_str())));

				m_TargetGroupIDlists.insert(std::pair<uint32, typeTargetGroupIDlist *>(count, TargetGroupIDs));
			}

			count++;
		}

		if( count == 0 )
		{
			;//Log::Error("ShipBonusModifier","Could not populate bonus modifier information for shipID: %u from the 'dgmShipBonusModifiers' table as the SQL query returned ZERO rows", shipID);
			m_ModifierLoaded = false;
		}
		else
		{
			m_numOfIDs = count;
			m_ModifierLoaded = true;
		}
	}

    delete res;
    res = NULL;
}

// ////////////////////// DGM_Effects_Table Class ////////////////////////////
DGM_Effects_Table::DGM_Effects_Table()
{
}

DGM_Effects_Table::~DGM_Effects_Table()
{
    // TODO: loop through entire std::map<> and delete ALL entries, calling ~MEffect() on each
}

int DGM_Effects_Table::Initialize()
{
    _Populate();

    return 1;
}

void DGM_Effects_Table::_Populate()
{
    //first get list of all effects from dgmEffects table
    DBQueryResult *res = new DBQueryResult();
    ModuleDB::GetAllDgmEffects(*res);

    //counter
    uint32 effectID;

	uint32 total_effect_count = 0;
	uint32 error_count = 0;

	//go through and populate each effect
    DBResultRow row;
    while( res->GetRow(row) )
    {
        effectID = row.GetInt(0);
        std::shared_ptr<MEffect> mEffectPtr = std::shared_ptr<MEffect>(new MEffect(effectID));
		if( mEffectPtr->IsEffectLoaded())
            m_EffectsMap.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
		else
			error_count++;

		total_effect_count++;
    }

	if( error_count > 0)
        SysLog::Error("DGM_Effects_Table::_Populate()", "ERROR Populating the DGM_Effects_Table memory object: %u of %u effects failed to load!", error_count, total_effect_count);

    SysLog::Log("DGM_Effects_Table", "%u total effects objects loaded", total_effect_count);

    //cleanup
    delete res;
    res = NULL;
}

std::shared_ptr<MEffect> DGM_Effects_Table::GetEffect(uint32 effectID)
{
    // return MEffect * corresponding to effectID from m_EffectsMap
    std::shared_ptr<MEffect> mEffectPtr;
    EffectMap::iterator mEffectMapIterator;

    if ((mEffectMapIterator = m_EffectsMap.find(effectID)) != m_EffectsMap.end())
    {
        mEffectPtr = mEffectMapIterator->second;
    }
    return mEffectPtr;
}

// ////////////////////// ModuleEffects Class ////////////////////////////

ModuleEffects::ModuleEffects(uint32 typeID) : m_typeID( typeID ), m_Cached( false )
{
    m_typeID = 0;
    m_HighPower = m_MediumPower = m_LowPower = m_Cached = false;

    _populate(typeID);
}

ModuleEffects::~ModuleEffects()
{
    //delete arrays

    //null ptrs
}

//useful accessors - probably a better way to do this, but at least it's fast
bool ModuleEffects::isHighSlot()
{
    if (m_Cached)
    {
        return m_HighPower;
    }

    return false;
}

bool ModuleEffects::isMediumSlot()
{
    if (m_Cached)
    {
        return m_MediumPower;
    }

    return false;
}

bool ModuleEffects::isLowSlot()
{
    if (m_Cached)
    {
        return m_LowPower;
    }

    return false;
}

bool ModuleEffects::HasEffect(uint32 effectID)
{
    return m_Effects.find(effectID) != m_Effects.end();
}

std::shared_ptr<MEffect> ModuleEffects::GetEffect(uint32 effectID)
{
    EffectMap::const_iterator cur;
    if ((cur = m_Effects.find(effectID)) != m_Effects.end())
    {
        return cur->second;
    }

    return NULL;
}


// ////////////////// PRIVATE MEMBERS /////////////////////////

void ModuleEffects::_populate(uint32 typeID)
{
    //first get list of all of the effects associated with the typeID
    DBQueryResult *res = new DBQueryResult();
    InvTypeRef type = InvType::getType(typeID);

    uint32 defaultEffect = type->m_defaultEffect;

    //counter
    std::shared_ptr<MEffect> mEffectPtr;
    m_defaultEffect = NULL;     // Set this to NULL until the default effect is found, if there is any
    uint32 effectID;
    uint32 isDefault;

    //go through and find each effect, then add pointer to effect to our own map
    for (auto cur : type->m_effects)
    {
        effectID = cur;
        mEffectPtr.reset(new MEffect(effectID));

        if (mEffectPtr)
		{
			if( mEffectPtr->IsEffectLoaded() )
            {
                isDefault = (cur == defaultEffect);
				switch( effectID )
				{
					case 11:    // loPower
					case 12:    // hiPower
					case 13:    // medPower
						// We do not need to make MEffect objects these effectIDs, since they do nothing
                        mEffectPtr.reset();
						break;

					default:
                        mEffectPtr = DGM_Effects_Table::GetEffect(effectID);
						break;
				}

				// Just in case our 'mEffectPtr' gets deleted above for certain cases, let's not proceed further lest we crash!
				if( mEffectPtr)
                {
                    // Save all effects.
                    m_Effects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    if (isDefault > 0)
                    {
                        m_defaultEffect = mEffectPtr;
                    }

					// This switch is assuming that all entries in 'dgmEffectsInfo' for this effectID are applied during the same module state,
					// which should really be the case anyway, for every effectID, so we just check the list of attributes
					// that are modified by this effect for which module state during which the effect is active:
                    uint32 moduleStateWhenEffectApplied = mEffectPtr->GetModuleStateWhenEffectApplied(0);
                    if (moduleStateWhenEffectApplied == MOD_UNFITTED)
                    {
                        SysLog::Error("ModuleEffects::_populate()", "Illegal value '%u' obtained from the 'effectAppliedInState' field of the 'dgmEffectsInfo' table", moduleStateWhenEffectApplied);
                    }

                    if (moduleStateWhenEffectApplied & MOD_OFFLINE)
                    {
                        // nothing
                    }

                    //					if( moduleStateWhenEffectApplied & MOD_ONLINE)
                    //                        m_OnlineEffects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    //
                    //					if( moduleStateWhenEffectApplied & MOD_ACTIVATED)
                    //                        m_ActiveEffects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    //
                    //					if( moduleStateWhenEffectApplied & MOD_OVERLOADED)
                    //                        m_OverloadEffects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    //
                    //					if( moduleStateWhenEffectApplied & MOD_GANG)
                    //                        m_GangEffects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    //
                    //					if( moduleStateWhenEffectApplied & MOD_FLEET)
                    //                        m_FleetEffects.insert(std::pair<uint32, std::shared_ptr < MEffect >> (effectID, mEffectPtr));
                    //
                    //					if( moduleStateWhenEffectApplied & MOD_DEACTIVATING )
                    //						;	// nothing
				}
			}
        }
    }

    //cleanup
    delete res;
    res = NULL;
}
