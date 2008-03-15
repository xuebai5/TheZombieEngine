#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  ngrowthseeds_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngrassgrowth/ngrowthseeds.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#define NGROWTHSEEDS_VERSION 2

//------------------------------------------------------------------------------
nNebulaScriptClass( nGrowthSeeds, "nresource");


//------------------------------------------------------------------------------
nGrowthSeeds::nGrowthSeeds() :
    size(0),
    allList(0)
{
    this->SetType( Other );
}


//------------------------------------------------------------------------------
nGrowthSeeds::~nGrowthSeeds()
{
}


//------------------------------------------------------------------------------
bool
nGrowthSeeds::LoadResource()
{
    n_assert(!this->IsLoaded());
    bool success = false;
 
    nFileServer2* fs = kernelServer->GetFileServer();
    nFile* file;
    file = fs->NewFileObject();
    n_assert(file);
    success = this->LoadBinaryFile(file);
    file->Release();

    this->SetState( success ? Valid : Unloaded );
    return success;
}

//------------------------------------------------------------------------------
void
nGrowthSeeds::UnloadResource()
{
    this->allList.SetSize( 0 );
    this->SetState( Unloaded);
}

//------------------------------------------------------------------------------
bool
nGrowthSeeds::CanLoadAsync() const
{
    return false;
}

//------------------------------------------------------------------------------
int
nGrowthSeeds::GetByteSize()
{
    return this->size;
}

//------------------------------------------------------------------------------
const nGrowthSeeds::ListSeed&
nGrowthSeeds::GetSeedsByGroup(int idx) const
{
    return this->allList[idx];
}

//------------------------------------------------------------------------------
const nGrowthSeeds::GroupListSeed&
nGrowthSeeds::GetSeeds() const
{
    return this->allList;
}

//------------------------------------------------------------------------------
bool
nGrowthSeeds::LoadBinaryFile(nFile* file)
{
    n_assert(file);
    // open the file
    if (!file->Open(this->GetFilename(), "rb"))
    {
        n_printf("nNvx2Loader: could not open file '%s'!\n", this->GetFilename().Get());
        ///this->Close();
        return false;
    }

    // read file header, including groups
    int magic = file->GetInt();
    if (magic != 'NGS2')
    {
        n_printf("nGrowthSeeds: '%s' is not a 'NGS2' file!\n",  this->GetFilename().Get() );
        return false;
    }

    int version = file->GetInt();
    if (version != NGROWTHSEEDS_VERSION  )
    {
        n_printf("nGrowthSeeds: '%s' is not a valid version data!\n", this->GetFilename().Get() );
        return false;
    }

    this->min.x =  file->GetFloat();
    this->min.y =  file->GetFloat();
    this->min.z =  file->GetFloat();

    this->factor.x =  file->GetFloat();
    this->factor.y =  file->GetFloat();
    this->factor.z =  file->GetFloat();



    int numGroups = file->GetInt();   
    if (numGroups > 256 ) // If the file has error prevent use big memory
    {
        n_printf("nGrowthSeeds: '%s' has a many groups \n", this->GetFilename().Get() );
        return false;
    }
    this->allList.SetSize( numGroups );
    this->size = 0;

    // The sizes
    for ( int idxGroup= 0;  idxGroup < this->allList.Size() ; ++idxGroup  )
    {
        int listSize = file->GetInt();
        if (numGroups > 1024 ) // If the file has error prevent use big memory
        {
            n_printf("nGrowthSeeds: '%s' has a many instance \n", this->GetFilename().Get() );
            return false;
        }
        this->allList[idxGroup ].SetSize( listSize );
        this->size+= listSize; 
    }

    this->size *= sizeof( class Seed );


    // Data
    for ( int idxGroup= 0;  idxGroup < this->allList.Size() ; ++idxGroup  )
    {
        if ( allList[idxGroup ].Size() > 0 )
        {
            int numBytes = allList[idxGroup ].Size()*sizeof( class Seed );
            int readBytes = file->Read( &(allList[idxGroup ][0]) , numBytes );
            if ( numBytes != readBytes) // If the file has error prevent use big memory
            {
                n_printf("nGrowthSeeds: '%s' has a few data \n", this->GetFilename().Get() );
                return false;
            }
        }
    }
    return true;
}

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
void
nGrowthSeeds::CreateList(const BuildGroupListSeed& buildList)
{
    this->allList.SetSize( buildList.Size() ); 
    this->size = 0;
    for ( int idxGroup = 0; idxGroup < buildList.Size() ; ++idxGroup )
    {
        //Copy from nArray to nFixedArray
        const int listSize = buildList[idxGroup].Size();
        this->size+=listSize;
        this->allList[idxGroup].SetSize(  listSize );


        for ( int idxList = 0; idxList < listSize ; ++idxList)
        {
            this->allList[idxGroup][idxList] = buildList[idxGroup][idxList];
        }
    }
}


