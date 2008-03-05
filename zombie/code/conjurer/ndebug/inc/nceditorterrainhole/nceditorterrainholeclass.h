#ifndef NCEDITORTERRAINHOLECLASS_H
#define NCEDITORTERRAINHOLECLASS_H
//------------------------------------------------------------------------------
/**
    @class ncEditorTerrainHoleClass
    @ingroup Entities

    Component Class to to edit line for terrain holes.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGAME

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nTerrainLine;

//------------------------------------------------------------------------------
class ncEditorTerrainHoleClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncEditorTerrainHoleClass,nComponentClass);

public:
    /// constructor
    ncEditorTerrainHoleClass();
    /// destructor
    ~ncEditorTerrainHoleClass();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    //Tool for Create a instance from hole
    nEntityObject* CreateNewHole(nTerrainLine* line);

    /// @name Script interface
    //@{
    /// entity class persistence to separate file
   // void SaveChunk ();
    //@}

        //
private:

};

#endif//!NGAME

//------------------------------------------------------------------------------
#endif//NCEDITORCLASS_H
