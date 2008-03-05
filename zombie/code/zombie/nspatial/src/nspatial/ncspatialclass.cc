#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialclass.h"
#include "nspatial/nspatialmodels.h"
#include "zombieentity/ncloaderclass.h"

#ifndef NGAME
#include "ndebug/nceditorclass.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSpatialClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialClass)
    NSCRIPT_ADDCMD_COMPCLASS('RSBX', void, SetBBox, 2, (vector3&, vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGBX', void, GetBBox, 0, (), 2, (vector3&, vector3&));
    NSCRIPT_ADDCMD_COMPCLASS('MSTM', void, SetTestModel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGTM', const char *, GetTestModel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLD', void, SetLevelDistance, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLD', float, GetLevelDistance, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLT', void, SetLevelThreshold, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLT', float, GetLevelThreshold, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLL', void, SetLevelLocked, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLL', int, GetLevelLocked, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSSL', void, SetShadowLength, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGSL', float, GetShadowLength, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSSC', void, SetShadowCaster, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGSC', bool, GetShadowCaster, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSHO', void, SetHasOccluders, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGHO', bool, GetHasOccluders, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('MSSB', void, SetStaticBatch, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGSB', bool, GetStaticBatch, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSBE', void, SetBatchLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGBE', int, GetBatchLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSSS', void, SetStaticShadow, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGSS', bool, GetStaticShadow, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSOD', void, SetLodDistances, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGOD', void, GetLodDistances, 0, (), 4, (float&, float&, float&, float&));
    NSCRIPT_ADDCMD_COMPCLASS('MSOT', void, SetLodThresholds, 3, (float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGOT', void, GetLodThresholds, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPCLASS('MSIL', void, SetLodImpostor, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGIL', int, GetLodImpostor, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSpatialClass::ncSpatialClass() :
    testModelType(nSpatialModel::SPATIAL_MODEL_NONE),
    m_shadowLength(10.f),
    shadowCaster(false),
    lockedLevel(-1),
    m_hasOccluders(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialClass::~ncSpatialClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSpatialClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // check if this class has occluders
    ncLoaderClass *loaderClass = this->GetComponent<ncLoaderClass>();
    if (this->m_hasOccluders && loaderClass)
    {
        nString fileName = loaderClass->GetResourceFile();
        fileName.StripTrailingSlash();
        fileName.Append("/spatial/occluders.n2");

        n_assert(nFileServer2::Instance()->FileExists(fileName));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialClass::SetTestModel(const char *modelName)
{
    nString modelStr(modelName);
    modelStr.ToLower();
    if (modelStr == nString("sphere"))
    {
        this->testModelType = nSpatialModel::SPATIAL_MODEL_SPHERE;
    }
    else if (modelStr == nString("frustum"))
    {
        this->testModelType = nSpatialModel::SPATIAL_MODEL_FRUSTUM;
    }
    else
    {
        this->testModelType = nSpatialModel::SPATIAL_MODEL_NONE;
    }
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncSpatialClass::GetTestModel()
{
    switch (this->testModelType)
    {
        case nSpatialModel::SPATIAL_MODEL_SPHERE:
            return "sphere";
        case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
            return "frustum";
        default:
            return "";
    }
}

//------------------------------------------------------------------------------
/**
    wrapper methods for class inspectors.
    they are required to not creat levels that are not defined.
*/
#ifndef NGAME

#define LEVEL_IMPOSTOR 3

void
ncSpatialClass::SetStaticBatch(bool value)
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass)
    {
        editorClass->SetClassKeyInt("BatchStatic", value ? 1 : 0);
    }
}

bool
ncSpatialClass::GetStaticBatch() const
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("BatchStatic"))
    {
        return (editorClass->GetClassKeyInt("BatchStatic") == 1);
    }
    return false;
}

void
ncSpatialClass::SetBatchLevel(int level)
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass)
    {
        int impostorLevel = -1;
        if (editorClass->IsSetClassKey("impostorLevel"))
        {
            impostorLevel = editorClass->GetClassKeyInt("impostorLevel");
        }

        if (level == LEVEL_IMPOSTOR)
        {
            if (impostorLevel != -1)
            {
                editorClass->SetClassKeyInt("BatchLevel", impostorLevel);
            }
        }
        else if (level >= 0 && level < this->GetNumLevels())
        {
            editorClass->SetClassKeyInt("BatchLevel", level);
        }
        else
        {
            editorClass->SetClassKeyInt("BatchLevel", -1);
        }
    }
}

int
ncSpatialClass::GetBatchLevel() const
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass)
    {
        if (editorClass->IsSetClassKey("BatchLevel"))
        {
            int impostorLevel = -1;
            if (editorClass->IsSetClassKey("impostorLevel"))
            {
                impostorLevel = editorClass->GetClassKeyInt("impostorLevel");
            }

            int level = editorClass->GetClassKeyInt("BatchLevel");
            if (level != -1 && level == impostorLevel)
            {
                return LEVEL_IMPOSTOR;
            }
            
            return level;
        }
    }
    return -1;
}

