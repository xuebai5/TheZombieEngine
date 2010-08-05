#--------------------------------------------------------------------------
# Visual Studio 2005 (8.0) Project & Solution Generator
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, string
import buildsys3.guid
import re

#--------------------------------------------------------------------------

STR_PROJECT_HEADER = '''\
<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject
\tProjectType="Visual C++"
\tVersion="8,00"
\tName="%(targetName)s"
\tProjectGUID="%(targetUUID)s"
\tKeyword="Win32Proj"
\t>
\t<Platforms>
\t\t<Platform 
\t\t\tName="Win32"
\t\t/>
\t</Platforms>
\t<ToolFiles>
\t</ToolFiles>    
'''

STR_PROJECT_FOOTER = '''\
\t<Globals>
\t</Globals>
</VisualStudioProject>
'''

STR_SRC_FILE = '''\
\t\t\t<File
\t\t\t\tRelativePath="%(relPath)s"
\t\t\t\t>
\t\t\t</File>
'''

STR_C_FILE = '''\
\t\t\t<File
\t\t\t\tRelativePath="%(relPath)s"
\t\t\t\t>
\t\t\t\t<FileConfiguration 
\t\t\t\t\tName="Debug|Win32"
\t\t\t\t>
\t\t\t\t\t<Tool 
\t\t\t\t\t\tName="VCCLCompilerTool" 
\t\t\t\t\t\tCompileAs="1" />
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Debug Optimized|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool
\t\t\t\t\t\tName="VCCLCompilerTool"
\t\t\t\t\t\tCompileAs="1"
\t\t\t\t\t\t/>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Release|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool 
\t\t\t\t\t\tName="VCCLCompilerTool"
\t\t\t\t\t\tCompileAs="1"
\t\t\t\t\t\t/>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Release Asserts|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool
\t\t\t\t\t\tName="VCCLCompilerTool"
\t\t\t\t\t\tCompileAs="1"
\t\t\t\t\t\t/>
\t\t\t\t</FileConfiguration>
\t\t\t</File>
'''

STR_BLD_FILE = '''\
\t\t\t<File
\t\t\t\tRelativePath="%(relPath)s"
\t\t\t\t>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Debug|Win32"
\t\t\t\t\tExcludedFromBuild="true"
\t\t\t\t\t>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Debug Optimized|Win32"
\t\t\t\t\tExcludedFromBuild="true"
\t\t\t\t\t>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Release|Win32"
\t\t\t\t\tExcludedFromBuild="true"
\t\t\t\t\t>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Release Asserts|Win32"
\t\t\t\t\tExcludedFromBuild="true"
\t\t\t\t\t>
\t\t\t\t</FileConfiguration>
\t\t\t</File>
'''

STR_PCH_FILE = '''\
\t\t\t<File
\t\t\t\tRelativePath="%(relPath)s"
\t\t\t\t>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Debug|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool 
\t\t\t\t\t\tName="VCCLCompilerTool" 
\t\t\t\t\t\tUsePrecompiledHeader="1" />
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Debug Optimized|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool
\t\t\t\t\tName="VCCLCompilerTool"
\t\t\t\t\t\tUsePrecompiledHeader="1"/>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\tName="Release|Win32"
\t\t\t\t\t>
\t\t\t\t\t<Tool
\t\t\t\t\t\tName="VCCLCompilerTool"
\t\t\t\t\t\tUsePrecompiledHeader="1"/>
\t\t\t\t</FileConfiguration>
\t\t\t\t<FileConfiguration
\t\t\t\t\t\tName="Release Asserts|Win32"
\t\t\t\t\t\t>
\t\t\t\t\t<Tool Name="VCCLCompilerTool"
\t\t\t\t\t\tUsePrecompiledHeader="1" />
\t\t\t\t</FileConfiguration>
\t\t\t</File>
'''

STR_SLN_PROJECT_CONFIGS = '''\
\t\t{%(uuid)s}.Debug|Win32.ActiveCfg = Debug|Win32
\t\t{%(uuid)s}.Debug|Win32.Build.0 = Debug|Win32
\t\t{%(uuid)s}.Debug Optimized.ActiveCfg = Debug Optimized|Win32
\t\t{%(uuid)s}.Debug Optimized.Build.0 = Debug Optimized|Win32
\t\t{%(uuid)s}.Release|Win32.ActiveCfg = Release|Win32
\t\t{%(uuid)s}.Release|Win32.Build.0 = Release|Win32
\t\t{%(uuid)s}.Release Asserts.ActiveCfg = Release Asserts|Win32
\t\t{%(uuid)s}.Release Asserts.Build.0 = Release Asserts|Win32
'''

