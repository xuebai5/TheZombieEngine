#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  nresourcepool_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "resource/nresourcepool.h"

//------------------------------------------------------------------------------
/**
*/
nResourcePool::nResourcePool(const char* rsrcPath) :
    uniqueId(0),
    rsrcNameMap(32)
{
    this->refPool = nKernelServer::Instance()->New("nroot", rsrcPath);
}

//------------------------------------------------------------------------------
/**
*/
nResourcePool::~nResourcePool()
{
    this->refPool->Release();
}

//------------------------------------------------------------------------------
/**
*/
nResource*
nResourcePool::GetHead()
{
    return (nResource*) this->refPool->GetHead();
}

//------------------------------------------------------------------------------
/**
*/
nResource*
nResourcePool::NewResource(const char* className, const char* rsrcName)
{
    nKernelServer *kernelServer = nKernelServer::Instance();

    ResourceNode* rsrcNode = rsrcName ? this->rsrcNameMap[rsrcName] : 0;
    if (rsrcNode)
    {
        if (rsrcNode->refResource.isvalid())
        {
            rsrcNode->refResource->AddRef();
            return rsrcNode->refResource.get();
        }
    }

    // mangled resource path
    //nFileServer2 *fileServer = nKernelServer::Instance()->GetFileServer();
    //nString mangledName(fileServer->ManglePath(rsrcName));

    // create a new resource object
    char rsrcId[N_MAXNAMELEN];
    this->NewResourceId(rsrcName, rsrcId, sizeof(rsrcId));

    kernelServer->PushCwd(this->refPool.get());
    nResource* obj = (nResource*) kernelServer->New(className, rsrcId);
    kernelServer->PopCwd();
    n_assert(obj);

    // insert new resource or reuse slot if it was empty
    if (rsrcNode)
    {
        rsrcNode->refResource = obj;
    }
    else
    {
        ResourceNode resNode;
        resNode.refResource = obj;
        //this->rsrcNameMap.Add(mangledName, &resNode);
        this->rsrcNameMap.Add(rsrcName, &resNode);
    }

    return obj;
}

//------------------------------------------------------------------------------
/**
    Find resource by name.
    just in case the rsrcname is a file path containing file assigns,
    it is first mangled just as resources are referenced in the table.
*/
nResource*
nResourcePool::Find(const char *rsrcName)
{
    //nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    //nString mangledName(fileServer->ManglePath(rsrcName));
    //ResourceNode* rsrcNode = this->rsrcNameMap[mangledName];
    ResourceNode* rsrcNode = this->rsrcNameMap[rsrcName];
    if (rsrcNode)
    {
        return rsrcNode->refResource.isvalid() ? rsrcNode->refResource.get() : 0;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
nResourcePool::GetNumResources()
{
    int num = 0;
    this->rsrcNameMap.Begin();
    ResourceNode* rsrcNode = this->rsrcNameMap.Next();
    while (rsrcNode)
    {
        if (rsrcNode->refResource.isvalid())
        {
            ++num;
        }
        rsrcNode = this->rsrcNameMap.Next();
    }
    return num;
}

//------------------------------------------------------------------------------
/**
    Create a resource id from a resource name. The resource name is usually
    just the filename of the resource file. The method strips off the last
    32 characters from the resource name, and replaces any invalid characters
    with underscores. It is valid to provide a 0-rsrcName for unshared resources.
    A unique rsrc identifier string will then be created.

    To avoid clashing of resource ids for long resource names, ids generated
    are checked under the pool's root node. In case they already exist, a
    sequence number is added to the mangled list name, eg.:
    "c:/textures1/very/long/path/to_texture_file.dds"
    "c:/textures2/very/long/path/to_texture_file.dds"

    would be added into the textures pool as:
    "ry_long_path_to_texture_file_dds"
    "ry_long_path_to_texture_file_001"

    thus removing possible name clashing for the generated node names.

    @param  rsrcName    pointer to a resource name (usually a file path), or 0
    @param  buf         pointer to a char buffer
    @param  bufSize     size of char buffer
    @return             a pointer to buf, which contains the result
*/
char*
nResourcePool::NewResourceId(const char* rsrcName, char* buf, int N_IFDEF_ASSERTS(bufSize) )
{
    n_assert(buf);
    n_assert(bufSize >= N_MAXNAMELEN);

    if (!rsrcName)
    {
        sprintf(buf, "unique%d", this->uniqueId++);
    }
    else
    {
        int len = static_cast<int>( strlen(rsrcName) + 1 );
        int offset = len - N_MAXNAMELEN;
        if (offset < 0)
        {
            offset = 0;
        }

        // copy string and replace illegal characters, this also copies the terminating 0
        char c;
        const char* from = &(rsrcName[offset]);
        char* to   = buf;
        while ( 0 != (c = *from++))
        {
            if (('.' == c) || (c == '/') || (c == ':') || (c == '\\'))
            {
                *to++ = '_';
            }
            else
            {
                *to++ = c;
            }
        }
        *to = 0;
    }

    // add sequence number in case of name clash
    int counter = 0;
    while (this->refPool->Find(buf))
    {
        ++counter;
        int len = static_cast<int>(strlen(buf));
        int offset = len - 3;
        const char* from = &(buf[offset]);
        sprintf((char*)from, "%03d", counter);
    }

    return buf;
}
