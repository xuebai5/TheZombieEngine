import servers

es = servers.get_entity_object_server()

entity = es.getfirstentityobject()        

while (entity != None):
        if entity.hascomponent("ncTransform") and not entity.hascomponent("ncPhyTerrainCell"):
                x,y,z = entity.getposition()
                entity.setposition(x, y, z)
        entity = es.getnextentityobject()
