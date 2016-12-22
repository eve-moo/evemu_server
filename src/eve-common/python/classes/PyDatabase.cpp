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
    Author:     Bloody.Rabbit
*/

#include "eve-common.h"

#include "python/classes/PyDatabase.h"

#include "utils/EVEUtils.h"

/************************************************************************/
/* DBRowDescriptor                                                      */
/************************************************************************/
DBRowDescriptor::DBRowDescriptor()
: PyObjectEx_Type1( new PyToken( "blue.DBRowDescriptor" ), _CreateArgs() )
{
}

DBRowDescriptor::DBRowDescriptor(PyList* keywords)
: PyObjectEx_Type1( new PyToken( "blue.DBRowDescriptor" ), _CreateArgs(), keywords )
{
}

DBRowDescriptor::DBRowDescriptor( const DBQueryResult& res )
: PyObjectEx_Type1( new PyToken( "blue.DBRowDescriptor" ), _CreateArgs() )
{
    uint32 cc = res.ColumnCount();

    for( uint32 i = 0; i < cc; i++ )
        AddColumn( res.ColumnName( i ), res.ColumnType( i ) );
}

DBRowDescriptor::DBRowDescriptor( const DBResultRow& row )
: PyObjectEx_Type1( new PyToken( "blue.DBRowDescriptor" ), _CreateArgs() )
{
    uint32 cc = row.ColumnCount();

    for( uint32 i = 0; i < cc; i++ )
        AddColumn( row.ColumnName( i ), row.ColumnType( i ) );
}

uint32 DBRowDescriptor::ColumnCount() const
{
    return _GetColumnList()->size();
}

PyString* DBRowDescriptor::GetColumnName( uint32 index ) const
{
    return pyAs(String, _GetColumn( index )->GetItem( 0 ));
}

DBTYPE DBRowDescriptor::GetColumnType( uint32 index ) const
{
    return (DBTYPE)pyAs(Int, _GetColumn( index )->GetItem( 1 ))->value();
}

uint32 DBRowDescriptor::FindColumn( const char* name ) const
{
    uint32 cc = ColumnCount();
    PyString* stringName = new PyString( name );

    for( uint32 i = 0; i < cc; i++ )
    {
        if( stringName->hash() == GetColumnName( i )->hash() )
        {
            PyDecRef( stringName );
            return i;
        }
    }

    PyDecRef( stringName );
    return cc;
}

bool DBRowDescriptor::VerifyValue( uint32 index, PyRep* value )
{
    return DBTYPE_IsCompatible( GetColumnType( index ), value );
}

void DBRowDescriptor::AddColumn( const char* name, DBTYPE type )
{
    PyTuple* col = new PyTuple( 2 );

    col->SetItem( 0, new PyString( name ) );
    col->SetItem( 1, new PyInt( type ) );

    _GetColumnList()->items.push_back( col );
}

void DBRowDescriptor::dump(std::ostringstream &ss, const std::string &pfx) const
{
    std::string pfx1(pfx + "    ");
    std::string pfx2(pfx1 + "    ");
    ss << pfx << "[DBRowDescriptor]" << std::endl;
    for(int i = 0; i < (int)ColumnCount(); i++)
    {
        ss << pfx1 << "['" << GetColumnName(i) << "' [" << DBTYPE_NAME[GetColumnType(i)] <<"] ]" << std::endl;
    }
    PyRep *keywords = nullptr;
    PyTuple* t = pyAs(Tuple, header());
    if( t->size() >= 3 )
    {
        keywords = t->GetItem( 2 );
    }
    if(keywords != nullptr)
    {
        ss << pfx1 << "Keywords:" << std::endl;
        keywords->dump(ss, pfx2);
    }
}

PyTuple* DBRowDescriptor::_GetColumnList() const
{
    return pyAs(Tuple, GetArgs()->GetItem( 0 ));
}

PyTuple* DBRowDescriptor::_GetColumn( size_t index ) const
{
    return pyAs(Tuple, _GetColumnList()->GetItem( index ));
}

PyTuple* DBRowDescriptor::_CreateArgs()
{
    PyTuple* columnList = new PyTuple( 0 );

    PyTuple* args = new PyTuple( 1 );
    args->SetItem( 0, columnList );

    return args;
}

/************************************************************************/
/* CRowSet                                                              */
/************************************************************************/
CRowSet::CRowSet( DBRowDescriptor** rowDesc)
: PyObjectEx_Type2(new_tuple(new PyToken("carbon.common.script.sys.crowset.CRowset")), _CreateKeywords(*rowDesc))
{
    *rowDesc = NULL;
}

CRowSet::CRowSet(DBRowDescriptor** rowDesc, PyTuple** columnList)
: PyObjectEx_Type2(new_tuple(new PyToken("carbon.common.script.sys.crowset.CRowset")), _CreateKeywords(new PyObjectEx(false, new_tuple(*rowDesc, *columnList))))
{
    *rowDesc = NULL;
    *columnList = NULL;
}

PyPackedRow* CRowSet::NewRow()
{
    DBRowDescriptor* rowDesc = _GetRowDesc();
    PyIncRef( rowDesc );

    PyPackedRow* row = new PyPackedRow( rowDesc );

    list().AddItem( row );
    return row;
}

