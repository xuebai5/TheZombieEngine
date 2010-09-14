#include "shootem/shootemapp.h"
#include "shootem/model.h"
#include "shootem/textfile.h"

#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"

char* modelnames[NumModels] = {
    "wc:export/assets/aNsforces_01_lod2",
    "wc:export/assets/aScv_lod2",
    "wc:export/assets/aAlamo_lod2"
};

//------------------------------------------------------------------------------

bool ShootemApp::LoadModels()
{
    for (int index=0; index<NumModels; index++)
    {
        Model* model = n_new(Model);
        if (!this->LoadModel(model, modelnames[index]))
        {
            n_delete(model);
            return false;
        }
        this->models.Append(model);
    }
    return true;
}

//------------------------------------------------------------------------------
void ShootemApp::ReleaseModels()
{
    while (!this->models.Empty())
    {
        Model** model = this->models.Begin();
        if (*model != 0)
        {
            this->ReleaseModel(*model);
            n_delete(*model);
            this->models.EraseQuick(0);
        }
    }
}

//------------------------------------------------------------------------------

bool ShootemApp::LoadModel(Model* model, const char* path)
{
    //read the scene file
    nString sceneFile(path);
    sceneFile.StripTrailingSlash();
    nString filename = sceneFile.ExtractFileName();
    sceneFile.Append("/scene/");
    sceneFile.Append(filename.Get());
    sceneFile.Append(".n2");

    return LoadScene(model, sceneFile.Get());
}

//------------------------------------------------------------------------------

bool ShootemApp::LoadScene(Model* model, const char* path)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    int numShapes = 0;

    TextFile text(path);
    while (!text.Eof())
    {
        text.ReadLine();
        if (text.Contains("nshapenode"))
            numShapes++;

        if (text.Contains("setmesh"))
        {
            nString filename;
            text.Extract("[[", "]]", filename);
            nMesh2* mesh = gfxServer->NewMesh(filename.Get());
            if (!this->LoadResource(mesh, filename))
                return false;

            model->shapes.At(numShapes-1).refMesh = mesh;
        }

        if (text.Contains("setsurface"))
        {
            nString filename;
            if (text.Extract("[[", "]]", filename))
                this->LoadMaterial(&model->shapes.At(numShapes-1), filename.Get());
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool ShootemApp::LoadMaterial(Shape* shape, const char* path)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    //use a default shader
    shape->refShader = this->refShaderDiffuse;
    shape->shaderParams.SetArg(nShaderState::MatDiffuse, vector4(1.f,1.f,1.f,1.f));

    nShaderState::Param param(nShaderState::InvalidParameter);
    nTexture2* texture;

    TextFile text(path);
    while (!text.Eof())
    {
        text.ReadLine();
        nString filename;

        //texture maps
        if (text.Contains("diffMap"))
            param=nShaderState::DiffMap0;
        //else if (text.Contains("bumpMap"))
        //    param=nShaderState::BumpMap0;
        //else if (text.Contains("levelMap"))
        //    param=nShaderState::SpecMap0;
        else
            continue;

        text.Extract("[[", "]]", filename);//skip the parameter
        text.Extract("[[", "]]", filename);
        texture = gfxServer->NewTexture(filename.Get());
        if (!this->LoadResource(texture, filename))
            return false;

        shape->shaderParams.SetArg(param, texture);
        //other parameters...
        //shape->shaderParams[param].SetTexture(texture);
    }
    return true;
}

//------------------------------------------------------------------------------

void ShootemApp::ReleaseModel(Model* model)
{
    int numShapes = model->shapes.Size();
    for (int index=0; index<numShapes; index++)
        N_REF_RELEASE(model->shapes[index].refMesh);

    int numTextures = model->textures.Size();
    for (int index=0; index<numTextures; index++)
        N_REF_RELEASE(model->textures[index]);
}

//------------------------------------------------------------------------------

void ShootemApp::DrawModel(Model* model, matrix44& matWorld)
{
    //draw model
    int numShapes = model->shapes.Size();
    numShapes--;//HACK- to remove the shadow mesh
    for (int index=0; index<numShapes; index++)
    {
        Shape& shape = model->shapes[index];
        shape.shaderParams.SetArg(nShaderState::FillMode, this->bWireframe ? nShaderState::Wireframe : nShaderState::Solid);

        nShader2* shader = shape.refShader;
        nMesh2* mesh = shape.refMesh;
        int numPasses = this->BeginDraw( shader, mesh );
        for (int passIndex = 0; passIndex < numPasses; passIndex++)
        {
            this->BeginPass( shader, passIndex );
            shader->SetParams( shape.shaderParams );
            shader->SetParams( this->shaderParams );//allow overriding parameters

            this->Draw( matWorld );
            this->EndPass( shader );
        }
    }
}