//------------------------------------------------------------------------------
void
nGrowthSeeds::SetCellSize( const vector3& min, const vector3& max)
{
    this->min = min;
    this->factor = max - min;
}

//------------------------------------------------------------------------------
bool
nGrowthSeeds::Save()
{
    bool retval(false);
    nFileServer2* fs = kernelServer->GetFileServer();
    nFile* file;
    file = fs->NewFileObject();
    n_assert(file);
    retval = this->SaveBinaryFile(file);
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
bool 
nGrowthSeeds::SaveBinaryFile(nFile* file)
{
    n_assert(file);

    if (!file->Open(this->GetFilename() , "wb"))
    {
        return false;
    }

        // read file header, including groups
    int magic = 'NGS2';
    file->PutInt( magic);
    file->PutInt( NGROWTHSEEDS_VERSION );
    file->PutFloat( this->min.x );
    file->PutFloat( this->min.y );
    file->PutFloat( this->min.z );

    file->PutFloat( this->factor.x );
    file->PutFloat( this->factor.y );
    file->PutFloat( this->factor.z );

    file->PutInt( this->allList.Size() ); //  The number of groups

    // The sizes
    for ( int idxGroup= 0;  idxGroup < this->allList.Size() ; ++idxGroup  )
    {
        int listSize =  this->allList[idxGroup ].Size();
        file->PutInt( listSize);
    }

    //The data
    for ( int idxGroup= 0;  idxGroup < this->allList.Size() ; ++idxGroup  )
    {
        int listSize =  this->allList[idxGroup ].Size();
        if ( listSize > 0 )
        {
            int numBytes = listSize * sizeof( class Seed );
            int writeBytes = file->Write( &(allList[idxGroup ][0]) , numBytes );

            if ( numBytes != writeBytes) // If the file has error prevent use big memory
            {
                n_printf("nGrowthSeeds: '%s' has problem for save data\n", this->GetFilename().Get() );
                return false;
            }
        }
    }
    return true;
}

#endif
#endif


//------------------------------------------------------------------------------
const matrix44 
nGrowthSeeds::GetMatrix(const Seed& seed) const
{
    

    vector3 pos = this->GetPosition( seed);
    float scale = this->GetScale( seed);
    float roty =  this->GetRotationY( seed);


    quaternion q0;  // This operation is posible optimize because the y vector has 0
    q0.set_from_axes( vector3( 0.0f, 1.0f, 0.0f ) , this->GetNormal( seed ) );
    matrix44 matrix; // Adatpe to terrain Normal
    
    matrix.scale(vector3( scale , scale , scale ) );
    matrix.rotate_y( roty );
    matrix.mult_simple( matrix44(q0) ); // adapte to terrain
    matrix.translate( pos );
    
    
    return matrix;
}


//------------------------------------------------------------------------------
void 
nGrowthSeeds::SetPosition(Seed& seed, const vector3& pos) const
{
    // Convert pos betwenn 0 an 1.0
    vector3 pos2 = pos - this->min;
    pos2.x =  pos2.x / this->factor.x;
    pos2.y =  pos2.y / this->factor.y;
    pos2.z =  pos2.z / this->factor.z;

    int maxValue10 = (1 << 10) - 1;
    pos2.x *= maxValue10;
    pos2.y *= maxValue10;
    pos2.z *= maxValue10;

    seed.posx = static_cast<int>(pos2.x);
    seed.posy = static_cast<int>(pos2.y);
    seed.posz = static_cast<int>(pos2.z);
   
}

//------------------------------------------------------------------------------
vector3 
nGrowthSeeds::GetPosition(const Seed& seed) const
{
    int maxValue10 = (1 << 10) - 1;
    float toFloat10 = 1.0f / maxValue10;
    vector3 pos( toFloat10 * seed.posx , toFloat10 *  seed.posy , toFloat10 * seed.posz );

    pos.x =  pos.x * this->factor.x;
    pos.y =  pos.y * this->factor.y;
    pos.z =  pos.z * this->factor.z;

    return pos + this->min;
}

//------------------------------------------------------------------------------
void 
nGrowthSeeds::SetNormal(Seed& seed, const vector3& nor) const
{
    int maxValue8 = (1 << 8) - 1;
    int div = maxValue8 / 2 ;

    vector3 nor2(nor);
    nor2.norm();

    nor2 *= 0.5f;
    nor2 *= float(maxValue8);

    int norx = static_cast<int>(nor2.x);
    int norz = static_cast<int>(nor2.z);

    norx += div;
    norz += div;

    seed.norx = norx;
    seed.norz = norz;
}

//------------------------------------------------------------------------------
vector3 
nGrowthSeeds::GetNormal(const Seed& seed) const
{
    int maxValue8 = (1 << 8) - 1;
    int div = maxValue8 / 2 ;
    float toFloat8 = 1.0f / maxValue8;

    int norx = seed.norx;
    int norz = seed.norz;

    norx -= div;
    norz -= div;

    vector2 nor2( static_cast<float>(norx) , static_cast<float>(norz) ); 

    nor2.x *= 2.0f*toFloat8 ;
    nor2.y *= 2.0f*toFloat8 ;

    return vector3( nor2.x , n_sqrt( 1.0f - ( nor2 % nor2) ),  nor2.y );
}

//------------------------------------------------------------------------------
void 
nGrowthSeeds::SetRotationY(Seed& seed,float rot) const
{
    float rot2 = rot / N_TWOPI ;

    int maxValue6 = (1 << 6) ;

    rot2 *= maxValue6;
    int rot3  = static_cast<int>(rot2);
    rot3 = rot3 % maxValue6;
    seed.roty = rot3;
}
//------------------------------------------------------------------------------
float 
nGrowthSeeds::GetRotationY(const Seed& seed) const
{
    int maxValue6 = (1 << 6) ;
    float toFloat6 = N_TWOPI / maxValue6 ;

    return toFloat6 * seed.roty;
}

//------------------------------------------------------------------------------
void 
nGrowthSeeds::SetScale(Seed& seed,float scale) const
{
    int maxValue4 = (1 << 4) - 1 ;
    scale = n_min( scale , 1.5f);
    scale = n_max( scale , 0.5f);

    scale  -= 0.5f;
    scale*= maxValue4;
    seed.scale = static_cast<int>(scale);
}

//------------------------------------------------------------------------------
float 
nGrowthSeeds::GetScale(const Seed& seed) const
{
    int maxValue4 = (1 << 4) -1;
    float toFloat4 = 1.0f / maxValue4 ;
    float val = toFloat4*seed.scale;
    return val + 0.5f;
}

//------------------------------------------------------------------------------
void 
nGrowthSeeds::SetMeshId(Seed& seed , int meshId) const
{
    n_assert( meshId>=0 && meshId< ( (1<<8) -1) );
    seed.meshid = static_cast<nuint8>(meshId);
}

//------------------------------------------------------------------------------
int 
nGrowthSeeds::GetMeshId(const Seed& seed ) const
{
    return seed.meshid;
}

