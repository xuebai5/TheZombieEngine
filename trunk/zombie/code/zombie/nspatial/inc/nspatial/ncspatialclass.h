#ifndef NC_SPATIALCLASS_H
#define NC_SPATIALCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialClass
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial component class for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nspatial/nspatialtypes.h"
#include "entity/nentity.h"
#include "mathlib/bbox.h"

//------------------------------------------------------------------------------
class ncSpatialClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSpatialClass, nComponentClass);

public:
    
    /// constructor
    ncSpatialClass();
    /// destructor
    virtual ~ncSpatialClass();

    /// set axis-aligned bounding box
    void SetBBox(vector3&, vector3&);
    /// get axis-aligned bounding box
    void GetBBox(vector3&, vector3&);
    /// set test model for spatial element
    void SetTestModel(const char *);
    /// get test model for spatial element
    const char * GetTestModel();

    /// set distance threshold for spatial element
    void SetLevelDistance(int, float);
    /// get distance threshold for spatial element
    float GetLevelDistance(int);
    /// set distance threshold for spatial element
    void SetLevelThreshold(int, float);
    /// get distance threshold for spatial element
    float GetLevelThreshold(int);
    /// lock a level of detail, -1 for none
    void SetLevelLocked(int);
    /// get if a level of detail is locked
    int GetLevelLocked() const;

    /// set shadow length
    void SetShadowLength(float);
    /// get shadow length
    float GetShadowLength() const;
    /// set shadow caster
    void SetShadowCaster(bool);
    /// get shadow caster
    bool GetShadowCaster() const;

    /// set the has occluders flag
    void SetHasOccluders(bool);
    /// says if this class has occluders
    bool GetHasOccluders() const;
    
    #ifndef NGAME
    /// wrapper methods to SetStaticBatch from inspectors
    void SetStaticBatch(bool);
    bool GetStaticBatch() const;
    /// wrapper methods to SetBatchLevel from inspectors
    void SetBatchLevel(int);
    int GetBatchLevel() const;
    /// wrapper methods to SetStaticShadow from inspectors
    void SetStaticShadow(bool);
    bool GetStaticShadow() const;
    /// wrapper methods for SetLevelDistance from inspectors
    void SetLodDistances(float, float, float, float);
    void GetLodDistances(float&, float&, float&, float&);
    /// wrapper methods for SetLevelThreshold from inspectors
    void SetLodThresholds(float, float, float);
    void GetLodThresholds(float&, float&, float&);
    /// wrapper methods for SetImpostorLevel from inspectors
    void SetLodImpostor(int);
    int GetLodImpostor() const;
    #endif

    enum
    {
        N_FAR_LOD = 5000,
        N_NEAR_LOD = -1
    };

    struct LODRangeType
    {
    public:
        /// constructor
        LODRangeType() : 
            begin(0.0f), end(0.0f), threshold(0.0f), 
            beginSq(0.0f), endSq(0.0f),
            upperFactor(FLT_MAX), lowerFactor(0.0f) { }

        /// destructor
        ~LODRangeType() { }

        /// set range start
        void SetStart(float start)
        {
            this->begin = start;
            this->beginSq = start * start;
            this->upperFactor = XSCALE_60 / this->beginSq;
        }
        
        /// get range start
        float GetStart()
        {
            return this->begin;
        }

        /// set range end
        void SetEnd(float end)
        {
            this->end = end;
            this->endSq = (end + this->threshold) * (end + this->threshold);
            this->lowerFactor = XSCALE_60 / this->endSq;
        }

        /// get range end
        float GetEnd()
        {
            return this->end;
        }

        /// set range threshold
        void SetThreshold(float threshold)
        {
            this->threshold = threshold;
            this->endSq = (this->end + threshold) * (this->end + threshold);
            this->lowerFactor = XSCALE_60 / this->endSq;
        }

        /// get range threshold
        float GetThreshold()
        {
            return this->threshold;
        }

        /// return if distance is in the range
        /// distance is squared to optimize comparison
        bool Contains(float distSq)
        {
            return (distSq >= this->beginSq) && (distSq < this->endSq);
        }

        /// return if distance is behind the range
        /// distance paramter must be squared
        bool IsBehind(float distSq)
        {
            return (distSq > this->endSq);
        }

        /// return if distance is nearer the range
        /// distance paramter must be squared
        bool IsInFront(float distSq)
        {
            return (distSq < this->beginSq);
        }
        
        /// get magnification factor for the range start
        float GetUpperFactor()
        {
            return this->upperFactor;
        }
        /// get magnification factor for the range end
        float GetLowerFactor()
        {
            return this->lowerFactor;
        }

    private:
        float begin;
        float end;
        float threshold;
        float beginSq;  //< begin*begin
        float endSq;    //< (end+threshold)*(end+threshold)

        /// magnification factor for fov=60 deg, at begin dist
        float upperFactor;
        /// magnification factor for fov=60 deg, at (end+threshold) dist
        float lowerFactor;
    };

    nArray<LODRangeType> ranges;
    int lockedLevel;

    /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// component class persistence
    bool SaveCmds(nPersistServer* ps);

    /// set original bbox
    void SetOriginalBBox(const bbox3 &bbox);
    /// get original bbox
    const bbox3& GetOriginalBBox();
    /// get test model type
    unsigned int GetTestModelType();

    /// get range for detail level
    LODRangeType& GetLevelRange(int level);
    /// get number of ranges
    int GetNumLevels();

