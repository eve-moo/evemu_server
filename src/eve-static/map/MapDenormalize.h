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

#ifndef MAPDENORMALIZE_H
#define MAPDENORMALIZE_H

#include "math/Vector3D.h"
#include <map>
#include <vector>
#include <memory>
#include "inv/InvType.h"

class MapDenormalize;
typedef std::shared_ptr<MapDenormalize> MapDenormalizeRef;

class MapDenormalize
{
public:
    MapDenormalize(
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
                   );

    const uint32 itemID;
    const uint32 typeID;
    const uint32 groupID;
    const uint32 solarSystemID;
    const uint32 constellationID;
    const uint32 regionID;
    const uint32 orbitID;
    const double x;
    const double y;
    const double z;
    const double radius;
    const std::string itemName;
    const double security;
    const uint32 celestialIndex;
    const uint32 orbitIndex;

    // Convenience values.
    const Vector3D location;
    const InvTypeRef typeRef;

    /**
     * Get the MapDenormalize for the specified map entity.
     * @param itemID Of the item to find.
     * @return The MapDenormalize or nullptr if not found.
     */
    static MapDenormalizeRef getDenormalize(uint32 itemID)
    {
        auto itr = s_AllDenormalize.find(itemID);
        if (itr == s_AllDenormalize.end())
        {
            return std::shared_ptr<MapDenormalize>();
        }
        return itr->second;
    }

    /**
     * Get the MapDenormalize for the specified map entity.
     * @param itemID Of the item to find.
     * @param type The location to store the found type.
     * @return True if type was found.
     */
    static bool getDenormalize(uint32 itemID, MapDenormalizeRef &denorm)
    {
        auto itr = s_AllDenormalize.find(itemID);
        if (itr == s_AllDenormalize.end())
        {
            denorm.reset();
            return false;
        }
        denorm = itr->second;
        return true;
    }

    static void getSystemObjects(uint32 systemID, std::vector<MapDenormalizeRef> &into)
    {
        for(auto item : s_AllDenormalize)
        {
            MapDenormalizeRef ref = item.second;
            if(ref->solarSystemID == systemID)
            {
                into.push_back(ref);
            }
        }
    }

    static void getRegionObjects(uint32 regionID, std::vector<MapDenormalizeRef> &into)
    {
        for(auto item : s_AllDenormalize)
        {
            MapDenormalizeRef ref = item.second;
            if(ref->regionID == regionID)
            {
                into.push_back(ref);
            }
        }
    }

private:
    MapDenormalize(const MapDenormalize& orig) = delete;
    virtual ~MapDenormalize();

    static std::map<uint32, MapDenormalizeRef> s_AllDenormalize;
};

#endif /* MAPDENORMALIZE_H */

