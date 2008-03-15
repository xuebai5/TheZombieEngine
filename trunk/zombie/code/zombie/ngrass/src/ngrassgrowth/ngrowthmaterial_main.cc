#include "precompiled/pchngrass_conjurer.h"
//------------------------------------------------------------------------------
//  ngrowthmaterial_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#ifndef NGAME
#include "ngrassgrowth/ngrowthmaterial.h"
#include "util/nrandomlogic.h"

nNebulaScriptClass(nGrowthMaterial, "nroot");


//------------------------------------------------------------------------------
/**
*/
void 
nGrowthMaterial::SetColor(const vector3& color)
{
    this->color = color;
}
//------------------------------------------------------------------------------
/**
*/
const vector3&
nGrowthMaterial::GetColor(void) const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
nGrowthBrush*
nGrowthMaterial::CreateGrowthBrush(const nString &assetName)
{
    nObject* obj;
    nKernelServer::Instance()->PushCwd( this );
    obj= nKernelServer::Instance()->New("ngrowthbrush" , assetName.Get() );
    nKernelServer::Instance()->PopCwd();
    n_assert(obj);

    nGrowthBrush* brush = static_cast<nGrowthBrush*>(obj);
    brush->SetAssetName( assetName );
    return brush;
}

//------------------------------------------------------------------------------
/**
*/
int
nGrowthMaterial::GetNumberOfGrowthBrushes()
{
    int count = 0;
    nRoot* node = this->GetHead();
    while ( node)
    {
        node = node->GetSucc();
        ++count;
    }
    return count;
}

//------------------------------------------------------------------------------
/**

*/
nGrowthBrush*
nGrowthMaterial::GetGrowthBrush(int idx)  const
{
    nRoot* node = 0;
    if ( idx >= 0 )
    {
        node = this->GetHead();
        for ( int i = 0; i < idx && node;  i++ )
        {
            node = node->GetSucc();
        }
    }
    return static_cast<nGrowthBrush*>(node);
}

//------------------------------------------------------------------------------
/**

*/
void
nGrowthMaterial::DeleteGrowthBrush(int idx )
{  
    nObject* material = this->GetGrowthBrush(idx);
    if ( material )
    {
        material->Release();
    }
}


//------------------------------------------------------------------------------
/**

*/
void
nGrowthMaterial::SetRandomSeed(int randomSeed)
{
    this->random = randomSeed;
    this->previousRandom = randomSeed;
}

//------------------------------------------------------------------------------
/**

*/
void
nGrowthMaterial::BeginBuild()
{
    this->listBrush.Reallocate(16,16);
    // Make array with children for fast access
    nRoot* node = this->GetHead();
    this->sumProbability = 0.0f;
    while ( node)
    {
        nGrowthBrush* brush = static_cast<nGrowthBrush*>( node );
        this->sumProbability += brush->GetSpawnProbability();
        this->listBrush.Append(  brush);
        node = node->GetSucc();
    }

    if ( this->sumProbability < 1.0f )
    {
        this->sumProbability = 1.0f;
    }
}

//------------------------------------------------------------------------------
/**

*/
nGrowthBrush* 
nGrowthMaterial::GetRandomBrush()
{
    nGrowthBrush* ret = 0;
    int tmp = n_getseed( ); // for restore it

    n_setseed(this->random); // set the our random 
    this->previousRandom = this->random;  // store it

    float probability= n_rand_real( 1.0f ); 

    this->random = n_getseed( );

    n_setseed(tmp); // retore the seed

    /*
        Example
        p = Uniform[ 0.0   .. 1.0 ]

        case  0.0< p < 0.3
                has a probability 0.3
        case  0.3< p < 0.8
                has a probability 0.5
        case  0.8 < p < 1.0
                has a probabilty 0.2
    */


    probability *= this->sumProbability ;
    for ( int idx = 0; idx < this->listBrush.Size() ; ++idx )
    {
        float brushProb = this->listBrush[idx]->GetSpawnProbability();
        if ( probability <=  brushProb )
        {
            return this->listBrush[idx];
        } else
        {
            probability -= brushProb;
        }
    }
    
    // return 0 is posible if the original sumProbability < 1.0f
    return ret;
}

//------------------------------------------------------------------------------
/**

*/
void
nGrowthMaterial::DiscardLastRandomBrush()
{
    this->random = this->previousRandom;
}

//------------------------------------------------------------------------------
/**

*/
void
nGrowthMaterial::EndBuild()
{
    this->listBrush.Reallocate(0,0);
}


#endif NGAME