#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpfacing.cc
//------------------------------------------------------------------------------

#include "ngpfacing/ngpfacing.h"
#include "ncaimovengine/ncaimovengine.h"
#include "zombieentity/nctransform.h"

nNebulaScriptClass(nGPFacing, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPFacing)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, const vector3&, bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPFacing::nGPFacing() : 
    nGPBasicAction()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPFacing::~nGPFacing()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @params the entity to facing and the point to align to
*/
bool
nGPFacing::Init (nEntityObject* entity, const vector3& point, bool onlyHead)
{
    n_assert(entity);

    bool valid = entity != 0;
    
    if ( valid )
    {        
        this->entity = entity ;
    }

    if ( valid )
    {
        float angle = N_HALFPI * 0.5f;
        float sinAngle = n_sin (angle);
        float cosAngle = n_cos (angle);
        quaternion orientation (point.x*sinAngle, point.y*sinAngle, point.z*sinAngle, cosAngle);

        if ( onlyHead )
        {
            this->GlanceTo (orientation);
        }
        else
        {
            this->FaceTo (point);
        }

        this->orientation = orientation;
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    FaceTo
*/
void
nGPFacing::FaceTo (const vector3& point)
{
    n_assert(this->init);

    ncAIMovEngine* engine = this->entity->GetComponent <ncAIMovEngine>();
    n_assert(engine);

    if ( engine )
    {
        engine->FaceTo (point);
    }
}

//------------------------------------------------------------------------------
/**
    GlanceTo
*/
void
nGPFacing::GlanceTo (const quaternion& /*orientation*/)
{
    n_assert(this->init);

    // @TODO:
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPFacing::IsDone() const
{
    n_assert(this->init);

    ncTransform* transform = this->entity->GetComponent <ncTransform>();
    quaternion orientation;

    if ( transform )
    {
        orientation = transform->GetQuat();
    }

    return orientation.isequal (this->orientation, 0.05f);
}