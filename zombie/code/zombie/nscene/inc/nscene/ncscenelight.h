#ifndef NC_SCENELIGHT_H
#define NC_SCENELIGHT_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLight
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for light entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncscene.h"

//------------------------------------------------------------------------------
class ncSceneLight : public ncScene
{

    NCOMPONENT_DECLARE(ncSceneLight,ncScene);

public:
    /// light flags
    enum LightFlag
    {
        LightStatic  = (1<<0),
        LightDynamic = (1<<1),
        CastLightmaps = (1<<2),

        AllLightFlags = (LightStatic|LightDynamic),
    };

    /// constructor
    ncSceneLight();
    /// destructor
    virtual ~ncSceneLight();

    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// load
    virtual bool Load();

    /// render
    virtual void Render(nSceneGraph* sceneGraph);

    /// check if the entity is affected by this light
    virtual bool AffectsEntity(nEntityObject* entityObject);

    /// add a light type
    void AddLightType(nFourCC fourcc);

    /// get number of light types
    int GetNumLightTypes();

    /// get light type by index
    nFourCC GetLightTypeAt(int index);

    /// set light priority
    void SetLightPriority(int priority);

    /// get light priority
    int GetLightPriority() const;

    /// set if the light cast lightmaps
    void SetCastLightmaps(bool);
    bool GetCastLightmaps() const;

    /// set if the light affect static (lightmapped) geometry
    void SetLightStatic(bool);
    bool GetLightStatic() const;

    /// set if the light affect dynamic (non-lightmapped) geometry
    void SetLightDynamic(bool);
    bool GetLightDynamic() const;

    #ifndef NGAME
    /// light colors
    void SetLightAmbient(const vector3&);
    void GetLightAmbient(float&, float&, float&);
    void SetLightDiffuse(const vector3&);
    void GetLightDiffuse(float&, float&, float&);
    void SetLightSpecular(const vector3&);
    void GetLightSpecular(float&, float&, float&);

    void SetLightMapAmbientFactor (float);
    float GetLightMapAmbientFactor();
    void SetLightMapDiffuseFactor (float);
    float GetLightMapDiffuseFactor();

    void SetTerrainAmbientFactor (float);
    float GetTerrainAmbientFactor();
    void SetTerrainDiffuseFactor (float);
    float GetTerrainDiffuseFactor();

    /// fog color
    void SetFogColor(const vector3&);
    void GetFogColor(float&, float&, float&);

    /// point light attenuation inner and outer radius
    void SetPointLightRadius(float, float);
    void GetPointLightRadius(float&, float&);

    /// cone light attenuation inner and outer angles
    void SetConeLightAngles(float, float);
    void GetConeLightAngles(float&, float&);
    #endif

private:
    #ifndef NGAME
    /// helper method for light property inspectors
    void GetColorOverride(nShaderState::Param param, float& r, float& g, float& b);
    void GetVectorOverride(nShaderState::Param param, float& x, float& y, float& z, float& w);
    #endif

    // grant access to lightnodes for collecting material selectors
    friend class nLightNode;
    nArray<nFourCC> lightTypes;
    int lightFlags;
    int lightPriority;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneLight::AddLightType(nFourCC fourcc)
{
    this->lightTypes.Append(fourcc);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLight::GetNumLightTypes()
{
    return this->lightTypes.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
ncSceneLight::GetLightTypeAt(int index)
{
    return this->lightTypes[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneLight::SetLightStatic(bool value)
{
    if (value)
    {
        this->lightFlags |= LightStatic;
    }
    else
    {
        this->lightFlags &= ~LightStatic;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncSceneLight::GetLightStatic() const
{
    return (this->lightFlags & LightStatic) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneLight::SetLightDynamic(bool value)
{
    if (value)
    {
        this->lightFlags |= LightDynamic;
    }
    else
    {
        this->lightFlags &= ~LightDynamic;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncSceneLight::GetLightDynamic() const
{
    return (this->lightFlags & LightDynamic) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneLight::SetCastLightmaps(bool value)
{
    if (value)
    {
        this->lightFlags |= CastLightmaps;
    }
    else
    {
        this->lightFlags &= ~CastLightmaps;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncSceneLight::GetCastLightmaps() const
{
    return (this->lightFlags & CastLightmaps) != 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncSceneLight::SetLightPriority(int priority)
{
    this->lightPriority = priority;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLight::GetLightPriority() const
{
    return this->lightPriority;
}

//------------------------------------------------------------------------------
#endif
