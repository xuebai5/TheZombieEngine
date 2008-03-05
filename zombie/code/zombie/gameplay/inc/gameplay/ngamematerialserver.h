#ifndef NGAMEMATERIALSERVER_H
#define NGAMEMATERIALSERVER_H

//-----------------------------------------------------------------------------
/**
    @class nGameMaterialServer
    @ingroup GamePlay
    @brief The material server takes care of creating and managing game materials.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Material Server
    
    @cppclass nGameMaterialServer
    
    @superclass nRoot

    @classinfo The material server takes care of creating and managing game materials.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "util/nkeyarray.h"

//-----------------------------------------------------------------------------

class nGameMaterial;

//-----------------------------------------------------------------------------

class nGameMaterialServer : public nRoot 
{
public:
    
    /// constructor
    nGameMaterialServer();

    /// destructor
    ~nGameMaterialServer();

    /// get instance pointer
    static nGameMaterialServer* Instance();

    /// destroys server's data
    void Destroy();

    /// returns a material type trough the id
    nGameMaterial* GetMaterial( const unsigned int id ) const;

    /// returns a material type trough the id
    nGameMaterial* GetMaterialByIndex( const int index ) const;
    /// return number of materials
    int GetNumMaterials() const;

        /// returns a material type trough the name
        nGameMaterial* GetMaterialByName(const nString&) const;

        /// creates a material
        nGameMaterial* CreateMaterial(const nString&);

        /// adds an already existing material
        void Add(nGameMaterial*);

#ifndef NGAME
        /// debug function to be call for persisting the object
        virtual bool Save();

        /// debug function to update the last changes in materials
        void Update();
    
        /// removes a material from the server
        void Remove(nGameMaterial*);
#endif

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

#ifndef NGAME
    /// returns if any of the materials is dirty (changed)
    const bool IsDirty() const;
#endif

    /// inits object
    void InitInstance(nObject::InitInstanceMsg initType);

private:

#ifndef NGAME
    /// removes a material
    void RemoveMaterial( nGameMaterial* material );
#endif
    /// loads materials
    void LoadMaterials();
    
    /// pointer to the unique instance
    static nGameMaterialServer* Singleton;

    /// type of container of materials
    typedef nKeyArray<nGameMaterial*> tContainerGameMaterials;

    static tContainerGameMaterials listMaterials;

    /// stores the number of materials stored
    static int MaterialsSize;

    /// initial space in the list of materials
    static const int NumInitialMaterialsSpace = 10;

    /// growth pace
    static const int NumMaterialsGrowthPace = 10;

    /// updates list of materials
    void UpdateList();
}; 

#endif