#ifndef N_GEOMETRYSTREAMEDITOR_H
#define N_GEOMETRYSTREAMEDITOR_H
//------------------------------------------------------------------------------
/**
    @file ngeometrystreameditor.h
    @class nGeometryStreamEditor
    @ingroup Conjurer
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief Encapsulates a high-level description of a geometry stream that
    is used as a humar interface for editing and building them.
    
    This class contains a description of a geometry including:
    - the set of geometry nodes that act as a source to build the stream
    - the frequency of each geometry node within the stream
    - the attributes that need to be merged together, 
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"

class nAbstractShaderNode;
class nGeometryNode;
class nSurfaceNode;
class nSceneNode;
class nMaterial;
//------------------------------------------------------------------------------
class nGeometryStreamEditor : public nRoot
{
public:
    /// constructor
    nGeometryStreamEditor();
    /// destructor
    virtual ~nGeometryStreamEditor();
    /// object persistence
    virtual bool SaveCmds(nPersistServer *ps);
    
    /// set stream editor dirty
    void SetDirty(const bool dirty);
    /// get stream editor dirty
    bool IsDirty() const;

    /// begin adding geometries to the stream
    void BeginGeometries(int);
    /// set source geometry at index (class, level, path)
    void SetGeometryAt(int, nString&, int, nString&);
    /// get source geometry at index (class, level, path)
    void GetGeometryAt(int, nString&, int&, nString&);
    /// set frequency at index
    void SetFrequencyAt(int, int);
    /// set frequency at index
    int GetFrequencyAt(int) const;
    /// end adding geometries to the stream
    void EndGeometries();
    /// apply the stream to the target geometries, remember saved streams
    void ApplyStream();
    /// remove the stream from the target geometries
    void RemoveStream();
    /// permanently remove the stream from the target
    void DeleteStream();
    /// build and save the stream geometries and textures
    void BuildStream();

    #ifndef NGAME
    /// set/get number of geometries (inspector only)
    void SetNumGeometries(int);
    int GetNumGeometries() const;
    /// set/get geometries for the first 8 slots (inspector only)
    void SetGeometryAt0(nString&, int, nString&, int);
    void GetGeometryAt0(nString&, int&, nString&, int&);
    void SetGeometryAt1(nString&, int, nString&, int);
    void GetGeometryAt1(nString&, int&, nString&, int&);
    void SetGeometryAt2(nString&, int, nString&, int);
    void GetGeometryAt2(nString&, int&, nString&, int&);
    void SetGeometryAt3(nString&, int, nString&, int);
    void GetGeometryAt3(nString&, int&, nString&, int&);
    void SetGeometryAt4(nString&, int, nString&, int);
    void GetGeometryAt4(nString&, int&, nString&, int&);
    void SetGeometryAt5(nString&, int, nString&, int);
    void GetGeometryAt5(nString&, int&, nString&, int&);
    void SetGeometryAt6(nString&, int, nString&, int);
    void GetGeometryAt6(nString&, int&, nString&, int&);
    void SetGeometryAt7(nString&, int, nString&, int);
    void GetGeometryAt7(nString&, int&, nString&, int&);
    #endif

private:
    /// load required resources
    bool LoadSceneResources();
    /// build a material with the common attributes
    bool BuildCommonMaterial();
    /// build a surface with the common attributes
    bool BuildCommonSurface();
    /// collect shader parameters from the node
    void CollectTexturesFromNode(nAbstractShaderNode*);
    /// get texture entry for the parameters, create if not found
    void AppendSharedTexture(nShaderState::Param texParam, const char* texName);
    /// get if there is a texture entry for the parameter
    bool FindSharedTextures(nShaderState::Param texParams, nArray<nString>& texNames) const;

    /// build the stream geometry using the stream material
    bool BuildStreamGeometry();
    /// find a child geometry node using the surface
    nGeometryNode* FindGeometryWithSurface(nSceneNode* sceneNode, const char *surface) const;

    struct GeometryEntry
    {
        /// constructor
        GeometryEntry();

        /// members that define the geometry entry
        nString className;
        /// user-friendly material identifier
        nString materialName;
        /// level of detail to which the geometry belongs
        int level;
        /// number of instances of this geometry in the stream, default 1
        int frequency;

        /// unique geometry that makes use of the material name materialName
        nDynAutoRef<nGeometryNode> refGeometryNode;
        nRef<nEntityClass> refEntityClass;

        /// members to revert the geometry entry to its previous state
        bool isPrevStreamValid;
        nString prevStream;
        int prevStreamIndex;

        /// members that contain the generated stream data
        //transform33 texTransform[nGfxServer2::MaxTextureStages];
    };

    /// set of primitive geometries that are composed into the stream
    nArray<GeometryEntry> geometryArray;

    struct TextureEntry
    {
        nShaderState::Param shaderParam;
        nArray<nString> textures;
    };

    /// set of texture that are composed into atlases
    nArray<TextureEntry> textureArray;

    /// the current geometry node that has been loaded or built for the stream
    nRef<nGeometryNode> refStreamGeometry;
    nRef<nMaterial> refMaterial;
    nRef<nSurfaceNode> refSurface;

    bool isDirty;
    bool isApplied;
    bool isDeleted;

    static int uniqueId;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGeometryStreamEditor::SetDirty(const bool dirty)
{
    this->isDirty = dirty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nGeometryStreamEditor::IsDirty() const
{
    return this->isDirty;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGeometryStreamEditor::GeometryEntry::GeometryEntry() :
    isPrevStreamValid(false),
    prevStreamIndex(-1),
    frequency(1)
{
    // empty
}

//------------------------------------------------------------------------------
#endif // N_GEOMETRYSTREAMEDITOR_H
