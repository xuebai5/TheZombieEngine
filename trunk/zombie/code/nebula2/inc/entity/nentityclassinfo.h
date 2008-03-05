#ifndef N_ENTITYCLASSINFO_H
#define N_ENTITYCLASSINFO_H
//------------------------------------------------------------------------------
/**
    @class nEntityClassInfo
    @ingroup NebulaEntitySystem
    @brief Entity class information
    @author Mateu Batle

    Holds information for an entity class used by the entity class server.

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
class nEntityClassInfo
{
public:
    // constructor
    nEntityClassInfo();
    /// constructor
    nEntityClassInfo(const char * name);
    /// copy constructor
    nEntityClassInfo(const nEntityClassInfo & cl);
    /// destructor
    ~nEntityClassInfo();

    /// assignment operator 
    nEntityClassInfo & operator=(const nEntityClassInfo & rhs);
    /// set the entity object
    void SetEntityClass(nEntityClass * neclass);
    /// get the entity object
    nEntityClass * GetEntityClass() const;

    /// get delete status of entity object 
    bool IsDeleted() const;
    /// mark the entity class to be deleted (on save time)
    void Delete();

    /// name of the class
    nString className;
    /// all superclasses
    nArray<nString> superClasses;
    /// dirty flag used to know if save or not
    /// state: new (not saved), modified (to be saved not new)
    bool dirty;
    /// magic key (who created the class)
    nEntityObjectId classKey;
    /// is an entity class or just a plain-old nClass
    bool isEntityClass;
    /// pointer to the non-native class
    nClass * nclass;

private:

    /// pointer to the entity class if loaded
    nRef<nEntityClass> refEntityClass;
    /// marked for later deletion
    bool deleted;
};

//--------------------------------------------------------------------
#endif //N_ENTITYCLASSINFO_H
