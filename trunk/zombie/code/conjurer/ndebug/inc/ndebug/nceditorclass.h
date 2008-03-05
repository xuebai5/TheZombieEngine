#ifndef NCEDITORCLASS_H
#define NCEDITORCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncEditorClass
    @ingroup Entities

    Component Class to save data of the exporter in game classes.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NGAME

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nVariable;

//------------------------------------------------------------------------------
class ncEditorClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncEditorClass,nComponentClass);

public:
    /// constructor
    ncEditorClass();
    /// destructor
    ~ncEditorClass();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// @name Script interface
    //@{
    /// entity class persistence to separate file
    void SaveChunk ();
    /// entity class persistence to separate file
    void SaveAssetChunk ();
    /// is set class key
    bool IsSetClassKey(const nString &) const;
    /// is set asset key
    bool IsSetAssetKey(const nString &) const;
    /// set a class key string
    void SetClassKeyString(const nString &, const nString &);
    /// get a class key string
    const char * GetClassKeyString(const nString &) const;
    /// set a class key integer
    void SetClassKeyInt(const nString &, int);
    /// get a class key integer
    int GetClassKeyInt(const nString &) const;
    /// set a class key float
    void SetClassKeyFloat(const nString &, float);
    /// get a class key float
    float GetClassKeyFloat(const nString &) const;
    /// set a asset key string
    void SetAssetKeyString(const nString &, const nString &);
    /// get a asset key string
    const char * GetAssetKeyString(const nString &) const;
    /// set a asset key integer
    void SetAssetKeyInt(const nString &, int);
    /// get a asset key integer
    int GetAssetKeyInt(const nString &) const;
    /// set a asset key float
    void SetAssetKeyFloat(const nString &, float);
    /// get a asset key float
    float GetAssetKeyFloat(const nString &) const;
    //@}

private:
    /// save the variable map in a file
    void SaveVariableMap( nPersistServer * ps, nMapTableTypes<nVariable*>::NString & data );
    /// create or get a created key data pointer
    nVariable * CreateKeyData( const nString & key, nMapTableTypes<nVariable*>::NString & data );

    nMapTableTypes<nVariable*>::NString classData;
    nMapTableTypes<nVariable*>::NString assetData;

};

#endif//!NGAME

//------------------------------------------------------------------------------
#endif//NCEDITORCLASS_H
