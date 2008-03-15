#include "precompiled/pchnkernel.h"
//-----------------------------------------------------------------------------
//  nstringlist_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "util/nstringlist.h"
#include "kernel/ncmd.h"

static void n_getlist( void *, nCmd *);
static void n_setlist( void *, nCmd *);

NSCRIPT_INITCMDS_BEGIN ( nStringList )
    // No macro support
    cl->AddCmd("l_getlist_v", 'GETL', n_getlist);
    cl->AddCmd("v_setlist_l", 'SETL', n_setlist);

    NSCRIPT_ADDCMD('ADDC', void, AppendString, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('SWZR', void, SetWizard, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SLWZ', void, SetLocalWizard , 0, (), 0, ());
    NSCRIPT_ADDCMD('ADCT', void, AddComponent, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('REMC', void, RemoveString, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('SETN', void, ChangeStringName, 2, (const nString&, const nString&), 0, ());
    NSCRIPT_ADDCMD('FNDC', int, FindString, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('GETS', nString*, GetString, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CLRL', void, Clear, 0, (), 0, ());
    NSCRIPT_ADDCMD('GSIZ', int, Size, 0, (), 0, ());
    NSCRIPT_ADDCMD('GMNB', int, GetMagicNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD('SMNB', void, SetMagicNumber, 1, (int), 0, ());
NSCRIPT_INITCMDS_END ()

//------------------------------------------------------------------------------
/**
*/
static void n_getlist (void *o, nCmd *cmd)
{
    nStringList *self = (nStringList *) o;

    int num_strings = self->Size();
    nArg* strings = n_new_array(nArg,num_strings);
    n_assert(strings);

    for (int i = 0; i < num_strings; i++)
    {
        strings[i].SetS(self->GetString(i)->Get());
    }
    cmd->Out()->SetL(strings, num_strings);    
}

//------------------------------------------------------------------------------
/**
*/
static void n_setlist (void *o, nCmd *cmd)  
{  
    nStringList *self = (nStringList *) o;  
    self->Clear();  
    nArg* strings;  
    int num_strings = cmd->In()->GetL( strings );     
    n_assert(strings);  

    for (int i = 0; i < num_strings; i++)  
    {  
        self->AppendString( strings[i].GetS() );  
    }
}  

//------------------------------------------------------------------------------
/**
*/
void
nStringList::PutString( nEntityClassServer::ArraySortedStrings & strings, int key, nString * string )
{
    int index = (key & 0xff000000) >> 24;
    bool valid = ( index >= 0 ) && ( index < strings.Size() );
    n_assert( valid );
    if( valid )
    {
        strings[ index ].Append( string );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nStringList::SaveStrings( nEntityClassServer::ArraySortedStrings & strings, nString path )
{
    // put the strings in its buckets
    for( int i = 0 ; i < this->list.Size() ; ++i )
    {
        nString * newString = n_new( nString );
        newString->Append( path );
        newString->Append( (*this->list[i]->name) );
        this->PutString( strings, this->list[i]->magic, newString );
    }

    // put childrens in its buckets
    nStringList * children = static_cast<nStringList*>( this->GetHead() );
    while( children )
    {
        nString * newString = n_new( nString );
        newString->Append( path );
        newString->Append( children->GetName() );
        newString->Append( "/" );
        this->PutString( strings, children->GetMagicNumber(), newString );
        children->SaveStrings( strings, *newString );
        children = static_cast<nStringList*>( children->GetSucc() );
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nStringList::SaveCmds (nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nEntityClassServer::ArraySortedStrings strings;
        strings.SetFixedSize( 0x100 );

        // get strings
        this->SaveStrings( strings, "" );

        // save strings
        for( int i = 0 ; i < strings.Size() ; ++i )
        {
            ps->Put( this, 'SWZR', (i << 24) );

            for( int j = 0 ; j < strings[ i ].Size() ; ++j )
            {
                ps->Put( this, 'ADCT', strings[ i ][ j ]->Get() );
            }
        }
        ps->Put( this, 'SLWZ' );

        // delete strings
        for( int i = 0 ; i < strings.Size() ; ++i )
        {
            for( int j = 0 ; j < strings[ i ].Size() ; ++j )
            {
                n_delete( strings[ i ][ j ] );
            }
        }
        strings.Reset();

        return true;
    }        
    return false;
}