#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsexporters/n3dsSystemcoordinates.h"
// UNITS in meters

#define METERS_PER_NEBULA_UNITS 1.0


//------------------------------------------------------------------------------
/**
            MAX          NEBULA
        +Z               +Y
        |  +Y            |
        | /              |
        |/______ +X      |_____ +X
                        /
                       +Z
*/

n3dsSystemCoordinates::n3dsSystemCoordinates()
{

    this->SetUnits();

    this->max2Neb.ident();
    this->max2Neb.rotate( 1.0, 0.0, 0.0, 1.0, 0.0); //rotate 90 in x-axis
    this->max2Neb.scale(unitsMax2Neb ,unitsMax2Neb, unitsMax2Neb); 

    this->neb2Max.ident();
    this->neb2Max.scale(unitsNeb2Max, unitsNeb2Max, unitsNeb2Max);
    this->neb2Max.rotate(1.0, 0.0, 0.0, -1.0, 0.0); //rotate -90 in x axis

    /*
        note if you change this matrix look the next methods:
        *TransformMaxToNebula

    */


}

//------------------------------------------------------------------------------
/**
                           meters/maxUnits               nebUnits
      unitsMax2Neb  =    ------------ ---------- =  v * ---------
                           meters/nebUnits               maxUnits
*/
void 
n3dsSystemCoordinates::SetUnits()
{

    this->unitsMax2Neb = GetMetersPerMaxUnit() / METERS_PER_NEBULA_UNITS;
    this->unitsNeb2Max = 1.0 / unitsMax2Neb;

}

//------------------------------------------------------------------------------
/**
        this function is equivalent to GetMasterScale(UNITS_METERS)
        but 'I don't like GetMasterScale because has inprecision by
        the default system is in US units
        example: setup 1 unit = 1 cm
        return 0.0099999934099999 m
*/
double 
n3dsSystemCoordinates::GetMetersPerMaxUnit()
{
    int unitsType;
    float scale;
    double val;
    GetMasterUnitInfo(&unitsType,&scale);
    switch(unitsType)
    {
        case UNITS_INCHES:
            val = 0.0254;
            break;
        case UNITS_FEET:
            val = 0.3048;
            break;
        case UNITS_MILES:
            val = 1609.344;
            break;
        case UNITS_MILLIMETERS:
            val = 0.001;
            break; 
        case UNITS_CENTIMETERS:
            val = 0.01;
            break;
        case UNITS_METERS:
            val = 1.0;
            break;
        case UNITS_KILOMETERS:
            val = 1000.0;
            break;
        default:
            val = 1.0;
            scale = 1.0;
    }

    // 1 unit = scale  UnitsType;

    return val*scale;

}

void
n3dsSystemCoordinates::SetCoordinates()
{
    IGameConversionManager* cm = GetConversionManager();
    n_assert(cm);
    cm->SetCoordSystem(IGameConversionManager::CoordSystem::IGAME_MAX);
    //
    // I' dont like use IGAME_USER because it  has problem with matrix.
    // and export local coordinates
}

//------------------------------------------------------------------------------
/**
     transform MAX uv IGAME uv
*/
vector2
n3dsSystemCoordinates::MaxToNebulaUV(const Point3& src)
{
    float u,v;
    u = src.x;
    v = -src.y;
    return vector2(u,v);
}



//------------------------------------------------------------------------------
/*

*/
matrix44d 
n3dsSystemCoordinates::MaxToNebulaMatrix(const matrix44d &matrix)
{
    return (neb2Max * matrix * max2Neb);
}

//------------------------------------------------------------------------------
/*

*/
matrix44d 
n3dsSystemCoordinates::MaxToNebulaMatrix(const GMatrix &g)
{
    const GRow *m = g.GetAddr();
    matrix44d nebMatrix;

    matrix44d maxMatrix(m[0][0],m[0][1],m[0][2],m[0][3],
                        m[1][0],m[1][1],m[1][2],m[1][3],
                        m[2][0],m[2][1],m[2][2],m[2][3],
                        m[3][0],m[3][1],m[3][2],m[3][3]);

    return MaxToNebulaMatrix(maxMatrix);
}

