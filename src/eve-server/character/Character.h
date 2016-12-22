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
    Author:        Zhur, Bloody.Rabbit
 */

#ifndef __CHARACTER__H__INCL__
#define __CHARACTER__H__INCL__

#include "inventory/Owner.h"
#include "inventory/Inventory.h"
#include "inventory/InventoryDB.h"
#include "character/Skill.h"
#include "chr/ChrBloodline.h"

/**
 * Container for raw character data.
 */
class CharacterData
{
public:
    CharacterData(
                  uint32 _accountID = 0,
                  const char *_title = "",
                  const char *_desc = "",
                  bool _gender = false,
                  double _bounty = 0.0,
                  double _balance = 0.0,
                  double _aurBalance = 0.0,
                  double _securityRating = 0.0,
                  uint32 _logonMinutes = 0,
                  double _skillPoints = 0,
                  uint32 _corporationID = 0,
                  uint32 _allianceID = 0,
                  uint32 _warFactionID = 0,
                  uint32 _stationID = 0,
                  uint32 _solarSystemID = 0,
                  uint32 _constellationID = 0,
                  uint32 _regionID = 0,
                  ChrBloodlineRef _bloodline = ChrBloodlineRef(),
                  uint32 _ancestryID = 0,
                  uint32 _careerID = 0,
                  uint32 _schoolID = 0,
                  uint32 _careerSpecialityID = 0,
                  uint64 _startDateTime = 0,
                  uint64 _createDateTime = 0,
                  uint64 _corporationDateTime = 0,
                  uint32 _shipID = 0,
                  uint32 _freeSkillPoints = 0);

    uint32 accountID;

    std::string title;
    std::string description;
    bool gender;

    double bounty;
    double balance;
    double aurBalance;
    double securityRating;
    uint32 logonMinutes;
    double skillPoints;

    uint32 corporationID;
    uint32 allianceID;
    uint32 warFactionID;

    uint32 stationID;
    uint32 solarSystemID;
    uint32 constellationID;
    uint32 regionID;

    ChrBloodlineRef bloodline;
    uint32 ancestryID;
    uint32 careerID;
    uint32 schoolID;
    uint32 careerSpecialityID;

    uint64 startDateTime;
    uint64 createDateTime;
    uint64 corporationDateTime;

    uint32 shipID;
    uint32 freeSkillPoints;
};

/**
 * Container for character appearance stuff.
 */
class CharacterAppearance
{
public:
    uint32 colorID;
    uint32 colorNameA;
    uint32 colorNameBC;
    double weight;
    double gloss;

    uint32 modifierLocationID;
    uint32 paperdollResourceID;
    uint32 paperdollResourceVariation;

    uint32 sculptID;
    double weightUpDown;
    double weightLeftRight;
    double weightForwardBack;

    void Build(uint32 ownerID, PyDict* data);

private:
};

/**
 * Container for some corporation-membership related stuff.
 */
class CorpMemberInfo
{
public:
    CorpMemberInfo(
                   uint32 _corpHQ = 0,
                   uint64 _corpRole = 0,
                   uint64 _rolesAtAll = 0,
                   uint64 _rolesAtBase = 0,
                   uint64 _rolesAtHQ = 0,
                   uint64 _rolesAtOther = 0
                   );

    uint32 corpHQ; //this really doesn't belong here...

    uint64 corpRole;
    uint64 rolesAtAll;
    uint64 rolesAtBase;
    uint64 rolesAtHQ;
    uint64 rolesAtOther;
};

/**
 * Class representing fleet data    -allan 31Jul14
 */
class FleetMemberInfo
{
public:
    FleetMemberInfo(
                    uint32 _fleetID = 0,
                    uint32 _wingID = 0,
                    uint32 _squadID = 0,
                    uint8 _fleetRole = 0,
                    uint8 _fleetBooster = 0,
                    uint8 _fleetJob = 0
                    );

    uint32 fleetID;
    uint32 wingID;
    uint32 squadID;
    uint8 fleetRole;
    uint8 fleetBooster;
    uint8 fleetJob;
};

/**
 * Class representing character.
 */
