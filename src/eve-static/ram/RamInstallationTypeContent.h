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

#ifndef RAMINSTALLATIONTYPECONTENT_H
#define RAMINSTALLATIONTYPECONTENT_H

#include "python/PyRep.h"
#include <map>
#include <memory>

class RamInstallationTypeContent;
typedef std::shared_ptr<RamInstallationTypeContent> RamInstallationTypeContentRef;

class RamInstallationTypeContent {
public:
    RamInstallationTypeContent(
            uint32 _installationTypeID,
            uint32 _assemblyLineTypeID,
            uint32 _quantity
            );
    
    const uint32 installationTypeID;
    const uint32 assemblyLineTypeID;
    const uint32 quantity;

    /**
     * Get the RamInstallationTypeContent for the specified typeID
     * @param typeID The typeID to find.
     * @return The RamInstallationTypeContent or nullptr if not found.
     */
    static RamInstallationTypeContentRef getType(uint32 typeID)
    {
        auto itr = s_AllTypes.find(typeID);
        if (itr == s_AllTypes.end())
        {
            return std::shared_ptr<RamInstallationTypeContent>();
        }
        return itr->second;
    }

    /**
     * Get the RamInstallationTypeContent for the specified typeID
     * @param typeID The typeID to find.
     * @param type The location to store the found type.
     * @return True if type was found.
     */
    static bool getType(uint32 typeID, RamInstallationTypeContentRef &type)
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
    
private:
    RamInstallationTypeContent(const RamInstallationTypeContent& orig) = delete;
    virtual ~RamInstallationTypeContent();

    static std::map<uint32, RamInstallationTypeContentRef> s_AllTypes;
};

#endif /* RAMINSTALLATIONTYPECONTENT_H */