//------------------------------------------------------------------------------
/*

*/
matrix44d 
n3dsSystemCoordinates::MaxToNebulaMatrix(const Matrix3 &g)
{
    // g is 4x3 float
    const MRow *m = g.GetAddr();
    matrix44d nebMatrix;
 
    matrix44d maxMatrix(m[0][0],m[0][1],m[0][2], 0.0,
                        m[1][0],m[1][1],m[1][2], 0.0,
                        m[2][0],m[2][1],m[2][2], 0.0,
                        m[3][0],m[3][1],m[3][2], 1.0);

    return MaxToNebulaMatrix(maxMatrix);
}

//------------------------------------------------------------------------------
/*
    This the real calculus
    matrix44d nebMatrix = neb2Max * maxMatrix * max2Neb;

    nebMatrix.get(sca,qua,tra);
    transform.setscale(sca);
    transform.setquatrotation(qua);
    transform.settranslation(tra);

    But this function is optimized dont' calcule 
    nebMatrix =neb2Max * maxMatrix * max2Neb;
    assume:
        max2Neb.rotate( 1.0, 0.0, 0.0, 1.0, 0.0);
        max2Neb.scale(unitsMax2Neb ,unitsMax2Neb, unitsMax2Neb); 

        neb2Max.scale(unitsNeb2Max, unitsNeb2Max, unitsNeb2Max);
        neb2Max.rotate(1.0, 0.0, 0.0, -1.0, 0.0); //rotate -90 in x axis

*/
transform44 
n3dsSystemCoordinates::MaxToNebulaTransform(const matrix44d &matrix)
{
    transform44 transform;
    vector3 tra,sca;
    quaternion qua;

    #if 0
    matrix44d nebMatrix =neb2Max * matrix * max2Neb;
    nebMatrix.get(sca,qua,tra);

    #else  //optimized
    matrix.get(sca,qua,tra);
    sca.set( sca.x, sca.z,  sca.y); //not change signed
    qua.set( qua.x, qua.z, -qua.y, qua.w);
    tra.set( tra.x, tra.z, -tra.y);
    tra *= float( this->unitsMax2Neb );
    #endif

    transform.setscale(sca); // in scale not change  signed
    transform.setquatrotation(qua);
    transform.settranslation(tra);

    return transform;
}

transform44 
n3dsSystemCoordinates::MaxToNebulaTransform(const GMatrix &g)
{
    const GRow *m = g.GetAddr();
    matrix44d maxMatrix(m[0][0],m[0][1],m[0][2],m[0][3],
                        m[1][0],m[1][1],m[1][2],m[1][3],
                        m[2][0],m[2][1],m[2][2],m[2][3],
                        m[3][0],m[3][1],m[3][2],m[3][3]);

    return MaxToNebulaTransform(maxMatrix);
}

transform44 
n3dsSystemCoordinates::MaxToNebulaTransform(const Matrix3 &g)
{
    // g is 4x3 float
    const MRow *m = g.GetAddr();
    matrix44d maxMatrix(m[0][0],m[0][1],m[0][2],0.0,
                        m[1][0],m[1][1],m[1][2],0.0,
                        m[2][0],m[2][1],m[2][2],0.0,
                        m[3][0],m[3][1],m[3][2],1.0);

    return MaxToNebulaTransform(maxMatrix);
}


//------------------------------------------------------------------------------
/**
    optimization
    assume:
        max2Neb.rotate( 1.0, 0.0, 0.0, 1.0, 0.0);
        max2Neb.scale(unitsMax2Neb ,unitsMax2Neb, unitsMax2Neb); 

        neb2Max.scale(unitsNeb2Max, unitsNeb2Max, unitsNeb2Max);
        neb2Max.rotate(1.0, 0.0, 0.0, -1.0, 0.0); //rotate -90 in x axis
*/
transform44  
n3dsSystemCoordinates::MaxToNebulaTransform(const transform44 &g)
{
    transform44 tmp;
    vector3 sca(g.getscale());
    quaternion qua(g.getquatrotation());
    vector3 tra(g.gettranslation());

    #if 0
    matrix44d matrix;
    matrix.scale(sca);
    matrix.mult_simple(matrix44d(qua));
    matrix.translate(tra);

    matrix = neb2Max * matrix * max2Neb;
    matrix.get(sca,qua,tra);

    #else  //optimization

    sca.set( sca.x, sca.z , sca.y); // scale no change singned
    qua.set( qua.x, qua.z, -qua.y, qua.w);
    tra.set( tra.x, tra.z, -tra.y);
    tra = vector3( float( unitsMax2Neb * tra.x ), float( unitsMax2Neb * tra.y ), float( unitsMax2Neb * tra.z ) );

    #endif

    tmp.setscale(sca);
    tmp.setquatrotation(qua);
    tmp.settranslation(tra);


    return tmp;
}