class Character
: public Owner,
public Inventory
{
    friend class InventoryItem; // to let it construct us
    friend class Owner; // to let it construct us
public:
    typedef InventoryDB::QueuedSkill QueuedSkill;
    typedef InventoryDB::SkillQueue SkillQueue;
    typedef InventoryDB::currentCertificates cCertificates;
    typedef InventoryDB::Certificates Certificates;

    /**
     * Loads character.
     *
     * @param[in] characterID ID of character to load.
     * @return Pointer to new Character object; NULL if failed.
     */
    static CharacterRef Load(uint32 characterID);
    /**
     * Spawns new character.
     *
     * @param[in] factory
     * @param[in] data ItemData (data for entity table) for new character.
     * @param[in] charData Character data for new character.
     * @param[in] appData Appearance data for new character.
     * @param[in] corpData Corporation membership data for new character.
     * @return Pointer to new Character object; NULL if failed.
     */
    static CharacterRef Spawn(ItemData &data, CharacterData &charData, CorpMemberInfo &corpData);

    /*
     * Primary public interface:
     */
    bool AlterBalance(double balanceChange);
    void SetLocation(uint32 stationID, uint32 solarSystemID, uint32 constellationID, uint32 regionID);
    void JoinCorporation(uint32 corporationID);
    void JoinCorporation(uint32 corporationID, const CorpMemberInfo &roles);
    void SetAccountKey(int32 accountKey); //not completed in this branch
    void SetDescription(const char *newDescription);
    void SetFleetData(FleetMemberInfo& fleet);

    void Delete();

    /**
     * Checks whether character has the skill.
     *
     * @param[in] skillTypeID ID of skill type to be checked.
     * @return True if character has the skill, false if doesn't.
     */
    bool HasSkill(uint32 skillTypeID) const;
    /**
     * Checks whether the character has the skill, and if so, if it has been trained to the level specified.
     *
     * @param[in] skillTypeID ID of skill type to be checked
     * @param[in] skillLevel Level of the skill to be checked to see if it is trained already to at least this level
     * @return True if character has the skill AND that skill has been trained to at least the level specified, False otherwise
     */
    bool HasSkillTrainedToLevel(uint32 skillTypeID, uint32 skillLevel) const;
    /**
     * Returns skill.
     *
     * @param[in] skillTypeID ID of skill type to be returned.
     * @param[in] newref Whether new reference should be returned.
     * @return Pointer to Skill object; NULL if skill was not found.
     */
    SkillRef GetSkill(uint32 skillTypeID) const;

    uint8 GetSkillLevel(uint32 skillTypeID, bool zeroForNotInjected = true) const;
    /**
     * Returns skill currently in training.
     *
     * @param[in] newref Whether new reference should be returned.
     * @return Pointer to Skill object; NULL if skill was not found.
     */
    SkillRef GetSkillInTraining() const;
    /**
     * Returns entire list of skills learned by this character
     *
     * @param[in] empty std::vector<InventoryItemRef> which is populated with list of skills
     */
    void GetSkillsList(std::vector<InventoryItemRef> &skills) const;

        /**
     * Calculates Total Skillpoints the character has trained
     *
     * @return Skillpoints per minute rate.
     * TO-DO; add current training skill sp
     */
    double GetTotalSPTrained()
    {
        return m_totalSPtrained;
    };
    /**
     * Calculates Skillpoints per minute rate.
     *
     * @param[in] skill Skill for which the rate is calculated.
     * @return Skillpoints per minute rate.
     */
    double GetSPPerMin(SkillRef skill);
    /**
     * @return Timestamp at which current skill training finishes.
     */
    EvilNumber GetEndOfTraining() const;

    /* InjectSkillIntoBrain(InventoryItem *skill)
     *
     * Perform injection of passed skill into the character.
     * @author xanarox
     * @param InventoryItem
     * @param client The client to notify.
     */
    SkillRef injectSkillIntoBrain(SkillRef skill, Client *client);
    /* AddSkillToSkillQueue()
     *
     * This will add a skill into the skill queue.
     * @author xanarox
     */
    void AddToSkillQueue(uint32 typeID, uint8 level);
    /**
     * Clears skill queue.
     */
    void ClearSkillQueue();
    /**
     * Stop the current skill training.
     * @param notify Should the client receive a notification?
     */
    void stopTraining(bool notify = true);
    /**
     * Start the specified skill training.
     * @param skillID The skillID of the still to start training.
     * @param nextStartTime The start time to use or 0 for now.
     * @param notify Should the client receive a notification?
     * @return The currently training.
     */
    SkillRef startTraining(uint32 skillID, uint64 nextStartTime = 0, bool notify = true);
    /**
     * Updates skill queue.
     */
    void updateSkillQueue();
    /**
     * Update skill training end time on char select screen.
     */
    void updateSkillQueueTimes();
    /**
     * Send a OnNewSkillQUeueSaved message to the client.
     */
    void sendSkillQueueChangedNotice(Client *client);

    /* GrantCertificate( uint32 certificateID )
     *
     * This will add a certificate into the character
     * @author almamu
     */
    bool GrantCertificate(uint32 certificateID);
    /* UpdateCertificate( uint32 certificateID, bool pub )
     *
     * This will change the public status of the certificate
     * @author almamu
     */
    void UpdateCertificate(uint32 certificateID, bool pub);
    /* HasCertificate( uint32 certificateID )
     *
     * This will check if the player has a certificate
     * @author almamu
     */
    bool HasCertificate(uint32 certificateID) const;
    /* GetCertificates( )
     *
     * This will check if the player has a certificate
     * @author almamu
     */
    void GetCertificates(Certificates &crt);

    // NOTE: We do not handle Split/Merge logic since singleton-restricted construction does this for us.

    /**
     * Gets char base attributes
     */
    PyObject *GetCharacterBaseAttributes();


    /*
     * Primary public packet builders:
     */
    PyTuple *CharGetInfo();
    PyObject *GetDescription() const;
    /* GetSkillQueue()
     *
     * This will get the skills from the skill queue for a character.
     * @author xanarox
     */
    PyList *getSkillQueue();
    /**
     * Get skill at queue index.
     * @param index The index.
     * @return The skill at that index.
     */
    SkillRef GetSkillInQueue(uint32 index);

    /**
     * Get the number of skill points an injector adds to the character.
     * @return The number of skill points that would be added.
     */
    uint32 getInjectorSP();
    /**
     * Check if we can use a skill injector.
     * @param The injector or stack of injectors to use.
     */
    bool useInjector(InventoryItemRef injector, int32 qty);
    /**
     * Send a OnFreeSkillPointsChanged notification.
     */
    void sendSkillFreePointsChanged();
    /**
     * Check if a skill extractor can be used.
     * @return True if an extractor can be used.
     */
    bool canUseSkillExtractor();

    /**
     * Get implant in slot.
     * @param slot The slot to look for the implant.
     * @return The implant.
     */
    InventoryItemRef GetImplant(uint32 slot);

        /*
     * Public fields:
     */
    uint32 bloodlineID() const
    {
        return m_bloodline->bloodlineID;
    }

    uint32 race() const
    {
        return m_bloodline->raceID;
    }

    // Account:

    uint32 accountID() const
    {
        return m_accountID;
    }

    const std::string & title() const
    {
        return m_title;
    }

    const std::string & description() const
    {
        return m_description;
    }

    bool gender() const
    {
        return m_gender;
    }

    double bounty() const
    {
        return m_bounty;
    }

    double balance() const
    {
        return m_balance;
    }

    double aurBalance() const
    {
        return m_aurBalance;
    }

    double securityRating() const
    {
        return m_securityRating;
    }

    uint32 logonMinutes() const
    {
        return m_logonMinutes;
    }

    void addSecurityRating(double secutiryAmount)
    {
        m_securityRating += secutiryAmount;
    }

    // Fleet:

    uint32 fleetID() const
    {
        return /*m_fleetID*/0;
    } //TODO  fixme when fleets are implemented

    uint32 wingID() const
    {
        return m_wingID;
    }

    uint32 squadID() const
    {
        return m_squadID;
    }

    uint8 fleetRole() const
    {
        return m_fleetRole;
    }

    uint8 fleetBooster() const
    {
        return m_fleetBooster;
    }

    uint8 fleetJob() const
    {
        return m_fleetJob;
    }

    // Corporation:

    uint32 corporationID() const
    {
        return m_corporationID;
    }

    uint32 corporationHQ() const
    {
        return m_corpHQ;
    }

    uint32 allianceID() const
    {
        return m_allianceID;
    }

    uint32 warFactionID() const
    {
        return m_warFactionID;
    }

    int32 corpAccountKey() const
    {
        return m_corpAccountKey;
    }

    // Corporation role:

    uint64 corpRole() const
    {
        return m_corpRole;
    }

    uint64 rolesAtAll() const
    {
        return m_rolesAtAll;
    }

    uint64 rolesAtBase() const
    {
        return m_rolesAtBase;
    }

    uint64 rolesAtHQ() const
    {
        return m_rolesAtHQ;
    }

    uint64 rolesAtOther() const
    {
        return m_rolesAtOther;
    }

    // Current location:

    uint32 stationID() const
    {
        return m_stationID;
    }

    uint32 solarSystemID() const
    {
        return m_solarSystemID;
    }

    uint32 constellationID() const
    {
        return m_constellationID;
    }

    uint32 regionID() const
    {
        return m_regionID;
    }

    // Ancestry, career:

    uint32 ancestryID() const
    {
        return m_ancestryID;
    }

    uint32 careerID() const
    {
        return m_careerID;
    }

    uint32 schoolID() const
    {
        return m_schoolID;
    }

    uint32 careerSpecialityID() const
    {
        return m_careerSpecialityID;
    }

    // Some importand dates:

    uint64 startDateTime() const
    {
        return m_startDateTime;
    }

    uint64 createDateTime() const
    {
        return m_createDateTime;
    }

    uint64 corporationDateTime() const
    {
        return m_corporationDateTime;
    }

    uint32 shipID() const
    {
        return m_shipID;
    }

    void SaveCharacter();
    void SaveFullCharacter();
    void SaveSkillQueue() const;
    void SaveCertificates() const;
    void SetActiveShip(uint32 shipID);

    bool canUse(InventoryItemRef item);

protected:
    Character(
              uint32 _characterID,
              // InventoryItem stuff:
              const InvTypeRef _charType,
              const ItemData &_data,
              // Character stuff:
              const CharacterData &_charData,
              const CorpMemberInfo &_corpData
              );

    /*
     * Member functions:
     */
    // Template loader:

    template<class _Ty>
    static RefPtr<_Ty> _LoadOwner(uint32 characterID,
                                  // InventoryItem stuff:
                                  const InvTypeRef charType, const ItemData &data)
    {
        // check it's a character
        if(charType->groupID != EVEDB::invGroups::Character)
        {
            SysLog::Error("Character", "Trying to load %s as Character.", charType->getGroup()->groupName.c_str());
            return RefPtr<_Ty>();
        }

        CharacterData charData;
        if(!InventoryDB::GetCharacter(characterID, charData))
            return RefPtr<_Ty>();

        CorpMemberInfo corpData;
        if(!InventoryDB::GetCorpMemberInfo(characterID, corpData))
            return RefPtr<_Ty>();

        // construct the item
        return CharacterRef(new Character(characterID, charType, data, charData, corpData));
    }

    virtual bool loadState();

    static uint32 _Spawn(
                         // InventoryItem stuff:
                         ItemData &data,
                         // Character stuff:
                         CharacterData &charData, CorpMemberInfo &corpData);

    uint32 inventoryID() const
    {
        return itemID();
    }

    PyRep *GetItem()
    {
        return getPackedRow();
    }

    void AddItem(InventoryItemRef item);
    void RemoveItem(InventoryItemRef item);

    void _CalculateTotalSPTrained();

    /*
     * Data members
     */
    uint32 m_accountID;

    std::string m_title;
    std::string m_description;
    bool m_gender;

    double m_bounty;
    double m_balance;
    double m_aurBalance;
    double m_securityRating;
    uint32 m_logonMinutes;

    uint32 m_fleetID;
    uint32 m_wingID;
    uint32 m_squadID;
    uint8 m_fleetRole;
    uint8 m_fleetBooster;
    uint8 m_fleetJob;

    int32 m_corpAccountKey;
    uint32 m_corporationID;
    uint32 m_corpHQ;
    uint32 m_allianceID;
    uint32 m_warFactionID;

    uint64 m_corpRole;
    uint64 m_rolesAtAll;
    uint64 m_rolesAtBase;
    uint64 m_rolesAtHQ;
    uint64 m_rolesAtOther;

    uint32 m_stationID;
    uint32 m_solarSystemID;
    uint32 m_constellationID;
    uint32 m_regionID;

    ChrBloodlineRef m_bloodline;
    uint32 m_ancestryID;
    uint32 m_careerID;
    uint32 m_schoolID;
    uint32 m_careerSpecialityID;

    uint64 m_startDateTime;
    uint64 m_createDateTime;
    uint64 m_corporationDateTime;

    uint32 m_shipID;

    // Skill queue:
    SkillQueue m_skillQueue;
    double m_totalSPtrained;
    uint64 m_trainingStartTime;
    uint32 m_freeSkillPoints;

    Certificates m_certificates;

    // Implants
    std::map<uint32, InventoryItemRef> m_implants;
};

#endif /* !__CHARACTER__H__INCL__ */

