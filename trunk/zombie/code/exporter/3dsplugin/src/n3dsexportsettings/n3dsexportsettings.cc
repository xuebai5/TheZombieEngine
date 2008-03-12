#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsexportsettings/n3dsExportSettings.h"
#include "n3dsutils/nmaxscriptcall.h"
#include "entity/nentityclassserver.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsexporters/n3dslog.h"

//------------------------------------------------------------------------------
/**
*/
n3dsExportSettings::n3dsExportSettings() :
    valid(false)
{    
}

//------------------------------------------------------------------------------
/**
*/
n3dsExportSettings 
n3dsExportSettings::GetFromScene()
{
   
    n3dsExportSettings tmp;

    bool boolValue(false);
    int  intValue(0);
    char* strValue(0);

    // Valid
    n_verify( nMaxScriptGetValue("nebNewExportOptions.valid" , boolValue) );
    tmp.valid = boolValue;

    // gameLibrary
    n_verify( nMaxScriptGetValue("nebNewExportOptions.gameLibrary" , boolValue) );
    tmp.gameLibrary = boolValue;

    // thumbNails
    n_verify( nMaxScriptGetValue("nebNewExportOptions.exportThumbNail" , boolValue) );
    tmp.exportThumbNail = boolValue;


    // Export Mode
    n_verify( nMaxScriptGetValue("nebNewExportOptions.exportMode" , intValue) );
    n_assert2( intValue > 0 && intValue< MaximExportMode , "exportMode Invalid value");
    tmp.exportMode = ExportModeType( intValue );

    // Anim mode
    n_verify( nMaxScriptGetValue("nebNewExportOptions.animMode" , intValue) );
    n_assert2( intValue > 0 && intValue< MaximModeType , "exportMode Invalid value");
    tmp.animMode = CharacterModeType( intValue );

    // Critter Name
    n_verify( nMaxScriptGetValue("nebNewExportOptions.critterName" , intValue) );
    n_assert2( intValue > 0 && intValue< MaximCritterNameType , "exportMode Invalid value");
    tmp.critterName= CritterNameType( intValue );

    // Entity name
    n_verify( nMaxScriptGetValue("nebNewExportOptions.entityName " , strValue ) );
    tmp.entityName = strValue;
    tmp.entityName.ToCapital();

    // Library folder
    n_verify( nMaxScriptGetValue("nebNewExportOptions.libraryFolder " , strValue ) );
    tmp.libraryFolder = strValue;

    // Resource File
    n_verify( nMaxScriptGetValue("nebNewExportOptions.resourceFile " , strValue ) );
    tmp.resourceName = strValue;
    tmp.resourceName.ToLower();

    // Skeleton Entity Name
    n_verify( nMaxScriptGetValue("nebNewExportOptions.skeletonName " , strValue ) );
    tmp.skeletonName = strValue;
    tmp.skeletonName.ToCapital();

    // CollectTextures
    n_verify( nMaxScriptGetValue("nebNewExportOptions.collectTextures" , boolValue) );
    tmp.collectTextures = boolValue;

    // BinaryResource
    n_verify( nMaxScriptGetValue("nebNewExportOptions.binaryResource" , boolValue) );
    tmp.binaryResource = boolValue;

    // OptimizeMesh
    n_verify( nMaxScriptGetValue("nebNewExportOptions.optimizeMesh" , boolValue) );
    tmp.optimizeMesh = boolValue;

    // FirstPerson
    n_verify( nMaxScriptGetValue("nebNewExportOptions.firstPerson" , boolValue) );
    tmp.firstPerson= boolValue;

    // OpenConjurer
    n_verify( nMaxScriptGetValue("nebNewExportOptions.openConjurer" , boolValue) );
    tmp.openConjurer= boolValue;
    
    // ExportMotion
    n_verify( nMaxScriptGetValue("nebNewExportOptions.exportMotion" , boolValue) );
    tmp.exportMotion= boolValue;
    // ExportMotionX
    n_verify( nMaxScriptGetValue("nebNewExportOptions.motionX" , boolValue) );
    tmp.motionX= boolValue;
    // ExportMotionY
    n_verify( nMaxScriptGetValue("nebNewExportOptions.motionY" , boolValue) );
    tmp.motionY= boolValue;
    // ExportMotionZ
    n_verify( nMaxScriptGetValue("nebNewExportOptions.motionZ" , boolValue) );
    tmp.motionZ= boolValue;

    tmp.lod = n3dsLOD::GetFromScene();

    return tmp;
}

