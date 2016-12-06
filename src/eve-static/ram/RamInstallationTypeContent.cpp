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

#include "RamInstallationTypeContent.h"

RamInstallationTypeContent::RamInstallationTypeContent(
            uint32 _installationTypeID,
            uint32 _assemblyLineTypeID,
            uint32 _quantity
) :
installationTypeID(_installationTypeID),
assemblyLineTypeID(_assemblyLineTypeID),
quantity(_quantity)
{
}

RamInstallationTypeContent::~RamInstallationTypeContent()
{
}

bool EVEStatic::loadRamInstallationTypeContents()
{
    DBQueryResult result;
    DBResultRow row;
    std::string columns = "installationTypeID, assemblyLineTypeID, quantity";
    std::string qry = "SELECT " + columns + " FROM ramInstallationTypeContents";
    if (!DBcore::RunQuery(result, qry.c_str()))
    {
        SysLog::Error("Static DB", "Error in query: %s", result.error.c_str());
        return false;
    }
    while (result.GetRow(row))
    {
        uint32 _installationTypeID = row.GetUInt(0);
        uint32 _assemblyLineTypeID = row.GetUInt(1);
        uint32 _quantity = row.GetUInt(2);

        new RamInstallationTypeContent(
                _installationTypeID,
                _assemblyLineTypeID,
                _quantity
                       );
    }

    return true;
}
