#include "entity_test/entity01.h"

nNebulaUsePackage(nkernel);
nNebulaUsePackage(nentity);
nNebulaUsePackage(entity01);

int main(int argc, char *argv[])
{
    nKernelServer* kernelServer = new nKernelServer();

    kernelServer->AddPackage(nkernel);
    kernelServer->AddPackage(nentity);
    kernelServer->AddPackage(entity01);

    //delete kernelServer;
    return 0;
}
