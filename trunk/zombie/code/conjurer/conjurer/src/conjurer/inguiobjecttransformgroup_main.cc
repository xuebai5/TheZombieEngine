#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  inguieditorobjecttransformgroup.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/inguiobjecttransformGroup.h"

//------------------------------------------------------------------------------
/**
*/
InguiObjectTransformGroup::InguiObjectTransformGroup()
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
InguiObjectTransformGroup::~InguiObjectTransformGroup()
{
    // empty
}
/**
    Set the objects array to be transformed
*/
void
InguiObjectTransformGroup::SetSelection(nArray<InguiObjectTransform>* set)
{
    this->objects = set;
}

//------------------------------------------------------------------------------
/**
    Begin an operation and set the initial matrix
*/
void
InguiObjectTransformGroup::Begin()
{
    // Calculate centroid of objects
    vector3 centroid(0.0f, 0.0f, 0.0f);
    int n = (*objects).Size();
    int total = n;
    for (int i=0; i < n;i++)
    {
        if ( ! (*objects)[i].GetContainingIndoor() )
        {
            transform44 t;
            (*objects)[i].GetTransform( t );
            centroid += t.gettranslation();
        }
        else
        {
            total --;
        }
        (*objects)[i].Begin();
    }
    if ( total > 0 )
    {
        centroid = centroid / float( total );
    }

    // Set the centroid as the translation of matrices
    this->initialMatrix.settranslation( centroid );
    this->groupMatrix = this->initialMatrix;

}
//------------------------------------------------------------------------------
/**
    End the operation
*/
void
InguiObjectTransformGroup::End()
{
    int n = (*objects).Size();
    for (int i=0; i < n;i++)
    {
        (*objects)[i].End();
    }
}
//------------------------------------------------------------------------------
/**
    Cancel the operation
*/
void
InguiObjectTransformGroup::Cancel()
{
    this->groupMatrix = this->initialMatrix;
    
    int n = (*objects).Size();
    for (int i=0;i < n; i++)
    {
        (*objects)[i].Cancel();
    }
}
//------------------------------------------------------------------------------
/**
    Set transform
*/
void
InguiObjectTransformGroup::SetTransform( transform44& t )
{
    #define mult_vec_comp( v, w ) vector3( v.x * w.x, v.y * w.y, v.z * w.z)

    matrix44 m( this->initialMatrix.getquatrotation() ); // = this->initialMatrix.getmatrix(); here cant use the matrix because it's scaled already
    vector3 p = this->initialMatrix.gettranslation();

    m.set_translation( p - this->refPoint );

    matrix44 rot;
    if ( t.iseulerrotation() )
    {
        vector3 angles = t.geteulerrotation();
        rot.rotate_x(angles.x);
        rot.rotate_y(angles.y);
        rot.rotate_z(angles.z);
    }
    else
    {
        rot.set( t.getquatrotation() );
    }

    rot.set_translation( t.gettranslation() );

    m = m * rot;

    vector3 s = mult_vec_comp( this->initialMatrix.getscale(), t.getscale());
    vector3 relPos = m.pos_component();
    relPos = mult_vec_comp( relPos, t.getscale());
    m.set_translation( relPos );

    m.translate( this->refPoint );

    groupMatrix.settranslation( m.pos_component() );
    groupMatrix.setquatrotation( m.get_quaternion() );
    groupMatrix.setscale( s );

    int n = (*objects).Size();
    for (int i=0;i < n; i++)
    {
        (*objects)[i].SetReferencePoint( this->initialMatrix.gettranslation() );
        (*objects)[i].SetTransform( t );
    }
}
//------------------------------------------------------------------------------
/**
    Get transform
*/
void
InguiObjectTransformGroup::GetTransform( transform44& t )
{
    t = this->groupMatrix;
}
//------------------------------------------------------------------------------
/**
    @brief Get selection centroid position

*/
void
InguiObjectTransformGroup::GetCenterPosition( vector3& v )
{
    v = this->groupMatrix.gettranslation();
}
//------------------------------------------------------------------------------
