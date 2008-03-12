#ifndef N3DSNEBULAFUNCTIONS_H
#define N3DSNEBULAFUNCTIONS_H
//------------------------------------------------------------------------------

namespace n3dsNebulaFunctions
{
    void GetEntityClasses( bool gamelib, int & numFolders, char *** folders, int & numClasses, char *** classes );
    void GetEntityClassesOf( bool gamelib, const char * parent, int & numClasses, char *** classes );
    void GetMaterialList( bool gamelib, int & numMaterials, char *** materials );
    void GetFileList( bool gamelib, const char* path ,int & numMaterials, char *** materials );
    void GetGfxMaterialList( bool gamelib, int & numMaterials, char *** materials );
    char* GetGfxMaterialPath();
    char* GetWCPath();
    void WhenDetachProcess();
}

//------------------------------------------------------------------------------
#endif//N3DSNEBULAFUNCTIONS_H
