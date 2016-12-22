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
    Author:        Zhur, Cometo
*/

#include "eve-server.h"

#include "PyServiceCD.h"
#include "character/CharMgrService.h"
#include "PyServiceMgr.h"

PyCallable_Make_InnerDispatcher(CharMgrService)

CharMgrService::CharMgrService()
: PyService("charMgr", new Dispatcher(this))
{
    CharacterDB::Init();
    PyCallable_REG_CALL(CharMgrService, GetPublicInfo)
    PyCallable_REG_CALL(CharMgrService, GetPublicInfo3)
    PyCallable_REG_CALL(CharMgrService, GetTopBounties)
    PyCallable_REG_CALL(CharMgrService, AddToBounty)
    PyCallable_REG_CALL(CharMgrService, GetOwnerNoteLabels)
    PyCallable_REG_CALL(CharMgrService, AddOwnerNote)
    PyCallable_REG_CALL(CharMgrService, GetContactList)
    PyCallable_REG_CALL(CharMgrService, GetCloneTypeID)
    PyCallable_REG_CALL(CharMgrService, GetHomeStation)
    PyCallable_REG_CALL(CharMgrService, GetFactions)
    PyCallable_REG_CALL(CharMgrService, SetActivityStatus)
    PyCallable_REG_CALL(CharMgrService, GetSettingsInfo)
    PyCallable_REG_CALL(CharMgrService, LogSettings)
    PyCallable_REG_CALL(CharMgrService, GetCharacterDescription)
    PyCallable_REG_CALL(CharMgrService, SetCharacterDescription)
    PyCallable_REG_CALL(CharMgrService, GetNote)
    PyCallable_REG_CALL(CharMgrService, SetNote)
    PyCallable_REG_CALL(CharMgrService, AddContact)
    PyCallable_REG_CALL(CharMgrService, EditContact)
    PyCallable_REG_CALL(CharMgrService, GetRecentShipKillsAndLosses)
    PyCallable_REG_CALL(CharMgrService, GetCharacterSettings)

    //these 2 are for labels in PnP window
    PyCallable_REG_CALL(CharMgrService, GetLabels)
    PyCallable_REG_CALL(CharMgrService, CreateLabel)
}

CharMgrService::~CharMgrService() {
}

PyResult CharMgrService::Handle_GetContactList(PyCallArgs &call)
{
    // another dummy
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("contactID", DBTYPE_I4);
    header->AddColumn("inWatchList", DBTYPE_BOOL);
    header->AddColumn("relationshipID", DBTYPE_R8);
    header->AddColumn("labelMask", DBTYPE_I8);
    CRowSet *rowset = new CRowSet( &header );

    PyDict* dict = new PyDict();
    dict->SetItemString("addresses", rowset);
    dict->SetItemString("blocked", rowset);
    PyObject *keyVal = new PyObject( "utillib.KeyVal", dict);

    return keyVal;
}

PyResult CharMgrService::Handle_AddOwnerNote( PyCallArgs& call )
{

  return NULL;
}
PyResult CharMgrService::Handle_GetOwnerNoteLabels(PyCallArgs &call)
{
    // just a dummy for now
    DBRowDescriptor *header = new DBRowDescriptor();
    header->AddColumn("noteID", DBTYPE_I4);
    header->AddColumn("label", DBTYPE_WSTR);
    CRowSet *rowset = new CRowSet( &header );

    return rowset;
}

PyResult CharMgrService::Handle_GetPublicInfo(PyCallArgs &call) {
    //takes a single int arg: char id
    // or corp id
    Call_SingleIntegerArg args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "%s: Failed to decode arguments.", call.client->GetName());
        return NULL;
    }

    /*if(IsAgent(args.arg)) {
        //handle agents special right now...
        PyRep *result = CharacterDB::GetAgentPublicInfo(args.arg);
        if(result == NULL) {
            codelog(CLIENT__ERROR, "%s: Failed to find agent %u", call.client->GetName(), args.arg);
            return NULL;
        }
        return result;
    }*/

    PyRep *result = CharacterDB::GetCharPublicInfo(args.arg);
    if(result == NULL) {
        codelog(CLIENT__ERROR, "%s: Failed to find char %u", call.client->GetName(), args.arg);
        return NULL;
    }

    return result;
}

