#--------------------------------------------------------------------------
# cmdparser.py
# *_cmds.cc file parser
# (c) 2005 Oleg Kreptul (Haron)
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import re, os
from types import *
import pdb

COMMENT_MINUS_NUM = 80
COMMENT_LINE = "//" + "-" * COMMENT_MINUS_NUM
PARAM_COMMENT_INDENT = 10
TABSIZE = 4
TABSPACES = " " * TABSIZE

TYPES_LIST = ['i', 's', 'b', 'f', 'ff', 'fff', 'ffff', 'o', 'l','q']
TYPE_CHARS = 'vifsbolq'

PARAMDESC_REGEX          = re.compile(r'^\s*([' + TYPE_CHARS + r']+)\s*\(\s*(\w[\w\d ]*)*\s*(;\s*\w[\w\d]*\s*=\s*.*)*\)\s*(?:--\s*(.+)\s*)?$')
ATTRVAL_REGEX            = re.compile(r'^\s*(\w[\w\d]*)\s*=\s*(.+?\s*(,.+?)?)\s*$')
BASEINFO_REGEX           = re.compile(r'^\s*\/\/(.+)')
INCLUDE_REGEX            = re.compile(r'^\s*#\s*include\s*"(.+)"\s*$')
CMDFUNCTIONDECL_REGEX    = re.compile(r'^.*n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*;')
INITCMDFUNCTIONDEF_REGEX = re.compile(r'^.*n_initcmds.*\(.*\)\s*')
ADDCMDFUNCTION_REGEX     = re.compile(r'^\s*\w[\w\d]*\s*->\s*AddCmd\s*\(\s*"([' + TYPE_CHARS + r']+)_([a-zA-Z][\w\d]+)_([' + TYPE_CHARS + r']+)"\s*,\s*\'(....)\'\s*,\s*n_([a-zA-Z][\w\d]+)\s*\)\s*;')
CMDDOC_REGEX             = re.compile(r'^\s*\/\*\*(.*)$')
CMDFUNCTIONDEF_REGEX     = re.compile(r'^.*n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*(\{)?')
SAVECMDDEF_REGEX         = re.compile(r'^\s*(bool)?\s*(.+?)::SaveCmds\s*\(.*\)\s*(\{)?')
INITCMDMACRODEF_REGEX    = re.compile(r'^\s*NSCRIPT_INITCMDS_BEGIN\s*\(\s*([a-zA-Z][\w\d]+)\s*\)')
#INITCMDMACRODEF_REGEX    = re.compile(r'^NCMDPROTONATIVECPP_INITCMDS_BEGIN\s*\(\s*([a-zA-Z][\w\d]+)\s*\)\s*$')
#CMDMACRODEF_REGEX        = re.compile(r'^\s*NCMDPROTONATIVECPP_ADDCMD\s*\(([a-zA-Z][\w\d]+)\);\s*$')
INITCLASSCMDDEF_REGEX    = re.compile(r'^.+::InitClass\(nClass\s*\*\s+cl\)\s*$')
NCLASSDEF_REGEX          = re.compile(r'^class ([a-zA-Z][\w\d]+)\s*:.*public ([a-zA-Z][\w\d]+)')
#NCOMPONENTCLASSDEF_REGEX = re.compile(r'^class (nc[a-zA-Z][\w\d]+Class)\s*:.*public (n[a-zA-Z][\w\d]+)')
NNEBULAENTITY_REGEX      = re.compile(r'^\s*nNebulaEntity\(')
NCOMPONENTOBJECTSUBDEF_REGEX   = re.compile(r'NCOMPONENT_DECLARE\s*\(([a-zA-Z0-9_]+)\s*(,\s*([a-zA-Z0-9_]+))?\)')
NCOMPONENTCLASSSUBDEF_REGEX    = re.compile(r'NCOMPONENT_DECLARE\s*\(([a-zA-Z0-9_]+Class)\s*(,\s*([a-zA-Z0-9_]+Class))?\)')

        
class CmdFileHeader:
    def __init__(self, gi = "", incl = [], cn = ""):
        self.includes = []
        self.generalInfo = ""
        self.className = ""
        if isinstance(gi, CmdFileHeader):
            self.setFrom(gi)
        else:
            if isinstance(gi, StringType): self.generalInfo = gi
            if isinstance(incl, ListType):
                for e in incl:
                    if isinstance(e, StringType): self.includes.append(e)
                    else: self.includes.append("")
            if isinstance(cn, StringType): self.className = cn
  
    def clear(self):
        self.generalInfo = ""
        self.includes = []
        self.className = ""
  
    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "//  " + str(self.className) + "_cmds.cc\n"
        for l in self.generalInfo.splitlines():
            res += "//  " + l.strip() + "\n"
        res += COMMENT_LINE + "\n"
        for incl in self.includes:
            res += "#include \"" + incl + "\"\n"
        return res
  
    def setFrom(self, hdr):
        if isinstance(hdr, CmdFileHeader):
            self.generalInfo = hdr.generalInfo[:]
            self.includes = []
            for e in hdr.includes:
                self.includes.append(e[:])
            self.className = hdr.className[:]
  
    def __cmp__(self, c):
        if not isinstance(c, CmdFileHeader): return 1
        if not (self.generalInfo == c.generalInfo and \
                self.className == c.className and\
                self.includes == c.includes):
            return 1
        return 0

class CmdClassInfo:
    def __init__(self,type="", cppcl = "", spcl = "", scrcl = "", cli = ""):
        self.type = type
        self.cppClass = ""
        self.superClass = ""
        self.info = ""
        self.className = ""
        self.components = []
        if isinstance(cppcl, CmdClassInfo):
            self.setFrom(cppcl)
        else:
            if isinstance(cppcl, StringType): self.cppClass = cppcl
            if isinstance(spcl, StringType): self.superClass = spcl
            if isinstance(cli, StringType): self.info = cli
            if isinstance(scrcl, StringType): self.className = scrcl
  
    def clear(self):
        self.cppClass
        self.superClass
        self.info
        self.className = "", "", "", "", ""
  
    def __repr__(self):
        if not self.className:
            self.className = self.cppClass.lower()        

        nameKey = ""
        if "scriptclass" == self.type:
            nameKey = "@scriptclass "
        elif "component" == self.type:
            nameKey = "@component "
        elif "componentclass" == self.type:
            nameKey = "@componentclass "
        elif "entityclass" == self.type:
            nameKey = "@entityclass "
        elif "entity" == self.type:
            nameKey = "@entity "

        res = ""
        res += "\n"
        #res += "//------------------- BEGIN NEEDED BY PARSER DO NOT DELETE!!! --------------------" + "\n"
        res += "//---------------------------------------------------------------------------------" + "\n"
        res += "/**\n"            
        res += TABSPACES + nameKey + self.className + "\n"
        res += TABSPACES + "@cppclass " + self.cppClass + "\n"
        res += TABSPACES + "@superclass " + self.superClass + "\n"
        if len(self.components) > 0:            
            res += TABSPACES + "@components\n"
            for comp in self.components:
                res += TABSPACES + TABSPACES + comp + "\n"        
        res += TABSPACES + "@classinfo\n"
        for l in self.info.splitlines():
            res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n" 
        #res += "//--------------------- END NEEDED BY PARSER DO NOT DELETE!!! --------------------" + "\n\n"

        return res

    def setFrom(self, c):
        if isinstance(c, CmdClassInfo):
            self.cppClass = c.cppClass[:]
            self.superClass = c.superClass[:]
            self.scriptClass = c.scriptClass[:]
            self.info = c.info[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdClassInfo): return 1
        if not (self.cppClass == c.cppClass and\
                self.superClass == c.superClass and\
                self.className == c.className and\
                self.info == c.info):
            return 1
        return 0

class CmdComponentInfo:
    def __init__(self, cppcl = "", spcl = "", comcl = "", cli = ""):
        self.cppClass = ""
        self.superClass = ""
        self.info = ""
        self.componentName = ""
        if isinstance(cppcl, CmdClassInfo):
            self.setFrom(cppcl)
        else:
            if isinstance(cppcl, StringType): self.cppClass = cppcl
            if isinstance(spcl, StringType): self.superClass = spcl
            if isinstance(cli, StringType): self.info = cli
            if isinstance(comcl, StringType): self.componentName = comcl
  
    def clear(self):
        self.cppClass
        self.superClass
        self.info
        self.componentName = "", "", "", "", ""
  
    def __repr__(self):
        if not self.componentName:
            self.componentName = self.cppClass.lower()
        res = ""
        res += "\n"
        res += COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + "@component " + self.componentName + "\n"
        res += TABSPACES + "@cppclass " + self.cppClass + "\n"
        res += TABSPACES + "@superclass " + self.superClass + "\n"
        res += TABSPACES + "@classinfo\n"
        for l in self.info.splitlines():
            res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdClassInfo):
            self.cppClass = c.cppClass[:]
            self.superClass = c.superClass[:]
            self.componentName = c.componentName[:]
            self.info = c.info[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdClassInfo): return 1
        if not (self.cppClass == c.cppClass and\
                self.superClass == c.superClass and\
                self.componentName == c.componentName and\
                self.info == c.info):
            return 1
        return 0