//------------------------------------------------------------------------------
/**
    check its is valids settings
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidSetting()
{
    if ( !valid ) return false;


    //in game library always
    if ( gameLibrary )
    {
        //Binary Mesh
        binaryResource = true; 

        //Copy texture to repository
        collectTextures = false; 

        //Copy optimize mesh
        optimizeMesh = true;
    }


    //FIXME add morphanimation when ready
    if (! nEntityClassServer::ValidClassName( entityName ) && ! ((exportMode == Character) && (animMode == SkinAnimation)) )
    {
        n_message( "Invalid entity name\nOnly use a characters and numbers\n and not begin by number");
        return false;
    }


    // @todo check if name class is
    switch ( exportMode)
    {  
        case Brush:
           // return CheckBrush();
        break;

        case Indoor:
        break;

        case Character:
            switch ( this->animMode )
            {
                case Mesh:
                    break;

                case Skeleton:
                    break;

                case SkinAnimation:
                     //not break
                case MorpherAnimation:
                    // @todo: Comprobe if valid resource file name.
                    break;
                
                default:
                n_assert_always();
            }
        break;

        default:
        n_assert_always();
    }

    return true;
}


//------------------------------------------------------------------------------
/**
    check its posible export with this settings, it posible create this class 
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidExport()
{
    if ( !valid ) return false;

    //@todo check if name class is
    switch ( exportMode)
    {  
        case Brush:
           return CheckValidExportBrush();
        break;

        case Indoor:
            return true;
        break;

        case Character:
        {
            switch ( animMode )
            {
            case Mesh:
                return this->CheckValidExportCharacterMesh();
                break;
            case Skeleton:
                return this->CheckValidSkeleton();
                break;
            case SkinAnimation:
                return this->CheckValidExportSkinAnimation();
                break;
            case MorpherAnimation:
                n_message("Not yet, sorry");
                return false;
                break;
            }
        }
        default:
        n_assert_always();
    }

    return false;
}
//------------------------------------------------------------------------------
/**
    check its is valids
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidExportBrush()
{
    n_assert( exportMode == Brush );

    nEntityClass * newEntityClass  = nEntityClassServer::Instance()->GetEntityClass( this->entityName.Get() );
    nEntityClass * nesimplebrush   = nEntityClassServer::Instance()->GetEntityClass( "nesimplebrush" );
    nEntityClass * nebrush         = nEntityClassServer::Instance()->GetEntityClass( "nebrush" );
    nEntityClass * nemirage         = nEntityClassServer::Instance()->GetEntityClass( "nemirage" );


    // comprobe if class exists
    if ( 0 != newEntityClass &&
         newEntityClass->GetSuperClass() != nesimplebrush &&
         newEntityClass->GetSuperClass() != nebrush  && 
         newEntityClass->GetSuperClass() != nemirage )
    {
        n_message("The entity exixts in repository\n and it not a brush object\n");
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
/**
    check its is valids
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidExportSkinAnimation()
{
    n_assert( ( exportMode == Character ) && ( animMode == SkinAnimation ));

    //check if skeleton exists
    if ( ! this->CheckSkeletonExists() )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    check its is valids
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidExportCharacterMesh()
{
    n_assert( ( exportMode == Character ) && ( animMode == Mesh ));

    if ( ! this->CheckSkeletonExists() )
    {
        return false;    
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    check if skeleton exists, used to check animations and meshes
    @return the result of check , false if it do not exist
*/
bool
n3dsExportSettings::CheckSkeletonExists()
{
    n_assert( exportMode == Character );

    nEntityClass * neskeleton   = nEntityClassServer::Instance()->GetEntityClass( "neskeleton" );

    //there's lod
    if( this->lod.GetCountLOD() != 0 )
    {
        for( int lodLevel=0; lodLevel<this->lod.GetCountLOD(); lodLevel++)
        {
            nString lodsk = this->skeletonName;
            lodsk += "_";
            lodsk += lodLevel;

            nEntityClass * newEntityClass  = nEntityClassServer::Instance()->GetEntityClass( lodsk.Get() );
            if ( ( 0 == newEntityClass ) || ( newEntityClass->GetSuperClass() != neskeleton ) )
            {
                N3DSERROR( animexport , ( 0 , "ERROR: Wrong skeleton, it does not exist in repository.\n LOD %i (n3dsExportSettings::CheckSkeletonExists)", lodLevel));
                return false;
            }
        }
        return true;
    }
    
    //only one layer
    nEntityClass * newEntityClass  = nEntityClassServer::Instance()->GetEntityClass( this->skeletonName.Get() );

    // check if class exists
    if ( ( 0 == newEntityClass ) || ( newEntityClass->GetSuperClass() != neskeleton ) )
    {
        N3DSERROR( animexport , ( 0 , "ERROR: Wrong skeleton, it does not exist in repository.\n (n3dsExportSettings::CheckSkeletonExists)"));
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    check if it exists
    @return the result of check , false if it is invalid
*/
bool
n3dsExportSettings::CheckValidSkeleton()
{
    n_assert( exportMode == Character );

    nEntityClass * newEntityClass  = nEntityClassServer::Instance()->GetEntityClass( this->entityName.Get() );
    nEntityClass * neskeleton   = nEntityClassServer::Instance()->GetEntityClass( "neskeleton" );

    // check if class exists
    if ( ( 0 != newEntityClass ) && ( newEntityClass->GetSuperClass() != neskeleton ) )
    {
        n_message("The entity already exists in repository and it isn't an skeleton\n");
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
int   
n3dsLOD::GetLODof(IGameNode* node) const
{
    int lod = 0;
    INodeLayerProperties* thisNodeProps = static_cast<INodeLayerProperties*>( node->GetMaxNode()->GetInterface(NODELAYERPROPERTIES_INTERFACE));
    nString layerName = thisNodeProps->getLayer()->getName();
    nString name = layerName.GetToken('_');
    name.ToLower();

    if ( name == "lod"  || name  == "facade")
    {
        int val  = layerName.AsInt();
        if ( val >= 0 && val < distance.Size() )
        {
            lod = val;
        }
    } 
    return lod;
}


//------------------------------------------------------------------------------
/**
*/
float 
n3dsLOD::GetDistance(int numLOD) const
{
    return distance[numLOD];
}

//------------------------------------------------------------------------------
/**
*/
int  
n3dsLOD::GetCountLOD() const
{
    return distance.Size();
}

//------------------------------------------------------------------------------
/**
*/
n3dsLOD 
n3dsLOD::GetFromScene()
{
    n3dsLOD tmp;
    int count=0;
        // Export Mode
    n_verify( nMaxScriptGetValue("NebLodData.num" , count) );


    for ( int idx = 1 ; idx < count+1 ; ++idx) // maxscript begin in1
    {
        float dist = 0;
        nString command = "NebLodData.list[";
        command.AppendInt( idx) ;
        command +="]";
        n_verify( nMaxScriptGetValue( command.Get() , dist )  );

        dist = n3dsExportServer::Instance()->GetSystemCoordinates()->MaxToNebulaDistance( dist);
        tmp.distance.Append( dist );
    }

    return tmp;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