STR_SLN_GLOBAL_HEADER = '''\
Global
\tGlobalSection(SolutionConfigurationPlatforms) = preSolution
\t\tDebug|Win32 = Debug|Win32
\t\tDebug Optimized|Win32 = Debug Optimized|Win32
\t\tRelease|Win32 = Release|Win32
\t\tRelease Asserts|Win32 = Release Asserts|Win32
\tEndGlobalSection
\tGlobalSection(ProjectConfigurationPlatforms) = postSolution
'''

STR_SLN_GLOBAL_FOOTER = '''\
\tEndGlobalSection
\tGlobalSection(SolutionProperties) = preSolution
\t\tHideSolutionNode = FALSE
\tEndGlobalSection
EndGlobal
'''

STR_PROJ_CONFIG_DEBUG = '''\
\t\t<Configuration
\t\t\tName="%(configName)s"
\t\t\tOutputDirectory="%(binaryDir)s"
\t\t\tIntermediateDirectory="%(interDir)s"
\t\t\tConfigurationType="%(configType)i"
\t\t\tCharacterSet="2"
\t\t\t>
\t\t\t<Tool
\t\t\t\tName="VCCLCompilerTool"
\t\t\t\tAdditionalOptions="%(addOptions)s"
\t\t\t\tOptimization="0"
\t\t\t\tWholeProgramOptimization="false"
\t\t\t\tAdditionalIncludeDirectories="%(incDirs)s"
\t\t\t\tPreprocessorDefinitions="__WIN32__;%(defs)s"
\t\t\t\tStringPooling="true"
\t\t\t\tMinimalRebuild="true"
\t\t\t\tExceptionHandling="%(exceptions)s"
\t\t\t\tUsePrecompiledHeader = "%(pchlevel)s"
\t\t\t\tBasicRuntimeChecks="3"
\t\t\t\tSmallerTypeCheck="true"
\t\t\t\tRuntimeLibrary="3"
\t\t\t\tBufferSecurityCheck="true"\t\t\t\t
\t\t\t\tEnableFunctionLevelLinking="false"\t\t\t\t
\t\t\t\tFloatingPointModel="0"
\t\t\t\tFloatingPointExceptions="true"\t\t\t\t
\t\t\t\tForceConformanceInForLoopScope="false"
\t\t\t\tRuntimeTypeInfo="%(rtti)s"\t\t\t\t
\t\t\t\tOpenMP="false"
\t\t\t\tPrecompiledHeaderThrough="%(pchfile)s"\t\t\t\t
\t\t\t\tWarningLevel="%(warningsLevel)s"
\t\t\t\tDetect64BitPortabilityProblems="true"\t\t\t\t
\t\t\t\tDebugInformationFormat="3"
\t\t\t\tUseFullPaths="false"\t\t\t
\t\t\t/>
'''
STR_PROJ_CONFIG_DEBUG_OPTIMIZED = '''\
\t\t<Configuration
\t\t\tName="%(configName)s"
\t\t\tOutputDirectory="%(binaryDir)s"
\t\t\tIntermediateDirectory="%(interDir)s"
\t\t\tConfigurationType="%(configType)i"
\t\t\tCharacterSet="2"
\t\t\tWholeProgramOptimization="1"
\t\t\t>
\t\t\t<Tool
\t\t\t\tName="VCCLCompilerTool"
\t\t\t\tAdditionalOptions="%(addOptions)s"                
\t\t\t\tOptimization="2"
\t\t\t\tInlineFunctionExpansion="2"
\t\t\t\tEnableIntrinsicFunctions="true"
\t\t\t\tFavorSizeOrSpeed="1"
\t\t\t\tOmitFramePointers="true"\t\t\t\t
\t\t\t\tWholeProgramOptimization="true"
\t\t\t\tAdditionalIncludeDirectories="%(incDirs)s"
\t\t\t\tPreprocessorDefinitions="__WIN32__;%(defs)s"
\t\t\t\tStringPooling="true"
\t\t\t\tMinimalRebuild="true"
\t\t\t\tExceptionHandling="%(exceptions)s"
\t\t\t\tUsePrecompiledHeader="%(pchlevel)s"
\t\t\t\tBasicRuntimeChecks="0"
\t\t\t\tRuntimeLibrary="3"
\t\t\t\tBufferSecurityCheck="false"
\t\t\t\tEnableFunctionLevelLinking="true"\t\t\t\t
\t\t\t\tEnableEnhancedInstructionSet="1"
\t\t\t\tFloatingPointModel="2"
\t\t\t\tFloatingPointExceptions="false"\t\t\t\t
\t\t\t\tForceConformanceInForLoopScope="false"
\t\t\t\tRuntimeTypeInfo="%(rtti)s"\t\t\t\t
\t\t\t\tOpenMP="false"
\t\t\t\tPrecompiledHeaderThrough="%(pchfile)s"
\t\t\t\tWarningLevel="%(warningsLevel)s"
\t\t\t\tDetect64BitPortabilityProblems="true"
\t\t\t\tDebugInformationFormat="3"\t\t\t\t
\t\t\t\tUseFullPaths="false"
\t\t\t/>
'''

