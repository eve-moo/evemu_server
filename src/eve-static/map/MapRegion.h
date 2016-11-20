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

#ifndef MAPREGION_H
#define MAPREGION_H

#include "math/Vector3D.h"
#include <map>
#include <memory>

class MapRegion;
typedef std::shared_ptr<MapRegion> MapRegionRef;

class MapRegion
{
public:
    MapRegion(
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
                double _radius
                   );

    const uint32 regionID;
    const std::string regionName;
    const double x;
    const double y;
    const double z;
    const double xMin;
    const double yMin;
    const double zMin;
    const double xMax;
    const double yMax;
    const double zMax;
    const uint32 factionID;
    const double radius;

    // Convenience values.
    const Vector3D location;
    const Vector3D locationMin;
    const Vector3D locationMax;

    /**
     * Get the MapRegion for the specified region.
     * @param regionID Of the region to find.
     * @return The MapRegion or nullptr if not found.
     */
    static MapRegionRef getRegion(uint32 regionID)
    {
        auto itr = s_AllRegions.find(regionID);
        if (itr == s_AllRegions.end())
        {
            return std::shared_ptr<MapRegion>();
        }
        return itr->second;
    }

    /**
     * Get the MapRegion for the specified region.
     * @param regionID Of the region to find.
     * @param type The location to store the found type.
     * @return True if type was found.
     */
    static bool getRegion(uint32 regionID, MapRegionRef &region)
    {
        auto itr = s_AllRegions.find(regionID);
        if (itr == s_AllRegions.end())
        {
            region.reset();
            return false;
        }
        region = itr->second;
        return true;
    }

private:
    MapRegion(const MapRegion& orig) = delete;
    virtual ~MapRegion();

    static std::map<uint32, MapRegionRef> s_AllRegions;
};

#endif /* MAPREGION_H */

