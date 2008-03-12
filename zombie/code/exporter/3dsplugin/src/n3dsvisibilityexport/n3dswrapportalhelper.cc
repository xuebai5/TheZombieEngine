#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "igame/igame.h"
#pragma warning( pop )

#include "util/nstring.h"
#include "n3dsvisibilityexport/n3dswrapportalhelper.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dssystemcoordinates.h"

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsWrapPortalHelper::ExtractFrom(INode* node)
{
    Animatable* obj = node->GetObjectRef(); // in BaseObject collision method name GetParamBlock
    Class_ID pp = obj->ClassID();
    Class_ID p1 = node->GetObjectRef()->ClassID();
    Class_ID p2 = node->ClassID();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();


    if ( PORTAL_HELPER_CLASS_ID != obj->ClassID() )  
    {
        return false;
    }

    const int numBlock = obj->NumParamBlocks();

    for ( int idxBlock = 0 ; idxBlock < numBlock  ; idxBlock++)
    {
        IParamBlock2* pb2 = obj->GetParamBlock(idxBlock);
        ParamBlockDesc2* pdc= pb2->GetDesc();

        int i;
        for ( i = 0; i< pdc->count ; i++)
        {
            ParamDef pD = pdc->paramdefs[i];
            nString nameParam = pD.int_name;
            if (nameParam == "cell1")
            {
                this->cell1 = pb2->GetINode( pD.ID );

            } else if (nameParam == "cell2")
            {
                this->cell2 = pb2->GetINode( pD.ID );
            } else if (nameParam == "size")
            {
                Point3 p = pb2->GetPoint3( pD.ID);
                boxSize = systemCoord->MaxtoNebulaVertex( p );                
            } else if (nameParam == "c1ToC2Active" ) 
            {
                int val = pb2->GetInt(  pD.ID);
                this->c1ToC2Active = 0 != val;
            }
            else if (nameParam == "c2ToC1Active" )
            {
                int val = pb2->GetInt(  pD.ID);
                this->c2ToC1Active = 0 != val;
            }
            else if (nameParam == "deactDist1" )
            {
                this->deactDist1 = systemCoord->MaxToNebulaDistance( pb2->GetFloat( pD.ID ) );
            }
            else if (nameParam == "deactDist2" )
            {
                this->deactDist2 = systemCoord->MaxToNebulaDistance( pb2->GetFloat( pD.ID ) );
            }
        }

    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsWrapPortalHelper::ExtractFrom(IGameNode* node)
{
    return ExtractFrom( node->GetMaxNode() );
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
