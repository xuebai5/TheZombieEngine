#--------------------------------------------------------------------------
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import re, os, string

class Module:
    
    #--------------------------------------------------------------------------
    def __init__(self, moduleName, bldFilename):
        self.buildSys = None
        # these more or less directly correspond to stuff in bld files
        self.name = moduleName
        self.annotation = ''
        self.dir = ''
        self.platform = 'all'
        self.modType = 'cpp'
        self.files = []
        self.headers = []
        self.libsWin32 = []
        self.libsWin32Debug = []
        self.libsWin32Release = []
        self.libsLinux = []
        self.libsMacOSX = []
        self.frameworksMacOSX = []
        self.moduleDeps = []
        #self.nopkg = False
        self.putInPkg = True
        self.modDefFile = ''
        self.bldFile = bldFilename
        # these are deduced after reading in the bld files
        self.ancestor = None
        self.resolvedFiles = []
        self.resolvedHeaders = []
        self.className = ''
        self.codeDir = ''
        self.doxFile = True
        # modules that must be added to a target if this module is added too
        self.submodules = []
        self.baseIncDir = ''
        self.baseSrcDir = ''
        self.component = ''
    
    #--------------------------------------------------------------------------
    # Get the fully qualified name of the module, but replace all :: with _
    # so the string can be safely used as part of a filename or as part of
    # a C++ identifier.
    def GetFullNameNoColons(self):
        return self.name.replace('::', '_')
    
    #--------------------------------------------------------------------------
    def Clean(self):
        self.modDefFile = self.buildSys.CleanRelPath(self.modDefFile)
    
    #--------------------------------------------------------------------------
    # Sort files and headers
    def Finalize(self):
        self.files.sort()
        self.headers.sort()

    #--------------------------------------------------------------------------
    # Get the directory (relative to the Nebula home directory) that the 
    # compiler will need to add to the list of include directories for any 
    # source file that includes headers from this module.
    # Note that the result of os.path.join(self.GetBaseIncDir(), self.dir) is 
    # the directory where the module *.h files live.
    def GetBaseIncDir(self):
        return self.baseIncDir
        
    #--------------------------------------------------------------------------
    # Get the directory (relative to the Nebula home directory) that contains
    # the module source directory.
    # Note that the result of os.path.join(self.GetBaseSrcDir(), self.dir) is 
    # the directory where the module *.cc files live.
    def GetBaseSrcDir(self):
        return self.baseSrcDir

    #--------------------------------------------------------------------------
    # Get the base directory of the project this module resides in, the path
    # returned is absolute.
    def GetProjectDir(self):
        modProjectDir = ''
        for projectDir in self.buildSys.projectDirs:
            tempDir = os.path.commonprefix([projectDir, 
                          self.buildSys.GetAbsPathFromRel(self.bldFile)])
            if len(tempDir) > len(modProjectDir):
                modProjectDir = tempDir        

        return modProjectDir

    #--------------------------------------------------------------------------
    # Resolve the filenames in the module definition to real paths, relative
    # to the Nebula home directory.
    def ResolvePaths(self):
        #print 'Resolving paths for module ' + self.name
        
        # get the trunk dir at home/baseDir/(inc/src)/dir
        for projectDir in self.buildSys.projectDirs:
            #print projectDir
        
            # extract the last component of projectDir
            head, tail = os.path.split(string.rstrip(projectDir, os.sep))
               
            if os.path.isdir(os.path.join(projectDir, 'inc', self.dir)):
                self.codeDir = tail
                break
        
            if os.path.isdir(os.path.join(projectDir, 'src', self.dir)):
                self.codeDir = tail
                break

            # if current subdir is zombie search inside
            if 'zombie' == tail:
                zombieMod = False
                for zombieDir in self.buildSys.zombieDirs:
                    #print zombieDir
                    
                    # extract the last component of zombieDir
                    head, tail = os.path.split(string.rstrip(zombieDir, os.sep))

                    if os.path.isdir(os.path.join(zombieDir, 'inc', self.dir)):
                        self.codeDir = os.path.join('zombie', tail)
                        zombieMod = True
                        break

                    if os.path.isdir(os.path.join(zombieDir, 'src', self.dir)):
                        self.codeDir = os.path.join('zombie', tail)
                        zombieMod = True
                        break
                    
                if zombieMod:
                    break;

            # if current subdir is conjurer search inside
            if 'conjurer' == tail:
                conjurerMod = False
                for conjurerDir in self.buildSys.conjurerDirs:
                    #print conjurerDir
                    
                    # extract the last component of conjurerDir
                    head, tail = os.path.split(string.rstrip(conjurerDir, os.sep))

                    if os.path.isdir(os.path.join(conjurerDir, 'inc', self.dir)):
                        self.codeDir = os.path.join('conjurer', tail)
                        conjurerMod = True
                        break

                    if os.path.isdir(os.path.join(conjurerDir, 'src', self.dir)):
                        self.codeDir = os.path.join('conjurer', tail)
                        conjurerMod = True
                        break
                    
                if conjurerMod:
                    break;

            # if current subdir is exporter search inside
            if 'exporter' == tail:
                exporterMod = False
                for exporterDir in self.buildSys.exporterDirs:
                    #print exporterDir

                    # extract the last component of exporterDir
                    head, tail = os.path.split(string.rstrip(exporterDir, os.sep))

                    if os.path.isdir(os.path.join(exporterDir, 'inc', self.dir)):
                        self.codeDir = os.path.join('exporter', tail)
                        exporterMod = True
                        break

                    if os.path.isdir(os.path.join(exporterDir, 'src', self.dir)):
                        self.codeDir = os.path.join('exporter', tail)
                        exporterMod = True
                        break

                if exporterMod:
                    break;

            # if current subdir is renaissance search inside
            if 'renaissance' == tail:
                renaissanceMod = False
                for renaissanceDir in self.buildSys.renaissanceDirs:
                    #print renaissanceDir

                    # extract the last component of renaissanceDir
                    head, tail = os.path.split(string.rstrip(renaissanceDir, os.sep))

                    if os.path.isdir(os.path.join(renaissanceDir, 'inc', self.dir)):
                        self.codeDir = os.path.join('renaissance', tail)
                        renaissanceMod = True
                        break

                    if os.path.isdir(os.path.join(renaissanceDir, 'src', self.dir)):
                        self.codeDir = os.path.join('renaissance', tail)
                        renaissanceMod = True
                        break

                if renaissanceMod:
                    break;

        # special case to deal with dummy.cc >:|
        if '.' == self.dir:
            self.dir = ''
            self.codeDir = 'nebula2'
    
        self.baseIncDir = os.path.join('code', self.codeDir, 'inc')
        self.baseSrcDir = os.path.join('code', self.codeDir, 'src')
    
        self.resolvedFiles = []
        for srcFile in self.files:
            root, ext = os.path.splitext(srcFile)
            # no extension? add the default .cc extension
            if '' == ext:
                srcFile = srcFile + '.cc'
            resolvedPath = os.path.join(self.baseSrcDir, self.dir, srcFile)
            
            self.resolvedFiles.append(resolvedPath)            
        
        self.resolvedHeaders = []
        for hdrFile in self.headers:
            root, ext = os.path.splitext(hdrFile)
            # no extension? add the default .h extension
            if '' == ext:
                hdrFile = hdrFile + '.h'
            resolvedPath = os.path.join(self.baseIncDir, self.dir, hdrFile)
            self.resolvedHeaders.append(resolvedPath)

        # create .dox file
        if self.doxFile:
            dox_path = os.path.join('code', self.codeDir, 'dox', self.dir)           
            resolvedPath = os.path.join(dox_path, self.name + '.dox')            
            if os.path.exists(resolvedPath):        
                self.resolvedFiles.append(resolvedPath)
            resolvedPath = os.path.join(dox_path, self.name + 'class.dox')            
            if os.path.exists(resolvedPath):        
                self.resolvedFiles.append(resolvedPath)

        
    #--------------------------------------------------------------------------
    # Find and set the ancestor module, also figure out if we'll need to
    # generate a pack file for this module later.
    # Returns False if any errors occured, True otherwise.
    def FindAncestor(self):
        foundClassMacro = False
        hasAncestor = False
        hasEntityAncestor = False
        detectedError = False
        isEntity = False
        component = ''
        args = ()
        
        regexpMacroBegin = re.compile('^\s*('
                            'nNebulaRootClass|'
                            'nNebulaClassStaticInit|'
                            'nNebulaClass|'
                            'nNebulaScriptClassStaticInit|'
                            'nNebulaScriptClass|'
                            'nNebulaComponentObject|'
                            'nNebulaComponentObjectAbstract|'
                            'nNebulaComponentClass|'
                            'nNebulaComponentClassAbstact|'
                            'nNebulaEntity|'
                            'nNebulaEntityClass|'
                            'nNebulaEntityObject)' );
                            
        regexpMacroEnd = re.compile('^[a-zA-Z0-9_,"\(\)\s]*;\s*$');
        
        regexpMacroNameArgs = re.compile('^\s*('
                            'nNebulaRootClass|'
                            'nNebulaClassStaticInit|'
                            'nNebulaClass|'
                            'nNebulaScriptClassStaticInit|'
                            'nNebulaScriptClass|'
                            'nNebulaComponentObject|'
                            'nNebulaComponentObjectAbstract|'
                            'nNebulaComponentClass|'
                            'nNebulaComponentClassAbstact|'
                            'nNebulaEntity|'
                            'nNebulaEntityClass|'
                            'nNebulaEntityObject)'
                            '\(([a-zA-Z0-9_,"\(\)\s]+)\);?\s*$')
                            
        # special regular expression to handle macro arguments enclosed in (),
        # used for list of components in the entity
        regexpEntityArgs = re.compile('^\s*('
                            # normal argument
                            '([a-zA-Z0-9_"\s]+)|'
                            # argument enclosed in ()
                            '\(([a-zA-Z0-9_,"\s]+)\)'
                            ')\s*,?\s*'
                            # rest of the string
                            '([a-zA-Z0-9_,"\(\)\s]+)$')
        for fileName in self.resolvedFiles :
            if not os.path.exists(fileName):
                self.buildSys.logger.warning('%s referenced in module %s'\
                                             ' doesn\'t exist!', fileName, 
                                             self.name)
                continue
            srcFile = file(fileName, 'rU')
            accumLine = ''
            previousLine = srcFile.readline()
            while (not foundClassMacro) and (not detectedError):
                detectedError = False
                
                line = previousLine
                if '' == line:
                    break
                    
                # detect beginning of expression
                matches = regexpMacroBegin.match(line)
                if matches:
                
                    accumLine = ''
                    endOfMacro = False
                    # get the rest of the expression (until end of expression or end of file)
                    while not endOfMacro:

                        accumLine = accumLine + line
                        
                        if '' == line:
                            endOfMacro = True
                            previousLine = srcFile.readline()
                            break

                        matches = regexpMacroEnd.match(line)
                        if matches:
                            endOfMacro = True
                            previousLine = srcFile.readline()
                            break
                        else:
                            line = srcFile.readline()
                            
                    if endOfMacro:
                    
                        # print "end of macro ", accumLine, line, "\n"
                                                
                        matches = regexpMacroNameArgs.match(accumLine)
                        if matches:
                            macroStr = matches.group(1)
                            argStr = matches.group(2)
                            args = string.split(argStr, ',')
                            if 'nNebulaRootClass' == macroStr:
                                if len(args) == 1:
                                    foundClassMacro = True
                                    hasAncestor = False
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaRootClass macro in ' + fileName)
                            elif 'nNebulaClass' == macroStr:
                                if len(args) == 2:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaClass macro in ' + fileName)
                            elif 'nNebulaScriptClass' == macroStr:
                                if len(args) == 2:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaScriptClass macro in ' + fileName)
                            elif 'nNebulaClassStaticInit' == macroStr:
                                if len(args) == 3:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed ' \
                                        'nNebulaClassStaticInit macro in ' + fileName)
                            elif 'nNebulaScriptClassStaticInit' == macroStr:
                                if len(args) == 3:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed ' \
                                        'nNebulaScriptClassStaticInit macro in %s',
                                        fileName)
                            elif 'nNebulaComponentObject' == macroStr or 'nNebulaComponentObjectAbstract' == macroStr :
                                if len(args) == 2:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = 'nComponentObject'
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaComponentObject macro in ' + fileName)
                            elif 'nNebulaComponentClass' == macroStr or 'nNebulaComponentClassAbstract' == macroStr :
                                if len(args) == 2:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = 'nComponentClass'
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaComponentClass macro in ' + fileName)
                            elif 'nNebulaEntity' == macroStr:
                                parseOk = True
        
                                # array to hold the macro parsed arguments
                                macroArgs = range(12)
                                remainingStr = argStr
                                component = ''
                                for i in range(12) :
                                    matches = regexpEntityArgs.match(remainingStr)
                                    if matches :
                                        macroArgs[i] = matches.group(1)
                                        remainingStr = matches.group(4)
                                        #print "parameter ", i, matches.group(1), "\n"
                                    else:
                                        print "parameter ", i, remainingStr
                                        parseOk = False
                                        break
                                    
                                # if all macro arguments were parsed ok
                                if parseOk:
                                    foundClassMacro = False
                                    hasAncestor = False
                                    hasEntityAncestor = False
                                    
                                    entityObjectName = macroArgs[0].lower()
                                    ancestorEntityObject = macroArgs[1].lower()
                                    entityClassName = macroArgs[6].lower()
                                    ancestorEntityClass = macroArgs[7].lower()
                                    
                                    entityClassMod = Module(entityClassName, '** autogenerated **')
                                    entityClassMod.hasAncestor = True
                                    entityClassMod.hasEntityAncestor = False
                                    entityClassMod.putInPkg = True
                                    entityClassMod.className = macroArgs[6]
                                    entityClassMod.ancestorName = ancestorEntityClass
                                    entityClassMod.ancestor = self.buildSys.modules[ancestorEntityClass]
                                    self.buildSys.modules[entityClassMod.name] = entityClassMod
                                    self.submodules.append(entityClassMod)

                                    entityObjectMod = Module(entityObjectName, '** autogenerated **')
                                    entityObjectMod.hasAncestor = True
                                    entityObjectMod.hasEntityAncestor = True
                                    entityObjectMod.putInPkg = True
                                    entityObjectMod.className = macroArgs[0]
                                    entityObjectMod.ancestorName = entityClassName
                                    entityObjectMod.ancestor = entityClassMod
                                    self.buildSys.modules[entityObjectMod.name] = entityObjectMod
                                    self.submodules.append(entityObjectMod)
                                                                        
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed ' \
                                        'nNebulaEntity macro in %s macro nNebulaEntity(%s)',
                                        fileName, argStr)
                            elif 'nNebulaEntityObject' == macroStr:
                                if len(args) == 3:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = True
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed ' \
                                        'nNebulaEntityObject macro in %s',
                                        fileName)
                            elif 'nNebulaEntityClass' == macroStr:
                                if len(args) == 2:
                                    foundClassMacro = True
                                    hasAncestor = True
                                    hasEntityAncestor = False
                                    component = ''
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Malformed '\
                                        'nNebulaEntityClass macro in ' + fileName)
                                        
                            if hasAncestor:
                                ancestorName = string.strip(args[1], '" ')
                                ancestorName = ancestorName.lower()
                                if ancestorName in self.buildSys.modules:
                                    if hasEntityAncestor == 1:
                                        ancestorName = string.strip(args[2], '" ')
                                        self.ancestor = self.buildSys.modules[ancestorName];
                                    else:
                                        self.ancestor = self.buildSys.modules[ancestorName];
                                else:
                                    detectedError = True
                                    self.buildSys.logger.error('Undefined module %s' \
                                        ' referenced in a nNebula*Class* macro in %s',
                                        ancestorName, fileName) 
                            self.className = string.strip(args[0], '" ')
                            self.component = component
                else:
                    previousLine = srcFile.readline()
            srcFile.close()
            if foundClassMacro or detectedError:
                break

        # if there is no class macro found the module won't be in a pkg
        if not foundClassMacro:
            #print 'Warning: module ' + self.name + ' is missing a class ' \
            #      'macro so it won\'t be added to a pkg!'
            self.putInPkg = False
        
        return not detectedError
        
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
