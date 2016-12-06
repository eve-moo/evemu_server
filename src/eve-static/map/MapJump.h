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

#ifndef MAPJUMP_H
#define MAPJUMP_H

#include "math/Vector3D.h"
#include <map>
#include <memory>
#include <vector>

class MapJump;
typedef std::shared_ptr<MapJump> MapJumpRef;

class MapJump
{
public:
    MapJump(
            uint32 _gateID,
            uint32 _destinationID
            );

    const uint32 gateID;
    const uint32 destinationID;

    static MapJumpRef getJump(uint32 gateID)
    {
        auto itr = s_AllJumps.find(gateID);
        if (itr == s_AllJumps.end())
        {
            return std::shared_ptr<MapJump>();
        }
        return itr->second;
    }

    static bool getJump(uint32 gateID, MapJumpRef &gate)
    {
        auto itr = s_AllJumps.find(gateID);
        if (itr == s_AllJumps.end())
        {
            gate.reset();
            return false;
        }
        gate = itr->second;
        return true;
    }

private:
    MapJump(const MapJump& orig) = delete;
    virtual ~MapJump();

    static std::map<uint32, MapJumpRef> s_AllJumps;

};

#endif /* MAPJUMP_H */

