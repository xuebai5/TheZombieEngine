//------------------------------------------------------------------------------
//  utilfunctions.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#pragma warning( push, 3 )
#include "maxscrpt/maxscrpt.h"
#include "maxscrpt/defextfn.h"
#include "maxscrpt/definsfn.h"
#pragma warning( pop )

#include "pluginutilfunctions/n3dsnebulafunctions.h"

#include <cassert>

//------------------------------------------------------------------------------
def_visible_primitive( function_nebula, "nebula" );
def_visible_primitive( function_get_entity_classes, "nGetEntityClasses" );
def_visible_primitive( function_get_entity_classes_of, "nGetEntityClassesOf" );
def_visible_primitive( function_get_material_list, "nGetMaterialList" );
def_visible_primitive( function_get_gfx_material_list, "nGetGFXMaterialList" );
def_visible_primitive( function_get_gfx_material_path, "nGetGFXMaterialPath" );
def_visible_primitive( function_get_working_copy_path, "nGetWCPath" );
def_visible_primitive( function_get_file_list,  "nGetFileList" );

//------------------------------------------------------------------------------
/**
    Ping function to check from Max that plugin is load
    @param arg_list list of arguments
    @param count number of arguments
    @returns true
*/
Value*
function_nebula_cf( Value** /*arg_list*/, int count )
{
    check_arg_count(nebula_power, 0, count);
    return &true_value;
}

#pragma warning( push, 3 )
#include "maxscrpt/Numbers.h"
#include "maxscrpt/strings.h"
#include "maxscrpt/arrays.h"
#pragma warning( pop )

//------------------------------------------------------------------------------
/**
    Get the list of entity classes and folders from grimoire
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with two sublist, the class names and the folder names
*/
Value*
function_get_entity_classes_cf( Value** arg_list, int count )
{

    // check number of arguments
    check_arg_count( nGetEntityClasses, 1, count);

    if (! (is_bool(arg_list[0])) )
    {
        throw SyntaxError("wanted boolean", arg_list[0]->tag->name);
    }

    // get arguments
    bool isGameLibrary = ( arg_list[0]->to_bool() == 1 );
    
    // create the result variable
    one_typed_value_local(Array * result);

    char ** classesList = 0;
    char ** foldersList = 0;
    int numFolders = 0, numClasses = 0;

    // get entity classes and folders from nebula
    n3dsNebulaFunctions::GetEntityClasses( 
        isGameLibrary, numFolders, &foldersList, numClasses, &classesList );
    
    // get folders in a 3ds Array
    Array * folders = new Array( numFolders );
    assert( folders );
    if( foldersList && folders )
    {
        for( int i = 0 ; i < numFolders ; ++i )
        {
            folders->append( new String( foldersList[ i ] ) );
            delete[] foldersList[ i ];
        }
        delete[] foldersList;
    }

    // get classes in a 3ds Array
    Array * classes = new Array( numClasses );
    assert( classes );
    if( classesList && classes )
    {
        for( int i = 0 ; i < numClasses ; ++i )
        {
            classes->append( new String( classesList[ i ] ) );
            delete[] classesList[ i ];
        }
        delete[] classesList;
    }

    // put classes and folders in return result
    if( folders && classes)
    {
        vl.result = new Array(2);
        assert( vl.result );

        if( vl.result )
        {
            vl.result->append( folders );
            vl.result->append( classes );
        }
    }

    return_value(vl.result);
}

//------------------------------------------------------------------------------
/**
    Get the list of entity classes with a same parent
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the classes with the parent
*/
Value*
function_get_entity_classes_of_cf( Value** arg_list, int count )
{

    // check number of arguments
    check_arg_count( nGetEntityClasses, 2, count);

    if (! (is_bool(arg_list[0])) )
    {
        throw SyntaxError("wanted boolean", arg_list[0]->tag->name);
    }

    if (! (is_string(arg_list[1])) )
    {
        throw SyntaxError("wanted string", arg_list[1]->tag->name);
    }

    // get arguments
    bool isGameLibrary = ( arg_list[0]->to_bool() == 1 );
    TCHAR * classParent = arg_list[1]->to_string();
    
    // create the result variable
    one_typed_value_local(Array * result);

    char ** classesList = 0;
    int numClasses = 0;

    // get entity classes
    n3dsNebulaFunctions::GetEntityClassesOf( isGameLibrary, classParent, numClasses, &classesList );
    
    // get classes in a 3ds Array
    Array * classes = new Array( numClasses );
    assert( classes );
    if( classesList && classes )
    {
        for( int i = 0 ; i < numClasses ; ++i )
        {
            classes->append( new String( classesList[ i ] ) );
            delete[] classesList[ i ];
        }
        delete[] classesList;
    }

    // put classes in return result
    if( classes)
    {
        vl.result = classes;
    }

    return_value(vl.result);
}

