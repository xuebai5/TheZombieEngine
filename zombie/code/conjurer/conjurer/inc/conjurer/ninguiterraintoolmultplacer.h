#ifndef N_INGUI_TERRAIN_TOOL_MULTPLACER_H
#define N_INGUI_TERRAIN_TOOL_MULTPLACER_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolmultplacer.h
    @class nInguiTerrainToolMultPlacer
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool Tool for multiple object placing

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nterrainline.h"
#include "gfx2/nlineserver.h"
#include "nphysics/nphygeomcylinder.h"
#include "zombieentity/nctransformclass.h"

class nAppViewport;
class nStringList;

//------------------------------------------------------------------------------
class nInguiTerrainToolMultPlacer : public nInguiTerrainTool
{
public:

    // constructor
    nInguiTerrainToolMultPlacer();

    // destructor
    virtual ~nInguiTerrainToolMultPlacer();

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the helper in the screen
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

    void ResetClassList ();
    void AddClass(nString);
    void SetIntensity(float);
    float GetIntensity ();
    void SetRandomRotation(bool);
    bool GetRandomRotation();
    void SetSizeVariation(float);
    float GetSizeVariation();

    /// Access to class names list
    void SetClassList( nStringList* );

    /// Access to entities id list created from first frame
    nArray<nEntityObjectId>* GetEntityList();

protected:

private:

    /// List of entity classes to instance
    nRef<nStringList> classList;

    /// Area coverage (~ intensity)
    float areaCoverage;

    /// Flag for placing objects in random orientation
    bool randomRotation;

    // Object size variation
    float sizeVariation;

    /// Array of entity instances placed, for undo
    nArray<nEntityObjectId> instanceArray;

    /// Geometry for collision checking
    nRef<nPhyGeomCylinder> refGeomCyl;

};
//------------------------------------------------------------------------------
/**
    @brief Set intensity
    @param Intensity
*/
inline
void
nInguiTerrainToolMultPlacer::SetIntensity( float i )
{
    this->areaCoverage = i;
}

//------------------------------------------------------------------------------
/**
    @brief Get intensity
    @return Intensity
*/
inline
float
nInguiTerrainToolMultPlacer::GetIntensity( void )
{
    return this->areaCoverage;
}
//------------------------------------------------------------------------------
/**
    @brief Set random rotation
    @param The flag
*/
inline
void
nInguiTerrainToolMultPlacer::SetRandomRotation( bool randomRot )
{
    this->randomRotation = randomRot;
}

//------------------------------------------------------------------------------
/**
    @brief Get random rotation
    @return The flag
*/
inline
bool
nInguiTerrainToolMultPlacer::GetRandomRotation( void )
{
    return this->randomRotation;
}
//------------------------------------------------------------------------------
/**
    @brief Set size variation
    @param size variation factor, e.g: 0.1 = variation of +-5%
*/
inline
void
nInguiTerrainToolMultPlacer::SetSizeVariation( float sizeVar )
{
    this->sizeVariation = sizeVar;
}

//------------------------------------------------------------------------------
/**
    @brief Get size variation
    @return Intensity
*/
inline
float
nInguiTerrainToolMultPlacer::GetSizeVariation( void )
{
    return this->sizeVariation;
}

//------------------------------------------------------------------------------
/**
    Set class names list
*/
inline
void
nInguiTerrainToolMultPlacer::SetClassList( nStringList*list )
{
    nEntityClassServer* entClassServer = nEntityClassServer::Instance();

    // Check that all classes have ncTransform
    for ( int i = 0; i < list->Size(); i++ )
    {
        nString* str = list->GetString( i );
        nEntityClass* cl = entClassServer->GetEntityClass( str->Get() );
        cl->GetComponentSafe<ncTransformClass>();
    }

    this->classList = list;
}
//------------------------------------------------------------------------------
/**
    Access to class names list
*/
inline
nArray<nEntityObjectId>*
nInguiTerrainToolMultPlacer::GetEntityList()
{
    return &this->instanceArray;
}
//------------------------------------------------------------------------------
#endif
