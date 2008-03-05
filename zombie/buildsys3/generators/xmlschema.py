import wx, os
import cmdparser
from xml.dom import minidom
import sys
#import xml.dom.ext

TABSIZE = 4
TABSPACES = " " * TABSIZE
MESSAGE_PROMPT = "SCHEMAPARSER:"

class xmlschema:
    def __init__(self, buildSys):    
        self.buildSys = buildSys
        self.description = "Nebula2 xml schema generator"
        self.file = os.path.join(buildSys.homeDir,"xmlschema.xml")
        self.failedFiles = {}
        self.configList = []
                
    def Generate (self, workspaceNames, targetName, moduleName):
        # create monidom document
        self.doc = minidom.Document()

        # get all files to be parsed
        self.collectFilesToParse(workspaceNames, targetName, moduleName)
        #print "FAILED--------------->" + str(self.failedFiles)
        #print "CORRECT-------------->" + self.files
        xml_TAG = self.doc.createElement("xml")
        self.doc.appendChild(xml_TAG)

        for config in self.configList:
            self.write_XML(config, xml_TAG)

        # OLD CODE: Used for add each entities to xml. Now is not needed because entities
        # can be queried at run-time
        configEntityList = self.collectAllEntities()
        for config in configEntityList:
            for entity_config in config:
                self.write_XML(entity_config, xml_TAG)


        #document = self.print_XML()
        #print document
        self.write_to_file()
       

    #--------------------------------------------------------------------------
    # Get entities from a specified target
    def get_entities_from_target (self, target, configList ):
        for moduleName in target.modules:
            module = self.buildSys.modules[moduleName]
            for file in module.resolvedFiles:
                if file.find("entitylist") != -1:                    
                    file_dir = os.path.dirname(file)
                    parser = cmdparser.CmdFileProcessor(file_dir)
                    config = parser.process_file(file, True)
                    if config != None:
                        configList.append(config)

    #--------------------------------------------------------------------------
    # Collect all entities from targets
    def collectAllEntities (self):
        configList = []
        target = self.buildSys.targets["zombieentity"]
        self.get_entities_from_target( target, configList )
        target = self.buildSys.targets["rnsentity"]
        self.get_entities_from_target( target, configList )
        target = self.buildSys.targets["zombieentityexp"]
        self.get_entities_from_target( target, configList )
        
        return configList

    #--------------------------------------------------------------------------
    # collect all valid modules from the selected workspaces
    def collectFilesToParse(self, workspaceNames, targetName, moduleName):
        self.moduleNames = []
        for workspaceName in workspaceNames:
            workspace = self.buildSys.workspaces[workspaceName]
            # Do target specified
            if targetName != 'all' and moduleName == 'all':
                target = self.buildSys.targets[targetName]
                if target:
                    print "Generating xmlschema for target %s..." %(targetName)
                    for moduleName in target.modules:
                        print "Generating xmlschema for module %s..." %(moduleName)
                        module = self.buildSys.modules[moduleName]
                        self.parse_module_files(module)
                else:
                    print 'Error: (xmlparser) Target not found'
                    return
            # Do module specified
            elif moduleName != 'all':
                print "Generating xmlschema for module %s..." %(moduleName)
                module = self.buildSys.modules[moduleName]
                if module:
                    self.parse_module_files(module)
                else:
                    print 'Error: (xmlparser) Module not found'
                    return
            # Do all modules and targets
            else:
                for targetName in workspace.targets:
                    target = self.buildSys.targets[targetName]
                    for moduleName in target.modules:
                        module = self.buildSys.modules[moduleName]
                        self.parse_module_files(module)
                    
    def get_header_from_cmds (self, module, file_cmds):
        if file_cmds.find("class_cmds.cc") != -1:
            header_file = os.path.join('code', module.codeDir, 'inc', module.dir, module.name + "class" + ".h")
        else:
            header_file = os.path.join('code', module.codeDir, 'inc', module.dir, module.name + ".h")

        return header_file

    def get_cmds_from_header (self, module, file_header):
        cmds_file = os.path.join('code', module.codeDir, 'src', module.dir, module.name + "_cmds.cc")

        return cmds_file

    def get_main_from_header (self, module, file_header):
        main_header = os.path.join('code', module.codeDir, 'src', module.dir, module.name + "_main.cc")

        return main_header

    def join_configs (self, module, cmds_file, main_file, cc_file, header_file):
        parser = cmdparser.CmdFileProcessor(module.dir) 
        cmds_config = parser.process_file(cmds_file)
        #print cmds_config

        parser = cmdparser.CmdFileProcessor(module.dir) 
        header_config = parser.process_file(header_file) 

        parser = cmdparser.CmdFileProcessor(module.dir) 
        main_config = parser.process_file(main_file)

        parser = cmdparser.CmdFileProcessor(module.dir) 
        cc_config = parser.process_file(cc_file)

        # NOTE: Add operator for configurations is not commutative, header config 
        # must be teh first
        return header_config + cmds_config + main_config + cc_config

    def parse_module_files (self, module):
        for file in module.resolvedHeaders:
            sys.stdout.flush()
            config = None
            file_dir = os.path.dirname(file)
            if not file.startswith("pkg_") and not file.startswith("pch"):
                # Only parse nebula classes
                if file.find(module.name) != -1:
                    ext = '.dox'
                    # If is a class save in a diferent dox file
                    if file.find(module.name + "class.h") != -1:
                        ext = "class.dox"

                    # get config from cmds file
                    cmds_file = self.get_cmds_from_header(module, file)
                    if not os.path.exists(cmds_file):
                        cmds_file = ""

                    # get config from main file
                    main_file = self.get_main_from_header(module, file)
                    if not os.path.exists(main_file):
                        main_file = ""

                    # get config from cc file
                    cc_file = file[:-2] + ".cc"  # substitute ".h" for ".cc"
                    cc_file = cc_file.replace("\\inc\\", "\\src\\")
                    if not os.path.exists(cc_file):
                        cc_file = ""

                    config = self.join_configs( module, cmds_file, main_file, cc_file, file )
                    config = self.join_with_dox(config, module, file, ext)               
                    if config != None:
                        self.configList.append(config)
                    else:
                        self.failedFiles[file] = module.name

    def join_with_dox (self, nodox_config, module, file, ext='.dox'):
        dox_file = os.path.join('code', module.codeDir, 'dox', module.dir, module.name + ext)
        dox_path = os.path.join('code', module.codeDir, 'dox', module.dir)        
        parser = cmdparser.CmdFileProcessor(dox_path)
        dox_config = None
        if os.path.exists(dox_file):
            dox_config = parser.process_file(dox_file)
            # Check if some command had been changed
            if dox_config:
                changes = False
                index_list = []
                cmd_count = 0   # command index
                overwrite_always = False  # If set always overwrite commands in dox
                del_always = False
                for cmd_dox in dox_config.cmds:
                    command_found = False                     
                    for cmd_nodox in nodox_config.cmds:
                        if cmd_dox.cmdName == cmd_nodox.cmdName:
                            command_found = True
                    
                    # If the command is not in source file, delete it from dox
                    if not command_found:
                        pass
                        #print "%s WARNING deleted command '%s' from source file. Review the dox %s" %(MESSAGE_PROMPT,cmd_dox.cmdName,dox_file)

                    # increment command index
                    cmd_count += 1

                # Check if there are new uncommented commands
                for cmd_nodox in nodox_config.cmds:
                    command_found = False
                    for cmd_dox in dox_config.cmds:
                        if cmd_dox.cmdName == cmd_nodox.cmdName:
                            command_found = True
    
                    if not command_found:
                        if not cmd_nodox.comment:
                            print "%s WARNING New uncommented command named '%s'."\
                                  " Source file: %s. Dox file: %s" %(MESSAGE_PROMPT, cmd_nodox.cmdName, file, dox_file)
                            #dox_config.cmds.append(cmd_nodox)
                            #changes = True

                # Check if there are new uncommented properties
                tmp_list = []
                for prop_nodox in nodox_config.properties:
                    prop_found = False                    
                    for prop_dox in dox_config.properties:
                        if prop_dox.name == prop_nodox.name:
                            prop_found = True
                            break;

                    if not prop_found:
                        #print "%s ADDED '%s' PROPERTY to dox file."\
                        #      " Source file: %s. Dox file: %s" %(MESSAGE_PROMPT, prop_nodox.name, file, dox_file)
                        print "%s New '%s' PROPERTY Found in\n"\
                              " Source file: %s. Dox file: %s" %(MESSAGE_PROMPT, prop_nodox.name, file, dox_file)
                        #tmp_list.append(prop_nodox)
                        #changes = True
                
                #dox_config.properties.extend(tmp_list)


                # If config has changed write it to file
                #dox_config_old = parser.process_file(dox_file)
                #if dox_config_old != dox_config:
                if len(dox_config.cmds) > 0 or len(dox_config.properties) > 0 or len(dox_config.signals) > 0:                    
                    # Overwrite nodox cmds with dox cmds                
                    try:
                        if dox_config.cmds != nodox_config.cmds:
                            changes = True
                            for cmd_dox in dox_config.cmds:
                                found = False
                                i = 0
                                for cmd_nodox in nodox_config.cmds:
                                    if cmd_dox.cmdName == cmd_nodox.cmdName:
                                        found = True
                                        nodox_config.cmds[i] = cmd_dox
                                    i += 1
                                # If command is not in source is added from dox file
                                if not found:
                                    nodox_config.cmds.append(cmd_dox)
                    except:
                        print "Something wrong with the parser..."
            
                    # Overwrite nodox properties config with dox properties
                    if dox_config.properties != nodox_config.properties:
                        changes = True
                        for prop_dox in dox_config.properties:
                            found = False
                            i = 0
                            for prop_nodox in nodox_config.properties:
                                if prop_dox.name == prop_nodox.name:
                                    found = True
                                    nodox_config.properties[i] = prop_dox
                                i += 1
    
                            # If property is not in source is added from dox file
                            if not found:
                                nodox_config.properties.append(prop_dox)

                    # Write new dox to file
                    #if changes:
                    #    doxFile = open(dox_file, 'w')
                    #    doxFile.write(repr(dox_config))
                    #    doxFile.close()

                else:
                    os.remove(dox_file)
                    if not os.path.exists(dox_file):
                        print "No command or properties for %s. REMOVED" %(dox_file)
                    else:
                        print "%s Cannot delete file %s" %(MESSAGE_PROMPT, dox_file)
            
        else:
            create_file = False
            stream_text = ''
            stream_text += repr(nodox_config.classInfo)
            # Properties always goes in dox files
            for prop in nodox_config.properties:
                create_file = True
                stream_text += repr(prop)
            # Get not commented commands
            for cmd in nodox_config.cmds:
                if not cmd.comment:
                    create_file = True
                    print "%s ADDED '%s' COMMAND to dox file %s" %(MESSAGE_PROMPT, cmd.cmdName, os.path.basename(dox_file))
                    stream_text += cmd.get_dox_text()                                 
            # Create dox file if needed
            if create_file:
                if not os.path.exists(dox_path):
                    os.makedirs(dox_path)
    
                doxFile = open(dox_file, 'w')
                # Write the dox file. Except commented commands all information is added
                doxFile.write(stream_text)
                doxFile.close()        
            

        return nodox_config
   
    def write_XML(self, config, root_TAG):
        # create the <xml> base element
        #print config
        classInfo = config.classInfo
        type = classInfo.type
        superClass = classInfo.superClass
        compName = classInfo.className
        cppClass = classInfo.cppClass
        docString = classInfo.info
        components = classInfo.components
         
        # get getter and setter commands
        getters = []
        setters = []
        normal_cmds = []
        subType = ""
        
        # split commands in lists by type
        for cmd in config.cmds:
            if isinstance(cmd, cmdparser.Cmd):
                if cmd.type == cmd.T_GETTER:
                    getters.append(cmd)
                elif cmd.type == cmd.T_SETTER:
                    setters.append(cmd)
            elif isinstance(cmd, cmdparser.CommonCmd):
                normal_cmds.append(cmd)

        # If the element exist exit. This avoid classes comented in multiple files.
        # As the cmds files are parse before than others, the comments there have 
        # priority over the others.
        for cl in self.doc.getElementsByTagName(type):
            if cppClass == cl.getAttribute("cppclass"):
                return

        # create the component class info
        if "componentclass" == type:            
            class_TAG = self.doc.createElement("componentclass")
        elif "component" == type:
            class_TAG = self.doc.createElement("component")
        elif "scriptclass" == type:
            class_TAG = self.doc.createElement("scriptclass")
        elif "entityclass" == type:
            class_TAG = self.doc.createElement("entityclass")
        elif "entity" == type:
            class_TAG = self.doc.createElement("entity")
        else:
            #print "ERROR: Bad configuration in %s" %(file)
            return

        class_TAG.setAttribute("group", compName)
        class_TAG.setAttribute("cppclass", cppClass)
        class_TAG.setAttribute("superclass", superClass)

        # add the element
        root_TAG.appendChild(class_TAG)       
        # add doc element
        compDoc = self.doc.createElement("doc")
        class_TAG.appendChild(compDoc)

        # add doc text
        docText = self.doc.createTextNode(docString)
        compDoc.appendChild(docText)			

        if "entityclass" == type or "entity" == type:
            for component in components:
                component_TAG = self.doc.createElement("component")
                component_TAG.setAttribute("name", component)
                class_TAG.appendChild(component_TAG)

        # properties are only in components		
        if "componentclass" == type or "component" == type or "scriptclass" == type:
            props = self.doc.createElement("properties")
            class_TAG.appendChild(props)
            for prop in config.properties:
                prop_TAG = self.doc.createElement("property")
                prop_TAG.setAttribute("name", prop.name)
                props.appendChild(prop_TAG)
        
                # Add doc tag
                propDoc_TAG = self.doc.createElement("doc")
                prop_TAG.appendChild(propDoc_TAG)
                docText = self.doc.createTextNode(prop.info)
                propDoc_TAG.appendChild(docText)
                

                # getterlist tag
                getList_TAG = self.doc.createElement("getterlist")
                prop_TAG.appendChild(getList_TAG)
                
                for getter in getters:
                    if getter.propertyName == prop.name:
                        # getter tag
                        getter_TAG = self.doc.createElement("getter")
                        getter_TAG.setAttribute("name", getter.cmdName)
                        getter_TAG.setAttribute("subtype", ["none", "count"][getter.subType])
                        getList_TAG.appendChild(getter_TAG)
                        
                        # getter doc tag
                        getterDoc_TAG = self.doc.createElement("doc")
                        getter_TAG.appendChild(getterDoc_TAG)
                        docText = self.doc.createTextNode(getter.info)
                        getterDoc_TAG.appendChild(docText)

                    
                # setterlist tag
                setList_TAG = self.doc.createElement("setterlist")
                prop_TAG.appendChild(setList_TAG)
                
                for setter in setters:
                    if setter.propertyName == prop.name:
                        # setter tag
                        setter_TAG = self.doc.createElement("setter")
                        setter_TAG.setAttribute("name", setter.cmdName + subType)
                        setter_TAG.setAttribute("subtype", ["none", "begin", "add", "end"][setter.subType])
                        setList_TAG.appendChild(setter_TAG)
                        
                        # setter doc tag
                        setterDoc_TAG = self.doc.createElement("doc")
                        setter_TAG.appendChild(setterDoc_TAG)
                        docText = self.doc.createTextNode(setter.info)
                        setterDoc_TAG.appendChild(docText)

        
                # format tag
                format_TAG = self.doc.createElement("format")
                prop_TAG.appendChild(format_TAG)
                
                # param tags
                self.__get_param_tag(format_TAG, prop.params)
                #if param_TAG != "":
                #    format_TAG.appendChild(param_TAG)				
                
                for type, value in map(None, prop.attribute.keys(), prop.attribute.values()):
                    propAttr_TAG = self.doc.createElement("attrs")
                    propAttr_TAG.setAttribute("type", type)
                    #print value
                    propAttr_TAG.setAttribute("value", value)
                    prop_TAG.appendChild(propAttr_TAG)
                    
        commands_TAG = self.doc.createElement("commands")
        class_TAG.appendChild(commands_TAG)
        
        # commands that are not for properties
        for cmd in normal_cmds:
            cmd_TAG = self.doc.createElement("cmd")
            cmd_TAG.setAttribute("name", cmd.cmdName)    
            class_TAG.appendChild(cmd_TAG)
            # set command doc
            cmdDoc_TAG = self.doc.createElement("doc")
            cmd_TAG.appendChild(cmdDoc_TAG)
            docText = self.doc.createTextNode(cmd.info)
            cmdDoc_TAG.appendChild(docText)
        
            output_TAG = self.doc.createElement("output")
            cmd_TAG.appendChild(output_TAG)         
            #print cmd.outParams
            self.__get_param_tag(output_TAG, cmd.outParams)
            
            input_TAG = self.doc.createElement("input")
            cmd_TAG.appendChild(input_TAG)                                
            self.__get_param_tag(input_TAG, cmd.inParams)
            
            commands_TAG.appendChild(cmd_TAG)
            
    def print_XML(self):
        return self.doc.toprettyxml(indent="    ")

    def write_to_file(self):
        file = open(self.file, 'w')
        self.doc.writexml(file, indent="    ", addindent="    ",newl="\n")
        file.close()
    
    def __get_param_tag(self, format_TAG, cmdList): 
        param_TAG = ""
        #print cmdList
        for param in cmdList:
            # void params don't appear in the xml
            if param != None:
                if param.type is not 'v':
                    param_TAG = self.doc.createElement("param")
                    param_TAG.setAttribute("name", param.name)
                    param_TAG.setAttribute("type", param.type)			
                    
                    # format doc tag
                    formatDoc_TAG = self.doc.createElement("doc")
                    param_TAG.appendChild(formatDoc_TAG)
                    docText = self.doc.createTextNode(param.comment)
                    formatDoc_TAG.appendChild(docText)
            
                    for type, value in map(None, param.attribute.keys(), param.attribute.values()):                
                        # attr tag
                        attr_TAG = self.doc.createElement("attr")
                        attr_TAG.setAttribute("type", type)
                        attr_TAG.setAttribute("value", value)
                        param_TAG.appendChild(attr_TAG)

                    format_TAG.appendChild(param_TAG)
            
        #return param_TAG

    def write_xml_file (self, codedir, file):
        parser = cmdparser.CmdFileProcessor(codedir)
        config = parser.process(file)
        print config