class CmdParam:
    def __init__(self, t = "v", n = "", a = {}, c = ""):
        self.type = "v"
        self.name = ""
        self.attribute = {}
        self.comment = ""
        if isinstance(t, CmdParam):
            self.setFrom(t)
        else:
            if isinstance(t, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', t): self.type = t
            
            # type cleanning
            self.type = self.type.replace('v', '')
            if self.type == '':
                self.type = 'v'
            else:
                if isinstance(n, StringType): self.name = n
                if isinstance(a, DictType):
                    for k, v in map(None, a.keys(), a.values()):
                        if isinstance(v, StringType): self.attribute[k] = v
                        elif isinstance(v, ListType):
                            v_ = []
                            for e in v:
                                if isinstance(v, StringType): v_.append(e)
                                else: v_.append("<unknown>")
                            self.attribute[k] = v_
                        else: self.attribute[k] = "<unknown>"
                if isinstance(c, StringType): self.comment = c
  
    def __repr__(self):
        res = self.type
        if self.type != 'v':
            res += "(" + self.name
            for k, v in map(None, self.attribute.keys(), self.attribute.values()):
                res += "; " + k + "="
                if isinstance(v, ListType): res += ','.join(v)
                else: res += v
            res += ")"
            ind = PARAM_COMMENT_INDENT - len(res)
            if ind < 1: ind = 1
            res += " " * ind + "-- " + self.comment
        res += "\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdParam):
            self.type = c.type[:]
            self.name = c.name[:]
            self.attribute = {}
            for k, v in map(None, self.attribute.keys(), self.attribute.values()):
                if isinstance(v, StringType): self.attribute[k] = v[:]
                elif isinstance(v, ListType):
                    v_ = []
                    for e in v: v_.append(e[:])
                    self.attribute[k] = v_
                else: self.attribute[k] = "<unknown>"
            self.comment = c.comment[:]
  
    def __cmp__(self, c):
        if not isinstance(c, CmdParam): return 1
        if not (self.type == c.type and\
                self.name == c.name and\
                self.comment == c.comment):
                #and\
                #self.attribute == c.attribute):
            return 1
        return 0

class CmdProperty:
    def __init__(self, n, p = [], i = ""):
        self.params = []
        self.info = ""
        self.attribute = {}
        self.name = n
        if isinstance(n, CmdProperty): self.setFrom(n)
        else:
            if isinstance(n, StringType): self.name = n
            else: self.name = "<unknown>"
            if isinstance(p, ListType):
                has_void = False
                has_not_void = False
                for e in p:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.params.append(e)
                                has_void = True
                        else:
                            self.params.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, p) in self.params.iteritems():
                        if p.type == 'v': del(self.params[k])
                        else: break
            else: self.params.append(CmdParam())
        if isinstance(i, StringType): self.info = i
  
    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + "@property " + self.name + "\n"
        res += TABSPACES + "@format\n"
        for p in self.params: res += TABSPACES + "  " + repr(p)
        res += TABSPACES + "@attrs " 
        for attr, value in map(None, self.attribute.keys(), self.attribute.values()):
            res += str(attr) + "=" + str(value) + ";"
        res = res[:-1] + "\n"
        res += TABSPACES + "@info\n"
        for l in self.info.splitlines(): res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdProperty):
            self.name = c.name[:]
            self.params = []
            for e in c.params: self.params.append(CmdParam(e))
            self.info = c.info[:]
  
    def __cmp__(self, c):
        if not isinstance(c, CmdProperty): return 1
        if not (self.name == c.name and\
                self.info == c.info and\
                self.params == c.params and\
                self.attribute == c.attribute):
            return 1
        return 0