PyResult CharMgrService::Handle_GetPublicInfo3(PyCallArgs &call) {
    //takes a single int arg: char id
    Call_SingleIntegerArg args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "%s: Failed to decode arguments.", call.client->GetName());
        return NULL;
    }

    PyRep *result = CharacterDB::GetCharPublicInfo3(args.arg);
    if(result == NULL) {
        codelog(CLIENT__ERROR, "%s: Failed to find char %u", call.client->GetName(), args.arg);
        return NULL;
    }

    return result;
}

PyResult CharMgrService::Handle_AddToBounty( PyCallArgs& call ) {
    Call_TwoIntegerArgs args;
    if(!args.Decode(&call.tuple)) {
        SysLog::Error("CharMgrService", "AddToBounty Error decoding arguments for Call_TwoIntegerArgs");
        return NULL;
    }
    if(call.client->GetCharacterID() == (uint32)args.arg1) {
        SysLog::Error("CharMgrService", "AddToBounty: Char %u tried to add bounty to themself", args.arg1);
        call.client->SendNotifyMsg("You cannot add a bounty to yourself.");
        return NULL;
    }
    if(call.client->GetChar()->AlterBalance(-args.arg2)) // Need to send it to an actual account for stats at some point
        CharacterDB::AddBounty(args.arg1, args.arg2);
    return new PyNone;
}

PyResult CharMgrService::Handle_GetTopBounties( PyCallArgs& call )
{
    PyRep *result = CharacterDB::GetTopBounties();
    if(result == NULL) {
        SysLog::Error("CharMgrService","GetTopBounties error, Failed to find bounties: %s", call.client->GetName());
        return NULL;
    }
    return result;
}

PyResult CharMgrService::Handle_GetCloneTypeID( PyCallArgs& call )
{
	uint32 typeID;
    if (!CharacterDB::GetActiveCloneType(call.client->GetCharacterID(), typeID))
	{
		// This should not happen, because a clone is created at char creation.
		// We don't have a clone, so return a basic one. cloneTypeID = 9917 (Clone Grade Delta)
		typeID = 9917;
		SysLog::Debug( "CharMgrService", "Returning a basic clone for Char %u of type %u", call.client->GetCharacterID(), typeID );
	}
    return new PyInt(typeID);
}

PyResult CharMgrService::Handle_GetHomeStation( PyCallArgs& call )
{
	uint32 stationID;
    if (!CharacterDB::GetCharHomeStation(call.client->GetCharacterID(), stationID))
	{
		SysLog::Debug( "CharMgrService", "Could't get the home station for Char %u", call.client->GetCharacterID() );
		return new PyNone;
	}
    return new PyInt(stationID);
}

PyResult CharMgrService::Handle_GetFactions( PyCallArgs& call )
{
    SysLog::Debug( "CharMgrService", "Called GetFactions stub." );

    return NULL;
}

PyResult CharMgrService::Handle_SetActivityStatus( PyCallArgs& call )
{
    SysLog::Debug( "CharMgrService", "Called SetActivityStatus stub." );

    return NULL;
}

