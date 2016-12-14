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

#include "MapSolarSystem.h"

#include "eveStatic.h"
#include "log/SystemLog.h"
#include "database/EVEDBUtils.h"

std::map<uint32, MapSolarSystemRef> MapSolarSystem::s_AllSolarSystems;

MapSolarSystem::MapSolarSystem(
                uint32 _solarSystemID,
                std::string _solarSystemName,
                uint32 _regionID,
                uint32 _constellationID,
                double _x,
                double _y,
                double _z,
                double _xMin,
                double _yMin,
                double _zMin,
                double _xMax,
                double _yMax,
                double _zMax,
                double _luminosity,
                bool _border,
                bool _fringe,
                bool _corridor,
                bool _hub,
                bool _international,
                bool _regional,
                bool _constellation,
                double _security,
                uint32 _factionID,
                double _radius,
                uint32 _sunTypeID,
                std::string _securityClass,
                std::vector<uint32> _gates
                               ) :
solarSystemID(_solarSystemID),
solarSystemName(_solarSystemName),
regionID(_regionID),
constellationID(_constellationID),
x(_x),
y(_y),
z(_z),
xMin(_xMin),
yMin(_yMin),
zMin(_zMin),
xMax(_xMax),
yMax(_yMax),
zMax(_zMax),
luminosity(_luminosity),
border(_border),
fringe(_fringe),
corridor(_corridor),
hub(_hub),
international(_international),
regional(_regional),
constellation(_constellation),
security(_security),
factionID(_factionID),
radius(_radius),
sunTypeID(_sunTypeID),
securityClass(_securityClass),
location(_x, _y, _z),
locationMin(_xMin, _yMin, _zMin),
locationMax(_xMax, _yMax, _zMax),
jumpGates(_gates)
{
    s_AllSolarSystems[solarSystemID] = MapSolarSystemRef(this, [](MapSolarSystem * type)
    {
        delete type;
    });
}

MapSolarSystem::~MapSolarSystem() { }

bool EVEStatic::loadMapSolarSystems(std::map<uint32, std::vector<uint32>> &regionSystems, std::map<uint32, std::vector<uint32>> &systemGates)
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "solarSystemID, solarSystemName, regionID,"
            "constellationID, x, y, z, xMin, yMin, zMin, xMax, yMax, zMax,"
            " luminosity, border, fringe, corridor, hub, international,"
            " regional, constellation, security, factionID, radius, sunTypeID, securityClass";
    std::string qry = "SELECT " + columns + " FROM mapSolarSystems";
    if (!DBcore::RunQuery(result, qry.c_str()))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _solarSystemID = row.GetInt(0);
        std::string _solarSystemName = row.getTextNC(1);
        uint32 _regionID = row.getIntNC(2);
        uint32 _constellationID = row.getIntNC(3);
        double _x = row.getDoubleNC(4);
        double _y = row.getDoubleNC(5);
        double _z = row.getDoubleNC(6);
        double _xMin = row.getDoubleNC(7);
        double _yMin = row.getDoubleNC(8);
        double _zMin = row.getDoubleNC(9);
        double _xMax = row.getDoubleNC(10);
        double _yMax = row.getDoubleNC(11);
        double _zMax = row.getDoubleNC(12);
        double _luminosity = row.getDoubleNC(13);
        bool _border = row.GetBool(14);
        bool _fringe = row.GetBool(15);
        bool _corridor = row.GetBool(16);
        bool _hub = row.GetBool(17);
        bool _international = row.GetBool(18);
        bool _regional = row.GetBool(19);
        bool _constellation = row.GetBool(20);
        double _security = row.getDoubleNC(21);
        uint32 _factionID = row.getIntNC(22);
        double _radius = row.getDoubleNC(23);
        uint32 _sunTypeID = row.getIntNC(24);
        std::string _securityClass = row.getTextNC(25);

        // Save the reference for the region.
        regionSystems[_regionID].push_back(_solarSystemID);
        new MapSolarSystem(
                _solarSystemID,
                _solarSystemName,
                _regionID,
                _constellationID,
                _x,
                _y,
                _z,
                _xMin,
                _yMin,
                _zMin,
                _xMax,
                _yMax,
                _zMax,
                _luminosity,
                _border,
                _fringe,
                _corridor,
                _hub,
                _international,
                _regional,
                _constellation,
                _security,
                _factionID,
                _radius,
                _sunTypeID,
                _securityClass,
                systemGates[_solarSystemID]
                           );
    }

    return true;
}

void MapSolarSystem::getStations(std::vector<uint32> &stations)
{
    std::vector<MapDenormalizeRef> systemObjects;
    MapDenormalize::getSystemObjects(solarSystemID, systemObjects);
    for(MapDenormalizeRef obj : systemObjects)
    {
        if(obj->typeRef->getCategoryID() == EVEItemCategories::Station)
        {
            stations.push_back(obj->itemID);
        }
    }
}

void MapSolarSystem::getDistanceMap(std::map<int, int> &distanceMap)
{
    distanceMap.clear();
    std::vector<int> systems;
    distanceMap[solarSystemID] = 0;
    systems.push_back(solarSystemID);
    while(systems.size() > 0)
    {
        std::vector<int>::iterator begin = systems.begin();
        int _solarSystemID = *begin;
        systems.erase(begin);
        MapSolarSystemRef system = MapSolarSystem::getSystem(_solarSystemID);
        if(system.get() == nullptr)
        {
            // Something went wrong, skip this system.
            continue;
        }
        int distance = distanceMap[_solarSystemID];
        for(int gateID : jumpGates)
        {
            MapJumpRef gate = MapJump::getJump(gateID);
            if(gate.get() != nullptr)
            {
                // Something went wrong, skip this gate.
                continue;
            }
            int destinationGateID = gate->destinationID;
            MapDenormalizeRef destinationGate = MapDenormalize::getDenormalize(destinationGateID);
            if(destinationGate.get() == nullptr)
            {
                // Something went wrong, skip this gate.
                continue;
            }
            int destinationSystemID = destinationGate->solarSystemID;
            if(distanceMap.find(destinationSystemID) != distanceMap.end())
            {
                // Was this route shorter?
                if(distanceMap[destinationSystemID] > distance)
                {
                    // Yes, save the new distance.
                    distanceMap[destinationSystemID] = distance;
                    // Are we still waiting to map this system?
                    if(std::find(systems.begin(), systems.end(), destinationSystemID) == systems.end())
                    {
                        // No, so we need to redo this system.
                        systems.push_back(destinationSystemID);
                    }
                }
                else
                {
                    //No, so skip it.
                    continue;
                }
            }
            else
            {
                // We have never reached this system before.
                distanceMap[destinationSystemID] = distance;
                systems.push_back(destinationSystemID);
            }
        }
    }
}

void MapSolarSystem::getSystemsInRange(int range, std::vector<int> &systems)
{
    std::map<int, int> &distanceMap = getDistanceMap();
    systems.clear();
    for(auto entry : distanceMap)
    {
        if(entry.second > range)
        {
            // To far away, skip it.
            continue;
        }
        int systemID = entry.first;
        MapSolarSystemRef system = MapSolarSystem::getSystem(systemID);
        if(system.get() != nullptr && system->regionID == regionID)
        {
            systems.push_back(systemID);
        }
    }
}
