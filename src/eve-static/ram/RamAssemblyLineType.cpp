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

#include "RamAssemblyLineType.h"

#include "eveStatic.h"
#include "log/SystemLog.h"
#include "database/EVEDBUtils.h"

std::map<uint32, RamAssemblyLineTypeRef> RamAssemblyLineType::s_AllTypes;

RamAssemblyLineTypeDetailByCategory::RamAssemblyLineTypeDetailByCategory(
            uint32 _categoryID,
            double _timeMultiplier,
            double _materialMultiplier,
            double _costMultiplier
            ) :
categoryID(_categoryID),
timeMultiplier(_timeMultiplier),
materialMultiplier(_materialMultiplier),
costMultiplier(_costMultiplier)
{
}

RamAssemblyLineTypeDetailByGroup::RamAssemblyLineTypeDetailByGroup(
            uint32 _groupID,
            double _timeMultiplier,
            double _materialMultiplier,
            double _costMultiplier
            ) :
groupID(_groupID),
timeMultiplier(_timeMultiplier),
materialMultiplier(_materialMultiplier),
costMultiplier(_costMultiplier)
{
}

RamAssemblyLineType::RamAssemblyLineType(
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
                               ) :
assemblyLineTypeID(_assemblyLineTypeID),
assemblyLineTypeName(_assemblyLineTypeName),
description(_description),
baseTimeMultiplier(_baseTimeMultiplier),
baseMaterialMultiplier(_baseMaterialMultiplier),
baseCostMultiplier(_baseCostMultiplier),
volume(_volume),
activityID(_activityID),
minCostPerHour(_minCostPerHour),
categoryDetails(_categoryDetails),
groupDetails(_groupDetails)
{
    s_AllTypes[assemblyLineTypeID] = RamAssemblyLineTypeRef(this, [](RamAssemblyLineType * type)
    {
        delete type;
    });
}

RamAssemblyLineType::~RamAssemblyLineType() { }

bool getDetails(uint32 lineType,
                std::map<uint32, RamAssemblyLineTypeDetailByCategoryRef> &category,
                std::map<uint32, RamAssemblyLineTypeDetailByGroupRef> &group
)
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "timeMultiplier, materialMultiplier, costMultiplier";

    std::string qry = "SELECT categoryID, " + columns + " FROM ramAssemblyLineTypeDetailPerCategory WHERE assemblyLineTypeID=%u";
    if (!DBcore::RunQuery(result, qry.c_str(), lineType))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _ID = row.GetUInt(0);
        double _baseTimeMultiplier = row.getDoubleNC(1);
        double _baseMaterialMultiplier = row.getDoubleNC(2);
        double _baseCostMultiplier = row.getDoubleNC(3);
        RamAssemblyLineTypeDetailByCategoryRef detail = std::make_shared<RamAssemblyLineTypeDetailByCategory>(
                _ID,
                _baseTimeMultiplier,
                _baseMaterialMultiplier,
                _baseCostMultiplier
                );
        category[detail->categoryID] = detail;
    }

    qry = "SELECT groupID, " + columns + " FROM ramAssemblyLineTypeDetailPerGroup WHERE assemblyLineTypeID=%u";
    if (!DBcore::RunQuery(result, qry.c_str(), lineType))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _ID = row.GetUInt(0);
        double _baseTimeMultiplier = row.getDoubleNC(1);
        double _baseMaterialMultiplier = row.getDoubleNC(2);
        double _baseCostMultiplier = row.getDoubleNC(3);
        RamAssemblyLineTypeDetailByGroupRef detail = std::make_shared<RamAssemblyLineTypeDetailByGroup>(
                _ID,
                _baseTimeMultiplier,
                _baseMaterialMultiplier,
                _baseCostMultiplier
                );
        group[detail->groupID] = detail;
    }
    return true;
}

bool EVEStatic::loadRamAssemblyLineTypes()
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "assemblyLineTypeID, assemblyLineTypeName, description,"
            "baseTimeMultiplier, baseMaterialMultiplier, baseCostMultiplier,"
            "volume, activityID, minCostPerHour";
    std::string qry = "SELECT " + columns + " FROM ramAssemblyLineTypes";
    if (!DBcore::RunQuery(result, qry.c_str()))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _assemblyLineTypeID = row.GetUInt(0);
        std::string _assemblyLineTypeName = row.getTextNC(1);
        std::string _description = row.getTextNC(2);
        double _baseTimeMultiplier = row.getDoubleNC(3);
        double _baseMaterialMultiplier = row.getDoubleNC(4);
        double _baseCostMultiplier = row.getDoubleNC(5);
        double _volume = row.getDoubleNC(6);
        int _activityID = row.getIntNC(7);
        double _minCostPerHour = row.getDoubleNC(8);

        std::map<uint32, RamAssemblyLineTypeDetailByCategoryRef> _categoryDetails;
        std::map<uint32, RamAssemblyLineTypeDetailByGroupRef> _groupDetails;
        getDetails(_assemblyLineTypeID, _categoryDetails, _groupDetails);

        new RamAssemblyLineType(
                _assemblyLineTypeID,
                _assemblyLineTypeName,
                _description,
                _baseTimeMultiplier,
                _baseMaterialMultiplier,
                _baseCostMultiplier,
                _volume,
                _activityID,
                _minCostPerHour,
                _categoryDetails,
                _groupDetails
                       );
    }

    return true;
}
