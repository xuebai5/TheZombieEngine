#include "precompiled/pchn3dsmaxexport.h"
/**
   @file n3dscustomattributes.cc
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "n3dsexporters/n3dscustomattributes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param attribute name of the Custom Attribute
    @param param name of the parameter in the attribute
    @param value [out] value of attribute.param
    @retval true if parameter is found
*/
bool 
n3dsCustomAttributes::GetParamString( INode * const node, const nString &attribute, 
                                 const nString &param, TSTR & value )
{
    // get the attribute container
    BaseObject *obj = node->GetObjectRef();
    ICustAttribContainer* attribContainer = obj->GetCustAttribContainer();
    
    if ( attribContainer ) 
    {
        int numAttribs = attribContainer->GetNumCustAttribs();
        TCHAR * name;

        // look for the correct attibute
        for (int i = 0 ; i < numAttribs ; ++i )
        {
            CustAttrib *attrib = attribContainer->GetCustAttrib( i );

            name = attrib->GetName();

            if( 0 == strcmp( name, attribute.Get() ) )
            {
                // look for the correct parameter
                for( int idxBlock = 0 ; idxBlock < attrib->NumParamBlocks() ; ++idxBlock )
                {
                    IParamBlock2 *paramBlock = attrib->GetParamBlock(idxBlock);

                    for( int idxParam = 0 ; idxParam < paramBlock->NumParams() ; ++idxParam )
                    {
                        ParamID idParam = paramBlock->IndextoID( idxParam );

                        TSTR name = paramBlock->GetLocalName( idParam );
                        ParamType2 type = paramBlock->GetParameterType( idParam );

                        if( 0 == _stricmp( name, param.Get() ) )
                        {
                            n_assert2( type == TYPE_STRING, "luis.cabellos: error of parameters Type" );
                            
                            // return the attribute.parameter
                            name = paramBlock->GetStr( idParam );
                            value = name;
                            return true;
                        }
                    }
                }
            }
        }
    }
    // parameter not found
    return false;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param attribute name of the Custom Attribute
    @param param name of the parameter in the attribute
    @param value [out] value of attribute.param
    @retval true if parameter is found
*/
bool 
n3dsCustomAttributes::GetParamInt( INode * const node, const nString &attribute, 
                                 const nString &param, int & value )
{
    // get the attribute container
    BaseObject *obj = node->GetObjectRef();
    ICustAttribContainer* attribContainer = obj->GetCustAttribContainer();
    
    if ( attribContainer ) 
    {
        int numAttribs = attribContainer->GetNumCustAttribs();
        TCHAR * name;

        // look for the correct attibute
        for (int i = 0 ; i < numAttribs ; ++i )
        {
            CustAttrib *attrib = attribContainer->GetCustAttrib( i );

            name = attrib->GetName();

            if( 0 == strcmp( name, attribute.Get() ) )
            {
                // look for the correct parameter
                for( int idxBlock = 0 ; idxBlock < attrib->NumParamBlocks() ; ++idxBlock )
                {
                    IParamBlock2 *paramBlock = attrib->GetParamBlock(idxBlock);

                    for( int idxParam = 0 ; idxParam < paramBlock->NumParams() ; ++idxParam )
                    {
                        ParamID idParam = paramBlock->IndextoID( idxParam );

                        TSTR name = paramBlock->GetLocalName( idParam );
                        ParamType2 type = paramBlock->GetParameterType( idParam );

                        if( 0 == _stricmp( name, param.Get() ) )
                        {
                            n_assert2( type == TYPE_INT ||type == TYPE_BOOL, "luis.cabellos: error of parameters Type" );
                            
                            // return the attribute.parameter
                            value = paramBlock->GetInt( idParam );
                            return true;
                        }
                    }
                }
            }
        }
    }
    // parameter not found
    return false;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param attribute name of the Custom Attribute
    @param param name of the parameter in the attribute
    @param value [out] value of attribute.param
    @retval true if parameter is found
*/
bool
n3dsCustomAttributes::GetParamNodes( INode * const node, const nString & attribute, 
                                    const nString & param, nArray<INode*> & value )
{
    // get the geometries
    BaseObject *obj = node->GetObjectRef();
    ICustAttribContainer* attribContainer = obj->GetCustAttribContainer();
    
    value.Clear();

    if ( attribContainer ) 
    {
        int numAttribs = attribContainer->GetNumCustAttribs();
        TCHAR * name;

        // look for the correct attibute
        for (int i = 0 ; i < numAttribs ; ++i )
        {
            CustAttrib *attrib = attribContainer->GetCustAttrib( i );

            name = attrib->GetName();

            if( 0 == strcmp( name, attribute.Get() ) )
            {

                // look for the correct parameter
                for( int idxBlock = 0 ; idxBlock < attrib->NumParamBlocks() ; ++idxBlock )
                {
                    IParamBlock2 *paramBlock = attrib->GetParamBlock(idxBlock);

                    for( int idxParam = 0 ; idxParam < paramBlock->NumParams() ; ++idxParam )
                    {
                        ParamID idParam = paramBlock->IndextoID( idxParam );

                        TSTR name = paramBlock->GetLocalName( idParam );

                        ParamType2 type = paramBlock->GetParameterType( idParam );

                        if( 0 == _stricmp( name, param.Get() ) )
                        {
                            n_assert2( type == TYPE_INODE_TAB, "luis.cabellos: error of parameters Type" );

                            // add geometries
                            for( int idxTab = 0 ; idxTab < paramBlock->Count( idParam ) ; ++idxTab )
                            {
                                INode * child = paramBlock->GetINode( idParam, 0, idxTab );

                                value.PushBack( child );
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    // parameter not found
    return false;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
