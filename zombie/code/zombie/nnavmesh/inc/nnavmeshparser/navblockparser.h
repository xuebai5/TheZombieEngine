#ifndef N_NAVBLOCKPARSER_H
#define N_NAVBLOCKPARSER_H

//------------------------------------------------------------------------------
/**
    @class NavBlockParser
    @ingroup NebulaNavmeshSystem

    Base class to parse a navigation block.
    
    Although the name parser usually refers to loaders, savers are also
    implemented from this base class. In fact, it's good that loaders and
    savers share the same base class since the overall sintax parsing is
    delegated to a single class, avoiding most potencial errors, specially
    when mantaining both loaders and savers.

    (C) 2005 Conjurer Services, S.A.
*/

#include "util/narray.h"

class NavTag;
class NavParserFactory;
struct nNavMeshFile;
class nNavMesh;

//------------------------------------------------------------------------------
/**
    Iterface to call a method that parses a particular block version
*/
struct IParseMethod
{
    /// Call to stored method
    virtual bool operator()() const = 0;
};

//------------------------------------------------------------------------------
/**
    Type used to store and call a method that parses a particular block version

    @param TClass Class type which owns the method to call
*/
template< class TClass >
class ParseMethod : public IParseMethod
{
public:
    /// Type of the method to call
    typedef bool (TClass::*TMethod)();

    /// Constructor
    ParseMethod(TClass* obj, TMethod method)
        : object(obj), method(method)
    {
        // Empty
    }

    /// Call to stored method
    virtual bool operator()() const
    {
        return (object->*method)();
    }

private:
    /// Pointer to class instance which to call the method for
    TClass* object;
    /// Class' method to call
    TMethod method;
};

//------------------------------------------------------------------------------
class NavBlockParser
{
public:
    /// Constructor
    NavBlockParser(const NavTag& blockId, NavParserFactory* factory);
    /// Destructor
    virtual ~NavBlockParser();

    /// Parse the block, returning true if block successfully parsed
    virtual bool Parse(nNavMeshFile* file, nNavMesh* mesh);

protected:
    /// Add a method for to parse the next block version
    void AddParseVersion( IParseMethod* method );

    /// Parse the beginning of the block
    virtual bool BeginBlock();
    /// Parse the ending of the block
    virtual bool EndBlock();
    /// Parse the version of the block
    bool Version(int& version);
    /// Parse a block within this block
    bool ParseBlock(const NavTag& tag);

    /// Factory used to create other block parsers if needed
    NavParserFactory* parserFactory;
    /// Navigation mesh where to write/read
    nNavMesh* mesh;
    /// File to parse
    nNavMeshFile* file;

private:
    /// Id of this block
    int blockId;
    /// Implementations of all block versions
    nArray<IParseMethod*> parseMethods;
};

//------------------------------------------------------------------------------
/**
    Register a method to parse a particular block version

    For all available block version an parse method must be added in the block constructor
    by using this macro. To make easier the readability name each parse method as ParseVersion#,
    where # is the version number, starting at 0.

    @param class_name Name of the class where this macro is used
    @param version Version number to register
*/
#define ADD_PARSE_VERSION(class_name, version) \
    this->AddParseVersion( n_new( ParseMethod<class_name> )(this, &class_name::ParseVersion##version) )

//------------------------------------------------------------------------------
#endif