STR_PROJ_CONFIG_RELEASE = '''\
\t\t<Configuration
\t\t\tName="%(configName)s"
\t\t\tOutputDirectory="%(binaryDir)s"
\t\t\tIntermediateDirectory="%(interDir)s"
\t\t\tConfigurationType="%(configType)i"
\t\t\tCharacterSet="2"
\t\t\tWholeProgramOptimization="1"
\t\t\t>
\t\t\t<Tool
\t\t\t\tName="VCCLCompilerTool"
\t\t\t\tAdditionalOptions="%(addOptions)s"                
\t\t\t\tOptimization="2"
\t\t\t\tInlineFunctionExpansion="2"
\t\t\t\tEnableIntrinsicFunctions="true"
\t\t\t\tFavorSizeOrSpeed="1"\t\t\t\t\t\t\t\t
\t\t\t\tOmitFramePointers="false"\t\t\t\t
\t\t\t\tWholeProgramOptimization="true"
\t\t\t\tAdditionalIncludeDirectories="%(incDirs)s"
\t\t\t\tPreprocessorDefinitions="__WIN32__;%(defs)s"
\t\t\t\tStringPooling="true"
\t\t\t\tMinimalRebuild="true"
\t\t\t\tExceptionHandling="%(exceptions)s"
\t\t\t\tUsePrecompiledHeader="%(pchlevel)s"
\t\t\t\tBasicRuntimeChecks="0"
\t\t\t\tRuntimeLibrary="2"
\t\t\t\tBufferSecurityCheck="false"
\t\t\t\tEnableFunctionLevelLinking="true"\t\t\t\t
\t\t\t\tEnableEnhancedInstructionSet="1"
\t\t\t\tFloatingPointModel="2"
\t\t\t\tFloatingPointExceptions="false"\t\t\t\t
\t\t\t\tForceConformanceInForLoopScope="false"
\t\t\t\tRuntimeTypeInfo="%(rtti)s"\t\t\t\t
\t\t\t\tOpenMP="false"
\t\t\t\tPrecompiledHeaderThrough="%(pchfile)s"
\t\t\t\tWarningLevel="%(warningsLevel)s"
\t\t\t\tDetect64BitPortabilityProblems="true"
\t\t\t\tDebugInformationFormat="3"
\t\t\t\tUseFullPaths="false"\t\t\t\t
\t\t\t/>
'''

STR_PROJ_CONFIG_LIBRARIAN_TOOL = '''\
\t\t\t<Tool
\t\t\t\tName="VCLibrarianTool"
\t\t\t\tOutputFile="%s"/>
'''

STR_PROJ_CONFIG_LINKER_TOOL_S1 = '''\
\t\t\t<Tool
\t\t\t\tName="VCLinkerTool"
\t\t\t\tUseLibraryDependencyInputs="true"
\t\t\t\tAdditionalDependencies="%(win32Libs)s"
\t\t\t\tOutputFile="%(outputFile)s"
\t\t\t\tAdditionalOptions="%(addOptions)s"
\t\t\t\tStackReserveSize="%(stackSize)s"
'''

