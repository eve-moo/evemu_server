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

#include "MapRegion.h"

#include "eveStatic.h"
#include "log/SystemLog.h"
#include "database/EVEDBUtils.h"

std::map<uint32, MapRegionRef> MapRegion::s_AllRegions;

MapRegion::MapRegion(
                uint32 _regionID,
                std::string _regionName,
                double _x,
                double _y,
                double _z,
                double _xMin,
                double _yMin,
                double _zMin,
                double _xMax,
                double _yMax,
                double _zMax,
                uint32 _factionID,
                double _radius,
                std::vector<uint32> _systems
                               ) :
regionID(_regionID),
regionName(_regionName),
x(_x),
y(_y),
z(_z),
xMin(_xMin),
yMin(_yMin),
zMin(_zMin),
xMax(_xMax),
yMax(_yMax),
zMax(_zMax),
factionID(_factionID),
radius(_radius),
location(_x, _y, _z),
locationMin(_xMin, _yMin, _zMin),
locationMax(_xMax, _yMax, _zMax),
solarSystems(_systems)
{
    s_AllRegions[regionID] = MapRegionRef(this, [](MapRegion * type)
    {
        delete type;
    });
}

MapRegion::~MapRegion() { }

bool EVEStatic::loadMapRegions(std::map<uint32, std::vector<uint32>> &regionSystems)
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "regionID, regionName,"
            " x, y, z, xMin, yMin, zMin, xMax, yMax, zMax,"
            " factionID, radius";
    std::string qry = "SELECT " + columns + " FROM mapRegions";
    if (!DBcore::RunQuery(result, qry.c_str()))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _regionID = row.GetInt(0);
        std::string _regionName = row.getTextNC(1);
        double _x = row.getDoubleNC(2);
        double _y = row.getDoubleNC(3);
        double _z = row.getDoubleNC(4);
        double _xMin = row.getDoubleNC(5);
        double _yMin = row.getDoubleNC(6);
        double _zMin = row.getDoubleNC(7);
        double _xMax = row.getDoubleNC(8);
        double _yMax = row.getDoubleNC(9);
        double _zMax = row.getDoubleNC(10);
        uint32 _factionID = row.getIntNC(11);
        double _radius = row.getDoubleNC(12);

        new MapRegion(
                _regionID,
                _regionName,
                _x,
                _y,
                _z,
                _xMin,
                _yMin,
                _zMin,
                _xMax,
                _yMax,
                _zMax,
                _factionID,
                _radius,
                regionSystems[_regionID]
                           );
    }

    return true;
}
