#include "precompiled/pchconjurerexp.h"
//-----------------------------------------------------------------------------
//  nlayermanager.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nlayermanager/nlayermanager.h"
#include "kernel/nkernelserver.h"
#include "kernel/nref.h"

nNebulaScriptClass(nLayerManager, "nroot");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayerManager::nLayerManager():
    m_newLayerId(0),
    m_layersArray(16, 16),
    m_selectedLayerId(NO_LAYER)
{    
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
        - 07-Jun-2005   Miquel Angel Rujula changed to call 'Clear' method
*/
nLayerManager::~nLayerManager()
{
    this->Clear();
}

//-----------------------------------------------------------------------------
/**
    Clear the layer manager

    history:
        - 07-Jun-2005   Miquel Angel Rujula created
*/
void 
nLayerManager::Clear()
{
    int arraySize = this->m_layersArray.Size();

    nLayer *layer;
    for (int i = 0; i < arraySize; i++)
    {
        layer = this->m_layersArray.GetElementAt(i);
        layer->Release();
    }
    
    this->m_layersArray.Clear();
    this->m_selectedLayerId = NO_LAYER;
}

//-----------------------------------------------------------------------------
/**
    Creates a new layer.

    history:
        - 11-Feb-2005   Miquel Angel Rujula. Created.
        - 17-Feb-2005   Miquel Angel Rujula. Removed id parameter. 
                        Id created authomatically.
        - 07-Jun-2005   Miquel Angel Rujula. Added 'alreadyCreated' flag to know
                        If there already was a layer with the same name.
*/
nLayer *
nLayerManager::CreateNewLayer(const char *layerName, bool &alreadyCreated)
{
    // if a layer with the same name is already created, don't create a new
    // one, just return it
    nLayer *alreadyCreatedLayer = this->SearchLayer(layerName);
    if (alreadyCreatedLayer)
    {
        alreadyCreated = true;
        return alreadyCreatedLayer;
    }

    nString layerNOHName("layer");
    nString layerIdString(this->m_newLayerId);
    layerNOHName.Append(layerIdString);

    // a layer with that name was not created
    // create a new layer
    this->kernelServer->PushCwd(this);
    nLayer *newLayer = static_cast<nLayer*>(this->kernelServer->New("nlayer", layerNOHName.Get()));
    this->kernelServer->PopCwd();
        
    // set layer's id
    newLayer->SetId(this->m_newLayerId);
    // set layer's name
    newLayer->SetLayerName(layerName);
    // insert the layer in the layer manager
    this->m_layersArray.Add(this->m_newLayerId, newLayer);
    this->m_newLayerId++;
    
    alreadyCreated = false;

    return newLayer;
}

//-----------------------------------------------------------------------------
/**
    Creates a new layer.

    history:
        - 11-Feb-2005   Miquel Angel Rujula. Created.
        - 17-Feb-2005   Miquel Angel Rujula. Removed id parameter. 
                        Id created authomatically.
        - 07-Jun-2005   Miquel Angel Rujula. Added 'alreadyCreated' flag to know
                If there already was a layer with the same name.
*/
nLayer *
nLayerManager::CreateNewLayer(nString &layerName, bool &alreadyCreated)
{
    return this->CreateNewLayer(layerName.Get(), alreadyCreated);
}

//-----------------------------------------------------------------------------
/**
    Creates a new layer.

    history:
        - 07-Jun-2005   Miquel Angel Rujula. Created.
*/
nLayer *
nLayerManager::CreateNewLayer(const char *layerName, int layerId, bool &alreadyCreated)
{
    // call CreateNewLayer, passing parameters given and 0 for the password
    return this->CreateNewLayer(layerName, layerId, 0, alreadyCreated);
}

//-----------------------------------------------------------------------------
/**
    Creates a new layer.
*/
nLayer *
nLayerManager::CreateNewLayer(const char *layerName, int layerId, const char * password, bool &alreadyCreated)
{
    // if a layer with the same name is already created, don't create a new
    // one, just return it
    nLayer *alreadyCreatedLayer = this->SearchLayer(layerName);
    if (alreadyCreatedLayer)
    {
        alreadyCreated = true;
        return alreadyCreatedLayer;
    }
    else 
    {
        // if a layer with the same id is already created, don't create a new
        // one, just return it
        alreadyCreatedLayer = this->SearchLayer(layerId);
        if (alreadyCreatedLayer)
        {
            alreadyCreated = true;
            return alreadyCreatedLayer;
        }
    }

    alreadyCreated = false;
    nString layerNOHName("layer");
    nString layerIdString(layerId);
    layerNOHName.Append(layerIdString);

    // a layer with that name was not created
    // create a new layer
    this->kernelServer->PushCwd(this);
    nLayer *newLayer = static_cast<nLayer*>(this->kernelServer->New("nlayer", layerNOHName.Get()));
    this->kernelServer->PopCwd();
        
    // set layer id
    newLayer->SetId(layerId);
    // set layer name
    newLayer->SetLayerName(layerName);
    // set layer password
    newLayer->SetPassword(password);
    // insert the layer in the layer manager
    this->m_layersArray.Add(layerId, newLayer);
    // make sure new layers created later without specifying an id are created with a free id
    if ( this->m_newLayerId <= layerId )
    {
        this->m_newLayerId = layerId + 1;
    }

    return newLayer;
}

//-----------------------------------------------------------------------------
/**
    Creates a new layer without returning the 'alreadyCreated' flag

    history:
        - 07-Jun-2005   Miquel Angel Rujula. Created.
*/
nLayer *
nLayerManager::CreateNewLayer(const char *layerName)
{
    bool alreadyCreated = false;
    return this->CreateNewLayer(layerName, alreadyCreated);
}

//-----------------------------------------------------------------------------
/**
    add layer
*/
void
nLayerManager::AddLayer(nLayer *layer)
{
    n_assert2(!this->SearchLayer(layer->GetId()), 
              "miquelangel.rujula: trying to insert a layer that already exists in the layer manager!");

    this->m_layersArray.Add(layer->GetId(), layer);
}

//-----------------------------------------------------------------------------
/**
    Remove a layer.
    Returns true if it found the layer and could remove it.

    history:
        - 17-Feb-2005   Miquel Angel Rujula created
        - 31-May-2005   Carles Ros Martinez deselect removed layer if it's the selected one
        - 02-Nov-2005   Carles Ros Martinez select first layer after deleting, if there's any layer left
*/
bool 
nLayerManager::RemoveLayer(const char *layerName)
{
    nLayer *layer = this->SearchLayer(layerName);
    if (layer)
    {
        this->m_layersArray.Rem(layer->GetId());
        if ( this->m_layersArray.Size() > 0 )
        {
            this->m_selectedLayerId = this->m_layersArray[0]->GetId();
        }
        else
        {
            this->m_selectedLayerId = NO_LAYER;
        }
        return layer->Release();
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Remove a layer.
    Returns true if it found the layer and could remove it.

    history:
        - 17-Feb-2005   Miquel Angel Rujula created
*/
bool 
nLayerManager::RemoveLayer(nString &layerName)
{
    return this->RemoveLayer(layerName.Get());
}

//-----------------------------------------------------------------------------
/**
    Remove a layer.
    Returns true if it found the layer and could remove it.

    history:
        - 17-Feb-2005   Miquel Angel Rujula created
        - 31-May-2005   Carles Ros Martinez deselect removed layer if it's the selected one
        - 02-Nov-2005   Carles Ros Martinez select first layer after deleting, if there's any layer left
*/
bool 
nLayerManager::RemoveLayer(int layerId)
{
    nLayer *layer = this->SearchLayer(layerId);
    if (layer)
    {
        this->m_layersArray.Rem(layerId);
        if ( this->m_layersArray.Size() > 0 )
        {
            this->m_selectedLayerId = this->m_layersArray[0]->GetId();
        }
        else
        {
            this->m_selectedLayerId = NO_LAYER;
        }
        return layer->Release();
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Searches a layer by its id. 
    Returns the layer's pointer or NULL if it can't find it

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer *
nLayerManager::SearchLayer(const int layerId)
{
    nLayer *layer;
    
    if (this->m_layersArray.Find(layerId, layer))
    {
        return layer;
    }
    
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Searches a layer by its name. 
    Returns the layer's pointer or NULL if it can't find it

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer *
nLayerManager::SearchLayer(const char *name) const
{
    nString strName(name);
    
    nLayer *layer;
    nString layerName;
    int arraySize = this->m_layersArray.Size();
    for (int i = 0; i < arraySize; i++)
    {
        layer = this->m_layersArray.GetElementAt(i);
        layerName = layer->GetLayerName();
        if (layerName == strName)
        {
            return layer;
        }
    }   

    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Searches a layer by its name. 
    Returns the layer's pointer or NULL if it can't find it

    history:
        - 11-Feb-2005   Miquel Angel Rujula created
*/
nLayer *
nLayerManager::SearchLayer(nString &name) const
{
    return this->SearchLayer(name.Get());
}

//-----------------------------------------------------------------------------
/**
    Get the selected layer id.
    Returns -1 if there isn't any layer selected.

    history:
        - 31-May-2005   Carles Ros Martinez created
*/
int 
nLayerManager::GetSelectedLayerId() const
{
    return this->m_selectedLayerId;
}

//-----------------------------------------------------------------------------
/**
    Set the current selected layer.
    Returns true if the layer could be selected, false otherwise (the layer doesn't exist).

    history:
        - 31-May-2005   Carles Ros Martinez created
*/
bool 
nLayerManager::SelectLayer(int layerId)
{
    if ( this->SearchLayer(layerId) )
    {
        this->m_selectedLayerId = layerId;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Save layers state to file. Also saves current selected layer
*/
bool
nLayerManager::SaveLayers(const char * fileName)
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, fileName)) 
    {
        for ( int layer = 0; layer < this->GetNumLayers(); layer++ )
        {
            nLayer * layerPtr = this->GetLayerAt( layer );
            n_assert( layerPtr );
    
            // do NOT save value of isLocked since this is part of the persisted state of the level
            //ps->Put(this, 'JSLL', layerPtr->GetLayerName(), layerPtr->IsLocked() );
            ps->Put(this, 'JSLA', layerPtr->GetLayerName(), layerPtr->IsActive() );
        }

        ps->Put(this, 'SLYR', this->GetSelectedLayerId() );

        ps->EndObject(true);
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Load layers state from file
*/
bool
nLayerManager::LoadLayers(const char * fileName)
{
    nFileServer2 * fileServer = nFileServer2::Instance();
    if (!fileServer->FileExists(fileName))
    {
        return false;
    }

    nKernelServer::Instance()->PushCwd( this );
    nKernelServer::Instance()->Load( fileName, false );
    nKernelServer::Instance()->PopCwd();

    return true;
}

//-----------------------------------------------------------------------------
/**
    Set a layer's locked state
*/
void
nLayerManager::SetLayerLocked(const char *layerName, bool locked)
{
    nLayer * layer = this->SearchLayer(layerName);
    if ( layer )
    {
        layer->SetLocked(locked);
    }
}

//-----------------------------------------------------------------------------
/**
    Set a layer's active state
*/
void
nLayerManager::SetLayerActive(const char *layerName, bool active)
{
    nLayer * layer = this->SearchLayer(layerName);
    if ( layer )
    {
        layer->SetActive( active );
    }
}

//-----------------------------------------------------------------------------
/**
    Set the password for the layer
*/
void
nLayerManager::SetLayerPassword(const char * layerName, const char * password)
{
    nLayer * layer = this->SearchLayer(layerName);
    if ( layer )
    {
        layer->SetPassword(password);
    }
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

