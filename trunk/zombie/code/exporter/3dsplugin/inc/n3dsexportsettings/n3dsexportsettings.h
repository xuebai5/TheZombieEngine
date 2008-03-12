#ifndef N_3DS_EXPORT_SETTINGS_H
#define N_3DS_EXPORT_SETTINGS_H

#include "util/nstring.h" 


class n3dsLOD
{
    public:
        // returnthe lod of max node
        int   GetLODof(IGameNode* node) const;
        // return to end of lod
        float GetDistance(int numLOD) const;
        // return the total number of lod
        int  GetCountLOD() const;
        /// GetFromScene
        static n3dsLOD GetFromScene();
private:
    nArray<float> distance;
};

class nSceneNode;

struct n3dsAssetInfo
{
    /// The asset's path
    nString     assetPath;
    /// pointer to scene
    nSceneNode* node;
    /// path of sceneno in noh
    nString     noh;
};

//-----------------------------------------------------------------------------
/**
    @class n3dsExportSettings
    @ingroup n3dsMaxExporterKernel
    @brief  the struct for export setting
    has same field  maxscript (export.ms struct NebStructExportOptions )
*/


class n3dsExportSettings
{
public:
    /// same values of maxscript
    enum ExportModeType
    {
        /// Export a entity class neBrush ...
        Brush = 1,
        /// Export Indoor
        Indoor,
        /// look the characcter mode type
        Character,
        /// Centinel
        MaximExportMode,

    };

    /// same values of maxscript
    enum CharacterModeType
    {
        /// Export Mesh o meshes( morpher). entity class type neCharacter?
        Mesh  = 1,
        /// Export the Skeleton ( entity class type neSkeleton)
        Skeleton ,
        /// Export the resource a skin animation
        SkinAnimation,
        /// Exporte the resource a akin animation
        MorpherAnimation,
        /// Centinel
        MaximModeType,

    };

    enum CritterNameType
    {
        Human = 1,
        Scavenger ,
        Strider,
        MaximCritterNameType,
    };

    /// Indicates if data is valid use in maxscript
    bool valid;
    /// True if use a working copy, false temporal directory
    bool gameLibrary;
    /// Tur if export thumb nails
    bool exportThumbNail;
    /// The export mode type
    ExportModeType exportMode;

    /// The anim mode 
    CharacterModeType animMode;
    /// critter name
    CritterNameType critterName;

    /// The name of entity class
    nString entityName;
    /// The Grimoire's name
    nString libraryFolder;
    /// The name of resource only use when export Skin animation or Morpher Animation
    nString resourceName;
    /// The name of skeleton class needed when export animation or character mesh
    nString skeletonName;

    /// first person skeleton
    bool firstPerson;

    /// export motion
    bool exportMotion;
    bool motionX;
    bool motionY;
    bool motionZ;


    // The next variables only use when export to temporal directory

    /// true save binary resource ( non binary only for programmers)
    bool binaryResource;
    /// optimize the  mesh. ( false use for programers)
    bool optimizeMesh;
    /// copy the texture  to temporal directory
    bool collectTextures;
    /// LOd options
    n3dsLOD lod;

    /// if after exporting conjurer has to be open
    bool openConjurer;

    /// The defualt constructor
    n3dsExportSettings();
    /// return the export options from scene
    static  n3dsExportSettings GetFromScene();
    /// return the export options from the last export
    //static  n3dsExportSettings GetLastFromScene();
    /// check if combination is posible
    bool CheckValidSetting();
    /// Check if ist posible create it
    bool CheckValidExport();

protected:
    /// Check its posible export options
    bool CheckValidExportBrush();
    /// check if skeleton exists and this animation is not repeated
    bool CheckValidExportSkinAnimation();
    /// check if skeleton exists
    bool CheckValidExportCharacterMesh();
    /// check if skeleton exists in repository ( false if it exists, different messages if it's an skeleton or another class)
    bool CheckValidSkeleton();
     /// check if skeleton exists (true if exists )
    bool CheckSkeletonExists();

};

#endif
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------