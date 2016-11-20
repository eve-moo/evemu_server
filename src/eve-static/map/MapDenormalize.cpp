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

#include "MapDenormalize.h"

#include "eveStatic.h"
#include "log/SystemLog.h"
#include "database/EVEDBUtils.h"

std::map<uint32, MapDenormalizeRef> MapDenormalize::s_AllDenormalize;

MapDenormalize::MapDenormalize(
                uint32 _itemID,
                uint32 _typeID,
                uint32 _groupID,
                uint32 _solarSystemID,
                uint32 _constellationID,
                uint32 _regionID,
                uint32 _orbitID,
                double _x,
                double _y,
                double _z,
                double _radius,
                std::string _itemName,
                double _security,
                uint32 _celestialIndex,
                uint32 _orbitIndex
                               ) :
itemID(_itemID),
typeID(_typeID),
groupID(_groupID),
solarSystemID(_solarSystemID),
constellationID(_constellationID),
regionID(_regionID),
orbitID(_orbitID),
x(_x),
y(_y),
z(_z),
radius(_radius),
itemName(_itemName),
security(_security),
celestialIndex(_celestialIndex),
orbitIndex(_orbitIndex),
location(_x, _y, _z)
{
    s_AllDenormalize[itemID] = MapDenormalizeRef(this, [](MapDenormalize * type)
    {
        delete type;
    });
}

MapDenormalize::~MapDenormalize() { }

bool EVEStatic::loadMapDenormalize()
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "itemID, typeID, groupID, solarSystemID, constellationID,"
            "regionID, orbitID, x, y, z, radius, itemName, security, celestialIndex, orbitIndex";
    std::string qry = "SELECT " + columns + " FROM mapDenormalize";
    if (!DBcore::RunQuery(result, qry.c_str()))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _itemID = row.GetInt(0);
        uint32 _typeID = row.getIntNC(1);
        uint32 _groupID = row.getIntNC(2);
        uint32 _solarSystemID = row.getIntNC(3);
        uint32 _constellationID = row.getIntNC(4);
        uint32 _regionID = row.getIntNC(5);
        uint32 _orbitID = row.getIntNC(6);
        double _x = row.getDoubleNC(7);
        double _y = row.getDoubleNC(8);
        double _z = row.getDoubleNC(9);
        double _radius = row.getDoubleNC(10);
        std::string _itemName = row.getTextNC(11);
        double _security = row.getDoubleNC(12);
        uint32 _celestialIndex = row.getIntNC(13);
        uint32 _orbitIndex = row.getIntNC(14);
        
        new MapDenormalize(
                _itemID,
                _typeID,
                _groupID,
                _solarSystemID,
                _constellationID,
                _regionID,
                _orbitID,
                _x,
                _y,
                _z,
                _radius,
                _itemName,
                _security,
                _celestialIndex,
                _orbitIndex
                           );
    }

    return true;
}
