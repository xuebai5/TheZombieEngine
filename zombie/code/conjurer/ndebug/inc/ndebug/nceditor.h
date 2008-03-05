#ifndef NCEDITOR_H
#define NCEDITOR_H
//------------------------------------------------------------------------------
/**
    @class ncEditor
    @ingroup Entities
    
    Component Object to save data of the editor in game objects.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGAME

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/nmaptabletypes.h"

#include "gfx2/nlineserver.h"

//------------------------------------------------------------------------------
class nSceneGraph;
class nGfxServer2;
class nMesh2;
class nVariable;
class neRnsWaypoint;
class nFile;

//------------------------------------------------------------------------------
class ncEditor : public nComponentObject
{

    NCOMPONENT_DECLARE(ncEditor,nComponentObject);

public:

    enum EditorDrawFlags {
        None             = 0,
        DrawLabel        = (1<<0),
        DrawWaypoint     = (1<<1),
        DrawNormals      = (1<<2),
        DrawTangents     = (1<<3),
        DrawBinormals    = (1<<4),
        DrawLabelEx      = (1<<5),
        DrawAll          = (DrawLabel|DrawWaypoint|DrawNormals|DrawTangents|DrawBinormals|DrawLabelEx)
    };

    /// constructor
    ncEditor();
    /// destructor
    ~ncEditor();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

#ifndef __ZOMBIE_EXPORTER__
    /// draw the information of the editor
    void Draw( nGfxServer2 * const gfxServer );

    /// draw the information of the editor
    void Draw( nSceneGraph * sceneGraph );
#endif

    /// @name Script interface
    //@{
    /// entity object persistence to separate file
    void SaveChunk ();
    /// entity object persistence to separate file
    void SaveChunkFile(nFile*);
    /// is set editor key
    bool IsSetEditorKey(const nString &) const;
    /// set a editor key string
    void SetEditorKeyString(const nString &, const nString &);
    /// get a editor key string
    const char * GetEditorKeyString(const nString &) const;
    /// set a editor key integer
    void SetEditorKeyInt(const nString &, int);
    /// get a editor key integer
    int GetEditorKeyInt(const nString &) const;
    /// set a editor key float
    void SetEditorKeyFloat(const nString &, float);
    /// get a editor key float
    float GetEditorKeyFloat(const nString &) const;
    /// set the layer id 
    void SetLayerId(int);
    /// get the layer id 
    int GetLayerId () const;

    /// Tells if entity has been deleted in the editor but has not been removed from entity server
    bool IsDeleted ();
    /// Sets deletion flag
    void SetDeleted(bool);
    //@}

    /// Insert entity in level
    static void InsertEntityInLevel( nEntityObject* newEntity );

    /// Set drawing flags
    void SetDrawingFlags( int flags );

    /// Get drawing flags
    int GetDrawingFlags();

private:
    /// create or get a created key data pointer
    nVariable * CreateKeyData( const nString & key );

    nMapTableTypes<nVariable*>::NString editorData;

    // flags for different drawing
    EditorDrawFlags drawFlags;

    // Tells if entity has been deleted in the editor but has not been removed from entity server
    bool isDeleted;
    
    // Object for line drawing
    nLineHandler lineHandler;

    /// mesh types for every vertex component
    enum
    {
        MeshNormal = 0,
        MeshTangent,
        MeshBinormal,
        
        NumMeshTypes,
    };
    nRef<nMesh2> componentMesh[NumMeshTypes];

};

#endif//!NGAME

//------------------------------------------------------------------------------
#endif//NCEDITOR_H