void
ncSpatialClass::SetStaticShadow(bool value)
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass)
    {
        editorClass->SetClassKeyInt("ShadowStatic", value ? 1 : 0);
    }
}

bool
ncSpatialClass::GetStaticShadow() const
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("ShadowStatic"))
    {
        return (editorClass->GetClassKeyInt("ShadowStatic") == 1);
    }
    return false;
}

void
ncSpatialClass::SetLodDistance(int level, float value)
{
    if (level < this->GetNumLevels())
    {
        this->SetLevelDistance(level, value);
    }
}

float
ncSpatialClass::GetLodDistance(int level)
{
    if (level < this->GetNumLevels())
    {
        return this->GetLevelDistance(level);
    }
    return 0.0f;
}

void
ncSpatialClass::SetLodDistances(float value0, float value1, float value2, float value3)
{
    //use value3 as impostor level
    int impostorLevel = -1;
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("impostorLevel"))
    {
        impostorLevel = editorClass->GetClassKeyInt("impostorLevel");
        this->SetLodDistance(impostorLevel, value3);
    }

    if (impostorLevel != 0) this->SetLodDistance(0, value0);
    if (impostorLevel != 1) this->SetLodDistance(1, value1);
    if (impostorLevel != 2) this->SetLodDistance(2, value2);
}

void
ncSpatialClass::GetLodDistances(float& value0, float& value1, float& value2, float& value3)
{
    //use value3 as impostor level
    int impostorLevel = -1;
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("impostorLevel"))
    {
        impostorLevel = editorClass->GetClassKeyInt("impostorLevel");
        value3 = this->GetLodDistance(impostorLevel);
    }

    if (impostorLevel != 0) value0 = this->GetLodDistance(0);
    if (impostorLevel != 1) value1 = this->GetLodDistance(1);
    if (impostorLevel != 2) value2 = this->GetLodDistance(2);
}

void
ncSpatialClass::SetLodThreshold(int level, float value)
{
    if (level < this->GetNumLevels())
    {
        this->SetLevelThreshold(level, value);
    }
}

float
ncSpatialClass::GetLodThreshold(int level)
{
    if (level < this->GetNumLevels())
    {
        return this->GetLevelThreshold(level);
    }
    return 0.0f;
}

void
ncSpatialClass::SetLodImpostor(int impostorLevel)
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("impostorLevel"))
    {
        editorClass->SetClassKeyInt("impostorLevel", impostorLevel);
    }
}

int
ncSpatialClass::GetLodImpostor() const
{
    ncEditorClass* editorClass = this->GetComponent<ncEditorClass>();
    if (editorClass && editorClass->IsSetClassKey("impostorLevel"))
    {
        return editorClass->GetClassKeyInt("impostorLevel");
    }
    return -1;
}

void
ncSpatialClass::SetLodThresholds(float value0, float value1, float value2)
{
    this->SetLodThreshold(0, value0);
    this->SetLodThreshold(1, value1);
    this->SetLodThreshold(2, value2);
}

void
ncSpatialClass::GetLodThresholds(float& value0, float& value1, float& value2)
{
    value0 = this->GetLodThreshold(0);
    value1 = this->GetLodThreshold(1);
    value2 = this->GetLodThreshold(2);
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialClass::SaveCmds(nPersistServer * ps)
{
    if (!nComponentClass::SaveCmds(ps))
    {
        return false;
    }

    //--- setbbox ---
    vector3 v0, v1;
    this->GetBBox(v0, v1);
    if (!ps->Put(this->GetEntityClass(), 'RSBX', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
    {
        return false;
    }
    
    //--- settestmodel ---
    if (this->testModelType != nSpatialModel::SPATIAL_MODEL_NONE)
    {
        ps->Put(this->GetEntityClass(), 'MSTM', this->GetTestModel());
    }

    int numLevels = this->GetNumLevels();
    for (int level = 0; level < numLevels; ++level)
    {
        //--- setleveldistance ---
        ps->Put(this->GetEntityClass(), 'MSLD', level, this->GetLevelDistance(level));

        //--- setlevelthreshold ---
        if (this->GetLevelThreshold(level) != 0.0f)
        {
            ps->Put(this->GetEntityClass(), 'MSLT', level, this->GetLevelThreshold(level));
        }
    }

    //--- setshadowlength ---
    if (!ps->Put(this->GetEntityClass(), 'RSSL', this->m_shadowLength))
    {
        return false;
    }

    //--- setshadowcaster ---
    if (!ps->Put(this->GetEntityClass(), 'MSSC', this->GetShadowCaster() ) )
    {
        return false;
    }

    //--- sethasoccluders ---
    if (!ps->Put(this->GetEntityClass(), 'RSHO', this->m_hasOccluders))
    {
        return false;
    }

    return true;
}
