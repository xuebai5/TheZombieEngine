//------------------------------------------------------------------------------
#include "precompiled/pchconjurerapp.h"
#include "conjurer/nstatelist.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaClass(nStateList, "nscriptserver");

//------------------------------------------------------------------------------
nStateList::nStateList() :
    numNestedObjects(0)
{
    //empty
}

//------------------------------------------------------------------------------
nStateList::~nStateList()
{
    this->stateList.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
nStateList::BeginWrite(nFile * /* file */, nObject* /* obj */, nCmd * /* cmd */)
{
    // super cutre ugly hack (made by javi.ferrer): not needed to open file, 
    // just return 1 to lie to nPersistServer

    // idea: change the interface of persist server and script server
    // persist server does not need to know the nFile *, just if the open was ok or not
    this->numNestedObjects = 0;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nStateList::WriteBeginNewObject(nFile * /*file*/, nObject * /*obj*/, nObject * /*owner*/)
{
    this->numNestedObjects++;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nStateList::WriteBeginNewObjectCmd(nFile * /*file*/, nObject * /*o*/, nObject * /*owner*/, nCmd * /*cmd*/)
{
    this->numNestedObjects++;
    return true;
}

//------------------------------------------------------------------------------
/** @brief
    This method is called by the persist server for get the list of commands
    needed for persist an object
*/
bool 
nStateList::WriteCmd(nFile * /*file*/, nCmd* cmd)
{
    n_assert(cmd);

    if (this->numNestedObjects == 0)
    {
        this->stateList.Append(n_new(nCmd(*cmd)));
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nStateList::WriteEndObject(nFile* /*file*/, nObject * /*o*/, nObject * /*owner*/)
{
    this->numNestedObjects--;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nStateList::EndWrite(nFile* /* file */)
{
    n_assert(this->numNestedObjects == 0);
    // overwrite in subclass
    return true;
}
