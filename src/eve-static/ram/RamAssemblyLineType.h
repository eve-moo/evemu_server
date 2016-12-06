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

#ifndef RAMASSEMBLYLINETYPE_H
#define RAMASSEMBLYLINETYPE_H

#include "python/PyRep.h"
#include <map>
#include <memory>

enum FacilityReference
{
    BLUEPRINT = 1,
    DECRYPTOR = 3,
    SYSTEM = 4,
    FACILITY = 5,
    SKILLS = 6,
    BASEITEM = 7,
    DISTRICT = 8,
    FACTION = 9
};

class RamAssemblyLineType;
class RamAssemblyLineTypeDetailByCategory;
class RamAssemblyLineTypeDetailByGroup;
typedef std::shared_ptr<RamAssemblyLineType> RamAssemblyLineTypeRef;
typedef std::shared_ptr<RamAssemblyLineTypeDetailByCategory> RamAssemblyLineTypeDetailByCategoryRef;
typedef std::shared_ptr<RamAssemblyLineTypeDetailByGroup> RamAssemblyLineTypeDetailByGroupRef;

class RamAssemblyLineTypeDetailByCategory
{
public:
    RamAssemblyLineTypeDetailByCategory(
            uint32 _categoryID,
            double _timeMultiplier,
            double _materialMultiplier,
            double _costMultiplier
            );
    
    const uint32 categoryID;
    const double timeMultiplier;
    const double materialMultiplier;
    const double costMultiplier;

    PyTuple *getTimeModifier()
    {
        PyFloat *_amount = new PyFloat(timeMultiplier);
        PyRep *_categoryID = new PyInt(categoryID);
        PyRep *_groupID = new PyNone();
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

    PyTuple *getMaterialModifier()
    {
        PyFloat *_amount = new PyFloat(materialMultiplier);
        PyRep *_categoryID = new PyInt(categoryID);
        PyRep *_groupID = new PyNone();
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

    PyTuple *getCostModifier()
    {
        PyFloat *_amount = new PyFloat(costMultiplier);
        PyRep *_categoryID = new PyInt(categoryID);
        PyRep *_groupID = new PyNone();
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

};

class RamAssemblyLineTypeDetailByGroup
{
public:
    RamAssemblyLineTypeDetailByGroup(
            uint32 _groupID,
            double _timeMultiplier,
            double _materialMultiplier,
            double _costMultiplier
            );
    
    const uint32 groupID;
    const double timeMultiplier;
    const double materialMultiplier;
    const double costMultiplier;

    PyTuple *getTimeModifier()
    {
        PyFloat *_amount = new PyFloat(timeMultiplier);
        PyRep *_categoryID = new PyNone();
        PyRep *_groupID = new PyInt(groupID);
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

    PyTuple *getMaterialModifier()
    {
        PyFloat *_amount = new PyFloat(materialMultiplier);
        PyRep *_categoryID = new PyNone();
        PyRep *_groupID = new PyInt(groupID);
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

    PyTuple *getCostModifier()
    {
        PyFloat *_amount = new PyFloat(costMultiplier);
        PyRep *_categoryID = new PyNone();
        PyRep *_groupID = new PyInt(groupID);
        PyRep *_reference = new PyInt(FACILITY);
        return new_tuple(_amount, _categoryID, _groupID, _reference);
    }

};

class RamAssemblyLineType
{
public:
    RamAssemblyLineType(
            uint32 _assemblyLineTypeID,
            std::string _assemblyLineTypeName,
            std::string _description,
            double _baseTimeMultiplier,
            double _baseMaterialMultiplier,
            double _baseCostMultiplier,
            double _volume,
            int _activityID,
            double _minCostPerHour,
            std::map<uint32, RamAssemblyLineTypeDetailByCategoryRef> &_categoryDetails,
            std::map<uint32, RamAssemblyLineTypeDetailByGroupRef> &_groupDetails
                   );

    const uint32 assemblyLineTypeID;
    const std::string assemblyLineTypeName;
    const std::string description;
    const double baseTimeMultiplier;
    const double baseMaterialMultiplier;
    const double baseCostMultiplier;
    const double volume;
    const int activityID;
    const double minCostPerHour;

    const std::map<uint32, RamAssemblyLineTypeDetailByCategoryRef> categoryDetails;
    const std::map<uint32, RamAssemblyLineTypeDetailByGroupRef> groupDetails;

    /**
     * Get the RamAssemblyLineType for the specified typeID
     * @param typeID The typeID to find.
     * @return The RamAssemblyLineType or nullptr if not found.
     */
    static RamAssemblyLineTypeRef getType(uint32 typeID)
    {
        auto itr = s_AllTypes.find(typeID);
        if (itr == s_AllTypes.end())
        {
            return std::shared_ptr<RamAssemblyLineType>();
        }
        return itr->second;
    }

    /**
     * Get the RamAssemblyLineType for the specified typeID
     * @param typeID The typeID to find.
     * @param type The location to store the found type.
     * @return True if type was found.
     */
    static bool getType(uint32 typeID, RamAssemblyLineTypeRef &type)
    {
        auto itr = s_AllTypes.find(typeID);
        if (itr == s_AllTypes.end())
        {
            type.reset();
            return false;
        }
        type = itr->second;
        return true;
    }

    PyTuple *getTimeModifier()
    {
        PyFloat *amount = new PyFloat(baseTimeMultiplier);
        PyRep *categoryID = new PyNone();
        PyRep *groupID = new PyNone();
        PyRep *reference = new PyInt(FACILITY);
        return new_tuple(amount, categoryID, groupID, reference);
    }

    PyTuple *getMaterialModifier()
    {
        PyFloat *amount = new PyFloat(baseMaterialMultiplier);
        PyRep *categoryID = new PyNone();
        PyRep *groupID = new PyNone();
        PyRep *reference = new PyInt(FACILITY);
        return new_tuple(amount, categoryID, groupID, reference);
    }

    PyTuple *getCostModifier()
    {
        PyFloat *amount = new PyFloat(baseCostMultiplier);
        PyRep *categoryID = new PyNone();
        PyRep *groupID = new PyNone();
        PyRep *reference = new PyInt(FACILITY);
        return new_tuple(amount, categoryID, groupID, reference);
    }

private:
    RamAssemblyLineType(const RamAssemblyLineType& orig) = delete;
    virtual ~RamAssemblyLineType();

    static std::map<uint32, RamAssemblyLineTypeRef> s_AllTypes;
};

#endif /* RAMASSEMBLYLINETYPE_H */

