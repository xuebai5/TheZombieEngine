
function CreateTerrainMaterial( handle, weightmapsize, indexmapsize, texturepath )

    matpath = '/usr/terrain/geomipmap/layermanager/material_' .. handle
    
    new('nterrainmaterial', matpath)
        sel(matpath)
    
        new('nfloatmap', matpath .. '/layer')
            sel(matpath .. '/layer')
            call('setsize', weightmapsize)
            call('setheightmapfilename', 'textures:layer0.h16')
            call('create')
        sel('..')

        new('nfloatmap', matpath .. '/layeruse')
            sel(matpath .. '/layeruse')
            call('setsize', indexmapsize)
            call('setheightmapfilename', 'textures:layeruse0.h16')
            call('create')
        sel('..')
    
        call('setuvscale', 1/32.0, 1/32.0 )
        call('settexturefilename', texturepath)
        call('setlayerpath', matpath .. '/layer')
        call('setlayerusepath', matpath .. '/layeruse')
    sel('..')
    
end


--
-- Create all necessary terrain objects with specified parameters.
function CreateTerrain( size, weightmapsize, indexmapsize, filename, gridscale, heightoffset, heightscale, blocksize, loderror )

    oldCwd = psel()

    new('ngeomipmapterrain','/usr/terrain/geomipmap')
        sel('/usr/terrain/geomipmap')

        new('nheightmap','/usr/terrain/geomipmap/heightmap')
            sel('/usr/terrain/geomipmap/heightmap')
            call('setgridscale', gridscale)
            call('setheightscale', heightscale)
            call('setheightoffset', heightoffset)
            
            if ( filename == '' ) then
                call('setsize', size)
            else
                call('setheightmapfilename', filename)
            end
        
            call('create')
        sel('..')

        -- Test - create layers
        new('nterrainlayermanager', '/usr/terrain/geomipmap/layermanager')
            sel('/usr/terrain/geomipmap/layermanager')

            -- Create weightmap
            new('nweightmap', '/usr/terrain/geomipmap/layermanager/layerweightmap')
                sel('/usr/terrain/geomipmap/layermanager/layerweightmap')
                call('setweightmapfilename','textures:weights.tga')
                --call('setindexmapfilename','textures:indexes.i40')
                call('setweightmapsize', weightmapsize)
                call('setindexmapsize', indexmapsize)
                call('create')
            sel('..')

            -- Create materials            
            CreateTerrainMaterial( 0, weightmapsize, indexmapsize, 'textures:cesped.dds' )
            CreateTerrainMaterial( 1, weightmapsize, indexmapsize, 'textures:roca_small.dds')
            CreateTerrainMaterial( 2, weightmapsize, indexmapsize, 'textures:terreno002.dds' )
            CreateTerrainMaterial( 3, weightmapsize, indexmapsize, 'textures:terreno004.dds' )
            CreateTerrainMaterial( 4, weightmapsize, indexmapsize, 'textures:agua.dds' )

            call('setweightmappath', '/usr/terrain/geomipmap/layermanager/layerweightmap')

            call('addlayer', '/usr/terrain/geomipmap/layermanager/material_0')
            call('addlayer', '/usr/terrain/geomipmap/layermanager/material_1')
            call('addlayer', '/usr/terrain/geomipmap/layermanager/material_2')
            call('addlayer', '/usr/terrain/geomipmap/layermanager/material_3')
            call('addlayer', '/usr/terrain/geomipmap/layermanager/material_4')

            call('selectlayer', 0)
            -- Test - Refresh weightmap
            --call('refreshweighttexture')            
        sel("..")

        -- geomipmapterrain parameters
        call('setheightmappath', [[/usr/terrain/geomipmap/heightmap]])
        call('setblocksize', blocksize)
        call('seterror', loderror)
        call('setterrainlayermanagerpath', [[/usr/terrain/geomipmap/layermanager]])        
    sel('..')


    sel(oldCwd)
    
    new('ngeomipmapnode','terrainnode')
        sel('terrainnode')
        call('setgeomipmapterrainpath', [[/usr/terrain/geomipmap]])

        call('setsurface', [[surface0]])
        new('nsurfacenode','surface0')
            sel('surface0')
            call('setshader', 'colr', 'shaders:ngeomipmaptex.fx')
            call('setvector', 'TexGenS', 1.0/512.0, 1.0/512.0, 1.0/32.0, 1.0/32.0)
            call('setvector', 'TexGenT', 1.0/8.0, 1.0/8.0, 1.0/32.0, 1.0/32.0)
            call('setvector', 'TexGenR', 1.0/32.0, 1.0/32.0, 1.0/32.0, 1.0/32.0)
            call('setvector', 'TexGenQ', 1.0/32.0, 1.0/32.0, 1.0/32.0, 1.0/32.0)
            call('settexture', 'AmbientMap0', 'textures:weights.tga')
            --call('settexture', 'DiffMap0', 'textures:cesped.dds')
            call('settexture', 'DiffMap1', 'textures:roca_small.dds')
            call('settexture', 'DiffMap2', 'textures:terreno002.dds')
            call('settexture', 'DiffMap3', 'textures:terreno004.dds')
            call('settexture', 'AmbientMap1', 'textures:agua.dds')
            call('settexture', 'AmbientMap2', 'textures:rojo.dds')
        sel('..')

    sel('..')

end