#ifndef NC_GAMECAMERA_H
#define NC_GAMECAMERA_H

//-----------------------------------------------------------------------------
/**
    @class ncGameCamera
    @brief Game Camera component, it stores a camera information an behaviour.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Game Camera
    
    @cppclass ncGameCamera
    
    @superclass nComponentObject

    @classinfo Game Camera component, it stores a camera information an behaviour.
*/    

//-----------------------------------------------------------------------------

#include "entity/nentity.h"

#include "util/npfeedbackLoop.h"
#include "napplication/nappviewport.h"

//-----------------------------------------------------------------------------

class nAppViewport;
class nPhysicsGeom;

//-----------------------------------------------------------------------------

class ncGameCamera : public nComponentObject
{

    NCOMPONENT_DECLARE(ncGameCamera,nComponentObject);

public:
    typedef enum {
        free = 0,
        attach,
        thirdperson,
        rails,
        hooked,
        firstperson
    } type;

    typedef enum {
        objOrientation =        (1 << 0),
        collision =             (1 << 1),
        lookat =                (1 << 3),
        directed =              (1 << 4)
    } attrib;

    typedef enum {
        none = 0,
        inmediatly,
        increment,
        dampen
    } transition;

    /// contructor
    ncGameCamera();

    /// destructor
    ~ncGameCamera();

    /// enables camera
    void Enable();

    /// disables camera
    void Disable();

    /// returns if the camera is enabled
    const bool IsEnabled() const;

    /// sets the camera type
    void SetCameraType(const type);

    /// returns the camera type
    const type GetCameraType() const;

    /// sets attributes to the camera
    void SetAttributes(const int);

    /// gets attributes to the camera
    const int GetAttributes() const;

    /// adds a set of attributes
    void AddAttributes(const int);

    /// removes a set of attributes
    void RemoveAttributes(const int);

    /// sets the anchor point
    void SetAnchorPoint(nEntityObject*);

    /// returns the anchor point
    nEntityObject* GetAnchorPoint() const;

    /// set camera offset
    void SetCameraOffset(const vector3&);

    /// get camera offset
    const vector3& GetCameraOffset() const;

    /// sets the height respect the anchor point
    void SetHeight(const float);

    /// gets the height respect the anchor point
    const float GetHeight() const;

    /// sets the distance respect the anchor point
    void SetDistance(const float);

    /// gets the distance respect the anchor point
    const float GetDistance() const;

    /// sets the route to follow (rail camera)
    void SetRoute(const int);

    /// gets the route to follow (rail camera)
    const int GetRoute() const;

    /// updates camera
    void Update(const nTime&);

    /// sets the viewport
    void SetViewPort(nAppViewport*);

    /// gets the viewport
    nAppViewport* GetViewPort() const;

    /// sets where to look at
    void LookAt(nEntityObject*);

    /// once setup build data for the camera
    void Build();

    /// sets the step pace for a route
    void SetStep(const float);

    /// returns the step pace for a route
    const float GetStep() const;

    /// sets the transposition type for position in X
    void SetTranspositionXType(const transition);

    /// gets the transposition type for position in X
    const transition GetTranspositionXType() const;

    /// sets the transposition type for position in Y
    void SetTranspositionYType(const transition);

    /// gets the transposition type for position in Y
    const transition GetTranspositionYType() const;

    /// sets the transposition type for position in Z
    void SetTranspositionZType(const transition);

    /// gets the transposition type for position in Z
    const transition GetTranspositionZType() const;

    /// sets the dampening for the different axis (0..1) in position
    void SetDampeningPosition(const vector3&);

    /// gets the dampening for the different axis (0..1) in position
    const vector3& GetDampeningPosition() const;

    /// sets the dampening for the orientation
    void SetDampeningOrientation(const float);

    /// gets the dampening for the orientation
    const float GetDampeningOrientation() const;

    /// sets the max distance of the camera
    void SetMaxDistance(const float);

    /// gets the max distance of the camera
    const float GetMaxDistance() const;

    /// sets the min distance of the camera
    void SetMinDistance(const float);

    /// gets the min distance of the camera
    const float GetMinDistance() const;

    /// sets the orientation transition type
    void SetTranspositionOrientationType(const transition);

    /// gets the orientation transition type
    const transition GetTranspositionOrientationType() const;

    /// sets the lookat offset in y axis
    void SetLookAtOffset(const float);

    /// gets the lookat offset in y axis
    const float GetLookAtOffset() const;

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

private:
    
    /// stores if the camera is enabled
    bool enabled;

    /// stores the type of camera
    type cameraType;

    /// stores attribs
    int cameraAttributes;

    /// anchor point if any
    ncTransform* anchor;

    /// anchor entity
    nEntityObject* anchorEntity;

    /// get internal anchor point
    ncTransform* GetInternalAnchor();

    /// updates camera position
    void UpdatePosition();

    /// updates orientation
    void UpdateOrientation();

    /// updates transition
    void UpdateTransition();

    /// viewport
    nAppViewport* viewport;

    /// stores camera speed (game units per second)
    float cameraSpeed;

    /// updates the position for an attached camera
    void UpdatePositionAttached();

    /// stores the camera offset
    vector3 cameraOffset;

    /// stores the transform for look at
    ncTransform* lookAt;

    /// updates lookat info
    void UpdateLookAt();

    /// stores the position where should be
    vector3 futurePosition;

    /// stores the orientation where should be
    vector3 futureOrientation;

    /// stores how to do transition in each axis position
    transition transPositionXType;
    transition transPositionYType;
    transition transPositionZType;

    /// stores how to do transition in each axis orientation
    transition transOrientationType;

    /// max distance
    float maxDistance;

    /// min distance
    float minDistance;

    /// compute inmediatly transition
    void ComputeInmediatlyTransition( float& value, const float sholdbe );

    /// updates the object orientation
    void UpdateObjectOrientation();

    /// hooked entity
    nEntityObject* hookedEntity;

    /// stores dampening for position
    vector3 dampening;

    /// stores dampening for Orientation
    float dampeningOrientation;

    /// compute dampened transition
    void ComputeDampenTransition( float& value, const float shouldbe, const float vDampen );

    /// updates position base in a route
    void UpdatePositionByRoute();

    /// stores the frame proportion for a given frame
    float frameProportion;

    /// computes the position for a third person camera
    void ComputePositionThirdPerson();

    /// checks for max and min distance
    void CheckMaxMinDistance();

    /// backup position
    vector3 backupPosition;

    /// collision object
    nPhysicsGeom* collideGeometry;

    /// checks collision
    void CheckCollision();

    /// stores route id
    int routeid;

    /// stores the step pace for routes
    float routeStep;

    /// stores the last position of the lookat
    vector3 lookAtPrevPosition;

    /// stores the offset to the lookat if any
    float offsetLookAt; // in y axis only

};

//-----------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(ncGameCamera::type, "i", (value = (ncGameCamera::type) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );
N_CMDARGTYPE_NEW_TYPE(ncGameCamera::transition, "i", (value = (ncGameCamera::transition) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );
//-----------------------------------------------------------------------------

#endif