void CRowSet::dump(std::ostringstream &ss, const std::string &pfx) const
{
    std::string pfx1(pfx + "    ");
    std::string pfx2(pfx1 + "    ");
    ss << pfx << "[CRowSet]" << std::endl;
    DBRowDescriptor* desc = _GetRowDesc();
    if(desc == nullptr)
    {
        ss << pfx1 << "No Descriptor." << std::endl;
        return;
    }
    desc->dump(ss, pfx1);
    ss << pfx1 << "Rows:" << std::endl;
    for(int i = 0; i < (int)GetRowCount(); i++)
    {
        PyPackedRow *row = GetRow(i);
        if(row != nullptr)
        {
            row->dump(ss, pfx2);
        }
    }
    PyRep *keywords = nullptr;
    PyTuple* t = pyAs(Tuple, header());
    if( t->size() >= 3 )
    {
        keywords = t->GetItem( 2 );
    }
    if(keywords != nullptr)
    {
        ss << pfx1 << "Keywords:" << std::endl;
        keywords->dump(ss, pfx2);
    }
}

DBRowDescriptor* CRowSet::_GetRowDesc() const
{
    PyRep* r = FindKeyword( "header" );
    assert( r );

    return (DBRowDescriptor*)pyAs(ObjectEx, r);
}

PyDict* CRowSet::_CreateKeywords(PyRep* rowDesc)
{
    assert( rowDesc );

    PyDict* keywords = new PyDict;
    keywords->SetItemString( "header", rowDesc );

    return keywords;
}

/************************************************************************/
/* CIndexedRowSet                                                              */
/************************************************************************/
CIndexedRowSet::CIndexedRowSet( DBRowDescriptor** rowDesc)
: PyObjectEx_Type2(new_tuple(new PyToken("dbutil.CIndexedRowset")), _CreateKeywords(*rowDesc))
{
    *rowDesc = NULL;
}

PyPackedRow* CIndexedRowSet::NewRow( PyRep* key )
{
    DBRowDescriptor* rowDesc = _GetRowDesc();
    PyIncRef( rowDesc );

    PyPackedRow* row = new PyPackedRow( rowDesc );

    dict().SetItem( key , row );
    return row;
}

void CIndexedRowSet::dump(std::ostringstream &ss, const std::string &pfx) const
{
    std::string pfx1(pfx + "    ");
    std::string pfx2(pfx1 + "    ");
    ss << pfx << "[CIndexedRowSet]" << std::endl;
    PyDict *keywords = nullptr;
    PyTuple* t = pyAs(Tuple, header());
    if( t->size() >= 3 )
    {
        PyRep *obj = t->GetItem( 2 );
        pyIsAs(Dict, obj, keywords);
    }
    if(keywords != nullptr)
    {
        PyRep *idx = keywords->GetItemString("columnName");
        PyString *str;
        if(idx != nullptr && pyIsAs(String, idx, str))
        {
            ss << pfx1 << "index: " << str->content() << std::endl;
        }
        else
        {
            ss << pfx1 << "No index." << std::endl;
        }
    }
    DBRowDescriptor* desc = _GetRowDesc();
    if(desc == nullptr)
    {
        ss << pfx1 << "No Descriptor." << std::endl;
        return;
    }
    desc->dump(ss, pfx1);
    ss << pfx1 << "Rows:" << std::endl;
    for(auto entry : dict().items)
    {
        PyRep *key = entry.first;
        key->dump(ss, pfx1 + "Key:");
        PyRep *value = entry.second;
        value->dump(ss, pfx2);
    }
    if(keywords != nullptr)
    {
        ss << pfx1 << "Keywords:" << std::endl;
        keywords->dump(ss, pfx2);
    }
}

DBRowDescriptor* CIndexedRowSet::_GetRowDesc() const
{
    PyRep* r = FindKeyword( "header" );
    assert( r );

    return (DBRowDescriptor*)pyAs(ObjectEx, r);
}

PyDict* CIndexedRowSet::_CreateKeywords(DBRowDescriptor* rowDesc)
{
    assert( rowDesc );

    PyDict* keywords = new PyDict;
    keywords->SetItemString( "header", rowDesc );
    keywords->SetItemString( "columnName", rowDesc->GetColumnName(0) );


    return keywords;
}


/************************************************************************/
/* CFilterRowSet                                                              */
/************************************************************************/
CFilterRowSet::CFilterRowSet( DBRowDescriptor** rowDesc)
: PyObjectEx_Type2(new_tuple(new PyToken("dbutil.CFilterRowset")), _CreateKeywords(*rowDesc))
{
    *rowDesc = NULL;
}

	

CRowSet* CFilterRowSet::NewRowset( PyRep* key )
{
    DBRowDescriptor* rowDesc = _GetRowDesc();
    PyIncRef( rowDesc );

    CRowSet* row = new CRowSet( &rowDesc );

    dict().SetItem( key , row );
    return row;
}

DBRowDescriptor* CFilterRowSet::_GetRowDesc() const
{
    PyRep* r = FindKeyword( "header" );
    assert( r );

    return (DBRowDescriptor*)pyAs(ObjectEx, r);
}

PyDict* CFilterRowSet::_CreateKeywords(DBRowDescriptor* rowDesc)
{
    assert( rowDesc );

    PyDict* keywords = new PyDict;
    keywords->SetItemString( "header", rowDesc );
    keywords->SetItemString( "columnName", rowDesc->GetColumnName(0) );


    return keywords;
}
