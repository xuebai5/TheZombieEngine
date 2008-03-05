// entity type 1
#include "entity_test/ncomponenta.h"
#include "entity_test/ncomponentb.h"

class nEntityObject1 : 
    public nEntityObject, 
    public nComponentObjectA, 
    public nComponentObjectB
{
public:

    nEntityObject1()
    {
        /// empty
    }

    bool SaveCmds(nPersistServer * ps)
    {
        nComponentObjectA::SaveCmds(ps);
        nComponentObjectB::SaveCmds(ps);
        return true;
    }

    static void InitClass(nClass * clazz)
    {
        nComponentObjectA::InitClass<nEntityObject1>(clazz);
        nComponentObjectB::InitClass<nEntityObject1>(clazz);
    }

};
