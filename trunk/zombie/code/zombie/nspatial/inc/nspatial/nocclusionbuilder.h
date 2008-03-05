#ifndef N_OCCLUSIONBUILDER_H
#define N_OCCLUSIONBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nOcclusionBuilder
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nOcclusionBuilder helps to build an instancer for the occluders associated to an asset.
    
    (C) 2005  Conjurer Services, S.A.   
*/
class nObjectInstancer;

class nOcclusionBuilder
{

public:

    /// constructor
    nOcclusionBuilder();
    /// destructor
    ~nOcclusionBuilder();
    /// add an occluder
    void AddOccluder(const vector3 *vertices, bool doubleSided);
    /// end building and save instancer at the given asset path
    void EndAndSave(const char *assetPath);

private:

    nObjectInstancer *instancer;

};

#endif // N_OCCLUSIONBUILDER_H

