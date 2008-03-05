//------------------------------------------------------------------------------
//  ndebuggraphicsserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebuggraphicsserver.h"

#include "ndebug/ndebuggraphicelement.h"

#include "gfx2/ngfxserver2.h"

#include "ndebug/ndebugtext.h"

//------------------------------------------------------------------------------
nDebugGraphicsServer nDebugGraphicsServer::instance;

const float nDebugGraphicsServer::MIN_LIFE = 0.001f;

//------------------------------------------------------------------------------
/**
    @returns the nDebugGraphicsServer instance
*/
nDebugGraphicsServer * 
nDebugGraphicsServer::Instance()
{
    return & nDebugGraphicsServer::instance;
}

//------------------------------------------------------------------------------
/**
*/
nDebugGraphicsServer::nDebugGraphicsServer()
{
    // empty
}
 
//------------------------------------------------------------------------------
/**
*/
nDebugGraphicsServer::~nDebugGraphicsServer()
{
    this->DeleteElements();
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugGraphicsServer::DeleteElements()
{
    for( int i = 0 ; i < this->elements.Size() ; /* empty */ )
    {
        n_delete( this->elements[i] );
        this->elements.Erase( i );
    }
}

//------------------------------------------------------------------------------
/**
    @returns the new debug trail
*/
nDebugTrail * 
nDebugGraphicsServer::NewDebugTrail()
{
    nDebugTrail * trail = n_new(nDebugTrail);
    n_assert( trail );
    if( trail )
    {
        this->elements.Append( trail );
    }
    return trail;
}

//------------------------------------------------------------------------------
/**
    @returns the new debug cross
*/
nDebugCross * 
nDebugGraphicsServer::NewDebugCross()
{
    nDebugCross * cross = n_new(nDebugCross);
    n_assert( cross );
    if( cross )
    {
        this->elements.Append( cross );
    }
    return cross;
}

//------------------------------------------------------------------------------
/**
    @returns the new debug vector
*/
nDebugVector * 
nDebugGraphicsServer::NewDebugVector()
{
    nDebugVector * vect = n_new(nDebugVector);
    n_assert( vect );
    if( vect )
    {
        this->elements.Append( vect );
    }
    return vect;
}

//------------------------------------------------------------------------------
/**
    @returns the new debug sphere
*/
nDebugSphere * 
nDebugGraphicsServer::NewDebugSphere()
{
    nDebugSphere * sphere = n_new(nDebugSphere);
    n_assert( sphere );
    if( sphere )
    {
        this->elements.Append( sphere );
    }
    return sphere;
}

//------------------------------------------------------------------------------
/**
    @returns the new debug capsule
*/
nDebugCapsule * 
nDebugGraphicsServer::NewDebugCapsule()
{
    nDebugCapsule * capsule = n_new(nDebugCapsule);
    n_assert( capsule );
    if( capsule )
    {
        this->elements.Append( capsule );
    }
    return capsule;
}

//------------------------------------------------------------------------------
/**
    @returns the new debug capsule
*/
nDebugText * 
nDebugGraphicsServer::NewDebugText()
{

    nDebugText * text = n_new(nDebugText);
    n_assert( text );
    if( text )
    {
        this->elements.Append( text );
    }
    return text;
}

//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void 
nDebugGraphicsServer::Draw( nGfxServer2 * const gfxServer )
{
    // put a good transformation
    matrix44 transformmatrix;
    gfxServer->SetTransform( nGfxServer2::Model, transformmatrix );

    // clean if there is too much elements
    for( int i = 0 ; 
        ( i < this->elements.Size() ) && 
        ( this->elements.Size() > nDebugGraphicsServer::MAX_ELEMENTS ) ; 
        /* empty */ )
    {
        if( this->elements[i]->IsDead() )
        {
            n_delete( this->elements[i] );
            this->elements.Erase( i );
        }
        else
        {
            ++i;
        }
    }

    // draw existing elements
    for( int i = 0 ; i < this->elements.Size() ; /* empty */ )
    {
        if( this->elements[i]->IsVisible() )
        {
            this->elements[i]->Draw( gfxServer );
        }

        this->elements[i]->DecreaseLife();

        if( this->elements[i]->GetLife() < nDebugGraphicsServer::MIN_LIFE )
        {
            n_delete( this->elements[i] );
            this->elements.Erase( i );
        }
        else
        {
            ++i;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param trail pointer to the debug trail to eliminate
*/
void 
nDebugGraphicsServer::Kill( nDebugGraphicElement * element )
{
    int index = this->elements.FindIndex( element );
    if( index != -1 )
    {
        this->elements[index]->SetDead( true );
        if( this->elements[index]->GetLife() < nDebugGraphicsServer::MIN_LIFE )
        {
            n_delete( this->elements[index] );
            this->elements.Erase( index );
        }
    }
}

//------------------------------------------------------------------------------
