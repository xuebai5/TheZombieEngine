#ifndef N_SPATIALTYPES_H
#define N_SPATIALTYPES_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Common types and definitions for the whole spatial module
    
    (C) 2006  Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
// magnification factor for a 60 deg fov, used as base factor for distance ranges
#define XSCALE_60 (float(1.0 / n_tan(n_deg2rad(60) * 0.5f)))

namespace nSpatialTypes
{

    enum SpatialFlags
    {
        SPF_ONE_INDOOR       = (1<<0),  // insert in only one indoor
        SPF_ALL_INDOORS      = (1<<1),  // insert in all the indoors
        SPF_OUTDOORS         = (1<<2),  // insert in the outdoors
        SPF_OCTREE           = (1<<3),  // insert in the octree space

        SPF_ONE_CELL         = (1<<4),  // insert in only one cell
        SPF_ALL_CELLS        = (1<<5),  // insert in all the cells
        
        SPF_CONTAINING       = (1<<6),  // insert in the cell or cells completely containing the item
        SPF_INTERSECTING     = (1<<7),  // insert in the cell or cells intersecting the item

        SPF_ALL_LEAFS        = (1<<8),  // insert in all the leafs in a tree (only for quadtree or octree spaces)
        SPF_INNER_CC_NODE    = (1<<9),  // insert in the inner tree node that completely contains the item
        SPF_2D               = (1<<10), // insert in a quadtree cell using 2D coordinates (x, z)

        SPF_USE_BBOX         = (1<<11), // using the element's bounding box
        SPF_USE_MODEL        = (1<<12), // using the element's sphere
        SPF_USE_POSITION     = (1<<13), // using the element's position
        SPF_USE_CELL_BBOX    = (1<<14), // using just cell's bounding box (not wrapper mesh)
    };

    // Spatial categories. 
    enum SpatialCategories
    {
        /// put below visibility categories
        CAT_DEFAULT = 0,
        CAT_BRUSHES,
        CAT_AGENTS,
        CAT_PORTALS,
        CAT_LIGHTS,        
        CAT_FACADES,
        CAT_INDOOR_BRUSHES,
        CAT_WEAPONS,
        CAT_VEHICLES,
        CAT_GRAPHBATCHES,
        /// put below entities not taken into account for visibility
        CAT_SCENE,
        CAT_OCCLUDERS,
		CAT_SOUNDSSOURCES,
        CAT_NAVMESHNODES,
        CAT_NAVMESHOBSTACLES,
        CAT_WAYPOINTS,
        CAT_CAMERAS,
        CAT_NON_VISIBLES,
        CAT_AREA_EVENTS,
#ifndef NGAME
        CAT_TEMP_INVISIBLES,
#endif // !NGAME

        // add categories here

        // DON'T TOUCH THIS LINE:
        NUM_SPATIAL_CATEGORIES
    };

};

//------------------------------------------------------------------------------
#endif