PyResult CharMgrService::Handle_GetSettingsInfo( PyCallArgs& call )
{
    PyTuple *res = new PyTuple(2);
    /* def dumb():
     *     return ""
     * marshal.dumps(dumb.func_code).encode('hex')
     * Taken on Python 2.7.11
     *
     * WARNING: THIS IS REMOTE CODE EXECUTION
     * SUGGEST WE PROVIDE OPTIONAL CLIENT MODIFICATION TO DISABLE THIS FUNCTION CALL COMPLETELY CLIENTSIDE
     * DUE TO PLACEBO CRYPTO BEING REQUIRED, NO CRYPTO VERIFICATION IS PERFORMED ON THE CLIENT FOR THIS FUNCTION CALL
     *
     */
    static const uint8 emptyString[] = { 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                                         0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x73, 0x04, 0x00,
                                         0x00, 0x00, 0x64, 0x01, 0x00, 0x53, 0x28, 0x02, 0x00, 0x00,
                                         0x00, 0x4e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
                                         0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
                                         0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x73, 0x07, 0x00,
                                         0x00, 0x00, 0x3c, 0x73, 0x74, 0x64, 0x69, 0x6e, 0x3e, 0x74,
                                         0x03, 0x00, 0x00, 0x00, 0x72, 0x65, 0x74, 0x01, 0x00, 0x00,
                                         0x00, 0x73, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 };
    res->items[ 0 ] = new PyBuffer( emptyString, emptyString + sizeof( emptyString ) );

    res->items[ 1 ] = new PyInt(0);
    return res;
}

//  this is a return call from client after GetSettingsInfo
PyResult CharMgrService::Handle_LogSettings( PyCallArgs& call ) {
  /*
    */
 return NULL;
}

PyResult CharMgrService::Handle_GetCharacterDescription(PyCallArgs &call)
{
    //takes characterID
    Call_SingleIntegerArg args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments");
        return NULL;
    }

    ItemFactory::SetUsingClient(call.client);
    CharacterRef c = ItemFactory::GetCharacter(args.arg);
    if( !c ) {
        _log(CLIENT__ERROR, "GetCharacterDescription failed to load character %u.", args.arg);
        return NULL;
    }

    return new PyString(c->description());
}

PyResult CharMgrService::Handle_SetCharacterDescription(PyCallArgs &call)
{
    //takes WString of bio
    Call_SingleWStringSoftArg args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments");
        return NULL;
    }

    CharacterRef c = call.client->GetChar();
    if( !c ) {
        _log(CLIENT__ERROR, "SetCharacterDescription called with no char!");
        return NULL;
    }
    c->SetDescription(args.arg.c_str());

    return NULL;
}


PyResult CharMgrService::Handle_GetNote( PyCallArgs& call )
{/*
    //  will add this completed code at a later date  -allan 25Jul14
    uint32 ownerID = call.client->GetCharacterID();
    uint32 itemID = call.tuple->GetItem(0)->AsInt()->value();

	PyString *str = CharacterDB::GetNote(ownerID, itemID);
    if(!str)
        str = new PyString("");

    return str;
	*/
	return new PyNone;
}

PyResult CharMgrService::Handle_SetNote(PyCallArgs &call)
{/*
    //  will add this completed code at a later date  -allan 25Jul14
    Call_SetNote args;
    if(!args.Decode(&call.tuple)) {
        codelog(CLIENT__ERROR, "Invalid arguments");
        return NULL;
    }

    CharacterDB::SetNote(call.client->GetCharacterID(), args.itemID, args.note.c_str());
*/
    return new PyNone;
}

PyResult CharMgrService::Handle_AddContact( PyCallArgs& call )
{
    //  will add this completed code at a later date  -allan 25Jul14
  return NULL;
}

PyResult CharMgrService::Handle_EditContact( PyCallArgs& call )
{
    //  will add this completed code at a later date  -allan 25Jul14
  return NULL;
}

PyResult CharMgrService::Handle_GetRecentShipKillsAndLosses( PyCallArgs& call )
{
  return NULL;
}

PyResult CharMgrService::Handle_GetCharacterSettings(PyCallArgs& call)
{
    // No args

    return new PyObject("utillib.KeyVal", new_dict(new PyString("__doc__"), new PyString("Settings KeyVal/Dictionary")));
}

PyResult CharMgrService::Handle_GetLabels(PyCallArgs& call)
{
    //  will add this completed code at a later date  -allan 25Jul14
  return NULL;
}

PyResult CharMgrService::Handle_CreateLabel( PyCallArgs& call )
{
    //  will add this completed code at a later date  -allan 25Jul14
  return NULL;
}
