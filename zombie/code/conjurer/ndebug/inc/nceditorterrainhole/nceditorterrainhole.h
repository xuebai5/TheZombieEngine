#ifndef NCEDITORTERRAINHOLE_H
#define NCEDITORTERRAINHOLE_H

//------------------------------------------------------------------------------
/**
    @class ncEditorTerrainHole
    @ingroup Entities
    
    Component Object to edit line for terrain holes.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGAME
#include "entity/nentity.h"
#include "util/narray.h"
class nTerrainLine;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class ncEditorTerrainHole : public nComponentObject
{

    NCOMPONENT_DECLARE(ncEditorTerrainHole,nComponentObject);

public:
    ncEditorTerrainHole();
    /// destructor
    ~ncEditorTerrainHole();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Set new line
    void SetLine(nTerrainLine* line);
    ///
    void ReCalculate(void);
    /// 

    /// @name Script interface
    //@{
    /// entity object persistence to separate file
   // void SaveChunk ();
    //@}

        //
private:
    /// Generate Scene, make mesh, and add hole to physic terrain
    void MakeHole(nTerrainLine* line);
    /// Remove scene, and remove mesh from physic terrain
    void RemoveHole(nTerrainLine* line);
    /// Create nTerrainLine
    nTerrainLine* NewTerrainLine();
    nArray<vector2> points;
    
};

#endif//!NGAME

//------------------------------------------------------------------------------
#endif//NCEDITORTERRAINHOLE_H