private:
    #ifndef NGAME
    /// set level distance from script (limit to existing levels)
    void SetLodDistance(int level, float value);
    /// get lod distances from script (limit to existing levels)
    float GetLodDistance(int level);
    /// set level threshold from script (limit to existing levels)
    void SetLodThreshold(int level, float value);
    /// get lod threshold from script (limit to existing levels)
    float GetLodThreshold(int level);
    #endif

    /// original entity's bounding box in world coordinates
    bbox3 m_originalBBox;

    /// model type
    unsigned int testModelType;

    /// shadow's length
    float m_shadowLength;

    /// cast shadows- default value for objects of this class
    bool shadowCaster;

    /// true if this class has occluders
    bool m_hasOccluders;

};

//------------------------------------------------------------------------------
/**
    set original bbox (script)
*/
inline
void
ncSpatialClass::SetBBox(vector3& center, vector3& extents)
{
    this->m_originalBBox.set(center, extents);
}

//------------------------------------------------------------------------------
/**
    get original bbox (script)
*/
inline
void
ncSpatialClass::GetBBox(vector3& center, vector3& extents)
{
    center = this->m_originalBBox.center();
    extents = this->m_originalBBox.extents();
}

//------------------------------------------------------------------------------
/**
    set original bbox
*/
inline
void
ncSpatialClass::SetOriginalBBox(const bbox3 &bbox)
{
    this->m_originalBBox = bbox;
}

//------------------------------------------------------------------------------
/**
    get original bbox
*/
inline
const bbox3&
ncSpatialClass::GetOriginalBBox()
{
    return this->m_originalBBox;
}

//------------------------------------------------------------------------------
/**
    get type of test model
*/
inline
unsigned int
ncSpatialClass::GetTestModelType()
{
    return this->testModelType;
}

//------------------------------------------------------------------------------
/**
    get range for level of detail
*/
inline
ncSpatialClass::LODRangeType&
ncSpatialClass::GetLevelRange(int level)
{
    return this->ranges[level];
}

//------------------------------------------------------------------------------
/**
    set max distance for level of detail
*/
inline
void
ncSpatialClass::SetLevelDistance(int level, float maxDist)
{
    while (level > this->ranges.Size() - 1)
    {
        LODRangeType newRange;
        this->ranges.Append(newRange);
    }

    if (level > 0)
    {
        this->GetLevelRange(level).SetStart(this->GetLevelRange(level - 1).GetEnd());
    }

    if (level < this->ranges.Size() - 1)
    {
        this->GetLevelRange(level + 1).SetStart(maxDist);
    }

    this->GetLevelRange(level).SetEnd(maxDist);
}

//------------------------------------------------------------------------------
/**
    set max distance for level of detail
*/
inline
float
ncSpatialClass::GetLevelDistance(int level)
{
    return this->ranges[level].GetEnd();
}

//------------------------------------------------------------------------------
/**
    set max distance for level of detail
*/
inline
void
ncSpatialClass::SetLevelThreshold(int level, float threshold)
{
    while (level > this->ranges.Size() - 1)
    {
        LODRangeType newRange;
        this->ranges.Append(newRange);
    }

    this->GetLevelRange(level).SetThreshold(threshold);
}

//------------------------------------------------------------------------------
/**
    set max distance for level of detail
*/
inline
float
ncSpatialClass::GetLevelThreshold(int level)
{
    return this->ranges[level].GetThreshold();
}

//------------------------------------------------------------------------------
/**
    lock a level of detail
*/
inline
void
ncSpatialClass::SetLevelLocked(int level)
{
    this->lockedLevel = level;
}

//------------------------------------------------------------------------------
/**
    lock a level of detail
*/
inline
int
ncSpatialClass::GetLevelLocked() const
{
    return this->lockedLevel;
}

//------------------------------------------------------------------------------
/**
    set max distance for level of detail
*/
inline
int
ncSpatialClass::GetNumLevels()
{
    return this->ranges.Size();
}

//------------------------------------------------------------------------------
/**
    set shadow length
*/
inline
void
ncSpatialClass::SetShadowLength(float length)
{
    this->m_shadowLength = length;
}
//------------------------------------------------------------------------------
/**
    get shadow length
*/
inline
float
ncSpatialClass::GetShadowLength() const
{
    return this->m_shadowLength;
}

//------------------------------------------------------------------------------
/**
    set shadow length
*/
inline
void
ncSpatialClass::SetShadowCaster(bool value)
{
    this->shadowCaster = value;
}
//------------------------------------------------------------------------------
/**
    get shadow length
*/
inline
bool
ncSpatialClass::GetShadowCaster() const
{
    return this->shadowCaster;
}

//------------------------------------------------------------------------------
/**
    set the has occluders flag
*/
inline
void 
ncSpatialClass::SetHasOccluders(bool flag)
{
    this->m_hasOccluders = flag;
}

//------------------------------------------------------------------------------
/**
    says if this class has occluders
*/
inline
bool 
ncSpatialClass::GetHasOccluders() const
{
    return this->m_hasOccluders;
}

//------------------------------------------------------------------------------
#endif // NC_SPATIALCLASS_H
