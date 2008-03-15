//------------------------------------------------------------------------------
//  nchorizonsegmentbuilder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnspatial.h"
#include "nspatial/nhorizonsegmentbuilder.h"
#include "nspatial/nchorizonclass.h"
#include "nspatial/nhorizonsegment.h"

//------------------------------------------------------------------------------
/**
*/
nHorizonSegmentBuilder::nHorizonSegmentBuilder() :
    blockSize(0),
    sidesize(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nHorizonSegmentBuilder::~nHorizonSegmentBuilder()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nHorizonSegmentBuilder::Setup(nFloatMap * heightMap, int blockSize, float sidesize)
{
    n_assert(heightMap);
    this->refHM = heightMap;
    this->blockSize = blockSize;
    this->sidesize = sidesize;
}

//------------------------------------------------------------------------------
/**
*/
void 
nHorizonSegmentBuilder::BuildHorizonSegmentX(int bx, int bz, float & y0, float & y1)
{
    nLSQ lsq;

    // calculate x, z
    const float scale = this->refHM->GetGridScale();
    const int xmin = this->BlockToHeightMapCoord(bx);
    const int xmax = this->BlockToHeightMapCoord(bx + 1);
    const int z = this->BlockToHeightMapCoord(bz);

    // calculate least squares line
    for(int x = xmin;x <= xmax;x++)
    {        
        float y = this->refHM->GetHeightLC(x, z);
        lsq.AddPoint(x * scale, y);
    }

    // calculate line parameters
    float a = lsq.CalculateA();
    float b = lsq.CalculateB();

    // calculate max and min distance to the line
    float distneg = 0.0f;
    for(int x = xmin;x <= xmax;x++)
    {        
        float y = this->refHM->GetHeightLC(x, z);
        float liney = a * (x * scale) + b;
        float diff = (y - liney);
        if (diff < 0.0)
        {
            distneg = n_min(diff, distneg);
        }
    }

    // calculate start and end points
    y0 = a * (xmin * scale) + b + distneg;
    y1 = a * (xmax * scale) + b + distneg;
}

//------------------------------------------------------------------------------
/**
*/
void 
nHorizonSegmentBuilder::BuildHorizonSegmentZ(int bx, int bz, float & y0, float & y1)
{
    nLSQ lsq;

    // calculate x, z
    const float scale = this->refHM->GetGridScale();
    const int x = this->BlockToHeightMapCoord(bx);
    const int zmin = this->BlockToHeightMapCoord(bz);
    const int zmax = this->BlockToHeightMapCoord(bz + 1);

    // calculate least squares line
    for(int z = zmin;z <= zmax;z++)
    {        
        float y = this->refHM->GetHeightLC(x, z);
        lsq.AddPoint(z * scale, y);
    }

    // calculate line parameters
    float a = lsq.CalculateA();
    float b = lsq.CalculateB();

    // calculate max and min distance to the line
    float distneg = 0.0f;
    for(int z = zmin;z <= zmax;z++)
    {        
        float y = this->refHM->GetHeightLC(x, z);
        float liney = a * (z * scale) + b;
        float diff = (y - liney);
        if (diff < 0.0)
        {
            distneg = n_min(diff, distneg);
        }
    }

    // calculate start and end points
    y0 = a * (zmin * scale) + b + distneg;
    y1 = a * (zmax * scale) + b + distneg;
}

//------------------------------------------------------------------------------
/**
*/
int 
nHorizonSegmentBuilder::BlockToHeightMapCoord(int a)
{
    return a * (this->blockSize - 1);
}

//------------------------------------------------------------------------------
/**
*/
bool
nHorizonSegmentBuilder::SaveChunk(nEntityClass * ec)
{
    ncHorizonClass * hc = ec->GetComponent<ncHorizonClass>();
    if (!hc)
    {
        return false;
    }

    nString path = hc->GetHorizonFilename();

    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if( ps )
    {
        // change command to 'THIS'
        nCmd * cmd = ps->GetCmd( ec, 'THIS' );
        n_assert( cmd );
        if( cmd )
        {
            if( nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() ) )
            {
                if( ps->BeginObjectWithCmd( ec, cmd, path.Get() ) ) 
                {
                    // Begin horizon segments
                    ps->Put(ec, 'BBHS', hc->GetNumSegmentsX(), hc->GetNumSegmentsZ());

                    // Add all horizon segments
                    for(int bz = 0;bz < hc->GetNumSegmentsZ();bz++)
                    {
                        for(int bx = 0;bx < hc->GetNumSegmentsX() - 1;bx++)
                        {
                            nHorizonSegment * hsX = hc->GetHorizonSegment(bx, bz, true);
                            ps->Put(ec, 'BAHS', bx, bz, true, hsX->GetStart().y, hsX->GetEnd().y);
                        }
                    }

                    for(int bz = 0;bz < hc->GetNumSegmentsZ() - 1;bz++)
                    {
                        for(int bx = 0;bx < hc->GetNumSegmentsX();bx++)
                        {
                            nHorizonSegment * hsZ = hc->GetHorizonSegment(bx, bz, false);
                            ps->Put(ec, 'BAHS', bx, bz, false, hsZ->GetStart().y, hsZ->GetEnd().y);
                        }
                    }

                    // End horizon segments
                    ps->Put(ec, 'BEHS');

                    // save commands
                    ps->EndObject( true );
                }
                return true;

            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nHorizonSegmentBuilder::Build(nEntityClass * ec)
{
    ncHorizonClass * hc = ec->GetComponent<ncHorizonClass>();
    if (!hc)
    {
        return;
    }

    // Add all horizon segments
    float y0, y1;
    for(int bz = 0;bz < hc->GetNumSegmentsZ();bz++)
    {
        for(int bx = 0;bx < hc->GetNumSegmentsX() - 1;bx++)
        {
            nHorizonSegment * hsX = hc->GetHorizonSegment(bx, bz, true);
            this->BuildHorizonSegmentX(bx, bz, y0, y1);
            hsX->InitSegmentX(bx, bz, this->sidesize, y0, y1);
        }
    }

    for(int bz = 0;bz < hc->GetNumSegmentsZ()- 1;bz++)
    {
        for(int bx = 0;bx < hc->GetNumSegmentsX();bx++)
        {
            nHorizonSegment * hsZ = hc->GetHorizonSegment(bx, bz, false);
            this->BuildHorizonSegmentZ(bx, bz, y0, y1);
            hsZ->InitSegmentZ(bx, bz, this->sidesize, y0, y1);
        }
    }
}
