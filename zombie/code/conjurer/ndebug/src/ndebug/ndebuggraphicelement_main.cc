//------------------------------------------------------------------------------
//  ndebuggraphicelement.h
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebuggraphicelement.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugGraphicElement::nDebugGraphicElement():
    dead( false ),
    visible( true ),
    life( 0.8f ),
    maxLife( 1.0f ),
    col( 1, 0, 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugGraphicElement::~nDebugGraphicElement()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param f the new life
*/
void
nDebugGraphicElement::SetLife( const float f )
{
    this->life = f;
    if( this->life > this->maxLife )
    {
        this->maxLife = this->life;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugGraphicElement::DecreaseLife( )
{
    if( this->dead )
    {
        this->life -= 0.05f;
        if( this->life < 0.0f )
        {
            this->life = 0.0f;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param value new color
*/
void
nDebugGraphicElement::SetColor( const vector3 & value )
{
    this->col = value;
}

//------------------------------------------------------------------------------
/**
    @param value returned color with transparency
*/
void
nDebugGraphicElement::GetColor( vector4 & value ) const
{
    value.x = this->col.x;
    value.y = this->col.y;
    value.z = this->col.z;
    value.w = this->life / this->maxLife;
}

//------------------------------------------------------------------------------
