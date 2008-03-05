#ifndef N_ENTITYTYPES_H
#define N_ENTITYTYPES_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntitySystem

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/natom.h"

//------------------------------------------------------------------------------
typedef unsigned int nEntityObjectId;
typedef nAtom nComponentId;

//------------------------------------------------------------------------------
extern nComponentId compIdInvalid;

N_CMDARGTYPE_NEW_TYPE(nComponentId, "s",
                      (value = nComponentId( (char *) cmd->In()->GetS()) ),
                      (cmd->Out()->SetS( value.AsChar() ))
                      );

//------------------------------------------------------------------------------
#endif // N_ENTITYTYPES_H
