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

#ifndef MAPSOLARSYSTEM_H
#define MAPSOLARSYSTEM_H

#include "math/Vector3D.h"
#include "MapJump.h"
#include "MapDenormalize.h"
#include <map>
#include <memory>
#include <vector>

class MapSolarSystem;
typedef std::shared_ptr<MapSolarSystem> MapSolarSystemRef;

class MapSolarSystem
{
public:
    MapSolarSystem(
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
                   );

    const uint32 solarSystemID;
    const std::string solarSystemName;
    const uint32 regionID;
    const uint32 constellationID;
    const double x;
    const double y;
    const double z;
    const double xMin;
    const double yMin;
    const double zMin;
    const double xMax;
    const double yMax;
    const double zMax;
    const double luminosity;
    const bool border;
    const bool fringe;
    const bool corridor;
    const bool hub;
    const bool international;
    const bool regional;
    const bool constellation;
    const double security;
    const uint32 factionID;
    const double radius;
    const uint32 sunTypeID;
    const std::string securityClass;
    
    //
    const std::vector<uint32> jumpGates;
    
    // Convenience values.
    const Vector3D location;
    const Vector3D locationMin;
    const Vector3D locationMax;

    /**
     * Get the MapSolarSystem for the specified solar system.
     * @param solarSystemID Of the solar system to find.
     * @return The MapSolarSystem or nullptr if not found.
     */
    static MapSolarSystemRef getSystem(uint32 solarSystemID)
    {
        auto itr = s_AllSolarSystems.find(solarSystemID);
        if (itr == s_AllSolarSystems.end())
        {
            return std::shared_ptr<MapSolarSystem>();
        }
        return itr->second;
    }

    /**
     * Get the MapSolarSystem for the specified solar system.
     * @param solarSystemID Of the solar system to find.
     * @param type The location to store the found type.
     * @return True if type was found.
     */
    static bool getSystem(uint32 solarSystemID, MapSolarSystemRef &system)
    {
        auto itr = s_AllSolarSystems.find(solarSystemID);
        if (itr == s_AllSolarSystems.end())
        {
            system.reset();
            return false;
        }
        system = itr->second;
        return true;
    }

    static bool getRegionSystems(uint32 regionID, std::vector<MapSolarSystemRef> &into)
    {
        for(auto item : s_AllSolarSystems)
        {
            MapSolarSystemRef ref = item.second;
            if(ref->regionID == regionID)
            {
                into.push_back(ref);
            }
        }
    }

    void getStations(std::vector<uint32> &stations);

    std::map<int, int> &getDistanceMap()
    {
        // Have we initialized this map?
        if(distanceMap.size() == 0)
        {
            // No, get the map.
            getDistanceMap(distanceMap);
        }
        return distanceMap;
    }
    void getSystemsInRange(int range, std::vector<int> &systems);

private:
    MapSolarSystem(const MapSolarSystem& orig) = delete;
    virtual ~MapSolarSystem();

    static std::map<uint32, MapSolarSystemRef> s_AllSolarSystems;
    
    std::map<int, int> distanceMap;
    void getDistanceMap(std::map<int, int> &distanceMap);
};

#endif /* MAPSOLARSYSTEM_H */