STR_PROJ_CONFIG_LINKER_TOOL_S2_DEBUG = '''\
\t\t\t\tGenerateDebugInformation="true"
\t\t\t\tLinkIncremental="2"
\t\t\t\tProgramDatabaseFile="%s"
'''

STR_PROJ_CONFIG_LINKER_TOOL_S2_RELEASE = '''\
\t\t\t\tGenerateDebugInformation="true"
\t\t\t\tLinkIncremental="2"
\t\t\t\tOptimizeReferences="2"
\t\t\t\tShowProgress="0"
\t\t\t\tProgramDatabaseFile="%s"\t\t\t\t
'''

STR_PROJ_CONFIG_LINKER_TOOL_S3 = '''\
\t\t\t\tAdditionalLibraryDirectories="%(libDirs)s"
\t\t\t\tModuleDefinitionFile="%(modDef)s"
\t\t\t\tIgnoreDefaultLibraryNames="libcp.lib %(ignoreLibs)s"
\t\t\t\tSubSystem="0"
\t\t\t\tTargetMachine="1"/>
'''

STR_PROJ_CONFIG_OTHER_TOOLS = '''\
\t\t\t<Tool Name="VCMIDLTool"/>
\t\t\t<Tool Name="VCPostBuildEventTool"/>
\t\t\t<Tool Name="VCPreBuildEventTool"/>
\t\t\t<Tool Name="VCPreLinkEventTool"/>
\t\t\t<Tool Name="VCWebServiceProxyGeneratorTool"/>
\t\t\t<Tool Name="VCXMLDataGeneratorTool"/>
\t\t\t<Tool Name="VCManagedWrapperGeneratorTool"/>
\t\t\t<Tool Name="VCAuxiliaryManagedWrapperGeneratorTool"/>
'''

STR_PROJ_CONFIG_TOOL_RES_COMPILER_EXE = '''\
\t\t\t<Tool
\t\t\t\tName="VCResourceCompilerTool"
\t\t\t\tPreprocessorDefinitions="_DEBUG"
\t\t\t\tCulture="1033"/>
'''

STR_PROJ_CONFIG_TOOL_RES_COMPILER = '''\
\t\t\t<Tool Name="VCResourceCompilerTool"/>
'''

