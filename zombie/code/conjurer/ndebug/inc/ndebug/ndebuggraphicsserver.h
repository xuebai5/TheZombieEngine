#ifndef NDEBUGGRAPHICSSERVER_H
#define NDEBUGGRAPHICSSERVER_H
//------------------------------------------------------------------------------
/**
    @class nDebugGraphicsServer
    @ingroup NebulaDebugSystem

    Debug singleton class that control debug graphic objects.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"

#include "ndebug/ndebugtrail.h"
#include "ndebug/ndebugcross.h"
#include "ndebug/ndebugvector.h"
#include "ndebug/ndebugsphere.h"
#include "ndebug/ndebugcapsule.h"

//------------------------------------------------------------------------------
class nGfxServer2;
class nDebugGraphicElement;
class nDebugText;

//------------------------------------------------------------------------------
class nDebugGraphicsServer
{
public:
    /// singleton instance
    static nDebugGraphicsServer * Instance();

    /// constructor
    nDebugGraphicsServer();
    /// destructor
    ~nDebugGraphicsServer();
  
    /// create a new debug trail
    nDebugTrail * NewDebugTrail();
    /// create a new debug cross
    nDebugCross * NewDebugCross();
    /// create a new debug vector
    nDebugVector * NewDebugVector();
    /// create a new debug sphere
    nDebugSphere * NewDebugSphere();
    /// create a new debug capsule
    nDebugCapsule * NewDebugCapsule();
    /// create a new debug text
    nDebugText * NewDebugText();

    /// draw the trails
    void Draw( nGfxServer2 * const gfxServer );
    /// eliminate an element
    void Kill( nDebugGraphicElement * element );
    /// delete all elements
    void DeleteElements();
private:
    nArray<nDebugGraphicElement*> elements;
    
    static nDebugGraphicsServer instance;

    static const float MIN_LIFE;
    static const int MAX_ELEMENTS = 100;
};

//------------------------------------------------------------------------------
#endif//NDEBUGGRAPHICSSERVER_H
