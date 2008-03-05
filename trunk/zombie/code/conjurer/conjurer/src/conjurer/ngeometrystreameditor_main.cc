#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ngeometrystreameditor_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ngeometrystreameditor.h"
#include "nscene/ngeometrynode.h"
#include "zombieentity/ncassetclass.h"

nNebulaScriptClass(nGeometryStreamEditor, "nroot");

//------------------------------------------------------------------------------
/**
*/
nGeometryStreamEditor::nGeometryStreamEditor() :
    isApplied(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGeometryStreamEditor::~nGeometryStreamEditor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::BeginGeometries(int num)
{
    this->geometryArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::SetGeometryAt(int index, nString& className, int level, nString& path)
{
    GeometryEntry& geometry = this->geometryArray[index];
    geometry.className = className;
    geometry.level = level;
    geometry.materialName = path;
    geometry.refGeometryNode.invalidate();

    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::GetGeometryAt(int index, nString& className, int& level, nString& path)
{
    GeometryEntry& geometry = this->geometryArray[index];
    className = geometry.className;
    level = geometry.level;
    path = geometry.materialName;

    // mark as dirty to identify that it has been opened for edition
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::SetFrequencyAt(int index, int frequency)
{
    this->geometryArray[index].frequency = frequency;

    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
int
nGeometryStreamEditor::GetFrequencyAt(int index) const
{
    return this->geometryArray[index].frequency;
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::EndGeometries()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    build a memory version of the stream using both batch stream nodes and
    in-memory texture atlas, to try the effect in the geometry without saving
    anything to disk.
*/
void
nGeometryStreamEditor::ApplyStream()
{
    if (this->LoadSceneResources())
    {
        if (this->BuildCommonMaterial() && this->BuildCommonSurface())
        {
            this->isApplied = this->BuildStreamGeometry();
        }
        else
        {
            //if something went wrong, just revert to the previous streams, if any
            this->RemoveStream();
        }
    }
}

//------------------------------------------------------------------------------
/**
    remove the current stream from any geometry to which we had set them
    originally. this is to revert any previously built stream node to its
    original form.
*/
void
nGeometryStreamEditor::RemoveStream()
{
    for (int index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];
        if (geometry.isPrevStreamValid && geometry.refGeometryNode.isvalid())
        {
            geometry.refGeometryNode->SetStream(geometry.prevStream.Get());
            geometry.refGeometryNode->SetStreamIndex(geometry.prevStreamIndex);
            geometry.isPrevStreamValid = false;
        }
        //else
        //completely remove the stream from the geometries
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryStreamEditor::DeleteStream()
{
    this->isDeleted = true;

    for (int index = 0; index < this->geometryArray.Size(); ++index)
    {
        GeometryEntry& geometry = this->geometryArray[index];

        geometry.refEntityClass = nEntityClassServer::Instance()->GetEntityClass(geometry.className.Get());
        if (geometry.refEntityClass.isvalid())
        {
            // remove stream dependency from target class
            ncAssetClass *assetClass = geometry.refEntityClass->GetComponentSafe<ncAssetClass>();
            assetClass->ClearSceneDependencies();
            //assetClass->Invalidate();
            nEntityClassServer::Instance()->SetEntityClassDirty(geometry.refEntityClass, true);

            // delete stream editor from file
            nString assetPath;
            assetPath.Format("wc:export/assets/%s", this->GetName());
            if (kernelServer->GetFileServer()->DirectoryExists(assetPath.Get()))
            {
                kernelServer->GetFileServer()->DeleteDirectory(assetPath.Get());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    this is called by the ngeometrystreameditorstate when the user wants to
    save all modified streams in optimized form. This implies:
    - building streamed geometry using a mesh builder
    - building atlas textures using a texture builder
    - saving the streams as dependencies in the classes of the geometries
*/
void
nGeometryStreamEditor::BuildStream()
{
    /** @todo rebuild the scene resource with the correct mesh and textures:
        build stream geometry as a mesh, instead of a batch node
        build atlas texture(s) */
    // check if everything went right
    this->ApplyStream();

    // check if everything went right
    if (this->isApplied)
    {
        //save stream and resources to disk, set class dependencies and save.
        nString assetPath;
        assetPath.Format("wc:export/assets/%s/scene/%s.n2", this->GetName(), this->GetName());
        kernelServer->GetFileServer()->MakePath(assetPath.ExtractDirName());
        if (this->refStreamGeometry->SaveAs(assetPath.Get()))
        {
            for (int index = 0; index < this->geometryArray.Size(); ++index)
            {
                GeometryEntry& geometry = this->geometryArray[index];
                ncAssetClass *assetClass = geometry.refEntityClass->GetComponentSafe<ncAssetClass>();

                /// @todo ma.garcias- find a cleaner way to set these
                assetClass->SetSceneDependency(geometry.level, geometry.refGeometryNode.getname(),
                    assetPath.Get(), "setstream", "/sys/servers/dependency/basic");
                assetClass->SetSceneIntOverride(geometry.level, geometry.refGeometryNode.getname(),
                    "setstreamindex", geometry.refGeometryNode->GetStreamIndex());

                // trigger resource reloading and set class as dirty
                assetClass->Invalidate();
                nEntityClassServer::Instance()->SetEntityClassDirty(geometry.refEntityClass, true);
            }
        }

        this->isDirty = false;
    }
}