//------------------------------------------------------------------------------
/**
    Get the list of materials from the Grimoire
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the material names
*/
Value *
function_get_material_list_cf( Value ** arg_list, int count )
{
    // check number of arguments
    check_arg_count( nGetEntityClasses, 1, count);

    if (! (is_bool(arg_list[0])) )
    {
        throw SyntaxError("wanted boolean", arg_list[0]->tag->name);
    }

    // get arguments
    bool isGameLibrary = ( arg_list[0]->to_bool() == 1 );

    // create the result variable
    one_typed_value_local(Array * result);

    char ** materialList = 0;
    int numMaterials = 0;

    // get material list
    n3dsNebulaFunctions::GetMaterialList( isGameLibrary, numMaterials, &materialList );

    // get classes in a 3ds Array
    Array * materials = new Array( numMaterials );
    assert( materials );
    if( materialList && materials )
    {
        for( int i = 0 ; i < numMaterials ; ++i )
        {
            materials->append( new String( materialList[ i ] ) );
            delete[] materialList[ i ];
        }
        delete[] materialList;
    }

    // put classes in return result
    if( materials)
    {
        vl.result = materials;
    }

    return_value(vl.result);
}


//------------------------------------------------------------------------------
/**
    Get the list of files in a directory
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the files name without extension
*/
Value *
function_get_file_list_cf( Value ** arg_list, int count )
{
    // check number of arguments
    check_arg_count( nGetEntityClasses, 2, count);

    if (! (is_bool(arg_list[0])) )
    {
        throw SyntaxError("wanted boolean", arg_list[0]->tag->name);
    }
    // get arguments
    bool isGameLibrary = ( arg_list[0]->to_bool() == 1 );

    if (! (is_string(arg_list[1])) )
    { 
        throw SyntaxError("wanted string", arg_list[0]->tag->name);
    }
    const char* path = arg_list[1]->to_string();


    // create the result variable
    one_typed_value_local(Array * result);

    char ** materialList = 0;
    int numMaterials = 0;

    // get material list
    n3dsNebulaFunctions::GetFileList( isGameLibrary, path ,numMaterials, &materialList );

    // get classes in a 3ds Array
    Array * materials = new Array( numMaterials );
    assert( materials );
    if( materialList && materials )
    {
        for( int i = 0 ; i < numMaterials ; ++i )
        {
            materials->append( new String( materialList[ i ] ) );
            delete[] materialList[ i ];
        }
        delete[] materialList;
    }

    // put classes in return result
    if( materials)
    {
        vl.result = materials;
    }

    return_value(vl.result);
}

//------------------------------------------------------------------------------
/**
    Get the list of graphics  materials 
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the material names
*/
Value *
function_get_gfx_material_list_cf( Value ** arg_list, int count )
{
    // check number of arguments
    check_arg_count( nGetEntityClasses, 1, count);

    if (! (is_bool(arg_list[0])) )
    {
        throw SyntaxError("wanted boolean", arg_list[0]->tag->name);
    }

    // get arguments
    bool isGameLibrary = ( arg_list[0]->to_bool() == 1 );

    // create the result variable
    one_typed_value_local(Array * result);

    char ** materialList = 0;
    int numMaterials = 0;

    // get material list
    n3dsNebulaFunctions::GetGfxMaterialList( isGameLibrary, numMaterials, &materialList );

    // get classes in a 3ds Array
    Array * materials = new Array( numMaterials );
    assert( materials );
    if( materialList && materials )
    {
        for( int i = 0 ; i < numMaterials ; ++i )
        {
            materials->append( new String( materialList[ i ] ) );
            delete[] materialList[ i ];
        }
        delete[] materialList;
    }

    // put classes in return result
    if( materials)
    {
        vl.result = materials;
    }

    return_value(vl.result);
}

//------------------------------------------------------------------------------
/**
    Get the path of working copy
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the files names
*/
Value* 
function_get_working_copy_path_cf( Value** /*arg_list*/, int count )
{
    check_arg_count(class_get_class_name, 0, count);
    one_typed_value_local(String* result);
    char* path = n3dsNebulaFunctions::GetWCPath();
    vl.result = new String( path);
    delete[] path;
    return_value(vl.result);
}

//------------------------------------------------------------------------------
/**
    Get the path of  graphics materials
    @param arg_list list of arguments
    @param count number of arguments
    @returns a list with the files names
*/
Value* 
function_get_gfx_material_path_cf( Value** /*arg_list*/, int count )
{
    check_arg_count(class_get_class_name, 0, count);
    one_typed_value_local(String* result);
    char* path = n3dsNebulaFunctions::GetGfxMaterialPath();
    vl.result = new String( path);
    delete[] path;
    return_value(vl.result);
}

//------------------------------------------------------------------------------
