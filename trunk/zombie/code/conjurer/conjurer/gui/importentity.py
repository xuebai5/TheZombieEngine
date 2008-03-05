##\file importentity.py
##\brief Import entity helper built around EntityImportManager class

import wx
import servers
import nebulaguisettings as cfg

import conjurerframework as cjr


class EntityImportManager:
    def __init__(self, aParentView, aTargetLayerId):
        self.parentView = aParentView
        self.targetLayerId = aTargetLayerId
        self.containerObject = None
        self.importedObjects = None
        self.importEntities()

    def chooseTargetFile(self):
        aFileDialog = wx.FileDialog(
            self.parentView, 
            message = "Choose a file", 
            defaultDir = self.getDefaultDirectory(),
            wildcard = "*.n2",
            style=wx.OPEN, 
            )
        if aFileDialog.ShowModal() == wx.ID_OK:
            return aFileDialog.GetPath()
        else:
            return False
        #aFileDialog.Destroy()

    def getDefaultDirectory(self):
        return (cfg.Repository.getsettingvalue(cfg.ID_DefaultEntityExportPath) )
        
    def loadContainerObjectFromFile(self, aFilePath):
        aKernelServer = servers.get_kernel_server()
        return aKernelServer.load( str(aFilePath) )

    def checkIfContainerObjectIsValid(self):
        if self.isContainerObjectValid():
            return True
        else:
            cjr.show_error_message(
                "Unable to import - invalid object"
                )
            return False

    def isContainerObjectValid(self):
        return self.containerObject.isa('nobjectinstancer')

    def unpackEntitiesAndAddToWorld(self):
        entity_server = servers.get_entity_object_server()
        world_server = servers.get_world_interface()
        number_of_elements = self.containerObject.size()
        for index in range(number_of_elements):
            entity = self.containerObject.at(index)
            original_id = self.containerObject.getobjectnameatindex(index)
            #check if the entity already exists
            match = entity_server.getentityobject( int(original_id) )
            if not match == None:
                result = cjr.confirm_yes_no(
                                self.parentView, 
                                "This entity already exists. "\
                                "Do you want to overwrite it?"
                                )
                wantsToOverwrite = (result == wx.ID_YES)
                if wantsToOverwrite:
                    entity_server.swapentityobjectids(match, entity)
                    entity_server.removeentityobject(match)
            entity.setlayerid(self.targetLayerId)
            world_server.addtoworld(entity)

    def importEntities(self):
        aTargetFilePath = self.chooseTargetFile()
        if aTargetFilePath:
            self.containerObject = self.loadContainerObjectFromFile(aTargetFilePath)
            if self.checkIfContainerObjectIsValid():
                self.unpackEntitiesAndAddToWorld()
                return True
            else:
                return False
        else:
            return False


def importEntitiesFromFile(aParentView, aLayerId):
    """Will return True if it all worked OK, otherwise False."""
    return EntityImportManager(aParentView, aLayerId)


    