//------------------------------------------------------------------------------
/**
    optimization
    assume:
        max2Neb.rotate( 1.0, 0.0, 0.0, 1.0, 0.0);
        max2Neb.scale(unitsMax2Neb ,unitsMax2Neb, unitsMax2Neb); 

        neb2Max.scale(unitsNeb2Max, unitsNeb2Max, unitsNeb2Max);
        neb2Max.rotate(1.0, 0.0, 0.0, -1.0, 0.0); //rotate -90 in x axis
*/
vector3
n3dsSystemCoordinates::MaxtoNebulaVertex(Point3 vertex)
{
    #if 0

    vector3 nebVertex(vertex.x, vertex.y, vertex.z); //assume max2Neb translation 0 0 0
    return max2Neb*nebVertex; // the operator * is same to nebVertex*max2Neb;

    #else //optimization

    vector3 nebVertex(vertex.x, vertex.z, -vertex.y);
    return vector3 ( float(unitsMax2Neb * nebVertex.x), float(unitsMax2Neb * nebVertex.y) ,  float(unitsMax2Neb * nebVertex.z) );
    #endif
}

//------------------------------------------------------------------------------
/**
    assume:
        max2Neb.rotate( 1.0, 0.0, 0.0, 1.0, 0.0);
        max2Neb.scale(unitsMax2Neb ,unitsMax2Neb, unitsMax2Neb); 

        neb2Max.scale(unitsNeb2Max, unitsNeb2Max, unitsNeb2Max);
        neb2Max.rotate(1.0, 0.0, 0.0, -1.0, 0.0); //rotate -90 in x axis
*/
vector3
n3dsSystemCoordinates::MaxtoNebulaNormal(Point3 normal)
{
   
    #if 0

    vector3 nebNormal(normal.x, normal.y, normal.z);
    nebVertex=max2Neb*nebVertex; // the operator * is nebVertex*max2Neb;
    nebVertex.norm(); 

    #else //optimized

    vector3 nebNormal(normal.x, normal.z, -normal.y);

    #endif
    return nebNormal;
}

//------------------------------------------------------------------------------
/**
    optimization
    assume Nebula cullMode=CW
    and the sign(scale.x * scale.y)==sign(scale.y*scale.z) 
            max2Neb.get(scale);
*/
void
n3dsSystemCoordinates::MaxToNebulaCulling(nMeshBuilder::Vertex &/*v0*/,nMeshBuilder::Vertex &/*v1*/,nMeshBuilder::Vertex& /*v2*/)
{
    //optimization
    // empty
}
//------------------------------------------------------------------------------
/**
*/
float
n3dsSystemCoordinates::MaxToNebulaDistance(const float dist)const
{
    return float( this->unitsMax2Neb * dist);
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsSystemCoordinates::HaveIndependentScale(const Matrix3 &g)
{
    matrix44d m = this->MaxToNebulaMatrix( g );

    vector3 A, B, C;
    A.x = float( m.m[0][0] );
    A.y = float( m.m[0][1] );
    A.z = float( m.m[0][2] );

    B.x = float( m.m[1][0] );
    B.y = float( m.m[1][1] );
    B.z = float( m.m[1][2] );

    C.x = float( m.m[2][0] );
    C.y = float( m.m[2][1] );
    C.z = float( m.m[2][2] );

    if( fabs( A.dot( B ) ) > 0.0001 )
    {
        return false;
    }

    if( fabs( B.dot( C ) ) > 0.0001 )
    {
        return false;
    }

    if( fabs( C.dot( A ) ) > 0.0001 )
    {
        return false;
    }

    return true;
}