#--------------------------------------------------------------------------
class vstudio8:
    
    #--------------------------------------------------------------------------
    def __init__(self, buildSys):
        self.buildSys = buildSys
        self.description = 'Microsoft Visual Studio 2005 (8.0) ' \
                           'Project & Solution Generator. This generator also ' \
                           'works with the Visual C++ 2005 Express Edition.'
        self.vcprojLocation = ''
        self.incDirStr = ''
        self.libDirStr = ''
        self.globalCplOptions = ''  # Options for all workspaces        
        
    #--------------------------------------------------------------------------
    def HasSettings(self):
        return False
        
    #--------------------------------------------------------------------------
    def Generate(self, workspaceNames):
        defaultLocation = os.path.join('build', 'vstudio8')
        
        progressVal = 0
        solutionAbsDirs = []
        self.buildSys.CreateProgressDialog('Generating Solutions', '', 
                                           len(workspaceNames))
        
        try:
            for workspaceName in workspaceNames:
                workspace = self.buildSys.workspaces[workspaceName]
                solutionLocation = os.path.join(defaultLocation, workspace.name)                
                
                # calculate these once for the workspace
                self.vcprojLocation = workspace.GetWorkspacePath(solutionLocation)
                self.incDirStr = workspace.GetIncSearchDirsString(solutionLocation)                                
                self.libDirStr = workspace.GetLibSearchDirsString('win32_vc_i386',
                                                                  solutionLocation)                
                
                # make sure the workspace/projects directory exists
                absPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation)
                if not os.path.exists(absPath):
                    os.makedirs(absPath)
                
                self.buildSys.UpdateProgressDialog(progressVal,
                    'Generating %s...' % workspaceName)
                if self.buildSys.ProgressDialogCancelled():
                    break
                
                # spit out the files
                self.GenerateSolution(workspace)
                for targetName in workspace.targets:
                    self.GenerateProject(self.buildSys.targets[targetName],
                                         workspace)
                                        
                absPath = absPath.lower()
                if absPath not in solutionAbsDirs:
                    solutionAbsDirs.append(absPath)
                                        
                progressVal += 1
        except:
            self.buildSys.logger.exception('Exception in vstudio8.Generate()')
        
        self.buildSys.DestroyProgressDialog()
        
        summaryDetails = { 'numOfWorkspacesBuilt' : progressVal,
                           'totalNumOfWorkspaces' : len(workspaceNames) }
        self.buildSys.DisplaySummaryDialog(summaryDetails)
        
        for absPath in solutionAbsDirs:
            os.startfile(absPath)
        
    #--------------------------------------------------------------------------
    # .sln files aren't in XML, they require the use of tabs instead of spaces.
    def GenerateSolution(self, workspace):
        self.buildSys.logger.info('Generating VS.NET 2005 solution %s',
                                  workspace.name)

        # write .sln file
        solutionPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation, 
                                    workspace.name + '.sln')
        try:
            slnFile = file(solutionPath, 'w')
        except IOError:
            self.buildSys.logger.error("Couldn't open %s for writing.", 
                                       solutionPath)
        else:
            # header
            slnFile.write('Microsoft Visual Studio Solution File, Format Version 9.00\n' \
                          '# Visual C++ Express 2005\n')

            # get a uuid for each project
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                if '' == target.uuid: # may have been generated previously
                    if target.SupportsPlatform('win32'):
                        #print 'Target ' + target.name + ' supports win32.'
                        target.uuid = self.GenerateUUID()
    
            # now with the UUIDs we can write the targets
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                slnFile.write('Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "%s", "%s.vcproj", "{%s}"\n' \
                              % (targetName, targetName, target.uuid))
                slnFile.write('\tProjectSection(ProjectDependencies) = postProject\n')
                for targetDepName in target.depends:
                    targetDep = self.buildSys.targets[targetDepName]
                    if targetDep.SupportsPlatform('win32'):
                        slnFile.write('\t\t{%s} = {%s}\n' \
                                      % (targetDep.uuid, targetDep.uuid))
                slnFile.write('\tEndProjectSection\n')
                slnFile.write('EndProject\n')
    
            slnFile.write(STR_SLN_GLOBAL_HEADER)
            # configurations
            for targetName in workspace.targets:
                target = self.buildSys.targets[targetName]
                if target.SupportsPlatform('win32'):
                    slnFile.write(STR_SLN_PROJECT_CONFIGS \
                                  % {'uuid' : target.uuid })
            slnFile.write(STR_SLN_GLOBAL_FOOTER)

            slnFile.close()
        
    #--------------------------------------------------------------------------
    def GenerateProject(self, target, workspace):
        print 'Generating VS.NET 2005 project: ' + target.name

        # write .vcproj file
        projPath = os.path.join(self.buildSys.homeDir, self.vcprojLocation, 
                                target.name + '.vcproj')
        try:
            projFile = file(projPath, 'w')
        except IOError:
            self.buildSys.logger.error('Failed to open %s for writing!', 
                                       projPath)
        else:
            self.writeProjectHeader(target, projFile)
            projFile.write('\t<Configurations>\n')
            self.writeProjectConfig(target, workspace, projFile, 1)
            self.writeProjectConfig(target, workspace, projFile, 2)
            self.writeProjectConfig(target, workspace, projFile, 3)
            self.writeProjectConfig(target, workspace, projFile, 4)
            projFile.write('\t</Configurations>\n' \
                           '\t<References>\n' \
                           '\t</References>\n')
            self.writeProjectFiles(target, projFile)
            self.writeProjectFooter(projFile)
            projFile.close()
    
    #--------------------------------------------------------------------------
    # Generates and returns a UUID (a string).
    # If an error occured the return value will be an empty string.
    def GenerateUUID(self):
        uuidStr = buildsys3.guid.GenerateGUID()
        if uuidStr != '':
            uuidStr = string.upper(string.strip(uuidStr))
        return uuidStr
        
    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------
        
    #--------------------------------------------------------------------------
    def writeProjectHeader(self, target, projFile):
        args = { 'targetName' : target.name,
                 'targetUUID' : target.uuid }
        projFile.write(STR_PROJECT_HEADER % args)

    #--------------------------------------------------------------------------
    def writeProjectConfig(self, target, workspace, projFile, debugMode):
        defStr = '' # preprocessor definitions

        modDefFileName = target.modDefFile
        if '' != modDefFileName:
            modDefFileName = self.buildSys.FindRelPath(self.vcprojLocation,
                                                       modDefFileName)

        configType = 0
        extension = target.GetExtension('win32')
        prefix = ''
        if 'lib' == target.type:
            configType = 4
            if debugMode == 1 or debugMode == 2:            
                prefix = 'd_'
            defStr = 'N_STATIC;'
        elif 'dll' == target.type:
            configType = 2
        elif 'exe' == target.type:
            configType = 1
            defStr = 'N_STATIC;'

        defStr += workspace.GetTargetDefsStringForTarget(target.name)
        
        interDir = 'inter'
        binaryDir = workspace.GetBinaryOutputPath('bin')
        binaryDir = self.buildSys.FindRelPath(self.vcprojLocation, binaryDir)
        
        libraryDir = self.buildSys.FindRelPath(self.vcprojLocation, workspace.GetLibraryOutputPath('lib'))
        libInDirs = os.path.join(libraryDir, 'win32', 'vstudio8') + ';' + os.path.join(libraryDir, 'win32')
        
        configName = ''
        win32Libs = ''
        extraOptions = ''
        wholeopt = "false"        
        
        exceptions = 0
        if target.exceptions :
            exceptions = 1                

        if debugMode == 1:
            configName = 'Debug|Win32'
            interDir = os.path.join(interDir, 'win32d')
            binaryDir = os.path.join(binaryDir, 'win32d')
            libraryDir = os.path.join(libraryDir, 'win32d')
            firstLib = True
            for lib in target.libsWin32DebugAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;_DEBUG;NT_PLUGIN;__BUILD_CONF__=1;UNHANDLE_EXCEPTION_ON_ASSERT;__NEBULA_STATS__'
        elif debugMode == 2:
            configName = 'Debug Optimized|Win32'
            libInDirs = os.path.join(libraryDir, 'win32d', 'vstudio8') + ';' + os.path.join(libraryDir, 'win32d') + libInDirs
            interDir = os.path.join(interDir, 'win32do')
            binaryDir = os.path.join(binaryDir, 'win32do')
            libraryDir = os.path.join(libraryDir, 'win32do')
            firstLib = True
            for lib in target.libsWin32DebugAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;_DEBUG;NT_PLUGIN;__BUILD_CONF__=2;UNHANDLE_EXCEPTION_ON_ASSERT;__NEBULA_STATS__'
        elif debugMode == 3:
            configName = 'Release Asserts|Win32'
            interDir = os.path.join(interDir, 'win32a')
            binaryDir = os.path.join(binaryDir, 'win32a')
            libraryDir = os.path.join(libraryDir, 'win32a')
            firstLib = True
            for lib in target.libsWin32ReleaseAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;NT_PLUGIN;NDEBUG;__BUILD_CONF__=3;UNHANDLE_EXCEPTION_ON_ASSERT;__NEBULA_STATS__'
            extraOptions = "/Oy- /GA /GF"
            wholeopt = "TRUE"
        elif debugMode == 4:
            configName = 'Release|Win32'
            interDir = os.path.join(interDir, 'win32')
            binaryDir = os.path.join(binaryDir, 'win32')
            libraryDir = os.path.join(libraryDir, 'win32')
            firstLib = True
            for lib in target.libsWin32ReleaseAll:
                if not firstLib:
                    win32Libs += ' '
                win32Libs += lib
                firstLib = False
            defStr += 'WIN32;NT_PLUGIN;NDEBUG;__BUILD_CONF__=4;UNHANDLE_EXCEPTION_ON_ASSERT;__NEBULA_NO_ASSERT__;__NEBULA_NO_LOG__'
            extraOptions = "/Oy- /GA /GF"
            wholeopt = "TRUE"
        else:
            print "Bad configuration..."            

        # get the additional options for compiler
        self.cplAddStr = ''
        for option in self.buildSys.globalCplOptions:
            if len(option) > 0:
                self.cplAddStr = self.cplAddStr + str(option)[1:-1] + " " 

        pchlevel = target.pchlevel
        # VS8 has changed the numbering of precompiledheader option
        if (pchlevel == "3"):
            pchlevel = 2

        args = { 'configName' : configName,
                 'binaryDir'  : binaryDir,
                 'interDir'   : os.path.join(interDir, target.name),
                 'configType' : configType,
                 'incDirs'    : self.incDirStr,
                 'defs'       : defStr,
                 'addOptions' : self.cplAddStr + str(extraOptions),
                 'exceptions' : str(exceptions),
                 'wholeopt'   : str(wholeopt),
                 'pchlevel'   : str(pchlevel) }

        pchfile = target.pchfile
        root, ext = os.path.splitext(pchfile)
        if '' == ext:
            pchfile = target.pchfile + '.h'

        if debugMode == 1:
            args.update(
                   { 'rtti'          : string.lower(str(target.rtti)),
                     'warningsLevel' : str(target.GetWarningsLevel('win32')),
                     'pchlevel'      : str(str(target.pchlevel)),
                     'pchfile'       : str(str(pchfile)) } )
            projFile.write(STR_PROJ_CONFIG_DEBUG % args)
        elif debugMode == 2:
            args.update( 
                   { 'rtti'          : string.lower(str(target.rtti)),
                     'warningsLevel' : str(target.GetWarningsLevel('win32')),
                     'pchlevel'      : str(str(target.pchlevel)),
                     'pchfile'       : str(str(pchfile)) } )
            projFile.write(STR_PROJ_CONFIG_DEBUG_OPTIMIZED % args)
        else:
            args.update( 
                   { 'rtti'          : string.lower(str(target.rtti)),
                     'warningsLevel' : str(target.GetWarningsLevel('win32')),
                     'pchlevel'      : str(str(target.pchlevel)),
                     'pchfile'       : str(str(pchfile)) } )
            projFile.write(STR_PROJ_CONFIG_RELEASE % args)
    
        projFile.write('\t\t\t<Tool Name="VCCustomBuildTool"/>\n')

        # now either the linker or the librarian
        libInDirs = libraryDir + ';' + libInDirs + ';' + self.libDirStr
        libraryDir = os.path.join(libraryDir, 'vstudio8')
        if 'lib' == target.type:
            projFile.write(STR_PROJ_CONFIG_LIBRARIAN_TOOL \
                           % os.path.join(libraryDir, 
                                 prefix + target.name + '.' + extension))
        else:
            # get the additional options for linker
            self.lnkAddStr = ''
            for option in target.linkerOptions:
                self.lnkAddStr = self.lnkAddStr + str(option)[1:-1] + " "
        
            args = { 'win32Libs'  : win32Libs,
                     'outputFile' : os.path.join(binaryDir,
                                        target.name + '.' + extension),
                     'addOptions' : self.lnkAddStr,
                     'stackSize'  : target.stacksize  }
            projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S1 % args)
            if debugMode == 1 or debugMode == 2:
                projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S2_DEBUG \
                               % os.path.join(interDir, target.name + '.pdb'))
            else:
                projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S2_RELEASE \
                               % os.path.join(interDir, target.name + '.pdb'))                
            
            if debugMode == 1 or debugMode == 2:
                ignoreLibs = "libc.lib;libcmt.lib;msvcrt.lib;libcd.lib;libcmtd.lib"
            else:
                ignoreLibs = "libc.lib;libcmt.lib;libcd.lib;libcmtd.lib;msvcrtd.lib"
            args = { 'libDirs'    : libraryDir + ';' + libInDirs,
                     'modDef'     : modDefFileName,
                     'ignoreLibs' : ignoreLibs }
                    
            projFile.write(STR_PROJ_CONFIG_LINKER_TOOL_S3 % args)

        # the other tools
        projFile.write(STR_PROJ_CONFIG_OTHER_TOOLS)

        if 'exe' == target.type:
            projFile.write(STR_PROJ_CONFIG_TOOL_RES_COMPILER_EXE)
        else:
            projFile.write(STR_PROJ_CONFIG_TOOL_RES_COMPILER)

        projFile.write('\t\t</Configuration>\n')
        
    #--------------------------------------------------------------------------
    def writeProjectFiles(self, target, projFile):
        projFile.write('\t<Files>\n')

        # generate a module for the bld
        target.GenerateBldMod()
        
        for moduleName in target.modules:
            module = self.buildSys.modules[moduleName]
            
            # ignore it if there are no files in the module
            if (len(module.resolvedFiles) == 0) and (len(module.resolvedHeaders) == 0):
                continue
            
            safeModName = module.GetFullNameNoColons()
            #nebSyms = 'N_INIT=n_init_' + safeModName + ';' \
            #          'N_NEW=n_new_' + safeModName + ';' \
            #          'N_INITCMDS=n_initcmds_' + safeModName
            nebSyms = ''
                    
            # source files
            projFile.write('''
\t\t<Filter
\t\t\tName="%s"
\t\t\tFilter="cpp;c;cxx;cc;h;hxx;hcc"
\t\t\t>
'''             % module.name)
            for fileName in module.resolvedFiles:
                root, ext = os.path.splitext(fileName) 
                templ = STR_SRC_FILE
                if module.modType == 'c' :
                    templ = STR_C_FILE
                excludeFile = "false"
                if '.bld' == ext or '.dox' == ext:
                    templ = STR_BLD_FILE
                relPath = self.buildSys.FindRelPath(self.vcprojLocation, 
                                                    fileName)
                
                pchlevel = target.pchlevel
                
                # change the precompiled to /Yc if needed and create files                
                if (pchlevel != "0"):                     
                    if (re.compile('\\\\(pch.*\.(cc|cpp)|' + target.pchfile + '\.(cc|cpp))').search(fileName)):                        
                        pchlevel = 1
                        templ = STR_PCH_FILE
                        if not os.path.exists(fileName):
                            f = file(fileName, 'w')
                            f.write('#include "' + str(target.pchfile) + '"')
                            f.close() 

                ignore, shortFileName = os.path.split(fileName)
                objectName, ignore = os.path.splitext(shortFileName)
                args = { 'relPath'       : relPath }
                projFile.write(templ % args)


            # header files
            for fileName in module.resolvedHeaders:
                # crate file if is a precompiled header
                if (re.compile('\\\\pch.*\.h').search(fileName)):
                    if not os.path.exists(fileName):
                        f = file(fileName, 'w')
                        f.write('#if _MSC_VER > 1000\n')
                        f.write('#pragma once\n')
                        f.write('#endif // _MSC_VER > 1000\n')
                        f.close()
                    
                # Special file for personal configurations
                elif (re.compile('\\\\personal.h').search(fileName)):
                    if not os.path.exists(fileName):                        
                        f = file(fileName, 'w')
                        f.write("#ifndef N_PERSONAL_H\n#define N_PERSONAL_H\n")
                        f.write("/****************************************************\n")
                        f.write(" This file is for personal configuration purposes\n")
                        f.write(" here you can add all this defines that only you want\n")
                        f.write(" to use but you don't know where put them ;)\n")
                        f.write("*****************************************************/\n\n#endif")
                        f.close()
                relPath = self.buildSys.FindRelPath(self.vcprojLocation, 
                                                fileName)
                projFile.write('\t\t\t<File\n\t\t\t\tRelativePath="%s"\n\t\t\t\t>\n\t\t\t</File>\n' \
                               % relPath)           
            projFile.write('\t\t</Filter>\n')
            
        # if the module definition file is set and target is dll then add it
        if ('' != target.modDefFile) and ('dll' == target.type):
            relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                target.modDefFile)
            projFile.write('\t\t<Filter Name="Module Definition" Filter="def">\n' \
                           '\t\t\t<File RelativePath="%s"/>\n' \
                           '\t\t</Filter>\n' % relPath)

        # resource files
        if ('exe' == target.type) or ('dll' == target.type):
            projFile.write('\t\t<Filter Name="Resource Files" Filter="rc">\n')
            # add standard nebula rsrc to exe
            if 'exe' == target.type:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                    os.path.join('build',
                                                                 'pkg'))
                #print 'Relative Resource Path: ' + relPath
                relPath = os.path.join(relPath, 'res_' + target.name + '.rc')
                projFile.write('\t\t\t<File RelativePath="%s"/>\n' \
                               % relPath)
            # add any custom resource files
            if '' != target.win32Resource:
                relPath = self.buildSys.FindRelPath(self.vcprojLocation,
                                                    target.win32Resource)
                projFile.write('\t\t\t<File RelativePath="%s"/>\n' \
                               % relPath)
            projFile.write('\t\t</Filter>\n')
            
        projFile.write('\t</Files>\n')
        
    #--------------------------------------------------------------------------
    def writeProjectFooter(self, projFile):
        projFile.write(STR_PROJECT_FOOTER)
    
#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
