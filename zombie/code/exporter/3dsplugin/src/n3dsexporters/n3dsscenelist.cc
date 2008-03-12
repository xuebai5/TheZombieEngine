#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "max.h"
#include "igame/igame.h"
#pragma warning( pop )
#include "n3dsexporters/n3dsscenelist.h"


//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::n3dsSceneList() : list()
{
    for ( int i = n3dsObject::graphics ; i < n3dsObject::NumTypes ; ++i)
    {
        listCount[i] = 0 ;
    }
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::n3dsSceneList(int initialSize, int initialGrow) : list(initialSize, initialGrow) 
{
    for ( int i = n3dsObject::graphics ; i < n3dsObject::NumTypes ; ++i)
    {
        listCount[i] = 0 ;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSceneList::Append(const n3dsObject& elm)
{
    n3dsObject::ObjectType type = elm.GetType();
    n3dsObject::ObjectSubType subType = elm.GetSubType();

    if ( type != n3dsObject::InvalidType )
    {
        listCount[type]++;
    }

    if ( subType != n3dsObject::InvalidSubType)
    {
        listCount[subType]++;
    }


    list.Append(elm);
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsSceneList::Count(n3dsObject::ObjectType type) const
{
    if ( type < n3dsObject::NumTypes )
    {
        return listCount[ type ] ;
    }
    else if ( type == n3dsObject::InvalidType )
    {
        int count = 0;
        for ( int i = n3dsObject::graphics ; i < n3dsObject::NumTypes ; ++i)
        {
            count += listCount[i];
        }

        return list.Size() - count;
    } else
    {
        n_assert_always();
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsSceneList::Count(n3dsObject::ObjectSubType subType) const
{
    if ( subType < n3dsObject::NumSubTypes )
    {
        return this->subCount[ subType ] ;
    }
    else if ( subType == n3dsObject::InvalidSubType )
    {
        int count = 0;
        for ( int i = 0 ; i < n3dsObject::NumSubTypes ; ++i)
        {
            count += subCount[i];
        }

        return list.Size() - count;
    } else
    {
        n_assert_always();
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::iterator() : 
    index( 0 ), 
    count( 0 ),
    list( 0 ), 
    showBar( false ),
    validSubType(false),
    validType(false),
    subType( n3dsObject::InvalidSubType),
    type( n3dsObject::InvalidType)
{
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::iterator(int index, n3dsSceneList const* list) : 
    showBar( false ),
    count(0),
    validSubType(false),
    validType( false),
    subType( n3dsObject::InvalidSubType),
    type( n3dsObject::InvalidType)
{
    this->index = index;
    this->list = list;
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::iterator(int index, n3dsSceneList const* list, n3dsObject::ObjectType const&  type) :
    showBar( false ),
    validSubType(false),
    subType( n3dsObject::InvalidSubType),
    count(0)
{
    this->index = index;
    this->list = list;
    this->validType = true;
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::iterator(int index, n3dsSceneList const* list, n3dsObject::ObjectType const&  type, n3dsObject::ObjectSubType const& subType) :
    showBar( false ),
    validSubType(false),
    subType( n3dsObject::InvalidSubType),
    count(0)
{
    this->index = index;
    this->list = list;
    this->validType = true;
    this->validSubType = true;
    this->type = type;
    this->subType = subType;
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::iterator(iterator const& it)
{
    *this = it;
    showBar = false;
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator& 
n3dsSceneList::iterator::operator = ( iterator const& it )
{
    this->index = it.index;
    this->count = it.count;
    this->list  = it.list;
    this->validType = it.validType;
    this->type      = it.type;
    this->subType = it.subType;
    this->validSubType = it.validSubType;
    this->showBar = false;
    return *this;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsSceneList::iterator::operator == ( iterator const& it ) const
{
    if( this->index != it.index )
        return false;
    if( this->list != it.list )
        return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator&
n3dsSceneList::iterator::operator ++ ()
{
    int const size = list->Size();
    if ( index < size)
    {
        ++index;
        ++count;
        if ( this->validSubType)
        {
            // increment index is diferent type or diferent subtype
            while ( index < size && ( this->type != (*list)[index].GetType() || this->subType !=  (*list)[index].GetSubType() ) ) 
            {
                index++;
            }
        } else if ( this->validType)
        {
            while ( index < size && this->type != (*list)[index].GetType() ) 
            {
                index++;
            }
        }
    
    }

    if (showBar)
    {
        if (index < size)
        {
            tmpString = (*list)[index].GetNode()->GetName() ;
            if ( validSubType)
            {
                GetCOREInterface()->ProgressUpdate( (int)( 100.0f*count / list->Count( subType ) ),FALSE, tmpString.data() ); 
            }
            else if ( validType )
            {
                GetCOREInterface()->ProgressUpdate( (int)( 100.0f*count / list->Count( type ) ),FALSE, tmpString.data() ); 
            } else
            {
                GetCOREInterface()->ProgressUpdate( (int)( 100.0f*count / size ),FALSE, tmpString.data() ); 
            }

        } else
        {
            GetCOREInterface()->ProgressEnd();
            showBar = false;
        }
    }
    return *this;
}

// Dummy function for progress bar
DWORD WINAPI dummybarfn(LPVOID /*arg*/)
{
    return(0);
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSceneList::iterator::ShowProgressBar( const nString& title)
{
    if ( index < list->Size() )
    {
        showBar = true;
        barTile = title;
        GetCOREInterface()->ProgressStart( _T((char*)barTile.Get() ) , TRUE, dummybarfn, 0 );
    }
}

//------------------------------------------------------------------------------
/**
*/
n3dsSceneList::iterator::~iterator()
{
    if ( showBar )
    {
        GetCOREInterface()->ProgressEnd();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsSceneList::SortMaxId() 
{
    qsort( &list[0], list.Size() , sizeof(n3dsObject) , n3dsSceneList::SortId );
}

//------------------------------------------------------------------------------
/**
*/
int __cdecl 
n3dsSceneList::SortId(const void* elm0, const void* elm1)
{
    n3dsObject* node0 = (n3dsObject*)elm0;
    n3dsObject* node1 = (n3dsObject*)elm1;
    ULONG val0 = node0->GetNode()->GetMaxNode()->GetHandle();
    ULONG val1 = node1->GetNode()->GetMaxNode()->GetHandle();

    if ( val0 < val1 )
    {
        return -1;
    } else if ( val0 > val1)
    {
        return 1;
    } else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------