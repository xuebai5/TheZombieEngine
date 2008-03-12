#ifndef N_3DSNEBULARUNTIME_H
#define N_3DSNEBULARUNTIME_H

class nString;
class nKernelServer;
class nScriptServer;
class nResourceServer;
class nSceneServer;
class nVariableServer;
class nWin32LogHandler;
class nPhysicsServer;
class nGameMaterialServer;
class nMaterialServer;
class nAtomTable;
class nComponentClassServer;
class nComponentObjectServer;
class nFileServer2;
class nScriptServer;

//------------------------------------------------------------------------------
/**
    @class n3dsNebulaRuntime
    @ingroup n3dsMaxExporterKernel
    @brief Export server, 
    Interface with nebula
*/

class n3dsNebulaRuntime
{
public:
    /// constructor
    n3dsNebulaRuntime();
    /// destructor
    virtual ~n3dsNebulaRuntime();
    /// open the Nebula runtime
   // virtual bool Open(const char* projectPath);
    /// open the NebulaRuntime()
    virtual bool Open(const nString &projectPath);
    /// close the Nebula runtime
    virtual void Close();
    /// get a pointer to the Nebula kernel server
    nKernelServer* GetKernelServer() const;
    /// get a pointer to the file server
    nFileServer2* GetFileServer() const;
    /// get a pointer to the script server
    nScriptServer* GetScriptServer() const;
    /// get a pointer to the variable server
    nVariableServer* GetVariableServer() const;
    /// connect to a remote Nebula application
    static void WhenDetachProcess();

private:
    bool isOpen;
    nWin32LogHandler* logHandler;
    nKernelServer* kernelServer;
    // this atomtable is the same for the process, for each export
    static nAtomTable * atomTable;
    nComponentClassServer * compClassServer;
    nComponentObjectServer * compObjectServer;
    nScriptServer* scriptServer;
    nVariableServer* varServer;
    nResourceServer* resourceServer;
    nSceneServer* sceneServer;
    nPhysicsServer* physicsServer;
    nGameMaterialServer* gamematerialServer;
    nMaterialServer* materialServer;

};


//------------------------------------------------------------------------------
#endif