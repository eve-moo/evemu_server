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

#ifndef CORPORATION_H
#define	CORPORATION_H

/*
corpactivityEducation = 18
corpactivityEntertainment = 8
corpactivityMilitary = 5
corpactivitySecurity = 16
corpactivityTrading = 12
corpactivityWarehouse = 10
corpDivisionDistribution = 22
corpDivisionMining = 23
corpDivisionSecurity = 24
corporationStartupCost = 1599800
corporationAdvertisementFlatFee = 500000
corporationAdvertisementDailyRate = 250000
corporationMaxCorpRecruiters = 6
corporationMaxRecruitmentAds = 3
corporationMaxRecruitmentAdDuration = 28
corporationMinRecruitmentAdDuration = 3
corporationRecMaxTitleLength = 40
corporationRecMaxMessageLength = 1000
crpApplicationAppliedByCharacter = 0
crpApplicationRenegotiatedByCharacter = 1
crpApplicationAcceptedByCharacter = 2
crpApplicationRejectedByCharacter = 3
crpApplicationRejectedByCorporation = 4
crpApplicationRenegotiatedByCorporation = 5
crpApplicationAcceptedByCorporation = 6
crpApplicationWithdrawnByCharacter = 7
crpApplicationInvitedByCorporation = 8
crpApplicationMaxSize = 1000

 */
enum
{
    corpRoleLocationTypeHQ = 1LL,
    corpRoleLocationTypeBase = 2LL,
    corpRoleLocationTypeOther = 3LL,
};

enum CorporationRolls
{
    corpRoleDirector = 1,
    corpRolePersonnelManager = 128,
    corpRoleAccountant = 256,
    corpRoleSecurityOfficer = 512,
    corpRoleFactoryManager = 1024,
    corpRoleStationManager = 2048,
    corpRoleAuditor = 4096,
    corpRoleHangarCanTake1 = 8192,
    corpRoleHangarCanTake2 = 16384,
    corpRoleHangarCanTake3 = 32768,
    corpRoleHangarCanTake4 = 65536,
    corpRoleHangarCanTake5 = 131072,
    corpRoleHangarCanTake6 = 262144,
    corpRoleHangarCanTake7 = 524288,
    corpRoleHangarCanQuery1 = 1048576,
    corpRoleHangarCanQuery2 = 2097152,
    corpRoleHangarCanQuery3 = 4194304,
    corpRoleHangarCanQuery4 = 8388608,
    corpRoleHangarCanQuery5 = 16777216,
    corpRoleHangarCanQuery6 = 33554432,
    corpRoleHangarCanQuery7 = 67108864,
    corpRoleAccountCanTake1 = 134217728,
    corpRoleAccountCanTake2 = 268435456,
    corpRoleAccountCanTake3 = 536870912,
    corpRoleAccountCanTake4 = 1073741824,
    corpRoleAccountCanTake5 = 2147483648L,
    corpRoleAccountCanTake6 = 4294967296L,
    corpRoleAccountCanTake7 = 8589934592L,
    corpRoleDiplomat = 17179869184L,
    corpRoleEquipmentConfig = 2199023255552L,
    corpRoleContainerCanTake1 = 4398046511104L,
    corpRoleContainerCanTake2 = 8796093022208L,
    corpRoleContainerCanTake3 = 17592186044416L,
    corpRoleContainerCanTake4 = 35184372088832L,
    corpRoleContainerCanTake5 = 70368744177664L,
    corpRoleContainerCanTake6 = 140737488355328L,
    corpRoleContainerCanTake7 = 281474976710656L,
    corpRoleCanRentOffice = 562949953421312L,
    corpRoleCanRentFactorySlot = 1125899906842624L,
    corpRoleCanRentResearchSlot = 2251799813685248L,
    corpRoleJuniorAccountant = 4503599627370496L,
    corpRoleStarbaseConfig = 9007199254740992L,
    corpRoleTrader = 18014398509481984L,
    corpRoleChatManager = 36028797018963968L,
    corpRoleContractManager = 72057594037927936L,
    corpRoleInfrastructureTacticalOfficer = 144115188075855872L,
    corpRoleStarbaseCaretaker = 288230376151711744L,
    corpRoleFittingManager = 576460752303423488L,
    corpRoleTerrestrialCombatOfficer = 1152921504606846976L,
    corpRoleTerrestrialLogisticsOfficer = 2305843009213693952L,

    //Some Combos
    corpRoleAllHangar = (corpRoleHangarCanTake1 | corpRoleHangarCanTake2 | corpRoleHangarCanTake3 | corpRoleHangarCanTake4 | corpRoleHangarCanTake5 | corpRoleHangarCanTake6 | corpRoleHangarCanTake7 | corpRoleHangarCanQuery1 | corpRoleHangarCanQuery2 | corpRoleHangarCanQuery3 | corpRoleHangarCanQuery4 | corpRoleHangarCanQuery5 | corpRoleHangarCanQuery6 | corpRoleHangarCanQuery7),
    corpRoleAllAccount = (corpRoleJuniorAccountant | corpRoleAccountCanTake1 | corpRoleAccountCanTake2 | corpRoleAccountCanTake3 | corpRoleAccountCanTake4 | corpRoleAccountCanTake5 | corpRoleAccountCanTake6 | corpRoleAccountCanTake7),
    corpRoleAllContainer = (corpRoleContainerCanTake1 | corpRoleContainerCanTake2 | corpRoleContainerCanTake3 | corpRoleContainerCanTake4 | corpRoleContainerCanTake5 | corpRoleContainerCanTake6 | corpRoleContainerCanTake7),
    corpRoleAllOffice = (corpRoleCanRentOffice | corpRoleCanRentFactorySlot | corpRoleCanRentResearchSlot),
    corpRoleAll = (corpRoleAllHangar | corpRoleAllAccount | corpRoleAllContainer | corpRoleAllOffice | corpRoleDirector | corpRolePersonnelManager | corpRoleAccountant | corpRoleSecurityOfficer | corpRoleFactoryManager | corpRoleStationManager | corpRoleAuditor | corpRoleStarbaseConfig | corpRoleEquipmentConfig | corpRoleTrader | corpRoleChatManager),
};


#endif	/* CORPORATION_H */

