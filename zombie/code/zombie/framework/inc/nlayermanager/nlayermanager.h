#ifndef N_LAYERMANAGER_H
#define N_LAYERMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nLayerManager
    @ingroup Framework

    @brief nLayerManager manages the layers system.
    
    (C) 2005  Conjurer Services, S.A.

    @author: Miquel Angel Rujula  
*/

#include "nlayermanager/nlayer.h"
#include "util/nkeyarray.h"
#include "kernel/nroot.h"

class nLayerManager : public nRoot
{

public:

    /// constructor
    nLayerManager();
    /// destructor
    ~nLayerManager();

    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// clear the layer manager
    void Clear();
    /// create a new layer
    nLayer *CreateNewLayer(const char *layerName);
    /// create a new layer. Also says if the layer was already created
    nLayer *CreateNewLayer(const char *layerName, bool &alreadyCreated);
    /// create a new layer. Also says if the layer was already created
    nLayer *CreateNewLayer(nString &layerName, bool &alreadyCreated);
    /// create a new layer. Also says if the layer was already created
    nLayer *CreateNewLayer(const char *layerName, int layerId, bool &alreadyCreated);
    /// create a new layer. Also says if the layer was already created
    nLayer *CreateNewLayer(const char *layerName, int layerId, const char * password, bool &alreadyCreated);
    /// add layer
    void AddLayer(nLayer *layer);
    /// remove a layer
    bool RemoveLayer(const char *layerName);
    /// remove a layer
    bool RemoveLayer(nString &layerName);
    /// remove a layer
    bool RemoveLayer(int layerId);
    /// get the number of layers
    int GetNumLayers() const;
    /// get the number of layers
    nLayer *GetLayerAt(const int index) const;
    /// find a layer by its name
    nLayer *SearchLayer(const char *name) const;
    /// find a layer by its name
    nLayer *SearchLayer(nString &name) const;
    /// find a layer by its id
    nLayer *SearchLayer(const int layerId);
    /// get the selected layer id
    int GetSelectedLayerId() const;
    /// set the current selected layer
    bool SelectLayer(int layerId);
    /// set a layer's locked state
    void SetLayerLocked(const char * layerName, bool locked);
    /// set a layer's active state
    void SetLayerActive(const char * layerName, bool active);
    /// set the password for the given layer
    void SetLayerPassword(const char * layerName, const char * password);

    /// save layers state
    bool SaveLayers(const char * fileName);
    /// load layers state
    bool LoadLayers(const char * fileName);
    
private:

    // layers array
    nKeyArray<nLayer*> m_layersArray;
    // id for a new layer
    int m_newLayerId;
    // selected layer id
    int m_selectedLayerId;

    // reserved id to identify a "no layer selected"
    static const int NO_LAYER = -1;
};

//-----------------------------------------------------------------------------
/**
    Get the number of layers

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
inline
int 
nLayerManager::GetNumLayers() const
{
    return this->m_layersArray.Size();
}

//-----------------------------------------------------------------------------
/**
*/
inline
nLayer *
nLayerManager::GetLayerAt(const int index) const
{
    return this->m_layersArray.GetElementAt(index);
}

#endif // N_LAYER_MANAGER

