#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nlensflarenode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nlensflarenode.h"

nNebulaScriptClass(nLensFlareNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nLensFlareNode::nLensFlareNode() :
    bBegun(false),
    bEnded(false),
    bWithinBounds(false),
    baseColor(1.0f, 1.0f, 1.0f, 1.0f),
    blindColor(1.0f, 1.0f, 0.8f, 1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLensFlareNode::~nLensFlareNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nLensFlareNode::LoadResources()
{
    if (!this->dynMesh.IsValid())
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleList, 
            nMesh2::Coord|nMesh2::Color|nMesh2::Uv0,
            nMesh2::WriteOnly | nMesh2::NeedsVertexShader, true);

        n_assert(this->dynMesh.IsValid());
    }
    return nGeometryNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::UnloadResources()
{
    return nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::BeginFlares(int iNumFlares)
{
    n_assert(iNumFlares <= MAXFLARES);
    this->flares.SetFixedSize(iNumFlares);
    this->bBegun = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::EndFlares()
{
    n_assert(bBegun == true);

    bEnded = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::SetFlareSizeAt(int ndx, float size)
{
    n_assert(bBegun == true);

    this->flares[ndx].fSize = size;
    this->flares[ndx].fHalfSize = size / 2;  // used for more efficient placement of
                                             // flares
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::SetFlarePosAt(int ndx, float pos)
{
    n_assert(bBegun == true);

    this->flares[ndx].fPosition = pos;
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::SetFlareColorAt(int ndx, const vector4& color)
{
    n_assert(bBegun == true);

    this->flares[ndx].color = color;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLensFlareNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (this->bEnded)
    {
        nGeometryNode::Render(sceneGraph, entityObject);

        nGfxServer2* gfxServer = nGfxServer2::Instance();
        const matrix44& modelView = gfxServer->GetTransform(nGfxServer2::ModelView);
        const matrix44& projection = gfxServer->GetTransform(nGfxServer2::Projection);

        // get the n3dnode (light source origin) in pseudo-screen space
        this->CalcSource2d(modelView, projection);

        // test to see if the n3dnode appears on the screen
        if (this->vSource2d.x >= -1.0f && this->vSource2d.x <= 1.0f &&
            this->vSource2d.y >= -1.0f && this->vSource2d.y <= 1.0f &&
            this->vSource2d.z <=  1.0f)
        {
            this->bWithinBounds = true;
        }
        else
        {
            this->bWithinBounds = false;
        }

        if (this->bWithinBounds)
        {
            // Compute how far off-center the flare source is.
            float flaredist = n_sqrt((this->vSource2d.x * this->vSource2d.x) +
                                     (this->vSource2d.y * this->vSource2d.y));

            float* dstVertices;
            ushort* dstIndices;
            int maxVertices;
            int maxIndices;

            this->dynMesh.BeginIndexed(dstVertices, dstIndices, maxVertices, maxIndices);
            n_assert(dstVertices);
            n_assert(dstIndices);

            // fill index buffer
            int iNumberOfIndices = this->GetNumFlares() * 6;     // 6 indices per quad
            n_assert(iNumberOfIndices < maxIndices);

            // FIXME exactly the same every frame!
            int indexIndex = 0;
            int vertexIndex = 0; 
            for (; vertexIndex < this->GetNumFlares() * 4; vertexIndex += 4)
            {
                //  upper-left triangle of quad
                dstIndices[indexIndex++] = (ushort) vertexIndex;
                dstIndices[indexIndex++] = (ushort) vertexIndex + 1;
                dstIndices[indexIndex++] = (ushort) vertexIndex + 2;

                //  lower-right triangle of quad
                dstIndices[indexIndex++] = (ushort) vertexIndex + 2;
                dstIndices[indexIndex++] = (ushort) vertexIndex + 3;
                dstIndices[indexIndex++] = (ushort) vertexIndex;
            }

            // fill vertex buffer
            float fARAdj = 0.75f;   // adjusts for aspect ratio so flares are square
            vector3 v(0.0f, 0.0f, 0.0f);

            int iNumberOfVertices = this->GetNumFlares() * 4;    // 4 vert's per quad (flare)
            n_assert(iNumberOfVertices < maxVertices);
            int curIndex = 0;
            int iCnt = 0;
            for (int i = 0; i < this->GetNumFlares() * 4; i += 4, iCnt++)
            {
                //this->PlaceFlares();
                float fRealPos_y = this->flares[iCnt].fPosition * -vSource2d.y;
                float fRealPos_x = this->flares[iCnt].fPosition * -vSource2d.x;

                //  position the flare (quad)
                v.x = vSource2d.x + (this->flares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
                v.y = vSource2d.y + this->flares[iCnt].fHalfSize + fRealPos_y;
                //vb_dest->Coord(i, v);
                dstVertices[curIndex++] = v.x;
                dstVertices[curIndex++] = v.y;
                dstVertices[curIndex++] = v.z;

                //this->UVFlares()
                vector2 c1(1.0f, 1.0f);
                vector2 c2(0.0f, 1.0f);
                vector2 c3(0.0f, 0.0f);
                vector2 c4(1.0f, 0.0f);

                //vb_dest->Uv(i    , 0, c1); // uv upper right
                dstVertices[curIndex++] = c1.x;
                dstVertices[curIndex++] = c1.y;

                //this->ColorFlares();
                vector4 color;
                color.x = this->flares[iCnt].color.x * this->baseColor.x;
                color.y = this->flares[iCnt].color.y * this->baseColor.y;
                color.z = this->flares[iCnt].color.z * this->baseColor.z;
                color.w = this->flares[iCnt].color.w * this->baseColor.w;
                color.w *= (1.0f - flaredist);

                //vb_dest->Color(i, c);
                dstVertices[curIndex++] = color.x;
                dstVertices[curIndex++] = color.y;
                dstVertices[curIndex++] = color.z;
                dstVertices[curIndex++] = color.w;

                // repeat for all 4 vertices
                v.x = vSource2d.x - (this->flares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
                v.y = vSource2d.y + this->flares[iCnt].fHalfSize + fRealPos_y;

                //vb_dest->Coord(i + 1, v);
                dstVertices[curIndex++] = v.x;
                dstVertices[curIndex++] = v.y;
                dstVertices[curIndex++] = v.z;

                //vb_dest->Uv(i + 1, 0, c2); // uv up
                dstVertices[curIndex++] = c2.x;
                dstVertices[curIndex++] = c2.y;

                //vb_dest->Color(i + 1, c);
                dstVertices[curIndex++] = color.x;
                dstVertices[curIndex++] = color.y;
                dstVertices[curIndex++] = color.z;
                dstVertices[curIndex++] = color.w;

                v.x = vSource2d.x - (this->flares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
                v.y = vSource2d.y - this->flares[iCnt].fHalfSize + fRealPos_y;
                //vb_dest->Coord(i + 2, v);
                dstVertices[curIndex++] = v.x;
                dstVertices[curIndex++] = v.y;
                dstVertices[curIndex++] = v.z;

                //vb_dest->Uv(i + 2, 0, c3); // uv origin
                dstVertices[curIndex++] = c3.x;
                dstVertices[curIndex++] = c3.y;

                //vb_dest->Color(i + 2, c);
                dstVertices[curIndex++] = color.x;
                dstVertices[curIndex++] = color.y;
                dstVertices[curIndex++] = color.z;
                dstVertices[curIndex++] = color.w;

                v.x = vSource2d.x + (this->flares[iCnt].fHalfSize * fARAdj) + fRealPos_x;
                v.y = vSource2d.y - this->flares[iCnt].fHalfSize + fRealPos_y;
                //vb_dest->Coord(i + 3, v);
                dstVertices[curIndex++] = v.x;
                dstVertices[curIndex++] = v.y;
                dstVertices[curIndex++] = v.z;

                //vb_dest->Uv(i + 3, 0, c4); // uv right
                dstVertices[curIndex++] = c4.x;
                dstVertices[curIndex++] = c4.y;

                //vb_dest->Color(i + 3, c);
                dstVertices[curIndex++] = color.x;
                dstVertices[curIndex++] = color.y;
                dstVertices[curIndex++] = color.z;
                dstVertices[curIndex++] = color.w;
            }

            this->dynMesh.EndIndexed(iNumberOfVertices, iNumberOfIndices);

            // add the blind effect
            //this->Blind();
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::CalcSource2d(const matrix44& mModelView, const matrix44& mProjection)
{
    //  get the current position of the lenseflare's n3dnode in model-view space
    this->vSource2d.set(mModelView.M41,
                        mModelView.M42,
                        mModelView.M43,
                        1.0f);

    //  calculate the n3dnode's position in pseudo-screen space
    this->vSource2d = mProjection * this->vSource2d;
    this->vSource2d.x = this->vSource2d.x / this->vSource2d.w;
    this->vSource2d.y = this->vSource2d.y / this->vSource2d.w;
    this->vSource2d.z = this->vSource2d.z / this->vSource2d.w;
}

//------------------------------------------------------------------------------
/**
*/
void
nLensFlareNode::Blind()
{
    float fAlpha = 0.0f;
    if (this->vSource2d.x >= -1.0f &&  this->vSource2d.x <= 1.0f &&
        this->vSource2d.y >= -1.0f &&  this->vSource2d.y <= 1.0f)
    {
        float fRadius = n_sqrt((this->vSource2d.x * this->vSource2d.x) +
                               (this->vSource2d.y * this->vSource2d.y));

        if (fRadius <= 0.25f)
        {
            fAlpha = 4 * (0.25f - fRadius);
        }

        fAlpha *= this->blindColor.w;

        //  update the overlay plane
        //ref_fx->AddOverlay(vBlindColor.x,
        //                   vBlindColor.y,
        //                   vBlindColor.z,
        //                   fAlpha);
    }
}
