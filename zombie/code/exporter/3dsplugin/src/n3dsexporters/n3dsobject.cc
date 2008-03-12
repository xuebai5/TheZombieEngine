#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsexporters/n3dsobject.h"
#pragma warning( push, 3 )
#include "max.h"
#include "igame/igame.h"
#pragma warning( pop )
#include "util/nstring.h"

static const char* objectTypesTable[n3dsObject::NumTypes] = 
{
    "graphics",
    "physics",
    "visibility",
    "plugs"
};

//------------------------------------------------------------------------------
/**
*/
n3dsObject::n3dsObject(IGameNode* node)
{
    this->node = node;
    this->type = n3dsObject::GetType(node);
    this->subType = n3dsObject::GetSubType( node , type);
}

//------------------------------------------------------------------------------
/**
*/
n3dsObject::ObjectType 
n3dsObject::GetType(IGameNode* const node)
{
    ObjectType tmp = GetType( node->GetMaxNode() );

    /// Problem with bones, diferent types in IGAME and MaxClass
    if ( tmp == ObjectType::graphics && node->GetIGameObject()->GetIGameType() != IGameObject::ObjectTypes::IGAME_MESH )
    {
        tmp = ObjectType::InvalidType;
    }
    return tmp;
}


//------------------------------------------------------------------------------
/**
*/
n3dsObject::ObjectType
n3dsObject::GetType(INode* const node)
{
    TSTR value("");
    node->GetUserPropString( _T("z_type") , value );
    nString str(value);
    
    // is str==""  and object is geometry use graphics default
    if ( str == ""  && !node->IsTarget() ) 
    {
        ObjectState os = node->EvalWorldState( 0 );
        if ( os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID && os.obj->ClassID() != BONE_OBJ_CLASSID )
        {
            return ObjectType::graphics; // The default value 
        }
    }

    for (int i = 0; i < ObjectType::NumTypes; i++)
    {
        if ( str == objectTypesTable[i] )
        {
            return (ObjectType) i;
        }
    }
   
    return n3dsObject::ObjectType::InvalidType;
}

//------------------------------------------------------------------------------
/**
*/
n3dsObject::ObjectSubType
n3dsObject::GetSubType(IGameNode* const node, ObjectType type)
{
    switch ( type )
    {
        case visibility:
            return GetSubTypeVisibility(node); 
            break;
        default:
            return ObjectSubType::InvalidSubType;
    }
}


n3dsObject::ObjectSubType
n3dsObject::GetSubTypeVisibility(IGameNode* const node)
{
    INode* const maxNode  = node->GetMaxNode();

    BOOL isPortal = FALSE;
    BOOL isCell = FALSE;
    BOOL isOccluder = FALSE;

    maxNode->GetUserPropBool( _T("neb_Vis_Cell"), isCell );
    maxNode->GetUserPropBool( _T("neb_Vis_Portal"), isPortal );
    maxNode->GetUserPropBool( _T("neb_Vis_Occluder"), isOccluder );

    if ( isPortal == isCell  && isPortal == isOccluder) 
    {
        /// this is a rectangle portal only it is editable mesh define a clip rectangle
        return ObjectSubType::InvalidSubType;
    } else if ( isPortal == TRUE )
    {
        return ObjectSubType::visPortal;

    } else if ( isCell == TRUE )
    {
        return ObjectSubType::visCell;

    } else if ( isOccluder == TRUE )
    {
        return ObjectSubType::visOccluder;
    } else
    {
        return ObjectSubType::InvalidSubType;
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------