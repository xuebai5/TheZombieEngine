#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nconjurerdebugcompserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nconjurerdebugcompserver.h"
#include "ndebug/nceditor.h"
#include "mathlib/cone.h"
#include "zombieentity/nctransform.h"
#include "gfx2/nshapeserver.h"

#include "ntrigger/nctriggershape.h"
#include "ntrigger/nctriggeroutput.h"
#include "ntrigger/ncmdoperation.h"
#include "ntrigger/ncareatrigger.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "ncsound/ncsound.h"

nNebulaClass(nConjurerDebugCompServer, "ndebugcomponentserver");

//------------------------------------------------------------------------------
/**
*/
nConjurerDebugCompServer::nConjurerDebugCompServer() :
    appViewport(0)
{
    this->extrudedLineDrawer = n_new(nLineDrawer);
    this->extrudedLineDrawer->SetShaderPath("shaders:defaulttool.fx");
}
 
//------------------------------------------------------------------------------
/**
*/
nConjurerDebugCompServer::~nConjurerDebugCompServer()
{
    if (this->extrudedLineDrawer)
    {
        n_delete(this->extrudedLineDrawer);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerDebugCompServer::Add(ncEditor * const object)
{
    nDebugComponentServer::Add(object);

    n_assert(object);

    if (object->GetComponent<ncSound>())
    {
        this->soundSourcesArray.Append(object->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerDebugCompServer::Remove(ncEditor * const object)
{
    nDebugComponentServer::Remove(object);

    n_assert(object);

    if (object->GetComponent<ncSound>())
    {
        this->RemoveFromArray(this->soundSourcesArray, object->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerDebugCompServer::RemoveFromArray(nArray<nRef<nEntityObject> >& array, nEntityObject* entity)
{
    nArray<nRef<nEntityObject> >::iterator arrayIter = array.Find(entity);
    if (arrayIter)
    {
        array.Erase(arrayIter);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nConjurerDebugCompServer::IsValid(const nRef<nEntityObject> *refEntityObject)
{
    static nAutoRef<nLayerManager> refLayerManager( "/sys/servers/layermanager" );
    n_assert(refLayerManager.isvalid());

    if (refEntityObject->isvalid())
    {
        ncEditor* editor = refEntityObject->get()->GetComponent<ncEditor>();
        if (editor && !editor->IsDeleted() )
        {
            nLayer* layer = refLayerManager->SearchLayer( editor->GetLayerId() );
            if (!layer || layer->IsActive())
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Draw AI intended motion
*/
void
nConjurerDebugCompServer::DrawSoundSources(bool drawRadius)
{
    this->DrawSoundSources(this->soundSourcesArray);

    this->DrawTriggerShapes(this->soundSourcesArray, vector4(0.8f, 0.4f, 0.0f, 1.0f), drawRadius);
}

//------------------------------------------------------------------------------
/**
    Draw trigger shape
*/
void 
nConjurerDebugCompServer::DrawTriggerShapes(nRefEntityArray& array, vector4 color, bool onlySelection)
{
    nObjectEditorState * objectState = static_cast<nObjectEditorState*>( nConjurerApp::Instance()->FindState("object") );
    n_assert( objectState );
    nArray<nRefEntityObject>& selection = objectState->GetSelection();

    nArray<nRef<nEntityObject> >::iterator entityIter;
    for (entityIter = array.Begin(); entityIter != array.End(); ++entityIter)
    {
        if (this->IsValid(entityIter))
        {
            nEntityObject* entity = entityIter->get();

            if ( onlySelection && selection.FindIndex( entity ) == -1 )
            {
                continue;
            }

            this->DrawTriggerShape( entity, color );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw a trigger shape with default color
*/
void 
nConjurerDebugCompServer::DrawTriggerShape( nEntityObject* entity )
{
    this->DrawTriggerShape( entity, vector4(0.0f, 0.4f, 0.8f, 1.0f) );
}

//------------------------------------------------------------------------------
/**
    Draw a trigger shape
*/
void 
nConjurerDebugCompServer::DrawTriggerShape( nEntityObject* entity, vector4 color )
{
    ncTriggerShape *shape = entity->GetComponent<ncTriggerShape>();
    if (shape)
    {
        // Draw trigger shape
        float height(shape->GetHeight());
        float heightPos( height / 2.0f + shape->GetHeightOffset() );

        nString shapeType(shape->GetShapeType());
        if (shapeType == "circle")
        {
            // Draw circle
            float radius(shape->GetCircle());

            matrix44 m;
            m.translate( entity->GetComponentSafe<ncTransform>()->GetPosition() );

            matrix44 m2;
            m2.ident();
            m2.rotate_x(PI / 2.0f);
            m2.scale(vector3(radius, height, radius));
            m2.translate(vector3(0, heightPos, 0));
            m2 *= m;
            nGfxServer2::Instance()->BeginShapes();
            color.w = 0.3f;
            nGfxServer2::Instance()->DrawShape(nGfxServer2::Cylinder, m2, color);

            // If it's a sound trigger, draw the fade distance
            if (entity->GetComponent<ncSound>() && n_abs( shape->GetFadeDistance() ) <= 1e-4f )
            {
                radius -= shape->GetFadeDistance();
                m2.ident();
                m2.rotate_x(PI / 2.0f);
                m2.scale( vector3(radius, ( height - shape->GetVerticalFadeDistance() ) * 1.0f + 0.05f, radius) );
                m2.translate( vector3(0, heightPos, 0) );
                m2 *= m;
                color.w = 0.5f;
                nGfxServer2::Instance()->DrawShape(nGfxServer2::Cylinder, m2, color);
            }
            nGfxServer2::Instance()->EndShapes();
        }
        else if (shapeType == "polygon")
        {
            // Draw polygon
            const polygon& pol = shape->GetPolygon();

            static nArray<vector3> vertices(100, 50);

            for (int i(0); i < pol.GetNumVertices(); i++)
            {
                vertices.At(i) = pol.GetVertex(i);
            }
            vertices.At( pol.GetNumVertices() ) = vertices[0];

            if (n_abs(height) <= 1e-4f)
            {
                // height = 0 -> draw just the outline
                matrix44 m;
                m.translate( entity->GetComponentSafe<ncTransform>()->GetPosition() );

                nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, m);
                nGfxServer2::Instance()->BeginLines();
                color.w = 1.0f;
                nGfxServer2::Instance()->DrawLines3d( &vertices[0], pol.GetNumVertices() + 1, color);
                nGfxServer2::Instance()->EndLines();
                nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);
            }
            else
            {
                vector3 pos = entity->GetComponentSafe<ncTransform>()->GetPosition() + vector3( 0.0f, shape->GetHeightOffset(), 0.0f );

                // height > 0 -> draw an extruded line
                for (int i(0); i < pol.GetNumVertices(); ++i)
                {
                    vertices[i] += pos;
                }
                color.w = 0.3f;
                this->extrudedLineDrawer->DrawExtruded3DLine( &vertices[0], pol.GetNumVertices(), &color, 1, vector3(0, height, 0), true);
            }
        }

        // show lines from area trigger to entities found in its output
        ncTriggerOutput* output = entity->GetComponent<ncTriggerOutput>();
        if (output && entity->GetComponent<ncAreaTrigger>())
        {
            const char* outputSetId[2] = {
                nGameEvent::GetEventPersistentId(nGameEvent::ENTER_IN_AREA),
                nGameEvent::GetEventPersistentId(nGameEvent::EXIT_FROM_AREA) };
            for (int j(0); j < 2; ++j)
            {
                for (int i(0); i < output->GetOperationsNumber(outputSetId[j]); ++i)
                {
                    // Skip entities without position
                    ncTransform* targetPosComp(NULL);
                    nOperation* operation = output->GetOperation(outputSetId[j], i);
                    switch (operation->GetType())
                    {
                        case nOperation::COMMAND:
                            {
                                nObject* object = static_cast<nCmdOperation*>(operation)->GetTargetObject();
                                if (object && object->IsA("nentityobject"))
                                {
                                    targetPosComp = static_cast<nEntityObject*>(object)->GetComponent<ncTransform>();
                                }
                            }
                            break;
                    }
                    if (!targetPosComp)
                    {
                        continue;
                    }

                    // Draw line from area trigger position to target entity position
                    matrix44 m;
                    m.ident();
                    vector3 v[2] = {
                        entity->GetComponentSafe<ncTransform>()->GetPosition(),
                        targetPosComp->GetPosition() };
                    color.w = 1.0f;
                    nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, m);
                    nGfxServer2::Instance()->BeginLines();
                    nGfxServer2::Instance()->DrawLines3d(v, 2, color);
                    nGfxServer2::Instance()->EndLines();
                    nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw AI intended motion
*/
void
nConjurerDebugCompServer::DrawSoundSources(nRefEntityArray& array)
{
    nArray<nRef<nEntityObject> >::iterator entityIter;
    for (entityIter = array.Begin(); entityIter != array.End(); ++entityIter)
    {
        if (this->IsValid(entityIter))
        {
            nEntityObject* entity = entityIter->get();

            ncTransform* transform = entity->GetComponentSafe<ncTransform>();

            static float radius(.1f);
            matrix44 model;
            model.scale(vector3(radius, radius, radius));
            model.set_translation(transform->GetPosition());
            nGfxServer2::Instance()->BeginShapes();
            nGfxServer2::Instance()->DrawShape(nGfxServer2::Sphere, model, vector4(1.0f, 0.0f, 0.0f, 1.0f));
            nGfxServer2::Instance()->EndShapes();
        }
    }
}
