#include "precompiled/pchnkernel.h"
//------------------------------------------------------------------------------
// nentityclasslibrary_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "util/nstringlist.h"

nNebulaScriptClass(nStringList, "nroot");

//------------------------------------------------------------------------------
/**
*/
nStringList::nStringList():
    currentMagic( 0 ),
    magicNumber( 0 )
{
    this->magicNumber = nEntityObjectServer::Instance()->GetHighId();
    this->currentMagic = this->magicNumber;
    this->SetSaveModeFlags( N_FLAG_SAVESHALLOW );
}

//------------------------------------------------------------------------------
/**
*/
nStringList::~nStringList()
{
    this->Clear();
}

//------------------------------------------------------------------------------
/**
*/
int __cdecl 
nStringList::StringItemSorter(StringItem * const* elm0, StringItem * const* elm1)
{
    return strcmp((*elm0)->name->Get() , (*elm1)->name->Get());
}

//------------------------------------------------------------------------------
/**
*/
void
nStringList::AddComponent( const nString & component )
{
    nString name = component.ExtractToLastSlash();

    if( -1 != name.FindChar( '/', 0 ) )
    {
        int index = component.FindChar( '/', 0 );
        name = component.ExtractRange( 0, index );
        name = name.TrimRight( "/" );

        name = this->GetFullName() + nString( "/" ) + name;

        nStringList * node;
        node = static_cast<nStringList*>( nKernelServer::Instance()->Lookup( name.Get() ) );
        if( ! node )
        {
            node = static_cast<nStringList*>( nKernelServer::Instance()->New( "nstringlist", name.Get() ) );
            node->SetMagicNumber( this->currentMagic );
            node->SetWizard( this->currentMagic );
        }
        n_assert( node );
        if( node )
        {
            name.Clear();

            int index = component.FindChar( '/', 0 );
            if( index != -1 )
            {
                name = component.ExtractRange( index, component.Length() - index );
                name = name.TrimLeft( "/" );
            }

            if( name.Length() )
            {
                node->AddComponent( name );
            }
            else
            {
                node->SetMagicNumber( this->currentMagic );
            }
        }
    }
    else
    {
        if( component.Length() > 0 )
        {
            this->AppendString( component );
        }
    }
}
