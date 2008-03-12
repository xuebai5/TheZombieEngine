#ifndef N_SYSTEM_COORDINATES_H
#define N_SYSTEM_COORDINATES_H
#pragma warning( push, 3 )
#include "max.h"
#include "IGame/IGame.h"
#pragma warning( pop  )
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "mathlib/transform44.h"
#include "tools/nmeshbuilder.h"


//#include vector2

class n3dsSystemCoordinates
{
public:
    n3dsSystemCoordinates();
    void SetCoordinates();
    /// convert texture coordinate max to nebula
    vector2      MaxToNebulaUV(const Point3& src);
    /// convert matrix to nebula
    matrix44d    MaxToNebulaMatrix(const matrix44d &matrix);
    /// convert matrix to nebula
    matrix44d    MaxToNebulaMatrix(const GMatrix &g);
     /// convert matrix to nebula
    matrix44d    MaxToNebulaMatrix(const Matrix3 &g);
    /// convert matrix to nebula transform44
    transform44  MaxToNebulaTransform(const matrix44d &matrix);
    /// convert matrix to nebula transform44
    transform44  MaxToNebulaTransform(const GMatrix &g);
    /// convert matrix to nebula transform44
    transform44  MaxToNebulaTransform(const Matrix3 &g);
    /// convert transfrom to nebula transform44
    transform44  MaxToNebulaTransform(const transform44 &g);
    /// convert vertex to nebula
    vector3      MaxtoNebulaVertex(Point3 vertex);
    /// convert convert normal to nebula;
    vector3      MaxtoNebulaNormal(Point3 normal);
    /// swap  vertex for culling CCW or CW
    void         MaxToNebulaCulling(nMeshBuilder::Vertex &v0,nMeshBuilder::Vertex &v1,nMeshBuilder::Vertex& v2);
    /// Convert distances
    float        MaxToNebulaDistance(const float dist)const;
    /// Say if scale was do before rotation.
    bool         HaveIndependentScale(const Matrix3 &g);
protected:
    void SetUnits();
    double GetMetersPerMaxUnit();
    matrix44d max2Neb;
    matrix44d neb2Max;
    double unitsMax2Neb;
    double unitsNeb2Max;
};


#endif
