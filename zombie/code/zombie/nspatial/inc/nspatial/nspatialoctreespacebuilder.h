#ifndef N_SPATIALOCTREESPACEBUILDER_H
#define N_SPATIALOCTREESPACEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nSpatialOctreeSpaceBuilder
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialOctreeSpaceBuilder helps to build nSpatialOctreeSpace's
    
    (C) 2004  Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
class nTransformNode;
class ncSpatialOctree;

//------------------------------------------------------------------------------
class nSpatialOctreeSpaceBuilder
{
public:
    /// constructor
    nSpatialOctreeSpaceBuilder();
    /// destructor
    ~nSpatialOctreeSpaceBuilder();

    /// set resource file to assign to the brush class created
    void SetResourceFile(const char *filename);
    /// get resource file
    const char *GetResourceFile();

    /// build an octree space from an indoor space
    void BuildOctreeSpace(ncSpatialIndoor* indoor);
    /// search the objects in the given scene and insert them in the space
    int CatchSpatialInfo(nTransformNode *scene);
    /// search the objects in the given scene and insert them in the space
    //int CatchIndoorSpatialInfo(nTransformNode *scene);

    /// set the octree space to build. Useful if you want to continue building it
    //void SetOctreeSpace(nSpatialOctreeSpace* octreeSpace);
    void SetOctreeSpace(ncSpatialOctree* octreeSpace);
    /// returns the octree space. Call it after construction.
    //nSpatialOctreeSpace *GetOctreeSpace();
    ncSpatialOctree* GetOctreeSpace();

private:

    //nRef<nSpatialOctreeSpace> m_octreeSpace;
    nRef<nEntityObject> m_octreeSpace;
    nString resourceFile;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSpatialOctreeSpaceBuilder::SetResourceFile(const char *filename)
{
    this->resourceFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nSpatialOctreeSpaceBuilder::GetResourceFile()
{
    return this->resourceFile.IsEmpty() ? 0 : this->resourceFile.Get();
}

//------------------------------------------------------------------------------
#endif
