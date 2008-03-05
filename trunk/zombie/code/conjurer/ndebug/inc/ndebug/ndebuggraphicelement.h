#ifndef NDEBUGGRAPHICELEMENT_H
#define NDEBUGGRAPHICELEMENT_H
//------------------------------------------------------------------------------
/**
    @class nDebugGraphicElement
    @ingroup NebulaDebugSystem
    
    Base class to draw debug elements
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugGraphicElement
{
public:
    /// constructor
    nDebugGraphicElement();
    /// destructor
    virtual ~nDebugGraphicElement();
    /// draw the trail
    virtual void Draw( nGfxServer2 * const gfxServer ) = 0;
    /// get the life of thre trail (to smoth death)
    float GetLife()const;
    /// set the life of the trail (to smoth death)
    void SetLife( const float f );
    /// set that the trail is dead so life decrease
    void SetDead( const bool value );
    /// update the life of element
    void DecreaseLife();
    /// set visible the element
    void SetVisible( const bool value );
    /// get if element is visible
    bool IsVisible() const ;
    /// get it element is dead
    bool IsDead() const;
    /// set the color of the element
    void SetColor( const vector3 & value );
    /// get the color for the element
    void GetColor( vector4 & value ) const;
protected:
    float life;
    float maxLife;
    vector3 col;

private:
    bool dead;
    bool visible;
};

//------------------------------------------------------------------------------
/**
    @returns the trail life
*/
inline
float
nDebugGraphicElement::GetLife()const
{
    return this->life;
}

//------------------------------------------------------------------------------
/**
    @param value dead bool value
    @remarks before set dead this element can be destroy, so should be no accesed
*/
inline
void
nDebugGraphicElement::SetDead( const bool value )
{
    this->dead = value;
}

//------------------------------------------------------------------------------
/**
    @retval true if element is dead
*/
inline
bool
nDebugGraphicElement::IsDead()const
{
    return this->dead;
}

//------------------------------------------------------------------------------
/**
    @param value visible bool value
*/
inline
void
nDebugGraphicElement::SetVisible( const bool value )
{
    this->visible = value;
}

//------------------------------------------------------------------------------
/**
    @retval true if element is visible
*/
inline
bool
nDebugGraphicElement::IsVisible()const
{
    return this->visible;
}

//------------------------------------------------------------------------------
#endif//NDEBUGGRAPHICELEMENT_H
