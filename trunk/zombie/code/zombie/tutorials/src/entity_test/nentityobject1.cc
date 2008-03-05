#include "entity_test/entity01.h"

nNebulaEntityObject(nEntityObject1,"nentityobject","nentityclass1");

/*
static void
n_print(void* slf, nCmd* cmd)
{
    nEntityObject1* self = (nEntityObject1*) slf;
    self->nComponentObjectA::print();
    self->nComponentObjectB::print();
    self->nComponentObjectC::print();
}

static void
n_printa(void* slf, nCmd* cmd)
{
    nComponentObjectA* self = (nEntityObject1*) slf;
    self->printa();
}

static void
n_printb(void* slf, nCmd* cmd)
{
    nComponentObjectB* self = (nEntityObject1*) slf;
    self->printb();
}

static void
n_printc(void* slf, nCmd* cmd)
{
    nComponentObjectC* self = (nEntityObject1*) slf;
    self->printc();
}

void n_initcmds_nEntityObject1(nClass * cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_print_v", 'PRIN', n_print);
    cl->AddCmd("v_printa_v", 'PRIA', n_printa);
    cl->AddCmd("v_printb_v", 'PRIB', n_printb);
    cl->AddCmd("v_printc_v", 'PRIC', n_printc);
    cl->EndCmds();
}
*/