#ifndef N_NSTRINGLIST_H
#define N_NSTRINGLIST_H
//-------------------------------------------------------------------
/**
    @file nstringlist.h
    @class nStringList
    @ingroup NebulaConjurerEditor

    @author Javier Ferrer Gomez

    @brief nString list with scripting interface

    (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "util/nstring.h"
#include "util/nsortedarray.h"

#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"

class nStringList: public nRoot
{
#ifndef NO_ENTITY

public:
    /// Constructor
    nStringList();
    /// Delete the object
    ~nStringList();

    /// Append a string to the list
    void AppendString(const nString&);

    /// Set wizard key id
    void SetWizard(int);

    /// Set local wizard key id
    void SetLocalWizard ();

    /// Append a string with path to the list
    void AddComponent(const nString&);

    /// Remove a string to list
    void RemoveString(const nString&);

    /// Change a string name
    void ChangeStringName(const nString&, const nString&);

    /// Return the string index in the array
    int FindString(const nString&);

    /// Return the string in the index
    nString* GetString(int);

    /// Clear the list
    void Clear();

    /// Return the lenght of the array
    int Size();

    /// get the root magic number
    int GetMagicNumber () const;

    /// set the root magic number
    void SetMagicNumber(int);

    /// String list persistency
    bool SaveCmds (nPersistServer* /*ps*/);

private:

    struct StringItem{
        nString * name;
        int magic;
    };
    static int __cdecl StringItemSorter(StringItem* const* elm0, StringItem* const* elm1);

    void SaveStrings( nEntityClassServer::ArraySortedStrings & strings, nString path );
    void PutString( nEntityClassServer::ArraySortedStrings & strings, int key, nString * string );
    nSortedArray<StringItem*, StringItemSorter> list;
    int currentMagic;
    int magicNumber;
    nRoot* test;
#endif
};

#ifndef NO_ENTITY

//------------------------------------------------------------------------------
/**
*/
inline
void 
nStringList::AppendString(const nString & name)
{
    int index = this->FindString( name );
    if (index == -1 ) {
        StringItem * item = n_new( StringItem );
        n_assert( item );
        if( item )
        {
            item->name = n_new(nString(name));
            item->magic = this->currentMagic;
        }
        this->list.Append( item );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nStringList::RemoveString( const nString & name)
{   
    int index = this->FindString( name );
    if (index == -1 ) {
        return;    
    }   
    StringItem * temp = this->list[ index ];
    list.nArray<StringItem*>::EraseQuick( index );
    n_delete( temp->name );
    n_delete( temp );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nStringList::ChangeStringName(const nString & oldName, const nString & newName)
{
    int index = this->FindString( oldName );
    if (index != -1)
    {        
        ( * this->list[index]->name ) = newName;
    }    
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nStringList::FindString(const nString & name)
{
    int i = 0;
    for (nArray<StringItem*>::iterator str  = this->list.Begin();
                                    str != this->list.End();
                                    str++, i++)
    {
        if ( (*(*str)->name) == name )
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString*
nStringList::GetString(int index)
{
    bool valid = index <= this->list.Size();
    n_assert( valid );
    if( valid )
    {
        return this->list[index]->name;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringList::Clear()
{
    for (int i=0; i < this->list.Size(); i++)
    {   
        n_delete(this->list[i]->name);
        n_delete(this->list[i]); 
    }
    this->list.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStringList::Size()
{
    return this->list.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringList::SetWizard( int number )
{
    this->currentMagic = number;
    // set childrens wizard
    nStringList * children = static_cast<nStringList*>( this->GetHead() );
    while( children )
    {
        children->SetWizard( number );
        children = static_cast<nStringList*>( children->GetSucc() );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringList::SetLocalWizard( )
{
    this->currentMagic = nEntityObjectServer::Instance()->GetHighId();
    // set childrens wizard
    nStringList * children = static_cast<nStringList*>( this->GetHead() );
    while( children )
    {
        children->SetWizard( this->currentMagic );
        children = static_cast<nStringList*>( children->GetSucc() );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStringList::SetMagicNumber( int number )
{
    this->magicNumber = number;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStringList::GetMagicNumber( ) const
{
    return this->magicNumber;
}

#endif

#endif
