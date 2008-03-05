// entity type 1
#include "entity_test/ncomponenta.h"
#include "entity_test/ncomponentb.h"

class nEntityClass1 : 
    public nEntityClass, 
    public nComponentClassA, 
    public nComponentClassB
{
public:

    nEntityClass1();

    bool SaveCmds(nPersistServer * ps)
    {
        nComponentClassA::SaveCmds(ps);
        nComponentClassB::SaveCmds(ps);
        return true;
    }

    static void InitClass(nClass * clazz)
    {
        nComponentClassA::InitClass<nEntityClass1>(clazz);
        nComponentClassB::InitClass<nEntityClass1>(clazz);
    }

};