class BaseCmd:
    CMD_DECLARED = 1 << 0
    CMD_ADDED    = 1 << 1
    CMD_DEFINED  = 1 << 2
  
    def __init__(self, fn, cn = "", out_f = "", in_f = "", fcc = "", i = "", cb = ""):
        self.funcName = "<unknown>"
        self.cmdName = "<unknown>"
        self.outFormat = "v"
        self.inFormat = "v"
        self.fourCC = "---"
        self.info = ""
        self.cmdBody = ""
        self.status = 0
        self.comment = True

        if isinstance(fn, BaseCmd):
            self.funcName = fn.funcName[:]
            self.cmdName = fn.cmdName[:]
            self.outFormat = fn.outFormat[:]
            self.inFormat = fn.inFormat[:]
            self.fourCC = fn.fourCC[:]
            self.info = fn.info[:]
            self.cmdBody = fn.cmdBody[:]
            self.status = fn.status
        else:
            if isinstance(fn, StringType): self.funcName = fn            
            if isinstance(cn, StringType): self.cmdName = cn
            else: self.cmdName = self.funcName[:]            
            if isinstance(out_f, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', out_f):
                self.outFormat = out_f            
            if isinstance(in_f, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', in_f):
                self.inFormat = in_f            
            if isinstance(fcc, StringType): self.fourCC = fcc            
            if isinstance(i, StringType): self.info = i            
            if isinstance(cb, StringType): self.cmdBody = cb
  
    def setStatus(self, st):
        if st == BaseCmd.CMD_DECLARED or st == BaseCmd.CMD_ADDED or st == BaseCmd.CMD_DEFINED:
            self.status |= st
  
    def _specific_str(self): return ""

    def _mode_str(self): return "@cmd "
  
    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + self._mode_str() + self.cmdName + "\n"
        res += self._specific_str()
        res += TABSPACES + "@info\n"
        for l in self.info.splitlines(): res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        res += "static void\n"
        res += "n_" + self.funcName + "(void* slf, nCmd* cmd)\n"
        res += "{\n"
        for l in self.cmdBody.splitlines(): res += l + "\n"
        res += "}\n"
        return res

    def get_dox_text (self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + self._mode_str() + self.cmdName + "\n"
        res += self._specific_str()
        res += TABSPACES + "@info \n"
        for l in self.info.splitlines(): res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"

        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            self.funcName = c.funcName[:]
            self.cmdName = c.cmdName[:]
            self.outFormat = c.outFormat[:]
            self.inFormat = c.inFormat[:]
            self.fourCC = c.fourCC[:]
            self.info = c.info[:]
            self.cmdBody = c.cmdBody[:]
            self.status = c.status
  
    def __cmp__(self, c):
        if not issubclass(c.__class__, BaseCmd): return 1
        if not (self.funcName.strip() == c.funcName.strip() and\
                self.cmdName.strip() == c.cmdName.strip() and\
                self.info.strip() == c.info.strip() and\
                self.outFormat.strip() == c.outFormat.strip() and\
                self.inFormat.strip() == c.inFormat.strip() and\
                self.fourCC.strip() == c.fourCC.strip()): # and self.status == c.status
            return 1
        return 0

class Cmd(BaseCmd):
    T_GETTER, T_SETTER = 0, 1
    M_NONE, M_COUNT, M_BEGIN, M_ADD, M_END = 0, 1, 2, 3, 4
  
    def __init__(self, common_cmd, t, st = M_NONE, p = ""):
        self.type = Cmd.T_GETTER 
        self.subType = Cmd.M_NONE
        self.propertyName = ""
        # default base initialization
        if isinstance(common_cmd, Cmd):
            self.setFrom(common_cmd)
        else:
            BaseCmd.__init__(self, common_cmd)
            if t == Cmd.T_SETTER: self.type = Cmd.T_SETTER
            if (t == Cmd.T_GETTER and st == Cmd.M_COUNT) or\
               (t == Cmd.T_SETTER and (st == Cmd.M_BEGIN or st == Cmd.M_ADD or st == Cmd.M_END)):
                self.subType = st
            if isinstance(p, StringType): self.propertyName = p

    def _specific_str(self):
        res = TABSPACES + "@cmdtype " + ["getter", "setter"][self.type]
        if self.subType != Cmd.M_NONE:
            res += "|" + ["", "count", "begin", "add", "end"][self.subType]
        res += "\n"
        res += TABSPACES + "@property " + self.propertyName + "\n"
        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            BaseCmd.setFrom(self, c)
            if isinstance(c, Cmd):
                self.type = c.type
                self.subType = c.subType
                self.propertyName = c.propertyName[:]
  
    def __cmp__(self, c):
        if not isinstance(c, Cmd): return 1
        if BaseCmd.__cmp__(self, c): return 1
        if not (self.type == c.type and\
                self.subType == c.subType and\
                self.propertyName == c.propertyName and\
                self.info == c.info):
            return 1
        return 0

class CommonCmd(BaseCmd):
    def __init__(self, common_cmd, op = [], ip = [], signal=False):
        self.outParams = []
        self.inParams = []
        self.signal = signal

        # default base initialization
        if isinstance(common_cmd, CommonCmd):
            self.setFrom(common_cmd)
        else:
            BaseCmd.__init__(self, common_cmd)
            #if isinstance(op, ListType):
            #    for e in op:
            #        if isinstance(e, CmdParam): self.outParams.append(e)
            #        else: self.outParams.append(CmdParam())
            if isinstance(op, ListType):
                has_void = False
                has_not_void = False
                for e in op:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.outParams.append(e)
                                has_void = True
                        else:
                            self.outParams.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, op) in self.outParams.iteritems():
                        if op.type == 'v': del(self.outParams[k])
                        else: break
            else: self.outParams.append(CmdParam())

            #if isinstance(ip, ListType):
            #    for e in ip:
            #        if isinstance(e, CmdParam): self.inParams.append(e)
            #        else: self.inParams.append(CmdParam())
            if isinstance(ip, ListType):
                has_void = False
                has_not_void = False
                for e in ip:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.inParams.append(e)
                                has_void = True
                        else:
                            self.inParams.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, ip) in self.inParams.iteritems():
                        if ip.type == 'v': del(self.inParams[k])
                        else: break
            else: self.inParams.append(CmdParam())

    def _specific_str(self):
        res = TABSPACES + "@output\n"
        for p in self.outParams:
            res += TABSPACES + "  " + repr(p)

        res += TABSPACES + "@input\n"
        for p in self.inParams:
            res += TABSPACES + "  " + repr(p)

        return res

    def _mode_str(self):
        if self.signal:
            res = "@signal "
        else:
            res = "@cmd "
        
        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            BaseCmd.setFrom(self, c)
            if isinstance(c, BaseCmd):
                self.outParams = []
                self.inParams = []
                for e in c.outParams: self.outParams.append(CmdParam(e))
                for e in c.inParams: self.inParams.append(CmdParam(e))

    def __cmp__(self, c):
        if not isinstance(c, BaseCmd): return 1
        if BaseCmd.__cmp__(self, c): return 1
        if not (self.outParams == c.outParams and\
                self.inParams == c.inParams):
            return 1
        return 0

class SaveCmdsFunction:
    def __init__(self, cd = "", cb = "", c = ""):
        self.classDef = ""
        self.cmdBody = ""
        self.comment = ""
        if isinstance(cd, SaveCmdsFunction): self.setFrom(cd)
        else:
            if isinstance(cd, StringType): self.classDef = cd
            if isinstance(cb, StringType): self.cmdBody = cb
            if isinstance(c, StringType): self.comment = c
  
    def clear(self):
        self.classDef, self.cmdBody, self.comment = "", "", ""
  
    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        for l in self.comment.splitlines(): res += "    " + l.strip() + "\n"
        res += "*/\n"
        res += "bool\n"
        res += self.classDef + "::SaveCmds(nPersistServer *ps)\n"
        res += "{\n"
        for l in self.cmdBody.splitlines(): res += l + "\n"
        res += "}\n"
        return res

    def setFrom(self, c):
        if isinstance(c, SaveCmdsFunction):
            self.classDef = c.classDef[:]
            self.cmdBody = c.cmdBody[:]
            self.comment = c.comment[:]

    def __cmp__(self, c):
        if not isinstance(c, SaveCmdsFunction): return 1
        if not (self.classDef == c.classDef and\
                self.cmdBody == c.cmdBody and\
                self.comment == c.comment):
            return 1        
        return 0

#---------------------------------------------------------------------------------------
class CmdFileConfig:
    def __init__(self, hdr = None, cli = None, classtype = None, p = [], c = [], scf = None):
        self.header = CmdFileHeader()
        self.classInfo = CmdClassInfo(classtype)
        self.properties = []
        self.cmds = []
        self.signals = []
        self.saveCmdsFunc = None
        self.classType = classtype
        if isinstance(hdr, CmdFileConfig): self.setFrom(hdr)
        else:
            if isinstance(hdr, CmdFileHeader): self.header = hdr
            if isinstance(cli, CmdClassInfo): self.classInfo = cli
            if isinstance(p, ListType):
                for e in p:
                    if isinstance(e, CmdProperty): self.properties.append(e)
                    else: self.properties.append(CmdProperty())
            if isinstance(c, ListType):
                for e in c:
                    if issubclass(e.__class__, BaseCmd): self.cmds.append(e)
                    else: self.cmds.append(BaseCmd(None))            
            if isinstance(scf, SaveCmdsFunction): self.saveCmdsFunc = scf
  
    def findProperty(self, name):
        for p in self.properties:
            if p.name == name: return p
        return None
  
    def findCmd(self, name):
        for c in self.cmds:
            if c.funcName == name: return c
        return None

    def findSignal(self, name):
        for c in self.signals:
            if c.funcName == name: return c
        return None
  
    def clear(self):
        self.header.clear()
        self.classInfo.clear()
        self.properties = []
        self.cmds = []
        self.saveCmdsFunc = None
  
    def sortCmds(self):
        self.properties.sort(lambda p1, p2: cmp(p1.name, p2.name))
        self.cmds.sort(self._cmdsSortFunc)

    def _cmdsSortFunc(self, c1, c2):
        if isinstance(c1, Cmd):
            if isinstance(c2, Cmd):
                res = cmp(c1.propertyName, c2.propertyName)
                if res == 0:
                    if c1.type == Cmd.T_SETTER: return 1
                    else: return -1
                else: return res
            else: return 1  # c1 is greater then c2 because cmd's with property should be above not propertied
        else:
            if isinstance(c2, Cmd): return -1
            else: return cmp(c1.cmdName, c2.cmdName)
  
    def __repr__(self):
        size = 0
        #res = repr(self.header)
        res = ""
        classType = self.classInfo.type
        res += repr(self.classInfo)        

        for property in self.properties:
            res += "\n" + repr(property)

        #printed_prop = {}
        # print the properties before the commands so the property command are printed between his property
        for cmd in self.cmds:
            #if isinstance(cmd, Cmd) and not printed_prop.has_key(cmd.propertyName):
            #    prop = self.findProperty(cmd.propertyName)
            #    if prop:
            #        res += '\n' + repr(prop)
            #        printed_prop[cmd.propertyName] = None
            res += '\n' + cmd.get_dox_text()
        
        res += "\n"        

        for signal in self.signals:
            res += str(signal) + "\n"

        res += COMMENT_LINE + "\n"

        return res


    def get_dox_text(self):
        size = 0
        res = repr(self.header)
        res += "\n"
        classType = self.classInfo.type
        if classType == "script":                        
            res += repr(self.classInfo)

        if classType == "component":
            res += "\n--------component---------\n" + repr(self.classInfo)

        if classType == "componentclass":
            res += "\n--------componentclass---------\n" + repr(self.classInfo)

        if classType == "entityclass":
            res += "\n--------entityclass---------\n" + repr(self.classInfo)

        if classType == "entity":
            res += "\n--------entity---------\n" + repr(self.classInfo)


        for property in self.properties:
            res += "\n" + repr(property)

        printed_prop = {}
        #print self.properties
        for cmd in self.cmds:
            if isinstance(cmd, Cmd) and not printed_prop.has_key(cmd.propertyName):
                prop = self.findProperty(cmd.propertyName)
                if prop:
                    res += '\n' + repr(prop)
                    printed_prop[cmd.propertyName] = None
            res += '\n' + repr(cmd)
        
        res += "\n"        

        for signal in self.signals:
            res += str(signal) + "\n"

        res += COMMENT_LINE + "\n"

        return res
  
    def saveTo(self, fileName):
        f = open(fileName, "wb")
        #for l in self.__repr__().splitlines(): f.write(l)
        f.write(self.__repr__())
        f.close()

    def setFrom(self, c):
        if isinstance(c, CmdFileConfig):
            self.header = CmdFileHeader(c.header)
            self.classInfo = CmdClassInfo(c.classInfo)
            self.properties = []
            self.cmds = []
            for e in c.properties:
                if isinstance(e, CmdProperty): self.properties.append(e)
                else: self.properties.append(CmdProperty())
            for e in c.cmds:
                if isinstance(e, BaseCmd): self.cmds.append(e)
                else: self.cmds.append(BaseCmd(None))
            if self.saveCmdsFunc: self.saveCmdsFunc = SaveCmdsFunction(c.saveCmdsFunc)
            else: self.saveCmdsFunc = None

    def __cmp__(self, c):
        if not isinstance(c, CmdFileConfig): return 1
        if not (self.header == c.header and\
                self.classInfo == c.classInfo and\
                self.properties == c.properties and\
                self.cmds == c.cmds and\
                self.saveCmdsFunc == c.saveCmdsFunc):
            return 1
        return 0

    def __add__ (self, config):
        # implements configurations add.
        # NOTE: Not is commutative, header config must be the first in the operation
        if config == None:
            return self

        command_found = False
        # Check for new commands in header
        for cmd in config.cmds:
            for self_cmds in self.cmds:
                if self_cmds.cmdName == cmd.cmdName:
                    command_fonund = True
            # add command to cmds config if needed
            if not command_found:
                self.cmds.append(cmd)

        # Add properties to config
        for prop in config.properties:
            self.properties.append(prop)

        # Add new signals to config
        for signal in config.signals:
            self.signals.append(signal)

        return self


#---------------------------------------------------------------------------------------
def dispatchExtComment(hf, firstStr):
    commentStr = ""
    curStr = firstStr.strip()
    if len(curStr) > 0: curStr += "\n"
    reg = re.compile(r'^(.*?)\*\/')
    g = reg.search(curStr)
    while not g:
        commentStr += curStr
        curStr = hf.readline().strip() + "\n"
        g = reg.search(curStr)
    commentStr += g.group(1)
    return commentStr.strip()

def dispatchClassDef(hf, firstStr):
    commentStr = ""
    curStr = firstStr.strip()
    if len(curStr) > 0: curStr += "\n"
    reg = re.compile(r'^{')
    g = reg.search(curStr)
    while not g:
        commentStr += curStr
        curStr = hf.readline().strip() + "\n"
        g = reg.search(curStr)
    commentStr += g.group(1)
    return commentStr.strip()

def dispatchDoc(docStr):
    atBlocks = {"?0": ""}
    restBlocks = ""
    atName = ""
    needEnd = ""
    freeBlockIdx = 0

    reg = re.compile(r'^@\s*(\w[\w\d]*)(?:\s+(.*))?$')
    for ln in docStr.splitlines():
        line = ln.strip()
        if len(line) > 0:
            g = reg.search(line)
            if g:
                if needEnd == "":
                    atName = g.group(1)
                    if g.group(2): atBlocks[atName] = g.group(2)
                    else: atBlocks[atName] = ""
                    if line == "verbatim": needEnd = "verbatim"
                    if line == "code": needEnd = "code"
                else:
                    if g.group(1) == "end" + needEnd: needEnd = ""
                    else:
                        if atName: atBlocks[atName] += "\n" + line
                        else: restBlocks += "\n" + line
            elif atName: atBlocks[atName] += "\n" + line
            else:
                restBlocks += "\n" + line
                fbNum = "?" + repr(freeBlockIdx)
                if not atBlocks.has_key(fbNum): atBlocks[fbNum] = ""
                atBlocks[fbNum] += "\n" + line
        else:
            atName = ""
            fbNum = "?" + repr(freeBlockIdx)
            if atBlocks.has_key(fbNum):
                if atBlocks[fbNum]:
                    freeBlockIdx += 1
                    atBlocks[fbNum] = ""
            else:
                atBlocks[fbNum] = ""
  
##    if atBlocks["?" + freeBlockIdx] == "":
##        atBlocks.delete("?" + freeBlockIdx.to_s) 
    for v in atBlocks.values(): v = v.strip()
    return atBlocks

class CmdFileProcessor:
    def __init__(self, code_dir, is_contrib = False, mod_name = "", pkg_name = "", file_name = ""):
        self.codeDir, self.isContrib, self.modName, self.pkgName, self.fileName = code_dir, is_contrib, mod_name, pkg_name, file_name

        self.config = None #CmdFileConfig()
        self.processed = False
        self.entityConfigList = []

        #print "--->" + self.codeDir
        if self.codeDir != "":
            if self.codeDir[-1] != "/":
                self.codeDir += "/"
      
    def setCmdFile(self, is_contrib, mod_name, pkg_name, file_name):
        if is_contrib != self.isContrib or mod_name != self.modName or pkg_name != self.pkgName or file_name != self.fileName:
            self.isContrib, self.modName, self.pkgName, self.fileName = is_contrib, mod_name, pkg_name, file_name
            self.config = CmdFileConfig() #.clear
            self.processed = False
  
    def dispatchParam(self, paramDesc):
        param = None
        paramDesc
        if paramDesc == 'v': param = CmdParam('v')
        else:
            g = PARAMDESC_REGEX.search(paramDesc)
            if g:
                param = CmdParam(g.group(1), g.group(2))              
                if g.group(4):
                    param.comment = g.group(4)
                if g.group(3):
                    attrList = g.group(3)[1:]
                    for av in attrList.split(';'):
                        g = ATTRVAL_REGEX.search(av)
                        if g:
                            attrName = g.group(1)
                            if g.group(3): 
                                param.attribute[attrName] = g.group(2)
                            else: 
                                param.attribute[attrName] = g.group(2)
        return param

    def dispatchAttrs (self, attrsDesc):
        attrs = {}        
        for at in attrsDesc.split(';'):
            g = ATTRVAL_REGEX.search(at)
            if g:
                attrName = g.group(1)
                attrs[attrName] = g.group(2)                                        
        return attrs


  
    def extractBody(self, file, startBracket):
        res = ""
        endBracket = False
        curFilePos = file.tell()
        line = file.readline()
        while line:
            if startBracket:
                if line[0] == '}':
                    endBracket = True
                    break
                else:
                    if line[0:TABSIZE-1] == TABSPACES:
                        res += line[TABSIZE:-1].rstrip() + "\n"
                    else:
                        res += line.rstrip() + "\n"
            elif line[0] == '{': #/^\{\s*(.+)?/
                startBracket = True
                l = line[1:-1].strip()
                if l: res += TABSPACES + l + "\n"
            line = file.readline()
    
        if not endBracket:
            res = ""
            file.seek(curFilePos)
            print "Error: can't found bracket }"

        return res.rstrip()
    
    def _parseBaseInfo(self, g, baseInfo, hf=None):
        s = g.group(1).strip()
        if not re.search(r'^-+$', s):
            if baseInfo:
                baseInfo += "\n" + s
            elif s and not re.search(r'^\s*\w[\w\d]*_cmds.cc', s):
                baseInfo = s
        return baseInfo

    def _parseInclude(self, g, baseInfo):
        if baseInfo:
            self.config.header.generalInfo = baseInfo
            baseInfo = None
        self.config.header.includes.append(g.group(1).strip())
        return baseInfo

    def _parseCmdFunctionDecl(self, g, baseInfo):
        if baseInfo:
            self.config.header.generalInfo = baseInfo
            baseInfo = None              
        cmd = BaseCmd(g.group(1))
        cmd.setStatus(BaseCmd.CMD_DECLARED)
        self.config.cmds.append(cmd)
        return baseInfo

    def _parseAddCmdFunction(self, g):
        cmd = self.config.findCmd(g.group(5))
        if not cmd:
            cmd = BaseCmd.new(g.group(5))
            self.config.cmds.append(cmd)
        cmd.cmdName = g.group(2)
        cmd.outFormat = g.group(1)
        if len(cmd.outFormat) > 1 and 'v' in cmd.outFormat:
            print "Error: wrong output format for the command " + cmd.cmdName + "."
        cmd.inFormat = g.group(3)
        if len(cmd.inFormat) > 1 and 'v' in cmd.inFormat:
            print "Error: wrong input format for the command " + cmd.cmdName + "."
        cmd.fourCC = g.group(4)
        cmd.setStatus(BaseCmd.CMD_ADDED)

    def _parseCmdDoc(self, g, ccFile, commentBlock, comment=True):
        if commentBlock:
            atBlocks = dispatchDoc(commentBlock)
            if atBlocks.has_key("property") and atBlocks.has_key("format"):
                prop = self.config.findProperty(atBlocks["property"].strip())
                if not prop:    
                    prop = CmdProperty(atBlocks["property"].strip())                
                    for l in atBlocks["format"].splitlines():
                        param = self.dispatchParam(l.strip())
                        if param:
                            prop.params.append(param)
                    if atBlocks.has_key("attrs"):
                        prop.attribute = self.dispatchAttrs(atBlocks["attrs"])
                    if atBlocks.has_key("info"):
                        prop.info = atBlocks["info"].strip()
                    else:
                        print "WARNING: %s property in %s doesn't have documentation" %(prop.name, ccFile.name)
                    self.config.properties.append(prop)                

            if atBlocks.has_key("component") or atBlocks.has_key("componentclass") or atBlocks.has_key("scriptclass") \
                    or atBlocks.has_key("entityclass") or atBlocks.has_key("entity"):
                if atBlocks.has_key("component"):
                    nameKey = "component"
                    self.config.classType = "component"
                    self.config.classInfo.type = "component"
                if atBlocks.has_key("componentclass"):
                    nameKey = "componentclass"
                    self.config.classType = "componentclass"
                    self.config.classInfo.type = "componentclass"
                if atBlocks.has_key("scriptclass"):
                    nameKey = "scriptclass"
                    self.config.classType = "scriptclass"
                    self.config.classInfo.type = "scriptclass"
                if atBlocks.has_key("entityclass"):
                    nameKey = "entityclass"
                    self.config.classType = "entityclass"
                    self.config.classInfo.type = "entityclass"
                if atBlocks.has_key("entity"):
                    nameKey = "entity"
                    self.config.classType = "entity"
                    self.config.classInfo.type = "entity"
                
                comp = CmdClassInfo()                
                comp.type = nameKey
                if atBlocks.has_key("cppclass"):
                    comp.cppClass = atBlocks["cppclass"].strip()
                else:
                    print "WARNING: %s file don't have cppclass key" %(ccFile.name)
                    commentBlock = None
                if atBlocks.has_key("superclass"):
                    comp.superClass = atBlocks["superclass"].strip()                
                else:
                    print "WARNING: %s file don't have superclass key" %(ccFile.name)
                    commentBlock = None
                if atBlocks.has_key("classinfo"):
                    comp.info = atBlocks["classinfo"].strip()
                else:
                    print "WARNING: %s file don't have classinfo key" %(ccFile.name)
                    commentBlock = None
                if atBlocks.has_key(nameKey):
                    comp.className = atBlocks[nameKey].strip()
                else:
                    print "WARNING: %s file don't have the name class definition" %(ccFile.name)
                    commentBlock = None
                  
                self.config.classInfo = comp

            if atBlocks.has_key("cmd"):
                cmdName = atBlocks["cmd"].strip()        
                cmd = self.config.findCmd(cmdName)
                if not cmd:
                    self._parseCmdComment(atBlocks, comment)
                else:
                    cmd.comment = comment

            if atBlocks.has_key("signal"):
                self._parseSignalComment(atBlocks)

        return commentBlock
                                                                            
    def _parseCmdComment(self, atBlocks, comment=True):
        cmdName = atBlocks["cmd"].strip()        
        cmd = self.config.findCmd(cmdName)

        if not cmd:
            cmd = BaseCmd(cmdName, cmdName, "v", "v", "----")
            #cmd.cmdName = cmdName
            cmd.comment = comment
            self.config.cmds.append(cmd)
        
        cmd.setStatus(BaseCmd.CMD_DEFINED)
        cmd_idx = self.config.cmds.index(cmd)        
          
        if atBlocks.has_key("info"): cmd.info = atBlocks["info"].strip()
        else: 
            print "WARNING: No comment documentation for command %s" %(name)
            cmd.info = "<no info>"
          
        if atBlocks["cmd"] != cmdName:
            atBlocks["cmd"] = cmdName
            # send error msg
          
        if atBlocks.has_key("property"): # getter/setter command
            tt = atBlocks["cmdtype"].split("|")
            type_s = tt[0].strip()
            subType_s = ""
            if len(tt) > 1: subType_s = tt[1].strip()
            type = 0
            subType = Cmd.M_NONE
            if type_s == "getter":
                type = Cmd.T_GETTER
                if subType_s == "count": subType = Cmd.M_COUNT
            elif type_s == "setter":
                type = Cmd.T_SETTER
                if subType_s == "begin": subType = Cmd.M_BEGIN
                elif subType_s == "add": subType = Cmd.M_ADD
                elif subType_s == "end": subType = Cmd.M_END
                else:
                    # try to guess what subType
                    if re.match(r'^begin', cmdName): subType = Cmd.M_BEGIN
                    elif re.match(r'^add', cmdName): subType = Cmd.M_ADD
                    elif re.match(r'^end', cmdName): subType = Cmd.M_END
                    else: subType = Cmd.M_NONE
            else:
                # try to guess type and subType
                if re.match(r'^set', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_NONE
                elif re.match(r'^begin', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_BEGIN
                elif re.match(r'^add', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_ADD
                elif re.match(r'^end', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_END
                else:
                    type = Cmd.T_GETTER
                    subType = Cmd.M_NONE

            propName = atBlocks["property"].strip()
            cmd = Cmd(cmd, type, subType, propName)
            cmd.comment = comment
            #prop = self.config.findProperty(propName)
            #if not prop:
            #    print "_parseCmdComment"
            #    prop = CmdProperty(propName)
            #    self.config.properties.append(prop)
                # try to guess property params
            #    p_type = cmd.outFormat
            #    if type == Cmd.T_SETTER: p_type = cmd.inFormat
            #    prop.params.append(CmdParam(p_type, "name"))
        else: # this is simple command, not getter/setter
            cmd = CommonCmd(cmd)
            cmd.cmdName = cmdName
            cmd.comment = comment
            if atBlocks.has_key("output"):
                lines = atBlocks["output"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.outParams.append(param)
                        else:
                            cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
                else:
                    cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
            else:
                cmd.outParams.append(CmdParam(cmd.outFormat, "name"))

            if atBlocks.has_key("input"):
                lines = atBlocks["input"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.inParams.append(param)
                        else:
                            cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
                else:
                    cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
            else:
                cmd.inParams.append(CmdParam(cmd.inFormat, "name"))

        # extract cmd body
        self.config.cmds[cmd_idx] = cmd

    def _parseSignalComment(self, atBlocks):
        signalName = atBlocks["signal"]        
        signal = self.config.findSignal(signalName)
    
        if not signal:
            signal = BaseCmd(signalName, signalName, "v", "v", "----")
            #signal.signalName = signalName
            self.config.signals.append(signal)
    
        signal_idx = self.config.signals.index(signal)        
    
        if atBlocks.has_key("info"): signal.info = atBlocks["info"].strip()
        else: signal.info = "<no info>"
    
        if atBlocks["signal"] != signalName:
            atBlocks["signal"] = signalName
            # send error msg

        signal = CommonCmd(signal, signal=True)
        signal.signalName = signalName
        if atBlocks.has_key("output"):
            lines = atBlocks["output"].strip().splitlines()
            if len(lines) > 0:
                for l in lines:
                    param = self.dispatchParam(l.strip())
                    if param:
                        signal.outParams.append(param)
                    else:
                        signal.outParams.append(CmdParam(signal.outFormat, "name"))
            else:
                signal.outParams.append(CmdParam(signal.outFormat, "name"))
        else:
            signal.outParams.append(CmdParam(signal.outFormat, "name"))

        if atBlocks.has_key("input"):
            lines = atBlocks["input"].strip().splitlines()
            if len(lines) > 0:
                for l in lines:
                    param = self.dispatchParam(l.strip())
                    if param:
                        signal.inParams.append(param)
                    else:
                        signal.inParams.append(CmdParam(signal.inFormat, "name"))
            else:
                signal.inParams.append(CmdParam(signal.inFormat, "name"))
        else:
            signal.inParams.append(CmdParam(signal.inFormat, "name"))

        # extract signal body
        self.config.signals[signal_idx] = signal
    
        
    def _parseInitCmdFunctionDef(self, className, commentBlock):
        if not commentBlock:
            #print "WARNING: There is no comment block (or wrong) for n_initcmds in %s. Default comment block will be set." %(className)
            commentBlock = " @scriptclass " + className + "\n @cppclass " + className + "\n @superclass nroot\n @classinfo No information"
        atBlocks = dispatchDoc(commentBlock)        
        commentBlock = None

        if atBlocks.has_key("scriptclass"):
            self.config.classInfo.className = atBlocks["scriptclass"].strip()
            self.config.classInfo.type = "scriptclass"
        else:
            self.config.classInfo.className = className

        if atBlocks.has_key("cppclass"):
            self.config.classInfo.cppClass = atBlocks["cppclass"].strip()
        else:
            self.config.classInfo.cppClass = className

        if atBlocks.has_key("superclass"):
            self.config.classInfo.superClass = atBlocks["superclass"].strip()
        else:
            self.config.classInfo.superClass = "nroot"

        if atBlocks.has_key("classinfo"):
            self.config.classInfo.info = atBlocks["classinfo"].strip()
        else:
            self.config.classInfo.info = ""

        return commentBlock
    

    def _parseEntityMacro (self, ccFile, className, commentBlock):
        # All this function is a very ugly hack for parse zombieentitylst.cc file
        # This is a file that define ALL the entities and entity classes, then, because
        # is not an only class it must by handle in a special way (this parser have been coded 
        # with the idea in mind that a file contains always only and only one class). 
        # The way chossen is storge all configurations in an array and return it if the boolean 
        # in the process_file method is enable. This method asume that only zombieentitylist.cc use
        # the macro nNebulaEntity

        # special regular expression to handle macro arguments enclosed in (),
        # used for list of components in the entity
        if not commentBlock:
            commentBlock = "@classinfo No info"
        atBlocksDoc = dispatchDoc(commentBlock)

        if not atBlocksDoc.has_key("classinfo"):
            atBlocksDoc["classinfo"] = "No info"

        commentBlock = None

        regexpEntityArgs = re.compile('^\s*('
                            # normal argument
                            '([a-zA-Z0-9_"\s]+)|'
                            # argument enclosed in ()
                            '\(([a-zA-Z0-9_,"\s]+)\)'
                            ')\s*,?\s*'
                            # rest of the string
                            '([a-zA-Z0-9_,"\(\)\s]+)$')
        regexpEndMacro = re.compile(r'\);')

        accumLine = ''
        curStr = ccFile.readline().strip() + "\n"
        # Extraxt all the macro to a buffer
        g = regexpEndMacro.search(curStr)
        while not g:
            accumLine += curStr
            curStr = ccFile.readline().strip() + "\n"
            g = regexpEndMacro.search(curStr)

        #print accumLine
        parseOk = True
    
        # array to hold the macro parsed arguments
        macroArgs = range(12)
        remainingStr = accumLine
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

        # Get the info 

        entityObjectName = macroArgs[0]
        ancestorEntityObject = macroArgs[1].lower()
        entityClassName = macroArgs[6]
        ancestorEntityClass = macroArgs[7].lower()
        # old code: not need since component can be getted at runtime
        #entityObjectComponents = macroArgs[3].strip('\n').lstrip('(').rstrip(')')        
        #entityClassComponents = macroArgs[9].strip('\n').lstrip('(').rstrip(')')        

        # Get config for entity object
        commentBlock = "@entity " + entityObjectName.lower() + "\n@cppclass " + entityObjectName + \
                       "\n@superclass " + ancestorEntityObject + "\n"# + "\n@components\n"

        #for comp in entityObjectComponents.split(','):
        #    commentBlock += TABSPACES + comp.strip() + "\n"

        commentBlock += "@classinfo " + atBlocksDoc["classinfo"]

        atBlocks = dispatchDoc(commentBlock)

        self._parseEntityComment(atBlocks, "entity")
                # Add the config to list
        self.entityConfigList.append(self.config)
        self.config = CmdFileConfig() # Clear config               

        # Get the config for the entityclass
        commentBlock = "@entityclass " + entityClassName.lower() + "\n@cppclass " + entityClassName + \
                       "\n@superclass " + ancestorEntityClass + "\n" #+ "\n@components\n" 

        #for comp in entityClassComponents.split(','):
        #    commentBlock += TABSPACES + comp.strip() + "\n"
        commentBlock += "@classinfo " + atBlocksDoc["classinfo"]

        atBlocks = dispatchDoc(commentBlock)

        self._parseEntityComment(atBlocks, "entityclass")


        # Add the config to list
        self.entityConfigList.append(self.config)
        self.config = CmdFileConfig() # Clear config               

        return None


    def _parseEntityComment (self, atBlocks, entityType):
        if atBlocks.has_key(entityType):
            self.config.classInfo.className = atBlocks[entityType].strip()
            self.config.classInfo.type = entityType

        if atBlocks.has_key("cppclass"):
            self.config.classInfo.cppClass = atBlocks["cppclass"].strip()
        else:
            self.config.classInfo.cppClass = className

        if atBlocks.has_key("superclass"):
            self.config.classInfo.superClass = atBlocks["superclass"].strip()
        else:
            self.config.classInfo.superClass = className

        if atBlocks.has_key("components"):
            lines = atBlocks["components"].strip().splitlines()
            for line in lines:
                self.config.classInfo.components.append(line)
        else:
            self.config.classInfo.components = []

        if atBlocks.has_key("classinfo"):
            self.config.classInfo.info = atBlocks["classinfo"].strip()
        else:
            self.config.classInfo.info = ""

    def _parseClassDef (self, g, ccFile, commentBlock, className, entityType):
        if not commentBlock:
            #print "WARNING: There is no comment block (or wrong) for class %s definition. Default comment block will be set." %(className)
            commentBlock = "@" + entityType + " " + className + "\n@cppclass " + g.group(1) + "\n@superclass " + g.group(2) +"\n @classinfo No information"
        atBlocks = dispatchDoc(commentBlock)
        if not atBlocks.has_key(entityType):
            commentBlock = "@" + entityType + " " + className + "\n@cppclass " + g.group(1) + "\n@superclass " + g.group(2) + "\n@classinfo No information"
            atBlocks = dispatchDoc(commentBlock)
        commentBlock = None

        self._parseEntityComment(atBlocks, entityType)       

        return commentBlock


    def _parseComponents (self, g):
        if g.group(1) not in self.config.classInfo.components:
            self.config.classInfo.components.append(g.group(1))

        return None

    def _parseComponentDef (self, g, ccFile, commentBlock, compType):
        if self.config.classType != compType:
            if not commentBlock:
                commentBlock = "@error"
    
            atBlocks = dispatchDoc(commentBlock)
            if not atBlocks.has_key(compType):
                if compType == "component":
                    super_name = "nEntityObject"
                elif compType == "componentclass":
                    super_name = "nEntityClass"
        
                comp_name = g.group(1)
                if g.group(2):
                    super_name = g.group(3)
        
                # try to group components, some classes don't have a pattern, if this happens
                # the name of the class is used, then the programer should change it manually
                # in the dox file
                name = comp_name.lower()
                if name.find("ncgameplay") != -1 or name.find("ncgp") != -1:
                    group = "Gameplay"
                elif name.find("ncphy") != -1:
                    group = "Physics"
                elif name.find("ncai") != -1:
                    group = "AI"
                elif name.find("ncspatial") != -1:
                    group = "Spatial"
                elif name.find("ncscene") != -1:
                    group = "Graphics"
                elif name.find("ncterrain") != -1:
                    group = "Terrain"
                elif name.find("ncsound") != -1:
                    group = "Sound" 
                else:
                    group = "Common"
        
                if not commentBlock:
                    commentBlock = "@" + compType + " " + group + "\n@cppclass " + comp_name + "\n@superclass " + super_name + "\n@classinfo No information"
        
                atBlocks = dispatchDoc(commentBlock)
                if not atBlocks.has_key("@component") or not atBlocks.has_key("@componentclass"):
                    commentBlock = "@" + compType + " " + group + "\n@cppclass " + comp_name + "\n@superclass " + super_name + "\n@classinfo No information"
                
                self._parseCmdDoc(None, ccFile, commentBlock)
        
        # Parse all the code for get properties and commands
        #endRegexp = re.compile(r'NCOMPONENT_CLASS_END|NCOMPONENT_OBJECT_END')
        #code = self.dispatchGeneralCode(ccFile, endRegexp)                
        #self._parseScriptInterface(code, ccFile)
                       
        return None

    def dispatchGeneralCode (self, hf, endRegexp):        
        scriptsStr = ''
        curStr = hf.readline().strip() + "\n"
        g = endRegexp.search(curStr)
        while not g:
            scriptsStr += curStr
            curStr = hf.readline().strip() + "\n"
            g = endRegexp.search(curStr)

        return scriptsStr

    def _createProperty (self, g, types, cmd_info=None ):
        format = ''  
        
        if cmd_info:
            info = cmd_info
        else:
            info = "No info"

        for p_type in types.split(','):
            format = format + TABSPACES
            p_type = p_type.strip()
            param_name = g.group(3)[3:]
            if p_type.find("float") != -1 or p_type.find("phyreal") != -1:
                format = format + 'f' + '(' + param_name + ')'
            elif p_type.find("int") != -1 or p_type.find("nEntityObjectId") != -1:
                format = format + 'i' + '(' + param_name + ')'
            elif p_type.find("bool") != -1:
                format = format + 'b' + '(' + param_name + ')'
            elif p_type.find("vector4") != -1:
                format = format + 'ffff' + '(' + param_name + ')'
            elif p_type.find("quaternion") != -1:
                format = format + 'q' + '(' + param_name + ')'
            elif p_type.find("vector3") != -1:
                format = format + 'fff' + '(' + param_name + ')'
            elif p_type.find("vector2") != -1:
                format = format + 'ff' + '(' + param_name + ')'
            elif p_type.find("nString") != -1 or p_type.find("const char") != -1 or p_type.find("const nComponentId") != -1:
                format = format + 's' + '(' + param_name + ')'
            elif p_type.find("nRoot") != -1 or p_type.find("nObject") != -1 \
                or p_type.find("nEntityObject") != -1 or p_type.find("nEntityClass") != -1:
                format = format + 'o' + '(' + param_name + ')'
            else:
                format = format + 'v' + '(' + param_name + ')'
            format = format + "    -- No info" + "\n"
                                       
        commentBlock = "@property " + g.group(3)[3:] + "\n@format\n" + format + "@attrs \n" + "@info " + info

        self._parseCmdDoc(g, "", commentBlock)
        
    def _createPropCmds(self, cmd):
        if cmd["info"]:
            info = cmd["info"]
        else:
            info = "No information"

        if cmd["type"] == 'Get' or cmd["type"] == 'Set':
            commentBlock = "@cmd " + cmd["name"] + "\n@cmdtype "
            if cmd["type"] == "Set":
                commentBlock = commentBlock + "setter"
            else:
                commentBlock = commentBlock + "getter"
    
            commentBlock = commentBlock + "\n@property " + str(cmd["name"][3:]) + "\n@info " + str(info) + "\n"
        else:
            commentBlock = "@cmd " + cmd["name"] + "\n@input"
            for input in cmd["in"]:
                if input == 'v':
                    param_info = ''
                else:
                    param_info = "(unnamed) -- No info" 

                commentBlock += TABSPACES + input.strip() + param_info + "\n"
            commentBlock += "\n@output\n"
            for output in cmd["out"]:
                if output == 'v':
                    param_info = ''
                else:
                    param_info = "(unnamed) -- No info"

                commentBlock += TABSPACES + output.strip() + param_info + "\n"
            commentBlock += "@info " + info

        self._parseCmdDoc(None, "", commentBlock, False)

    def _parseScriptInterface(self, codeBlock, ccFile):
        CMD_DECLARATION_STRING = '^\s*NSCRIPT_ADDCMD(_COMPOBJECT|_COMPCLASS|.*)\s*\(' \
                                 '.*,' \
                                 '.*,' \
                                 '\s*([a-zA-Z][\w\d]+)\s*\,' \
                                 '\s*([0-9]+)\s*,' \
                                 '\s*(\(.*\))\s*,' \
                                 '\s*([0-9]+)\s*,' \
                                 '\s*(\(.*\))\s*' \
                                 '\).*;'

        PROP_DECLARATION_STRING = r'^\s*NSCRIPT_ADDCMD(_COMPOBJECT|_COMPCLASS|.*)\s*\(' \
                                  '.*,' \
                                  '(.*),' \
                                  '\s*((Set|Get)[a-zA-Z][\w\d]+)\s*\,' \
                                  '\s*([0-9]+)\s*,' \
                                  '\s*(\(.*\))\s*,' \
                                  '\s*([0-9]+)\s*,' \
                                  '\s*(\(.*\))\s*' \
                                  '\).*;'
                                                       
        # Regular expresions
        CMD_REGEX          = re.compile( CMD_DECLARATION_STRING )                            
        PROP_REGEX         = re.compile( PROP_DECLARATION_STRING )
        CMDCOMMENT         = re.compile(r'^\s*\/\/\/(.+)')

        cmd_info = None
        property = {}
        propcmds = []
        props = []
        props_temp = []
        # Property is created only when getter and setter are found.
        for line in codeBlock.splitlines():
            cmd = {}
            g = CMDCOMMENT.search(line)
            if g:                
                cmd_info = g.group(1)                
            else:
                g = PROP_REGEX.search(line)
                if g:
                    prop_name = g.group(3)[3:]                    
                    if prop_name in props_temp:                        
                        props.append( prop_name )
                        if g.group(3)[:3].lower() == "get":
                            types = g.group(2).strip("( ) ")
                            self._createProperty(g, types, cmd_info ) 
                        else:
                            types = g.group(6).strip("( ) ")
                            self._createProperty(g, types, cmd_info )
                    else:
                        props_temp.append( prop_name )

                    cmd["name"] = g.group(3)
                    cmd["type"] = cmd["name"][:3]
                    cmd["info"] = cmd_info
                    propcmds.append(cmd)
                    if cmd_info == None:
                        print "WARNING Command %s don't have documentation comment in file %s" %(cmd["name"], ccFile.name)
                else:
                    g = CMD_REGEX.search(line)
                    if g:
                        # normal commands
                        cmd["name"] = g.group(2)
                        cmd["type"] = "normal"
                        cmd["info"] = cmd_info
                        cmd["in"] = []
                        for type in g.group(4).strip('(').strip(')').split(","):
                            cmd["in"].append(self.get_formated_type(type))
                        cmd["out"] = []
                        for type in g.group(6).strip('(').strip(')').split(","):
                            cmd["out"].append(self.get_formated_type(type))
                        propcmds.append(cmd)
                cmd_info = None

        # Property commands
        for pcmd in propcmds:
            self._createPropCmds(pcmd)

    def get_formated_type (self, p_type):
        formated_type = 'v'

        if p_type.find("float") != -1:
            formated_type = 'f'
        elif p_type.find("int") != -1:
            formated_type = 'i'
        elif p_type.find("bool") != -1:
            formated_type = 'b'
        elif p_type.find("vector4") != -1:
            formated_type = 'ffff'
        elif p_type.find("quaternion") != -1:
            formated_type = 'q'
        elif p_type.find("vector3") != -1:
            formated_type = 'fff'
        elif p_type.find("vector2") != -1:
            formated_type = 'ff'
        elif p_type.find("nString") != -1 or p_type.find("const char") != -1:
            formated_type = 's'
        elif p_type.find("nRoot") != -1 or p_type.find("nObject") != -1 \
            or p_type.find("nEntityObject") != -1 or p_type.find("nEntityClass") != -1:
            formated_type = 'o'
        
        return formated_type

    def _parseCmdFunctionDef(self, g, ccFile, commentBlock, macro=False):
        cmdName = g.group(1)
        cmd = self.config.findCmd(cmdName)
        comment = True

        if not cmd:
            cmd = BaseCmd(cmdName, cmdName, "v", "v", "----")
            self.config.cmds.append(cmd)
        cmd.setStatus(BaseCmd.CMD_DEFINED)
        cmd_idx = self.config.cmds.index(cmd)
          
        if not commentBlock:
            comment = False
            #print "WARNING: There is no comment block (or wrong) for n_" + cmdName + "."
            commentBlock = " @cmd " + cmdName + "\n @output\n  " + cmd.outFormat + "(unnamed) -- No info\n @input\n  " +cmd.inFormat + "(unnamed) -- No info\n @info No information"
          
        atBlocks = dispatchDoc(commentBlock)
        if not atBlocks.has_key("cmd"):
            comment = False
            commentBlock = " @cmd " + cmdName + "\n @output\n  " + cmd.outFormat + "(unnamed) -- No info\n @input\n  " +cmd.inFormat + "(unnamed) -- No info\n @info No information"
            atBlocks = dispatchDoc(commentBlock)

        commentBlock = None
          
        if atBlocks.has_key("info"): cmd.info = atBlocks["info"].strip()
        else: cmd.info = "<no info>"
          
        if atBlocks["cmd"] != cmdName:
            atBlocks["cmd"] = cmdName
            # send error msg
          
        if atBlocks.has_key("property"): # getter/setter command
            tt = atBlocks["cmdtype"].split("|")
            type_s = tt[0].strip()
            subType_s = ""
            if len(tt) > 1: subType_s = tt[1].strip()
            type = 0
            subType = Cmd.M_NONE
            if type_s == "getter":
                type = Cmd.T_GETTER
                if subType_s == "count": subType = Cmd.M_COUNT
            elif type_s == "setter":
                type = Cmd.T_SETTER
                if subType_s == "begin": subType = Cmd.M_BEGIN
                elif subType_s == "add": subType = Cmd.M_ADD
                elif subType_s == "end": subType = Cmd.M_END
                else:
                    # try to guess what subType
                    if re.match(r'^begin', cmdName): subType = Cmd.M_BEGIN
                    elif re.match(r'^add', cmdName): subType = Cmd.M_ADD
                    elif re.match(r'^end', cmdName): subType = Cmd.M_END
                    else: subType = Cmd.M_NONE
            else:
                # try to guess type and subType
                if re.match(r'^set', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_NONE
                elif re.match(r'^begin', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_BEGIN
                elif re.match(r'^add', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_ADD
                elif re.match(r'^end', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_END
                else:
                    type = Cmd.T_GETTER
                    subType = Cmd.M_NONE

            propName = atBlocks["property"].strip()
            cmd = Cmd(cmd, type, subType, propName)
            cmd.comment = comment
            prop = self.config.findProperty(propName)
            #if not prop:
            #    print "_parseCmdFunctionDef"
            #    prop = CmdProperty(propName)
            #    self.config.properties.append(prop)
                # try to guess property params
            #    p_type = cmd.outFormat
            #    if type == Cmd.T_SETTER: p_type = cmd.inFormat
            #    prop.params.append(CmdParam(p_type, "name"))
        else: # this is simple command, not getter/setter
            cmd = CommonCmd(cmd)
            cmd.comment = comment
            if atBlocks.has_key("output"):
                lines = atBlocks["output"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.outParams.append(param)
                        else:
                            cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
                else:
                    cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
            else:
                cmd.outParams.append(CmdParam(cmd.outFormat, "name"))

            if atBlocks.has_key("input"):
                lines = atBlocks["input"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.inParams.append(param)
                        else:
                            cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
                else:
                    cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
            else:
                cmd.inParams.append(CmdParam(cmd.inFormat, "name"))

        # extract cmd body
        if macro:
            cmd.cmdBody = "Macro body"
        else:
            cmd.cmdBody = self.extractBody(ccFile, g.group(2))

        self.config.cmds[cmd_idx] = cmd
        return commentBlock
    
    def process_file(self, fp = "", entity=False):        
        if self.processed: return self.config
        if fp == "":
            return CmdFileConfig()
        
        file_path = fp
        #print "checking file...->", fp
        self.config = CmdFileConfig()
        
        className = ""

        #processing
        foo, class_name = os.path.split(file_path)
        #print class_name
        g = re.search(r'^(.+)\.cc$|^(.+)\.h$|^(.+)\.dox$', class_name)
        if g:
            if g.group(1) != None:
                className = g.group(1)
            elif g.group(2) != None:
                className = g.group(2)
            else:
                className = g.group(3)

            self.config.header.className = g.group(1)
        else: 
            return None
        
        ccFile = open(file_path, "rb")
        
        baseInfo = None
        commentBlock = None
        line = ccFile.readline()
        #pdb.set_trace()
        while line:
            #print className, "->", line
            g = BASEINFO_REGEX.search(line)
            if g: 
                baseInfo = self._parseBaseInfo(g, baseInfo, ccFile)
            else:
                g = INCLUDE_REGEX.search(line)
                if g: baseInfo = self._parseInclude(g, baseInfo)                    
                else:
                    g = CMDFUNCTIONDECL_REGEX.search(line)
                    if g: baseInfo = self._parseCmdFunctionDecl(g, baseInfo)
                    else:
                        g = CMDDOC_REGEX.search(line) #extract documentation
                        if g:  
                            commentBlock = dispatchExtComment(ccFile, g.group(1))
                            commentBlock = self._parseCmdDoc(g, ccFile, commentBlock)
                        else:
                            g = INITCMDFUNCTIONDEF_REGEX.search(line)
                            if g: 
                                commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)
                            else:
                                g = INITCMDMACRODEF_REGEX.search(line)
                                if g:
                                    endRegexp = re.compile(r'NSCRIPT_INITCMDS_BEGIN|NSCRIPT_INITCMDS_END')
                                    code = self.dispatchGeneralCode(ccFile, endRegexp)
                                    commentBlock = self._parseScriptInterface(code, ccFile)
                                else:
                                    g = NCLASSDEF_REGEX.search(line)
                                    if g:
                                        commentBlock = self._parseClassDef(g, ccFile, commentBlock, className, "scriptclass")
                                    else:
                                        g = ADDCMDFUNCTION_REGEX.search(line)
                                        if g: self._parseAddCmdFunction(g)
                                        else:
                                            g = CMDFUNCTIONDEF_REGEX.search(line)
                                            if g:
                                                commentBlock = self._parseCmdFunctionDef(g, ccFile, commentBlock)                                        
                                            else:
                                                g = SAVECMDDEF_REGEX.search(line)
                                                if g:
                                                    c = ""
                                                    if commentBlock: c = commentBlock
                                                    body = self.extractBody(ccFile, g.group(3))
                                                    self.config.saveCmdsFunc = SaveCmdsFunction(g.group(2), body, c)
                                                else:
                                                    g = INITCLASSCMDDEF_REGEX.search(line)
                                                    if g:
                                                        commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)                                            
                                                    else:
                                                        g = NCOMPONENTCLASSSUBDEF_REGEX.search(line)
                                                        if g:
                                                            commentBlock = self._parseComponentDef(g, ccFile, commentBlock, "componentclass")                                                        
                                                        else:
                                                            g = NCOMPONENTOBJECTSUBDEF_REGEX.search(line)
                                                            if g:
                                                                commentBlock = self._parseComponentDef(g, ccFile, commentBlock, "component")                                                           
                                                            else:
                                                                g = NNEBULAENTITY_REGEX.search(line)
                                                                if g:
                                                                    if entity:
                                                                        commentBlock = self._parseEntityMacro(ccFile, className, commentBlock) 
                                                                    else:
                                                                        #print "Detected nNebulaEntity macro but bool not passed, so ignore the file"
                                                                        return None
            line = ccFile.readline() #read next line from file
        ccFile.close()
        
        #self.config.sortCmds()
        self.processed = True
        if entity:
            return self.entityConfigList
        else:
            return self.config    

    def process(self, is_contrib = False, mod_name = None, pkg_name = None, file_name = None):
        if not mod_name: mod_name = self.modName
        if not pkg_name: pkg_name = self.pkgName
        if not file_name: file_name = self.fileName
        self.setCmdFile(is_contrib, mod_name, pkg_name, file_name)
        
        if self.processed: return self.config
        
        #print "Start processing: " + self.fileName
        
        s = ""
        if self.isContrib: s = "contrib/"
        file_path = self.codeDir + s + self.modName + "/src/" + self.pkgName + "/" + self.fileName
        
        className = ""
        #processing        
        g = re.search(r'^(.+)_cmds.cc$', self.fileName)
        if g:
            className = g.group(1)
            self.config.header.className = g.group(1)            
        else: return None
        
        ccFile = open(file_path, "rb")
        #tmpFile = open(fileName + ".new", "wb")
        
        baseInfo = None
        commentBlock = None
        
        line = ccFile.readline()        
        
        while line:
            g = BASEINFO_REGEX.search(line)
            if g: baseInfo = self._parseBaseInfo(g, baseInfo)
            else:
                g = INCLUDE_REGEX.search(line)
                if g: baseInfo = self._parseInclude(g, baseInfo)                    
                else:
                    g = CMDFUNCTIONDECL_REGEX.search(line)
                    if g: baseInfo = self._parseCmdFunctionDecl(g, baseInfo)
                    else:
                        if g:  
                            commentBlock = dispatchExtComment(ccFile, g.group(1))
                            commentBlock = self._parseCmdDoc(g, ccFile, commentBlock)
                        else:
                            g = INITCMDFUNCTIONDEF_REGEX.search(line)
                            if g: 
                                commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)
                            else:
                                g = INITCMDMACRODEF_REGEX.search(line)
                                if g:
                                    commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)
                                else:
                                    g = ADDCMDFUNCTION_REGEX.search(line)
                                    if g: self._parseAddCmdFunction(g)
                                    else:
                                        g = CMDFUNCTIONDEF_REGEX.search(line)
                                        if g: 
                                            commentBlock = self._parseCmdFunctionDef(g, ccFile, commentBlock)
                                        else:
                                            g = CMDMACRODEF_REGEX.search(line)
                                            if g:
                                                commentBlock = self._parseCmdFunctionDef(g, ccFile, commentBlock, True)
                                            else:
                                                g = SAVECMDDEF_REGEX.search(line)
                                                if g:
                                                    c = ""
                                                    if commentBlock: c = commentBlock
                                                    body = self.extractBody(ccFile, g.group(3))
                                                    self.config.saveCmdsFunc = SaveCmdsFunction(g.group(2), body, c)
                                                else:
                                                    g = INITCLASSCMDDEF_REGEX.search(line)
                                                    if g:
                                                        commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)



                                                                                                    #else: pass
            line = ccFile.readline() #read next line from file
        ccFile.close()
        
        #self.config.sortCmds()
        self.processed = True
        return self.config    
  


### testing
##p = CmdFileProcessor("D:\\Projects\\VisualC\\nebula2\\code\\")
##
### D:\Projects\VisualC\nebula2\code\contrib\bombsquadbruce\src\BombsquadBruce\bbcamera_cmds.cc 
###p.setCmdFile(True, "bombsquadbruce", "BombsquadBruce", "bbcamera_cmds.cc")
##
### D:\Projects\VisualC\nebula2\code\nebula2\src\scene\nabstractshadernode_cmds.cc 
##p.setCmdFile(False, "nebula2", "scene", "nabstractshadernode_cmds.cc")
##
##p.process().saveTo("test.log")
