# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

"""
The `PropertyGrid` provides a specialized two-column grid for editing
properties such as strings, numbers, colours, and string lists.
"""

import _propgrid

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if hasattr(self,name) or (name in ("this", "thisown")):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
PropertyGridNameStr = "wxPropertyGrid"
PropertyGridManagerNameStr = "wxPropertyGridManager"

PG_VERSION = _propgrid.PG_VERSION
PG_INCLUDE_BASICPROPS = _propgrid.PG_INCLUDE_BASICPROPS
PG_INCLUDE_ADVPROPS = _propgrid.PG_INCLUDE_ADVPROPS
PG_INCLUDE_CHECKBOX = _propgrid.PG_INCLUDE_CHECKBOX
PG_USE_CLIENT_DATA = _propgrid.PG_USE_CLIENT_DATA
PG_INCLUDE_WXOBJECT = _propgrid.PG_INCLUDE_WXOBJECT
PG_FULL_CUSTOM_PAINT_WIDTH = _propgrid.PG_FULL_CUSTOM_PAINT_WIDTH
PG_AUTO_SORT = _propgrid.PG_AUTO_SORT
PG_HIDE_CATEGORIES = _propgrid.PG_HIDE_CATEGORIES
PG_ALPHABETIC_MODE = _propgrid.PG_ALPHABETIC_MODE
PG_BOLD_MODIFIED = _propgrid.PG_BOLD_MODIFIED
PG_SPLITTER_AUTO_CENTER = _propgrid.PG_SPLITTER_AUTO_CENTER
PG_TOOLTIPS = _propgrid.PG_TOOLTIPS
PG_HIDE_MARGIN = _propgrid.PG_HIDE_MARGIN
PG_STATIC_SPLITTER = _propgrid.PG_STATIC_SPLITTER
PG_STATIC_LAYOUT = _propgrid.PG_STATIC_LAYOUT
PG_LIMITED_EDITING = _propgrid.PG_LIMITED_EDITING
PG_TOOLBAR = _propgrid.PG_TOOLBAR
PG_DESCRIPTION = _propgrid.PG_DESCRIPTION
PG_COMPACTOR = _propgrid.PG_COMPACTOR
PG_EX_INIT_NOCAT = _propgrid.PG_EX_INIT_NOCAT
PG_EX_NO_FLAT_TOOLBAR = _propgrid.PG_EX_NO_FLAT_TOOLBAR
PG_EX_MODE_BUTTONS = _propgrid.PG_EX_MODE_BUTTONS
PG_EX_HELP_AS_TOOLTIPS = _propgrid.PG_EX_HELP_AS_TOOLTIPS
PG_DEFAULT_STYLE = _propgrid.PG_DEFAULT_STYLE
PGMAN_DEFAULT_STYLE = _propgrid.PGMAN_DEFAULT_STYLE
PG_KEEP_STRUCTURE = _propgrid.PG_KEEP_STRUCTURE
PG_RECURSE = _propgrid.PG_RECURSE
PG_LOW = _propgrid.PG_LOW
PG_HIGH = _propgrid.PG_HIGH
PG_FULL_VALUE = _propgrid.PG_FULL_VALUE
PG_REPORT_ERROR = _propgrid.PG_REPORT_ERROR
PG_PROPERTY_SPECIFIC = _propgrid.PG_PROPERTY_SPECIFIC
PG_ID_IS_PTR = _propgrid.PG_ID_IS_PTR

def PGTypeOperationFailed(*args, **kwargs):
    """PGTypeOperationFailed(PGProperty p, wxChar typestr, wxChar op)"""
    return _propgrid.PGTypeOperationFailed(*args, **kwargs)

def PGGetFailed(*args, **kwargs):
    """PGGetFailed(PGProperty p, wxChar typestr)"""
    return _propgrid.PGGetFailed(*args, **kwargs)
PG_PROP_MODIFIED = _propgrid.PG_PROP_MODIFIED
PG_PROP_DISABLED = _propgrid.PG_PROP_DISABLED
PG_PROP_HIDEABLE = _propgrid.PG_PROP_HIDEABLE
PG_PROP_CUSTOMIMAGE = _propgrid.PG_PROP_CUSTOMIMAGE
PG_PROP_NOEDITOR = _propgrid.PG_PROP_NOEDITOR
PG_PROP_UNSPECIFIED = _propgrid.PG_PROP_UNSPECIFIED
PG_PROP_CLASS_SPECIFIC_1 = _propgrid.PG_PROP_CLASS_SPECIFIC_1
PG_PROP_CLASS_SPECIFIC_2 = _propgrid.PG_PROP_CLASS_SPECIFIC_2
PG_INHERITED_PROPFLAGS = _propgrid.PG_INHERITED_PROPFLAGS
PG_BOOL_USE_CHECKBOX = _propgrid.PG_BOOL_USE_CHECKBOX
PG_BOOL_USE_DOUBLE_CLICK_CYCLING = _propgrid.PG_BOOL_USE_DOUBLE_CLICK_CYCLING
PG_FLOAT_PRECISION = _propgrid.PG_FLOAT_PRECISION
PG_STRING_PASSWORD = _propgrid.PG_STRING_PASSWORD
PG_UINT_BASE = _propgrid.PG_UINT_BASE
PG_UINT_PREFIX = _propgrid.PG_UINT_PREFIX
PG_FILE_WILDCARD = _propgrid.PG_FILE_WILDCARD
PG_FILE_SHOW_FULL_PATH = _propgrid.PG_FILE_SHOW_FULL_PATH
PG_FILE_SHOW_RELATIVE_PATH = _propgrid.PG_FILE_SHOW_RELATIVE_PATH
PG_FILE_INITIAL_PATH = _propgrid.PG_FILE_INITIAL_PATH
PG_FILE_DIALOG_TITLE = _propgrid.PG_FILE_DIALOG_TITLE
PG_DIR_DIALOG_MESSAGE = _propgrid.PG_DIR_DIALOG_MESSAGE
PG_DATE_FORMAT = _propgrid.PG_DATE_FORMAT
PG_DATE_PICKER_STYLE = _propgrid.PG_DATE_PICKER_STYLE
PG_CUSTOM_PAINT_CALLBACK = _propgrid.PG_CUSTOM_PAINT_CALLBACK
PG_CUSTOM_PRIVATE_CHILDREN = _propgrid.PG_CUSTOM_PRIVATE_CHILDREN
PG_COLOUR_ALLOW_CUSTOM = _propgrid.PG_COLOUR_ALLOW_CUSTOM
PG_USER_ATTRIBUTE = _propgrid.PG_USER_ATTRIBUTE

def PGConstructTextCtrlEditorClass(*args, **kwargs):
    """PGConstructTextCtrlEditorClass() -> PGEditor"""
    return _propgrid.PGConstructTextCtrlEditorClass(*args, **kwargs)

def PGConstructChoiceEditorClass(*args, **kwargs):
    """PGConstructChoiceEditorClass() -> PGEditor"""
    return _propgrid.PGConstructChoiceEditorClass(*args, **kwargs)

def PGConstructComboBoxEditorClass(*args, **kwargs):
    """PGConstructComboBoxEditorClass() -> PGEditor"""
    return _propgrid.PGConstructComboBoxEditorClass(*args, **kwargs)

def PGConstructTextCtrlAndButtonEditorClass(*args, **kwargs):
    """PGConstructTextCtrlAndButtonEditorClass() -> PGEditor"""
    return _propgrid.PGConstructTextCtrlAndButtonEditorClass(*args, **kwargs)

def PGConstructCheckBoxEditorClass(*args, **kwargs):
    """PGConstructCheckBoxEditorClass() -> PGEditor"""
    return _propgrid.PGConstructCheckBoxEditorClass(*args, **kwargs)

def PGConstructChoiceAndButtonEditorClass(*args, **kwargs):
    """PGConstructChoiceAndButtonEditorClass() -> PGEditor"""
    return _propgrid.PGConstructChoiceAndButtonEditorClass(*args, **kwargs)
class PGValueType(object):
    """Proxy of C++ PGValueType class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGValueType instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_propgrid.delete_PGValueType):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetTypeName(*args, **kwargs):
        """GetTypeName(self) -> wxChar"""
        return _propgrid.PGValueType_GetTypeName(*args, **kwargs)

    def GetCustomTypeName(*args, **kwargs):
        """GetCustomTypeName(self) -> wxChar"""
        return _propgrid.PGValueType_GetCustomTypeName(*args, **kwargs)

    def GetDefaultValue(*args, **kwargs):
        """GetDefaultValue(self) -> PGVariant"""
        return _propgrid.PGValueType_GetDefaultValue(*args, **kwargs)

    def GenerateVariant(*args, **kwargs):
        """GenerateVariant(self, PGVariant value, String name) -> wxVariant"""
        return _propgrid.PGValueType_GenerateVariant(*args, **kwargs)

    def GenerateProperty(*args, **kwargs):
        """GenerateProperty(self, String label, String name) -> PGProperty"""
        return _propgrid.PGValueType_GenerateProperty(*args, **kwargs)

    def SetValueFromVariant(*args, **kwargs):
        """SetValueFromVariant(self, PGProperty property, wxVariant value)"""
        return _propgrid.PGValueType_SetValueFromVariant(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> wxChar"""
        return _propgrid.PGValueType_GetType(*args, **kwargs)


class PGValueTypePtr(PGValueType):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGValueType
_propgrid.PGValueType_swigregister(PGValueTypePtr)
cvar = _propgrid.cvar

class PGVariantUnion(object):
    """Proxy of C++ PGVariantUnion class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGVariantUnion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    m_long = property(_propgrid.PGVariantUnion_m_long_get, _propgrid.PGVariantUnion_m_long_set)
    m_ptr = property(_propgrid.PGVariantUnion_m_ptr_get, _propgrid.PGVariantUnion_m_ptr_set)
    m_bool = property(_propgrid.PGVariantUnion_m_bool_get, _propgrid.PGVariantUnion_m_bool_set)

class PGVariantUnionPtr(PGVariantUnion):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGVariantUnion
_propgrid.PGVariantUnion_swigregister(PGVariantUnionPtr)

class PGVariant(object):
    """Proxy of C++ PGVariant class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGVariant instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PGVariant"""
        newobj = _propgrid.new_PGVariant(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRawLong(*args, **kwargs):
        """GetRawLong(self) -> long"""
        return _propgrid.PGVariant_GetRawLong(*args, **kwargs)

    def GetRawPtr(*args, **kwargs):
        """GetRawPtr(self) -> void"""
        return _propgrid.PGVariant_GetRawPtr(*args, **kwargs)

    m_v = property(_propgrid.PGVariant_m_v_get, _propgrid.PGVariant_m_v_set)

class PGVariantPtr(PGVariant):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGVariant
_propgrid.PGVariant_swigregister(PGVariantPtr)

class PGProperty(object):
    """Proxy of C++ PGProperty class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGProperty instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def SetCallBackDialog(*args, **kwargs):
        """SetCallBackDialog(self, Object ??)"""
        return _propgrid.PGProperty_SetCallBackDialog(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGProperty):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def DoSetValue(*args, **kwargs):
        """DoSetValue(self, PGVariant value)"""
        return _propgrid.PGProperty_DoSetValue(*args, **kwargs)

    def DoGetValue(*args, **kwargs):
        """DoGetValue(self) -> PGVariant"""
        return _propgrid.PGProperty_DoGetValue(*args, **kwargs)

    def GetValueAsString(*args, **kwargs):
        """GetValueAsString(self, int argFlags=0) -> String"""
        return _propgrid.PGProperty_GetValueAsString(*args, **kwargs)

    def GetValueAsLong(*args, **kwargs):
        """GetValueAsLong(self) -> long"""
        return _propgrid.PGProperty_GetValueAsLong(*args, **kwargs)

    def SetValueFromString(*args, **kwargs):
        """SetValueFromString(self, String text, int flags=0) -> bool"""
        return _propgrid.PGProperty_SetValueFromString(*args, **kwargs)

    def SetValueFromInt(*args, **kwargs):
        """SetValueFromInt(self, long value, int flags=0) -> bool"""
        return _propgrid.PGProperty_SetValueFromInt(*args, **kwargs)

    def GetImageSize(*args, **kwargs):
        """GetImageSize(self) -> Size"""
        return _propgrid.PGProperty_GetImageSize(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """OnEvent(self, PropertyGrid propgrid, Window wnd_primary, Event event) -> bool"""
        return _propgrid.PGProperty_OnEvent(*args, **kwargs)

    def GetClassName(*args, **kwargs):
        """GetClassName(self) -> wxChar"""
        return _propgrid.PGProperty_GetClassName(*args, **kwargs)

    def GetValueType(*args, **kwargs):
        """GetValueType(self) -> PGValueType"""
        return _propgrid.PGProperty_GetValueType(*args, **kwargs)

    def DoGetEditorClass(*args, **kwargs):
        """DoGetEditorClass(self) -> PGEditor"""
        return _propgrid.PGProperty_DoGetEditorClass(*args, **kwargs)

    def DoGetValidator(*args, **kwargs):
        """DoGetValidator(self) -> Validator"""
        return _propgrid.PGProperty_DoGetValidator(*args, **kwargs)

    def GetParentingType(*args, **kwargs):
        """GetParentingType(self) -> signed char"""
        return _propgrid.PGProperty_GetParentingType(*args, **kwargs)

    def GetChoiceInfo(*args, **kwargs):
        """GetChoiceInfo(self, wxPGChoiceInfo choiceinfo) -> int"""
        return _propgrid.PGProperty_GetChoiceInfo(*args, **kwargs)

    def OnCustomPaint(*args, **kwargs):
        """OnCustomPaint(self, DC dc, Rect rect, PGPaintData paintdata)"""
        return _propgrid.PGProperty_OnCustomPaint(*args, **kwargs)

    def SetAttribute(*args, **kwargs):
        """SetAttribute(self, int id, wxVariant value)"""
        return _propgrid.PGProperty_SetAttribute(*args, **kwargs)

    def AppendChoice(*args, **kwargs):
        """AppendChoice(self, String label, int value=INT_MAX) -> int"""
        return _propgrid.PGProperty_AppendChoice(*args, **kwargs)

    def DeleteChoice(*args, **kwargs):
        """DeleteChoice(self, int index)"""
        return _propgrid.PGProperty_DeleteChoice(*args, **kwargs)

    def GetAttributes(*args, **kwargs):
        """GetAttributes(self, unsigned int flagmask=0xFFFF) -> String"""
        return _propgrid.PGProperty_GetAttributes(*args, **kwargs)

    def GetClassInfo(*args, **kwargs):
        """GetClassInfo(self) -> wxPGPropertyClassInfo"""
        return _propgrid.PGProperty_GetClassInfo(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> String"""
        return _propgrid.PGProperty_GetLabel(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _propgrid.PGProperty_GetName(*args, **kwargs)

    def DoSetName(*args, **kwargs):
        """DoSetName(self, String str)"""
        return _propgrid.PGProperty_DoSetName(*args, **kwargs)

    def EnsureDataExt(*args, **kwargs):
        """EnsureDataExt(self) -> bool"""
        return _propgrid.PGProperty_EnsureDataExt(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _propgrid.PGProperty_GetY(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, Window primary)"""
        return _propgrid.PGProperty_UpdateControl(*args, **kwargs)

    def GetDisplayedString(*args, **kwargs):
        """GetDisplayedString(self) -> String"""
        return _propgrid.PGProperty_GetDisplayedString(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> PGProperty"""
        return _propgrid.PGProperty_GetId(*args, **kwargs)

    def GetGrid(*args, **kwargs):
        """GetGrid(self) -> PropertyGrid"""
        return _propgrid.PGProperty_GetGrid(*args, **kwargs)

    def GetMainParent(*args, **kwargs):
        """GetMainParent(self) -> PGProperty"""
        return _propgrid.PGProperty_GetMainParent(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent(self) -> PGPropertyWithChildren"""
        return _propgrid.PGProperty_GetParent(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _propgrid.PGProperty_IsOk(*args, **kwargs)

    def IsFlagSet(*args, **kwargs):
        """IsFlagSet(self, unsigned char flag) -> bool"""
        return _propgrid.PGProperty_IsFlagSet(*args, **kwargs)

    def CanHaveExtraChildren(*args, **kwargs):
        """CanHaveExtraChildren(self) -> bool"""
        return _propgrid.PGProperty_CanHaveExtraChildren(*args, **kwargs)

    def GetDataExt(*args, **kwargs):
        """GetDataExt(self) -> wxPGPropertyDataExt"""
        return _propgrid.PGProperty_GetDataExt(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> unsigned int"""
        return _propgrid.PGProperty_GetFlags(*args, **kwargs)

    def GetEditorClass(*args, **kwargs):
        """GetEditorClass(self) -> PGEditor"""
        return _propgrid.PGProperty_GetEditorClass(*args, **kwargs)

    def GetType(*args, **kwargs):
        """GetType(self) -> wxChar"""
        return _propgrid.PGProperty_GetType(*args, **kwargs)

    def InsertChoice(*args, **kwargs):
        """InsertChoice(self, String label, int index, int value=INT_MAX) -> int"""
        return _propgrid.PGProperty_InsertChoice(*args, **kwargs)

    def IsKindOf(*args, **kwargs):
        """IsKindOf(self, wxPGPropertyClassInfo info) -> bool"""
        return _propgrid.PGProperty_IsKindOf(*args, **kwargs)

    def IsSubProperty(*args, **kwargs):
        """IsSubProperty(self) -> bool"""
        return _propgrid.PGProperty_IsSubProperty(*args, **kwargs)

    def GetMaxLength(*args, **kwargs):
        """GetMaxLength(self) -> int"""
        return _propgrid.PGProperty_GetMaxLength(*args, **kwargs)

    def GetValue(self):
        return self.GetGrid().GetPropertyValue(self)

    def GetValueImage(*args, **kwargs):
        """GetValueImage(self) -> Bitmap"""
        return _propgrid.PGProperty_GetValueImage(*args, **kwargs)

    def GetChildCount(*args, **kwargs):
        """GetChildCount(self) -> size_t"""
        return _propgrid.PGProperty_GetChildCount(*args, **kwargs)

    def GetArrIndex(*args, **kwargs):
        """GetArrIndex(self) -> unsigned int"""
        return _propgrid.PGProperty_GetArrIndex(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth(self) -> unsigned int"""
        return _propgrid.PGProperty_GetDepth(*args, **kwargs)

    def GetIndexInParent(*args, **kwargs):
        """GetIndexInParent(self) -> unsigned int"""
        return _propgrid.PGProperty_GetIndexInParent(*args, **kwargs)

    def Hide(*args, **kwargs):
        """Hide(self, bool hide) -> bool"""
        return _propgrid.PGProperty_Hide(*args, **kwargs)

    def RecreateEditor(*args, **kwargs):
        """RecreateEditor(self) -> bool"""
        return _propgrid.PGProperty_RecreateEditor(*args, **kwargs)

    def SetAttrib(*args, **kwargs):
        """SetAttrib(self, int id, wxVariant value)"""
        return _propgrid.PGProperty_SetAttrib(*args, **kwargs)

    def SetAttributes(*args, **kwargs):
        """SetAttributes(self, String attributes)"""
        return _propgrid.PGProperty_SetAttributes(*args, **kwargs)

    def SetEditor(*args, **kwargs):
        """SetEditor(self, String editorName)"""
        return _propgrid.PGProperty_SetEditor(*args, **kwargs)

    def SetValueImage(*args, **kwargs):
        """SetValueImage(self, Bitmap bmp)"""
        return _propgrid.PGProperty_SetValueImage(*args, **kwargs)

    def SetChoicesExclusive(*args, **kwargs):
        """SetChoicesExclusive(self)"""
        return _propgrid.PGProperty_SetChoicesExclusive(*args, **kwargs)

    def SetFlag(*args, **kwargs):
        """SetFlag(self, unsigned char flag)"""
        return _propgrid.PGProperty_SetFlag(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, String helpString)"""
        return _propgrid.PGProperty_SetHelpString(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, String label)"""
        return _propgrid.PGProperty_SetLabel(*args, **kwargs)

    def SetValidator(*args, **kwargs):
        """SetValidator(self, Validator validator)"""
        return _propgrid.PGProperty_SetValidator(*args, **kwargs)

    def GetValidator(*args, **kwargs):
        """GetValidator(self) -> Validator"""
        return _propgrid.PGProperty_GetValidator(*args, **kwargs)

    def StdValidationProcedure(*args, **kwargs):
        """StdValidationProcedure(self, PGVariant value) -> bool"""
        return _propgrid.PGProperty_StdValidationProcedure(*args, **kwargs)

    def PrepareValueForDialogEditing(*args, **kwargs):
        """PrepareValueForDialogEditing(self, PropertyGrid propgrid) -> bool"""
        return _propgrid.PGProperty_PrepareValueForDialogEditing(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """GetClientData(self) -> void"""
        return _propgrid.PGProperty_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """SetClientData(self, void clientData)"""
        return _propgrid.PGProperty_SetClientData(*args, **kwargs)

    def SetChoices(*args):
        """
        SetChoices(self, PGChoices choices) -> bool
        SetChoices(self, wxArrayString labels, wxArrayInt values=wxArrayInt_wxPG_EMPTY) -> bool
        """
        return _propgrid.PGProperty_SetChoices(*args)

    def SetMaxLength(*args, **kwargs):
        """SetMaxLength(self, int maxLen) -> bool"""
        return _propgrid.PGProperty_SetMaxLength(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self) -> String"""
        return _propgrid.PGProperty_GetHelpString(*args, **kwargs)

    def ClearFlag(*args, **kwargs):
        """ClearFlag(self, unsigned char flag)"""
        return _propgrid.PGProperty_ClearFlag(*args, **kwargs)

    def IsSomeParent(*args, **kwargs):
        """IsSomeParent(self, PGProperty candidate_parent) -> bool"""
        return _propgrid.PGProperty_IsSomeParent(*args, **kwargs)

    def ShowError(*args, **kwargs):
        """ShowError(self, String msg)"""
        return _propgrid.PGProperty_ShowError(*args, **kwargs)


class PGPropertyPtr(PGProperty):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGProperty
_propgrid.PGProperty_swigregister(PGPropertyPtr)

class PGPropertyWithChildren(PGProperty):
    """Proxy of C++ PGPropertyWithChildren class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGPropertyWithChildren instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_propgrid.delete_PGPropertyWithChildren):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetValueAsString(*args, **kwargs):
        """GetValueAsString(self, int argFlags=0) -> String"""
        return _propgrid.PGPropertyWithChildren_GetValueAsString(*args, **kwargs)

    def SetValueFromString(*args, **kwargs):
        """SetValueFromString(self, String text, int flags) -> bool"""
        return _propgrid.PGPropertyWithChildren_SetValueFromString(*args, **kwargs)

    def RefreshChildren(*args, **kwargs):
        """RefreshChildren(self)"""
        return _propgrid.PGPropertyWithChildren_RefreshChildren(*args, **kwargs)

    def ChildChanged(*args, **kwargs):
        """ChildChanged(self, PGProperty p)"""
        return _propgrid.PGPropertyWithChildren_ChildChanged(*args, **kwargs)

    def AddChild(*args, **kwargs):
        """AddChild(self, PGProperty prop)"""
        return _propgrid.PGPropertyWithChildren_AddChild(*args, **kwargs)

    def AddChild2(*args, **kwargs):
        """AddChild2(self, PGProperty prop, int index=-1, bool correct_mode=True)"""
        return _propgrid.PGPropertyWithChildren_AddChild2(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> size_t"""
        return _propgrid.PGPropertyWithChildren_GetCount(*args, **kwargs)

    def Item(*args, **kwargs):
        """Item(self, size_t i) -> PGProperty"""
        return _propgrid.PGPropertyWithChildren_Item(*args, **kwargs)

    def Last(*args, **kwargs):
        """Last(self) -> PGProperty"""
        return _propgrid.PGPropertyWithChildren_Last(*args, **kwargs)

    def Index(*args, **kwargs):
        """Index(self, PGProperty p) -> int"""
        return _propgrid.PGPropertyWithChildren_Index(*args, **kwargs)

    def Empty(*args, **kwargs):
        """Empty(self)"""
        return _propgrid.PGPropertyWithChildren_Empty(*args, **kwargs)

    def IsExpanded(*args, **kwargs):
        """IsExpanded(self) -> bool"""
        return _propgrid.PGPropertyWithChildren_IsExpanded(*args, **kwargs)

    def FixIndexesOfChildren(*args, **kwargs):
        """FixIndexesOfChildren(self, size_t starthere=0)"""
        return _propgrid.PGPropertyWithChildren_FixIndexesOfChildren(*args, **kwargs)

    def GetItemAtY(*args, **kwargs):
        """GetItemAtY(self, unsigned int y, unsigned int lh) -> PGProperty"""
        return _propgrid.PGPropertyWithChildren_GetItemAtY(*args, **kwargs)

    def GetPropertyByName(*args, **kwargs):
        """GetPropertyByName(self, String name) -> PGProperty"""
        return _propgrid.PGPropertyWithChildren_GetPropertyByName(*args, **kwargs)


class PGPropertyWithChildrenPtr(PGPropertyWithChildren):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGPropertyWithChildren
_propgrid.PGPropertyWithChildren_swigregister(PGPropertyWithChildrenPtr)

class PGRootPropertyClass(PGPropertyWithChildren):
    """Proxy of C++ PGRootPropertyClass class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGRootPropertyClass instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetValueType(*args, **kwargs):
        """GetValueType(self) -> PGValueType"""
        return _propgrid.PGRootPropertyClass_GetValueType(*args, **kwargs)

    def DoGetEditorClass(*args, **kwargs):
        """DoGetEditorClass(self) -> PGEditor"""
        return _propgrid.PGRootPropertyClass_DoGetEditorClass(*args, **kwargs)

    def GetClassName(*args, **kwargs):
        """GetClassName(self) -> wxChar"""
        return _propgrid.PGRootPropertyClass_GetClassName(*args, **kwargs)

    def GetClassInfo(*args, **kwargs):
        """GetClassInfo(self) -> wxPGPropertyClassInfo"""
        return _propgrid.PGRootPropertyClass_GetClassInfo(*args, **kwargs)

    def __init__(self, *args, **kwargs):
        """__init__(self) -> PGRootPropertyClass"""
        newobj = _propgrid.new_PGRootPropertyClass(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PGRootPropertyClass):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class PGRootPropertyClassPtr(PGRootPropertyClass):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGRootPropertyClass
_propgrid.PGRootPropertyClass_swigregister(PGRootPropertyClassPtr)

class PropertyCategoryClass(PGPropertyWithChildren):
    """Proxy of C++ PropertyCategoryClass class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyCategoryClass instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetValueType(*args, **kwargs):
        """GetValueType(self) -> PGValueType"""
        return _propgrid.PropertyCategoryClass_GetValueType(*args, **kwargs)

    def DoGetEditorClass(*args, **kwargs):
        """DoGetEditorClass(self) -> PGEditor"""
        return _propgrid.PropertyCategoryClass_DoGetEditorClass(*args, **kwargs)

    def GetClassName(*args, **kwargs):
        """GetClassName(self) -> wxChar"""
        return _propgrid.PropertyCategoryClass_GetClassName(*args, **kwargs)

    def GetClassInfo(*args, **kwargs):
        """GetClassInfo(self) -> wxPGPropertyClassInfo"""
        return _propgrid.PropertyCategoryClass_GetClassInfo(*args, **kwargs)

    def __init__(self, *args):
        """
        __init__(self) -> PropertyCategoryClass
        __init__(self, String label, String name=wxString_wxPG_LABEL) -> PropertyCategoryClass
        """
        newobj = _propgrid.new_PropertyCategoryClass(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PropertyCategoryClass):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetValueAsString(*args, **kwargs):
        """GetValueAsString(self, int argFlags) -> String"""
        return _propgrid.PropertyCategoryClass_GetValueAsString(*args, **kwargs)

    def GetTextExtent(*args, **kwargs):
        """GetTextExtent(self) -> int"""
        return _propgrid.PropertyCategoryClass_GetTextExtent(*args, **kwargs)

    def CalculateTextExtent(*args, **kwargs):
        """CalculateTextExtent(self, Window wnd, Font font)"""
        return _propgrid.PropertyCategoryClass_CalculateTextExtent(*args, **kwargs)


class PropertyCategoryClassPtr(PropertyCategoryClass):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyCategoryClass
_propgrid.PropertyCategoryClass_swigregister(PropertyCategoryClassPtr)

class PGChoices(object):
    """Proxy of C++ PGChoices class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGChoices instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> PGChoices
        __init__(self, PGChoices a) -> PGChoices
        __init__(self, wxChar labels, long values=None) -> PGChoices
        __init__(self, wxArrayString labels, wxArrayInt values=wxArrayInt_wxPG_EMPTY) -> PGChoices
        __init__(self, wxPGChoicesData data) -> PGChoices
        """
        newobj = _propgrid.new_PGChoices(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PGChoices):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AssignData(*args, **kwargs):
        """AssignData(self, wxPGChoicesData data)"""
        return _propgrid.PGChoices_AssignData(*args, **kwargs)

    def Assign(*args, **kwargs):
        """Assign(self, PGChoices a)"""
        return _propgrid.PGChoices_Assign(*args, **kwargs)

    def Add(*args):
        """
        Add(self, wxChar labels, long values=None)
        Add(self, wxArrayString arr, long values=None)
        Add(self, wxArrayString arr, wxArrayInt arrint)
        Add(self, wxChar label, int value=INT_MAX)
        """
        return _propgrid.PGChoices_Add(*args)

    def AddAsSorted(*args, **kwargs):
        """AddAsSorted(self, String label, int value=INT_MAX)"""
        return _propgrid.PGChoices_AddAsSorted(*args, **kwargs)

    def EnsureData(*args, **kwargs):
        """EnsureData(self)"""
        return _propgrid.PGChoices_EnsureData(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _propgrid.PGChoices_IsOk(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> wxPGChoicesId"""
        return _propgrid.PGChoices_GetId(*args, **kwargs)

    def RemoveAt(*args, **kwargs):
        """RemoveAt(self, size_t nIndex, size_t count=1)"""
        return _propgrid.PGChoices_RemoveAt(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, wxArrayString labels, wxArrayInt values=wxArrayInt_wxPG_EMPTY)"""
        return _propgrid.PGChoices_Set(*args, **kwargs)

    def SetExclusive(*args, **kwargs):
        """SetExclusive(self)"""
        return _propgrid.PGChoices_SetExclusive(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self, size_t ind) -> String"""
        return _propgrid.PGChoices_GetLabel(*args, **kwargs)

    def GetLabels(*args):
        """
        GetLabels(self) -> wxArrayString
        GetLabels(self) -> wxArrayString
        """
        return _propgrid.PGChoices_GetLabels(*args)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> size_t"""
        return _propgrid.PGChoices_GetCount(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self, size_t ind) -> int"""
        return _propgrid.PGChoices_GetValue(*args, **kwargs)

    def GetValues(*args):
        """
        GetValues(self) -> wxArrayInt
        GetValues(self) -> wxArrayInt
        """
        return _propgrid.PGChoices_GetValues(*args)

    def Index(*args, **kwargs):
        """Index(self, String str) -> int"""
        return _propgrid.PGChoices_Index(*args, **kwargs)

    def Insert(*args, **kwargs):
        """Insert(self, wxChar label, int index, int value=INT_MAX)"""
        return _propgrid.PGChoices_Insert(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData(self) -> wxPGChoicesData"""
        return _propgrid.PGChoices_GetData(*args, **kwargs)

    def GetDataPtr(*args, **kwargs):
        """GetDataPtr(self) -> wxPGChoicesData"""
        return _propgrid.PGChoices_GetDataPtr(*args, **kwargs)

    def ExtractData(*args, **kwargs):
        """ExtractData(self) -> wxPGChoicesData"""
        return _propgrid.PGChoices_ExtractData(*args, **kwargs)

    def AddString(*args, **kwargs):
        """AddString(self, String str)"""
        return _propgrid.PGChoices_AddString(*args, **kwargs)

    def AddInt(*args, **kwargs):
        """AddInt(self, int val)"""
        return _propgrid.PGChoices_AddInt(*args, **kwargs)

    def SetLabels(*args, **kwargs):
        """SetLabels(self, wxArrayString arr)"""
        return _propgrid.PGChoices_SetLabels(*args, **kwargs)

    def SetValues(*args, **kwargs):
        """SetValues(self, wxArrayInt arr)"""
        return _propgrid.PGChoices_SetValues(*args, **kwargs)


class PGChoicesPtr(PGChoices):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGChoices
_propgrid.PGChoices_swigregister(PGChoicesPtr)


def StringProperty(*args, **kwargs):
    """StringProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.StringProperty(*args, **kwargs)

def IntProperty(*args, **kwargs):
    """IntProperty(String label, String name=wxString_wxPG_LABEL, long value=0) -> PGProperty"""
    return _propgrid.IntProperty(*args, **kwargs)

def UIntProperty(*args, **kwargs):
    """UIntProperty(String label, String name=wxString_wxPG_LABEL, unsigned long value=0) -> PGProperty"""
    return _propgrid.UIntProperty(*args, **kwargs)

def FloatProperty(*args, **kwargs):
    """FloatProperty(String label, String name=wxString_wxPG_LABEL, double value=0.0) -> PGProperty"""
    return _propgrid.FloatProperty(*args, **kwargs)

def BoolProperty(*args, **kwargs):
    """BoolProperty(String label, String name=wxString_wxPG_LABEL, bool value=False) -> PGProperty"""
    return _propgrid.BoolProperty(*args, **kwargs)

def LongStringProperty(*args, **kwargs):
    """LongStringProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.LongStringProperty(*args, **kwargs)

def FileProperty(*args, **kwargs):
    """FileProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.FileProperty(*args, **kwargs)

def ArrayStringProperty(*args, **kwargs):
    """ArrayStringProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString value=wxArrayString()) -> PGProperty"""
    return _propgrid.ArrayStringProperty(*args, **kwargs)

def DirProperty(*args, **kwargs):
    """DirProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.DirProperty(*args, **kwargs)

def EnumProperty(*args, **kwargs):
    """
    EnumProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString choices=wxArrayString(), 
        wxArrayInt values=wxArrayInt(), 
        int value=0) -> PGProperty
    """
    return _propgrid.EnumProperty(*args, **kwargs)

def EditEnumProperty(*args, **kwargs):
    """
    EditEnumProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString choices=wxArrayString(), 
        wxArrayInt values=wxArrayInt(), 
        String value=wxEmptyString) -> PGProperty
    """
    return _propgrid.EditEnumProperty(*args, **kwargs)

def FlagsProperty(*args, **kwargs):
    """
    FlagsProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString flag_labels=wxArrayString(), 
        wxArrayInt values=wxArrayInt(), 
        int value=0) -> PGProperty
    """
    return _propgrid.FlagsProperty(*args, **kwargs)

def CustomProperty(*args, **kwargs):
    """CustomProperty(String label, String name=wxString_wxPG_LABEL) -> PGProperty"""
    return _propgrid.CustomProperty(*args, **kwargs)

def ParentProperty(*args, **kwargs):
    """ParentProperty(String label, String name) -> PGProperty"""
    return _propgrid.ParentProperty(*args, **kwargs)

def PropertyCategory(*args, **kwargs):
    """PropertyCategory(String label, String name=wxString_wxPG_LABEL) -> PGProperty"""
    return _propgrid.PropertyCategory(*args, **kwargs)

def StringCallBackProperty(*args, **kwargs):
    """StringCallBackProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.StringCallBackProperty(*args, **kwargs)

def PG_VariantToWxObject(*args, **kwargs):
    """PG_VariantToWxObject(wxVariant variant, wxClassInfo classInfo) -> Object"""
    return _propgrid.PG_VariantToWxObject(*args, **kwargs)
class PropertyContainerMethods(object):
    """Proxy of C++ PropertyContainerMethods class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyContainerMethods instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_propgrid.delete_PropertyContainerMethods):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AddPropertyChoice(*args):
        """
        AddPropertyChoice(self, PGProperty id, String label, int value=INT_MAX)
        AddPropertyChoice(self, String name, String label, int value=INT_MAX)
        """
        return _propgrid.PropertyContainerMethods_AddPropertyChoice(*args)

    def InsertPropertyChoice(*args):
        """
        InsertPropertyChoice(self, PGProperty id, String label, int index, int value=INT_MAX)
        InsertPropertyChoice(self, String name, String label, int index, int value=INT_MAX)
        """
        return _propgrid.PropertyContainerMethods_InsertPropertyChoice(*args)

    def DeletePropertyChoice(*args):
        """
        DeletePropertyChoice(self, PGProperty id, int index)
        DeletePropertyChoice(self, String name, int index)
        """
        return _propgrid.PropertyContainerMethods_DeletePropertyChoice(*args)

    def CreatePropertyByClass(*args, **kwargs):
        """CreatePropertyByClass(String classname, String label, String name) -> PGProperty"""
        return _propgrid.PropertyContainerMethods_CreatePropertyByClass(*args, **kwargs)

    CreatePropertyByClass = staticmethod(CreatePropertyByClass)
    def CreatePropertyByType(*args, **kwargs):
        """CreatePropertyByType(String valuetype, String label, String name) -> PGProperty"""
        return _propgrid.PropertyContainerMethods_CreatePropertyByType(*args, **kwargs)

    CreatePropertyByType = staticmethod(CreatePropertyByType)
    def Delete(*args):
        """
        Delete(self, PGProperty id)
        Delete(self, String name)
        """
        return _propgrid.PropertyContainerMethods_Delete(*args)

    def GetFirstChild(*args):
        """
        GetFirstChild(self, PGProperty id) -> PGProperty
        GetFirstChild(self, String name) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_GetFirstChild(*args)

    def GetNextSibling(*args):
        """
        GetNextSibling(self, PGProperty id) -> PGProperty
        GetNextSibling(self, String name) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_GetNextSibling(*args)

    def GetPropertyAttributes(*args, **kwargs):
        """GetPropertyAttributes(self, PGProperty id, unsigned int flagmask=0xFFFF) -> String"""
        return _propgrid.PropertyContainerMethods_GetPropertyAttributes(*args, **kwargs)

    def SetPropertyAttributes(*args):
        """
        SetPropertyAttributes(self, PGProperty id, String attributes)
        SetPropertyAttributes(self, String name, String attributes)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyAttributes(*args)

    def GetPropertyByName(*args):
        """
        GetPropertyByName(self, String name) -> PGProperty
        GetPropertyByName(self, String name, String subname) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_GetPropertyByName(*args)

    def GetPropertyChoices(*args):
        """
        GetPropertyChoices(self, PGProperty id) -> PGChoices
        GetPropertyChoices(self, String name) -> PGChoices
        """
        return _propgrid.PropertyContainerMethods_GetPropertyChoices(*args)

    def GetPropertyClassName(*args):
        """
        GetPropertyClassName(self, PGProperty id) -> wxChar
        GetPropertyClassName(self, String name) -> wxChar
        """
        return _propgrid.PropertyContainerMethods_GetPropertyClassName(*args)

    def GetPropertyClientData(*args):
        """
        GetPropertyClientData(self, PGProperty id)
        GetPropertyClientData(self, String name) -> void
        """
        return _propgrid.PropertyContainerMethods_GetPropertyClientData(*args)

    def GetPropertyEditor(*args):
        """
        GetPropertyEditor(self, PGProperty id) -> PGEditor
        GetPropertyEditor(self, String name) -> PGEditor
        """
        return _propgrid.PropertyContainerMethods_GetPropertyEditor(*args)

    def GetPropertyImage(*args):
        """
        GetPropertyImage(self, PGProperty id) -> Bitmap
        GetPropertyImage(self, String name) -> Bitmap
        """
        return _propgrid.PropertyContainerMethods_GetPropertyImage(*args)

    def GetPropertyIndex(*args):
        """
        GetPropertyIndex(self, PGProperty id) -> unsigned int
        GetPropertyIndex(self, String name) -> unsigned int
        """
        return _propgrid.PropertyContainerMethods_GetPropertyIndex(*args)

    def GetPropertyLabel(*args):
        """
        GetPropertyLabel(self, PGProperty id) -> String
        GetPropertyLabel(self, String name) -> String
        """
        return _propgrid.PropertyContainerMethods_GetPropertyLabel(*args)

    def GetPropertyName(*args, **kwargs):
        """GetPropertyName(self, PGProperty id) -> String"""
        return _propgrid.PropertyContainerMethods_GetPropertyName(*args, **kwargs)

    def GetPropertyParent(*args):
        """
        GetPropertyParent(self, PGProperty id) -> PGProperty
        GetPropertyParent(self, String name) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_GetPropertyParent(*args)

    def GetPropertyPriority(*args):
        """
        GetPropertyPriority(self, PGProperty id) -> int
        GetPropertyPriority(self, String name) -> int
        """
        return _propgrid.PropertyContainerMethods_GetPropertyPriority(*args)

    def GetPropertyPtr(*args):
        """
        GetPropertyPtr(self, PGProperty id) -> PGProperty
        GetPropertyPtr(self, String name) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_GetPropertyPtr(*args)

    def GetPropertyHelpString(*args):
        """
        GetPropertyHelpString(self, PGProperty id) -> String
        GetPropertyHelpString(self, String name) -> String
        """
        return _propgrid.PropertyContainerMethods_GetPropertyHelpString(*args)

    def GetPropertyShortClassName(*args, **kwargs):
        """GetPropertyShortClassName(self, PGProperty id) -> String"""
        return _propgrid.PropertyContainerMethods_GetPropertyShortClassName(*args, **kwargs)

    def GetPropertyValidator(*args):
        """
        GetPropertyValidator(self, PGProperty id) -> Validator
        GetPropertyValidator(self, String name) -> Validator
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValidator(*args)

    def GetPropertyValueAsVoidPtr(*args, **kwargs):
        """GetPropertyValueAsVoidPtr(self, PGProperty id) -> void"""
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsVoidPtr(*args, **kwargs)

    def GetPropertyValueAsString(*args):
        """
        GetPropertyValueAsString(self, PGProperty id) -> String
        GetPropertyValueAsString(self, String name) -> String
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsString(*args)

    def GetPropertyValueAsLong(*args):
        """
        GetPropertyValueAsLong(self, PGProperty id) -> long
        GetPropertyValueAsLong(self, String name) -> long
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsLong(*args)

    def GetPropertyValueAsBool(*args):
        """
        GetPropertyValueAsBool(self, PGProperty id) -> bool
        GetPropertyValueAsBool(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsBool(*args)

    def GetPropertyValueAsDouble(*args):
        """
        GetPropertyValueAsDouble(self, PGProperty id) -> double
        GetPropertyValueAsDouble(self, String name) -> double
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsDouble(*args)

    def GetPropertyValueAsArrayString(*args):
        """
        GetPropertyValueAsArrayString(self, PGProperty id) -> wxArrayString
        GetPropertyValueAsArrayString(self, String name) -> wxArrayString
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsArrayString(*args)

    def GetPropertyValueAsWxObjectPtr(*args):
        """
        GetPropertyValueAsWxObjectPtr(self, PGProperty id) -> Object
        GetPropertyValueAsWxObjectPtr(self, String name) -> Object
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsWxObjectPtr(*args)

    def GetPropertyValueAsPoint(*args):
        """
        GetPropertyValueAsPoint(self, PGProperty id) -> Point
        GetPropertyValueAsPoint(self, String name) -> Point
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsPoint(*args)

    def GetPropertyValueAsSize(*args):
        """
        GetPropertyValueAsSize(self, PGProperty id) -> Size
        GetPropertyValueAsSize(self, String name) -> Size
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsSize(*args)

    def GetPropertyValueAsDateTime(*args):
        """
        GetPropertyValueAsDateTime(self, PGProperty id) -> DateTime
        GetPropertyValueAsDateTime(self, String name) -> DateTime
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsDateTime(*args)

    def GetPropertyValueAsArrayInt(*args):
        """
        GetPropertyValueAsArrayInt(self, PGProperty id) -> wxArrayInt
        GetPropertyValueAsArrayInt(self, String name) -> wxArrayInt
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueAsArrayInt(*args)

    def GetPropertyValueType(*args):
        """
        GetPropertyValueType(self, PGProperty id) -> PGValueType
        GetPropertyValueType(self, String name) -> PGValueType
        """
        return _propgrid.PropertyContainerMethods_GetPropertyValueType(*args)

    def GetPVTN(*args):
        """
        GetPVTN(self, PGProperty id) -> String
        GetPVTN(self, String name) -> String
        """
        return _propgrid.PropertyContainerMethods_GetPVTN(*args)

    def GetPVTI(*args):
        """
        GetPVTI(self, PGProperty id) -> size_t
        GetPVTI(self, String name) -> size_t
        """
        return _propgrid.PropertyContainerMethods_GetPVTI(*args)

    def GetValueType(*args, **kwargs):
        """GetValueType(String type) -> PGValueType"""
        return _propgrid.PropertyContainerMethods_GetValueType(*args, **kwargs)

    GetValueType = staticmethod(GetValueType)
    def HideProperty(*args):
        """
        HideProperty(self, PGProperty id, bool hide=True) -> bool
        HideProperty(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_HideProperty(*args)

    def RegisterAdditionalEditors(*args, **kwargs):
        """RegisterAdditionalEditors()"""
        return _propgrid.PropertyContainerMethods_RegisterAdditionalEditors(*args, **kwargs)

    RegisterAdditionalEditors = staticmethod(RegisterAdditionalEditors)
    def InitAllTypeHandlers(*args, **kwargs):
        """InitAllTypeHandlers()"""
        return _propgrid.PropertyContainerMethods_InitAllTypeHandlers(*args, **kwargs)

    InitAllTypeHandlers = staticmethod(InitAllTypeHandlers)
    def IsPropertyEnabled(*args):
        """
        IsPropertyEnabled(self, PGProperty id) -> bool
        IsPropertyEnabled(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyEnabled(*args)

    def IsPropertyShown(*args):
        """
        IsPropertyShown(self, PGProperty id) -> bool
        IsPropertyShown(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyShown(*args)

    def IsPropertyValueType(*args):
        """
        IsPropertyValueType(self, PGProperty id, wxChar typestr) -> bool
        IsPropertyValueType(self, PGProperty id, PGValueType valuetype) -> bool
        IsPropertyValueType(self, PGProperty id, wxClassInfo classinfo) -> bool
        IsPropertyValueType(self, String name, wxChar typestr) -> bool
        IsPropertyValueType(self, String name, PGValueType valuetype) -> bool
        IsPropertyValueType(self, String name, wxClassInfo classinfo) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyValueType(*args)

    def IsPropertyExpanded(*args):
        """
        IsPropertyExpanded(self, PGProperty id) -> bool
        IsPropertyExpanded(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyExpanded(*args)

    def IsPropertyKindOf(*args):
        """
        IsPropertyKindOf(self, PGProperty id, wxPGPropertyClassInfo info) -> bool
        IsPropertyKindOf(self, String name, wxPGPropertyClassInfo info) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyKindOf(*args)

    def IsModified(*args):
        """
        IsModified(self, PGProperty id) -> bool
        IsModified(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsModified(*args)

    def IsPropertyCategory(*args):
        """
        IsPropertyCategory(self, PGProperty id) -> bool
        IsPropertyCategory(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyCategory(*args)

    def IsPropertyModified(*args):
        """
        IsPropertyModified(self, PGProperty id) -> bool
        IsPropertyModified(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyModified(*args)

    def IsPropertyUnspecified(*args):
        """
        IsPropertyUnspecified(self, PGProperty id) -> bool
        IsPropertyUnspecified(self, String name) -> bool
        """
        return _propgrid.PropertyContainerMethods_IsPropertyUnspecified(*args)

    def RegisterAdvancedPropertyClasses(*args, **kwargs):
        """RegisterAdvancedPropertyClasses()"""
        return _propgrid.PropertyContainerMethods_RegisterAdvancedPropertyClasses(*args, **kwargs)

    RegisterAdvancedPropertyClasses = staticmethod(RegisterAdvancedPropertyClasses)
    def RegisterPropertyClass(*args, **kwargs):
        """RegisterPropertyClass(wxChar name, wxPGPropertyClassInfo classinfo) -> bool"""
        return _propgrid.PropertyContainerMethods_RegisterPropertyClass(*args, **kwargs)

    RegisterPropertyClass = staticmethod(RegisterPropertyClass)
    def ReplaceProperty(*args):
        """
        ReplaceProperty(self, PGProperty id, PGProperty property) -> PGProperty
        ReplaceProperty(self, String name, PGProperty property) -> PGProperty
        """
        return _propgrid.PropertyContainerMethods_ReplaceProperty(*args)

    def SetBoolChoices(*args, **kwargs):
        """SetBoolChoices(wxChar true_choice, wxChar false_choice)"""
        return _propgrid.PropertyContainerMethods_SetBoolChoices(*args, **kwargs)

    SetBoolChoices = staticmethod(SetBoolChoices)
    def SetPropertyChoices(*args):
        """
        SetPropertyChoices(self, PGProperty id, PGChoices choices)
        SetPropertyChoices(self, String name, PGChoices choices)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyChoices(*args)

    def SetPropertyChoicesExclusive(*args):
        """
        SetPropertyChoicesExclusive(self, PGProperty id)
        SetPropertyChoicesExclusive(self, String name)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyChoicesExclusive(*args)

    def SetPropertyAttribute(*args):
        """
        SetPropertyAttribute(self, PGProperty id, int attrid, wxVariant value, long argFlags=0)
        SetPropertyAttribute(self, String name, int attrid, wxVariant value, long argFlags=0)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyAttribute(*args)

    def SetPropertyEditor(*args):
        """
        SetPropertyEditor(self, PGProperty id, String editorName)
        SetPropertyEditor(self, String name, String editorName)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyEditor(*args)

    def SetPropertyClientData(*args):
        """
        SetPropertyClientData(self, PGProperty id, void clientData)
        SetPropertyClientData(self, String name, void clientData)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyClientData(*args)

    def SetPropertyHelpString(*args):
        """
        SetPropertyHelpString(self, PGProperty id, String helpString)
        SetPropertyHelpString(self, String name, String helpString)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyHelpString(*args)

    def SetPropertyImage(*args):
        """
        SetPropertyImage(self, PGProperty id, Bitmap bmp)
        SetPropertyImage(self, String name, Bitmap bmp)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyImage(*args)

    def SetPropertyMaxLength(*args):
        """
        SetPropertyMaxLength(self, PGProperty id, int maxLen) -> bool
        SetPropertyMaxLength(self, String name, int maxLen) -> bool
        """
        return _propgrid.PropertyContainerMethods_SetPropertyMaxLength(*args)

    def SetPropertyPriority(*args):
        """
        SetPropertyPriority(self, PGProperty id, int priority) -> bool
        SetPropertyPriority(self, String name, int priority) -> bool
        """
        return _propgrid.PropertyContainerMethods_SetPropertyPriority(*args)

    def SetPropertyValidator(*args):
        """
        SetPropertyValidator(self, PGProperty id, Validator validator)
        SetPropertyValidator(self, String name, Validator validator)
        """
        return _propgrid.PropertyContainerMethods_SetPropertyValidator(*args)

    def TogglePropertyPriority(*args):
        """
        TogglePropertyPriority(self, PGProperty id)
        TogglePropertyPriority(self, String name)
        """
        return _propgrid.PropertyContainerMethods_TogglePropertyPriority(*args)

    def MapType(class_,factory):
        """\
        Registers Python type/class to property mapping.

        factory: Property builder function/class.
        """
        global _type2property
        try:
            mappings = _type2property
        except NameError:
            raise AssertionError("call only after a propertygrid or manager instance constructed")

        mappings[class_] = factory


    def DoDefaultTypeMappings(self):
        """\
        Map built-in properties.
        """
        global _type2property
        try:
            mappings = _type2property

            return
        except NameError:
            mappings = {}
            _type2property = mappings

        mappings[str] = StringProperty
        mappings[unicode] = StringProperty
        mappings[int] = IntProperty
        mappings[float] = FloatProperty
        mappings[bool] = BoolProperty
        mappings[list] = ArrayStringProperty
        mappings[tuple] = ArrayStringProperty
        mappings[wx.Font] = FontProperty
        mappings[wx.Colour] = ColourProperty
        mappings[wx.Size] = SizeProperty
        mappings[wx.Point] = PointProperty
        mappings[wx.FontData] = FontDataProperty


    def GetPropertyValue(self,p):
        """\
        Returns Python object value for property.

        Caches getters on value type id basis for performance purposes.
        """
        global _vt2getter
        vtid = self.GetPVTI(p)
        try:
            getter = _vt2getter[vtid]
        except KeyError:

            cls = PropertyContainerMethods
            vtn = self.GetPVTN(p)

            if vtn == 'long':
                getter = cls.GetPropertyValueAsLong
            elif vtn == 'string':
                getter = cls.GetPropertyValueAsString
            elif vtn == 'double':
                getter = cls.GetPropertyValueAsDouble
            elif vtn == 'bool':
                getter = cls.GetPropertyValueAsBool
            elif vtn == 'arrstring':
                getter = cls.GetPropertyValueAsArrayString
            elif vtn == 'wxArrayInt':
                getter = cls.GetPropertyValueAsArrayInt
            elif vtn == 'datetime':
                getter = cls.GetPropertyValueAsDateTime
            elif vtn == 'wxPoint':
                getter = cls.GetPropertyValueAsPoint
            elif vtn == 'wxSize':
                getter = cls.GetPropertyValueAsSize
            elif vtn.startswith('wx'):
                getter = cls.GetPropertyValueAsWxObjectPtr
            elif not vtn:
                if p:
                    raise ValueError("no property with name '%s'"%p)
                else:
                    raise ValueError("NULL property")
            else:
                raise AssertionError("Unregistered property grid value type '%s'"%vtn)
            _vt2getter[vtid] = getter
        return getter(self,p)


    def _SetPropertyValueArrstr(self,p,v):
        """\
        NB: We must implement this in Python because SWIG has problems combining
            conversion of list to wxArrayXXX and overloaded arguments.
        """
        if not isinstance(p,basestring):
            self._SetPropertyValueArrstr(p,v)
        else:
            self._SetPropertyValueArrstr(self.GetPropertyByNameA(p),v)


    def _SetPropertyValueArrint(self,p,v):
        """\
        NB: We must implement this in Python because SWIG has problems combining
            conversion of list to wxArrayXXX and overloaded arguments.
        """
        if not isinstance(p,basestring):
            self._SetPropertyValueArrint(p,v)
        else:
            self._SetPropertyValueArrint(self.GetPropertyByNameA(p),v)


    def SetPropertyValue(self,p,v):
        """\
        Set property value from Python object.

        Caches setters on value type id basis for performance purposes.
        """
        cls = self.__class__
        if not isinstance(v,basestring):
            _vt2setter = cls._vt2setter
            vtid = self.GetPVTI(p)
            try:
                setter = _vt2setter[vtid]
            except KeyError:

                vtn = self.GetPVTN(p)

                if vtn == 'long':
                    setter = cls.SetPropertyValueLong
                elif vtn == 'string':
                    setter = cls.SetPropertyValueString
                elif vtn == 'double':
                    setter = cls.SetPropertyValueDouble
                elif vtn == 'bool':
                    setter = cls.SetPropertyValueBool
                elif vtn == 'arrstring':
                    setter = cls._SetPropertyValueArrstr
                elif vtn == 'wxArrayInt':
                    setter = cls.setPropertyValueArrint2
                elif vtn == 'datetime':
                    setter = cls.setPropertyValueDatetime
                elif vtn == 'wxPoint':
                    setter = cls.SetPropertyValuePoint
                elif vtn == 'wxSize':
                    setter = cls.SetPropertyValueSize
                elif vtn.startswith('wx'):
                    setter = cls.SetPropertyValueWxObjectPtr
                elif not vtn:
                    if p:
                        raise ValueError("no property with name '%s'"%p)
                    else:
                        raise ValueError("NULL property")
                else:
                    raise AssertionError("Unregistered property grid value type '%s'"%vtn)
                _vt2setter[vtid] = setter
        else:
            setter = cls.SetPropertyValueString

        return setter(self,p,v)


    def DoDefaultValueTypeMappings(self):
        """\
        Map pg value type ids to getter methods.
        """
        global _vt2getter
        try:
            vt2getter = _vt2getter

            return
        except NameError:
            vt2getter = {}
            _vt2getter = vt2getter


    def _GetValues(self,parent,fc,dict_,getter):
        p = fc

        while p:
            pfc = self.GetFirstChild(p)
            if pfc:
                self._GetValues(p,pfc,dict_,getter)
            else:
                dict_[p.GetName()] = getter(p)

            p = self.GetNextSibling(p)


    def GetPropertyValues(self,dict_=None,as_strings=False):
        """\
        Returns values in the grid.

        dict_: if not given, then a new one is created. dict_ can be
          object as well, in which case it's __dict__ is used.
        as_strings: if True, then string representations of values
          are fetched instead of native types. Useful for config and such.

        Return value: dictionary with values. It is always a dictionary,
        so if dict_ was object with __dict__ attribute, then that attribute
        is returned.
        """

        if dict_ is None:
            dict_ = {}
        elif hasattr(dict_,'__dict__'):
            dict_ = dict_.__dict__

        if not as_strings:
            getter = self.GetPropertyValue
        else:
            getter = self.GetPropertyValueAsString

        root = self.GetRoot()
        self._GetValues(root,self.GetFirstChild(root),dict_,getter)

        return dict_

    GetValues = GetPropertyValues


    def SetPropertyValues(self,dict_):
        """\
        Sets property values from dict_, which can be either
        dictionary or an object with __dict__ attribute.

        autofill: If true, keys with not relevant properties
          are auto-created. For more info, see AutoFill.

        Notes:
          * Keys starting with underscore are ignored.
        """

        autofill = False

        if dict_ is None:
            dict_ = {}
        elif hasattr(dict_,'__dict__'):
            dict_ = dict_.__dict__

        def set_sub_obj(k0,dict_):
            for k,v in dict_.iteritems():
                if k[0] != '_':
                    try:
                        self.SetPropertyValue(k,v)
                    except:
                        try:
                            if autofill:
                                self._AutoFillOne(k0,k,v)
                                continue
                        except:
                            if isinstance(v,dict):
                                set_sub_obj(k,v)
                            elif hasattr(v,'__dict__'):
                                set_sub_obj(k,v.__dict__)


        cur_page = False
        is_manager = isinstance(self,PropertyGridManager)

        try:
            set_sub_obj(self.GetRoot(),dict_)
        except:
            import traceback
            traceback.print_exc()

        self.Refresh()


    SetValues = SetPropertyValues


    def _AutoFillMany(self,cat,dict_):
        for k,v in dict_.iteritems():
            self._AutoFillOne(cat,k,v)


    def _AutoFillOne(self,cat,k,v):
        global _type2property

        factory = _type2property.get(v.__class__,None)

        if factory:
            self.AppendIn( cat, factory(k,k,v) )
        elif hasattr(v,'__dict__'):
            cat2 = self.AppendIn( cat, PropertyCategory(k) )
            self._AutoFillMany(cat2,v.__dict__)
        elif isinstance(v,dict):
            cat2 = self.AppendIn( cat, PropertyCategory(k) )
            self._AutoFillMany(cat2,v)
        elif not k.startswith('_'):
            raise AssertionError("member '%s' is of unregisted type/class '%s'"%(k,v.__class__))


    def AutoFill(self,obj,parent=None):
        """\
        Clears properties and re-fills to match members and
        values of given object or dictionary obj.
        """

        self.edited_objects[parent] = obj

        cur_page = False
        is_manager = isinstance(self,PropertyGridManager)

        if not parent:
            if is_manager:
                page = self.GetTargetPage()
                self.ClearPage(page)
                parent = self.GetPageRoot(page)
            else:
                self.Clear()
                parent = self.GetRoot()
        else:
            p = self.GetFirstChild(parent)
            while p:
                self.Delete(p)
                p = self.GetNextSibling(p)

        if not is_manager or page == self.GetSelectedPage():
            self.Freeze()
            cur_page = True

        try:
            self._AutoFillMany(parent,obj.__dict__)
        except:
            import traceback
            traceback.print_exc()

        if cur_page:
            self.Thaw()



class PropertyContainerMethodsPtr(PropertyContainerMethods):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyContainerMethods
_propgrid.PropertyContainerMethods_swigregister(PropertyContainerMethodsPtr)

def PropertyContainerMethods_CreatePropertyByClass(*args, **kwargs):
    """PropertyContainerMethods_CreatePropertyByClass(String classname, String label, String name) -> PGProperty"""
    return _propgrid.PropertyContainerMethods_CreatePropertyByClass(*args, **kwargs)

def PropertyContainerMethods_CreatePropertyByType(*args, **kwargs):
    """PropertyContainerMethods_CreatePropertyByType(String valuetype, String label, String name) -> PGProperty"""
    return _propgrid.PropertyContainerMethods_CreatePropertyByType(*args, **kwargs)

def PropertyContainerMethods_GetValueType(*args, **kwargs):
    """PropertyContainerMethods_GetValueType(String type) -> PGValueType"""
    return _propgrid.PropertyContainerMethods_GetValueType(*args, **kwargs)

def PropertyContainerMethods_RegisterAdditionalEditors(*args, **kwargs):
    """PropertyContainerMethods_RegisterAdditionalEditors()"""
    return _propgrid.PropertyContainerMethods_RegisterAdditionalEditors(*args, **kwargs)

def PropertyContainerMethods_InitAllTypeHandlers(*args, **kwargs):
    """PropertyContainerMethods_InitAllTypeHandlers()"""
    return _propgrid.PropertyContainerMethods_InitAllTypeHandlers(*args, **kwargs)

def PropertyContainerMethods_RegisterAdvancedPropertyClasses(*args, **kwargs):
    """PropertyContainerMethods_RegisterAdvancedPropertyClasses()"""
    return _propgrid.PropertyContainerMethods_RegisterAdvancedPropertyClasses(*args, **kwargs)

def PropertyContainerMethods_RegisterPropertyClass(*args, **kwargs):
    """PropertyContainerMethods_RegisterPropertyClass(wxChar name, wxPGPropertyClassInfo classinfo) -> bool"""
    return _propgrid.PropertyContainerMethods_RegisterPropertyClass(*args, **kwargs)

def PropertyContainerMethods_SetBoolChoices(*args, **kwargs):
    """PropertyContainerMethods_SetBoolChoices(wxChar true_choice, wxChar false_choice)"""
    return _propgrid.PropertyContainerMethods_SetBoolChoices(*args, **kwargs)

PG_SEL_FOCUS = _propgrid.PG_SEL_FOCUS
PG_SEL_FORCE = _propgrid.PG_SEL_FORCE
PG_SEL_NONVISIBLE = _propgrid.PG_SEL_NONVISIBLE
PG_SEL_NOVALIDATE = _propgrid.PG_SEL_NOVALIDATE
class PropertyGrid(_windows.ScrolledWindow,PropertyContainerMethods):
    """Proxy of C++ PropertyGrid class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Refresh(*args, **kwargs):
        """
        Refresh(self)

        Mark the specified rectangle (or the whole window) as "dirty" so it
        will be repainted.  Causes an EVT_PAINT event to be generated and sent
        to the window.
        """
        return _propgrid.PropertyGrid_Refresh(*args, **kwargs)

    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=(0), 
            wxChar name=wxPyPropertyGridNameStr) -> PropertyGrid
        """
        newobj = _propgrid.new_PropertyGrid(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        
        self._setOORInfo(self)
        self.DoDefaultTypeMappings()
        self.edited_objects = {}
        self.DoDefaultValueTypeMappings()
        if not hasattr(self.__class__,'_vt2setter'):
            self.__class__._vt2setter = {}
    

    def Append(*args, **kwargs):
        """Append(self, PGProperty property) -> PGProperty"""
        return _propgrid.PropertyGrid_Append(*args, **kwargs)

    def AppendCategory(*args, **kwargs):
        """AppendCategory(self, String label, String name=wxString_wxPG_LABEL) -> PGProperty"""
        return _propgrid.PropertyGrid_AppendCategory(*args, **kwargs)

    def GetSizeControl(*args, **kwargs):
        """GetSizeControl(self) -> Size"""
        return _propgrid.PropertyGrid_GetSizeControl(*args, **kwargs)

    sizeControl = property(_propgrid.PropertyGrid_sizeControl_get, _propgrid.PropertyGrid_sizeControl_set)
    def SetSizeControl(*args, **kwargs):
        """SetSizeControl(self, Size value)"""
        return _propgrid.PropertyGrid_SetSizeControl(*args, **kwargs)

    positionControl = property(_propgrid.PropertyGrid_positionControl_get, _propgrid.PropertyGrid_positionControl_set)
    def SetPositionControl(*args, **kwargs):
        """SetPositionControl(self, Point value)"""
        return _propgrid.PropertyGrid_SetPositionControl(*args, **kwargs)

    def GetPositionControl(*args, **kwargs):
        """GetPositionControl(self) -> Point"""
        return _propgrid.PropertyGrid_GetPositionControl(*args, **kwargs)

    def AppendIn(*args):
        """
        AppendIn(self, PGProperty id, PGProperty property) -> PGProperty
        AppendIn(self, String name, PGProperty property) -> PGProperty
        AppendIn(self, PGProperty id, String label, String propname, wxVariant value) -> PGProperty
        AppendIn(self, String name, String label, String propname, wxVariant value) -> PGProperty
        """
        return _propgrid.PropertyGrid_AppendIn(*args)

    def AutoGetTranslation(*args, **kwargs):
        """AutoGetTranslation(bool enable)"""
        return _propgrid.PropertyGrid_AutoGetTranslation(*args, **kwargs)

    AutoGetTranslation = staticmethod(AutoGetTranslation)
    def CanClose(*args, **kwargs):
        """CanClose(self) -> bool"""
        return _propgrid.PropertyGrid_CanClose(*args, **kwargs)

    def CenterSplitter(*args, **kwargs):
        """CenterSplitter(self, bool enable_auto_centering=False)"""
        return _propgrid.PropertyGrid_CenterSplitter(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=(0), 
            wxChar name=wxPropertyGridNameStr) -> bool
        """
        return _propgrid.PropertyGrid_Create(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _propgrid.PropertyGrid_Clear(*args, **kwargs)

    def ClearModifiedStatus(*args):
        """
        ClearModifiedStatus(self, PGProperty id)
        ClearModifiedStatus(self)
        """
        return _propgrid.PropertyGrid_ClearModifiedStatus(*args)

    def ClearPropertyValue(*args):
        """
        ClearPropertyValue(self, PGProperty id) -> bool
        ClearPropertyValue(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_ClearPropertyValue(*args)

    def ClearSelection(*args, **kwargs):
        """ClearSelection(self) -> bool"""
        return _propgrid.PropertyGrid_ClearSelection(*args, **kwargs)

    def ClearTargetPage(*args, **kwargs):
        """ClearTargetPage(self)"""
        return _propgrid.PropertyGrid_ClearTargetPage(*args, **kwargs)

    def Collapse(*args):
        """
        Collapse(self, PGProperty id) -> bool
        Collapse(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_Collapse(*args)

    def CollapseAll(*args, **kwargs):
        """CollapseAll(self) -> bool"""
        return _propgrid.PropertyGrid_CollapseAll(*args, **kwargs)

    def Compact(*args, **kwargs):
        """Compact(self, bool compact) -> bool"""
        return _propgrid.PropertyGrid_Compact(*args, **kwargs)

    def Disable(*args):
        """
        Disable(self, PGProperty id) -> bool
        Disable(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_Disable(*args)

    def DisableProperty(*args):
        """
        DisableProperty(self, PGProperty id) -> bool
        DisableProperty(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_DisableProperty(*args)

    def EnableCategories(*args, **kwargs):
        """EnableCategories(self, bool enable) -> bool"""
        return _propgrid.PropertyGrid_EnableCategories(*args, **kwargs)

    def EnableProperty(*args):
        """
        EnableProperty(self, PGProperty id, bool enable=True) -> bool
        EnableProperty(self, String name, bool enable=True) -> bool
        """
        return _propgrid.PropertyGrid_EnableProperty(*args)

    def EnsureVisible(*args):
        """
        EnsureVisible(self, PGProperty id) -> bool
        EnsureVisible(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_EnsureVisible(*args)

    def Expand(*args):
        """
        Expand(self, PGProperty id) -> bool
        Expand(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_Expand(*args)

    def ExpandAll(*args, **kwargs):
        """ExpandAll(self)"""
        return _propgrid.PropertyGrid_ExpandAll(*args, **kwargs)

    def GetCaptionFont(*args, **kwargs):
        """GetCaptionFont(self) -> Font"""
        return _propgrid.PropertyGrid_GetCaptionFont(*args, **kwargs)

    def GetCaptionBackgroundColour(*args, **kwargs):
        """GetCaptionBackgroundColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetCaptionBackgroundColour(*args, **kwargs)

    def GetCaptionForegroundColour(*args, **kwargs):
        """GetCaptionForegroundColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetCaptionForegroundColour(*args, **kwargs)

    def GetCellBackgroundColour(*args, **kwargs):
        """GetCellBackgroundColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetCellBackgroundColour(*args, **kwargs)

    def GetCellTextColour(*args, **kwargs):
        """GetCellTextColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetCellTextColour(*args, **kwargs)

    def GetChildrenCount(*args):
        """
        GetChildrenCount(self) -> size_t
        GetChildrenCount(self, PGProperty id) -> size_t
        GetChildrenCount(self, String name) -> size_t
        """
        return _propgrid.PropertyGrid_GetChildrenCount(*args)

    def GetFirst(*args, **kwargs):
        """GetFirst(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetFirst(*args, **kwargs)

    def GetFirstVisible(*args, **kwargs):
        """GetFirstVisible(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetFirstVisible(*args, **kwargs)

    def GetFontHeight(*args, **kwargs):
        """GetFontHeight(self) -> int"""
        return _propgrid.PropertyGrid_GetFontHeight(*args, **kwargs)

    def GetGrid(*args, **kwargs):
        """GetGrid(self) -> PropertyGrid"""
        return _propgrid.PropertyGrid_GetGrid(*args, **kwargs)

    def GetFirstCategory(*args, **kwargs):
        """GetFirstCategory(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetFirstCategory(*args, **kwargs)

    def GetFirstProperty(*args, **kwargs):
        """GetFirstProperty(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetFirstProperty(*args, **kwargs)

    def GetImageSize(*args, **kwargs):
        """GetImageSize(self, PGProperty id=(wxPGProperty *) NULL) -> Size"""
        return _propgrid.PropertyGrid_GetImageSize(*args, **kwargs)

    def GetItemAtY(*args, **kwargs):
        """GetItemAtY(self, int y) -> PGProperty"""
        return _propgrid.PropertyGrid_GetItemAtY(*args, **kwargs)

    def GetLastProperty(*args, **kwargs):
        """GetLastProperty(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetLastProperty(*args, **kwargs)

    def GetLastChild(*args):
        """
        GetLastChild(self, PGProperty id) -> PGProperty
        GetLastChild(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGrid_GetLastChild(*args)

    def GetLastVisible(*args, **kwargs):
        """GetLastVisible(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetLastVisible(*args, **kwargs)

    def GetLineColour(*args, **kwargs):
        """GetLineColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetLineColour(*args, **kwargs)

    def GetMarginColour(*args, **kwargs):
        """GetMarginColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetMarginColour(*args, **kwargs)

    def GetNextProperty(*args, **kwargs):
        """GetNextProperty(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGrid_GetNextProperty(*args, **kwargs)

    def GetNextCategory(*args, **kwargs):
        """GetNextCategory(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGrid_GetNextCategory(*args, **kwargs)

    def GetNextVisible(*args, **kwargs):
        """GetNextVisible(self, PGProperty property) -> PGProperty"""
        return _propgrid.PropertyGrid_GetNextVisible(*args, **kwargs)

    def GetPrevProperty(*args, **kwargs):
        """GetPrevProperty(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGrid_GetPrevProperty(*args, **kwargs)

    def GetPrevSibling(*args):
        """
        GetPrevSibling(self, PGProperty id) -> PGProperty
        GetPrevSibling(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGrid_GetPrevSibling(*args)

    def GetPrevVisible(*args, **kwargs):
        """GetPrevVisible(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGrid_GetPrevVisible(*args, **kwargs)

    def GetPropertyCategory(*args):
        """
        GetPropertyCategory(self, PGProperty id) -> PGProperty
        GetPropertyCategory(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGrid_GetPropertyCategory(*args)

    def GetPropertyColour(*args):
        """
        GetPropertyColour(self, PGProperty id) -> Colour
        GetPropertyColour(self, String name) -> Colour
        """
        return _propgrid.PropertyGrid_GetPropertyColour(*args)

    def GetPropertyByLabel(*args, **kwargs):
        """GetPropertyByLabel(self, String name) -> PGProperty"""
        return _propgrid.PropertyGrid_GetPropertyByLabel(*args, **kwargs)

    def GetRoot(*args, **kwargs):
        """GetRoot(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetRoot(*args, **kwargs)

    def GetRowHeight(*args, **kwargs):
        """GetRowHeight(self) -> int"""
        return _propgrid.PropertyGrid_GetRowHeight(*args, **kwargs)

    def GetSelectedProperty(*args, **kwargs):
        """GetSelectedProperty(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetSelectedProperty(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> PGProperty"""
        return _propgrid.PropertyGrid_GetSelection(*args, **kwargs)

    def GetSelectionBackgroundColour(*args, **kwargs):
        """GetSelectionBackgroundColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetSelectionBackgroundColour(*args, **kwargs)

    def GetSelectionForegroundColour(*args, **kwargs):
        """GetSelectionForegroundColour(self) -> Colour"""
        return _propgrid.PropertyGrid_GetSelectionForegroundColour(*args, **kwargs)

    def GetSplitterPosition(*args, **kwargs):
        """GetSplitterPosition(self) -> int"""
        return _propgrid.PropertyGrid_GetSplitterPosition(*args, **kwargs)

    def GetVerticalSpacing(*args, **kwargs):
        """GetVerticalSpacing(self) -> int"""
        return _propgrid.PropertyGrid_GetVerticalSpacing(*args, **kwargs)

    def HasSelection(*args, **kwargs):
        """HasSelection(self) -> bool"""
        return _propgrid.PropertyGrid_HasSelection(*args, **kwargs)

    def HideLowPriority(*args, **kwargs):
        """HideLowPriority(self)"""
        return _propgrid.PropertyGrid_HideLowPriority(*args, **kwargs)

    def InsertCategory(*args, **kwargs):
        """InsertCategory(self, PGProperty id, int index, String label, String name=wxString_wxPG_LABEL) -> PGProperty"""
        return _propgrid.PropertyGrid_InsertCategory(*args, **kwargs)

    def MakeIntegerSpinControl(*args, **kwargs):
        """MakeIntegerSpinControl(self, String name)"""
        return _propgrid.PropertyGrid_MakeIntegerSpinControl(*args, **kwargs)

    def MakeIntegerSpinControlById(*args, **kwargs):
        """MakeIntegerSpinControlById(self, PGProperty id)"""
        return _propgrid.PropertyGrid_MakeIntegerSpinControlById(*args, **kwargs)

    def MakeAnyControl(*args, **kwargs):
        """MakeAnyControl(self, String name)"""
        return _propgrid.PropertyGrid_MakeAnyControl(*args, **kwargs)

    def MakeAnyControlById(*args, **kwargs):
        """MakeAnyControlById(self, PGProperty id)"""
        return _propgrid.PropertyGrid_MakeAnyControlById(*args, **kwargs)

    def Insert(*args):
        """
        Insert(self, PGProperty priorthis, PGProperty newproperty) -> PGProperty
        Insert(self, String name, PGProperty newproperty) -> PGProperty
        Insert(self, PGProperty id, int index, PGProperty newproperty) -> PGProperty
        Insert(self, String name, int index, PGProperty newproperty) -> PGProperty
        Insert(self, PGProperty id, int index, String label, String name, 
            String value=wxEmptyString) -> PGProperty
        Insert(self, PGProperty id, int index, String label, String name, 
            int value) -> PGProperty
        Insert(self, PGProperty id, int index, String label, String name, 
            double value) -> PGProperty
        Insert(self, PGProperty id, int index, String label, String name, 
            bool value) -> PGProperty
        """
        return _propgrid.PropertyGrid_Insert(*args)

    def IsAnyModified(*args, **kwargs):
        """IsAnyModified(self) -> bool"""
        return _propgrid.PropertyGrid_IsAnyModified(*args, **kwargs)

    def IsFrozen(*args, **kwargs):
        """IsFrozen(self) -> bool"""
        return _propgrid.PropertyGrid_IsFrozen(*args, **kwargs)

    def IsPropertySelected(*args):
        """
        IsPropertySelected(self, PGProperty id) -> bool
        IsPropertySelected(self, String name) -> bool
        """
        return _propgrid.PropertyGrid_IsPropertySelected(*args)

    def LimitPropertyEditing(*args):
        """
        LimitPropertyEditing(self, PGProperty id, bool limit=True)
        LimitPropertyEditing(self, String name, bool limit=True)
        """
        return _propgrid.PropertyGrid_LimitPropertyEditing(*args)

    def SetSplitterLeft(*args, **kwargs):
        """SetSplitterLeft(self, bool subProps=False)"""
        return _propgrid.PropertyGrid_SetSplitterLeft(*args, **kwargs)

    def RegisterValueType(*args, **kwargs):
        """RegisterValueType(PGValueType valueclass, bool noDefCheck=False) -> PGValueType"""
        return _propgrid.PropertyGrid_RegisterValueType(*args, **kwargs)

    RegisterValueType = staticmethod(RegisterValueType)
    def RegisterEditorClass(*args, **kwargs):
        """RegisterEditorClass(PGEditor valueclass, String name, bool noDefCheck=False) -> PGEditor"""
        return _propgrid.PropertyGrid_RegisterEditorClass(*args, **kwargs)

    RegisterEditorClass = staticmethod(RegisterEditorClass)
    def ResetColours(*args, **kwargs):
        """ResetColours(self)"""
        return _propgrid.PropertyGrid_ResetColours(*args, **kwargs)

    def SetButtonShortcut(*args, **kwargs):
        """SetButtonShortcut(self, int keycode, bool ctrlDown=False, bool altDown=False)"""
        return _propgrid.PropertyGrid_SetButtonShortcut(*args, **kwargs)

    def SetCurrentCategory(*args):
        """
        SetCurrentCategory(self, PGProperty id)
        SetCurrentCategory(self, String name=wxEmptyString)
        """
        return _propgrid.PropertyGrid_SetCurrentCategory(*args)

    def SetPropertyAttributeAll(*args, **kwargs):
        """SetPropertyAttributeAll(self, int attrid, wxVariant value)"""
        return _propgrid.PropertyGrid_SetPropertyAttributeAll(*args, **kwargs)

    def SetPropertyColour(*args):
        """
        SetPropertyColour(self, PGProperty id, Colour col)
        SetPropertyColour(self, String name, Colour col)
        """
        return _propgrid.PropertyGrid_SetPropertyColour(*args)

    def SetPropertyColourToDefault(*args):
        """
        SetPropertyColourToDefault(self, PGProperty id)
        SetPropertyColourToDefault(self, String name)
        """
        return _propgrid.PropertyGrid_SetPropertyColourToDefault(*args)

    def SetCaptionBackgroundColour(*args, **kwargs):
        """SetCaptionBackgroundColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetCaptionBackgroundColour(*args, **kwargs)

    def SetCaptionForegroundColour(*args, **kwargs):
        """SetCaptionForegroundColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetCaptionForegroundColour(*args, **kwargs)

    def SetCellBackgroundColour(*args, **kwargs):
        """SetCellBackgroundColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetCellBackgroundColour(*args, **kwargs)

    def SetCellTextColour(*args, **kwargs):
        """SetCellTextColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetCellTextColour(*args, **kwargs)

    def SetLineColour(*args, **kwargs):
        """SetLineColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetLineColour(*args, **kwargs)

    def SetMarginColour(*args, **kwargs):
        """SetMarginColour(self, Colour col)"""
        return _propgrid.PropertyGrid_SetMarginColour(*args, **kwargs)

    def SetSelectionBackground(*args, **kwargs):
        """SetSelectionBackground(self, Colour col)"""
        return _propgrid.PropertyGrid_SetSelectionBackground(*args, **kwargs)

    def SetSelectionForeground(*args, **kwargs):
        """SetSelectionForeground(self, Colour col)"""
        return _propgrid.PropertyGrid_SetSelectionForeground(*args, **kwargs)

    def SetSplitterPosition(*args, **kwargs):
        """SetSplitterPosition(self, int newxpos, bool refresh=True)"""
        return _propgrid.PropertyGrid_SetSplitterPosition(*args, **kwargs)

    def SelectProperty(*args):
        """
        SelectProperty(self, PGProperty id, bool focus=False) -> bool
        SelectProperty(self, String name, bool focus=False) -> bool
        """
        return _propgrid.PropertyGrid_SelectProperty(*args)

    def SetPropertyLabel(*args):
        """
        SetPropertyLabel(self, PGProperty id, String newproplabel)
        SetPropertyLabel(self, String name, String newproplabel)
        """
        return _propgrid.PropertyGrid_SetPropertyLabel(*args)

    def SetPropertyName(*args):
        """
        SetPropertyName(self, PGProperty id, String newname)
        SetPropertyName(self, String name, String newname)
        """
        return _propgrid.PropertyGrid_SetPropertyName(*args)

    def _SetPropertyValueArrstr(*args, **kwargs):
        """_SetPropertyValueArrstr(self, PGProperty id, wxArrayString value)"""
        return _propgrid.PropertyGrid__SetPropertyValueArrstr(*args, **kwargs)

    def _SetPropertyValueArrint(*args, **kwargs):
        """_SetPropertyValueArrint(self, PGProperty id, wxArrayInt value)"""
        return _propgrid.PropertyGrid__SetPropertyValueArrint(*args, **kwargs)

    def SetPropertyValueLong(*args):
        """
        SetPropertyValueLong(self, PGProperty id, long value)
        SetPropertyValueLong(self, String name, long value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueLong(*args)

    def SetPropertyValueDouble(*args):
        """
        SetPropertyValueDouble(self, PGProperty id, double value)
        SetPropertyValueDouble(self, String name, double value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueDouble(*args)

    def SetPropertyValueBool(*args):
        """
        SetPropertyValueBool(self, PGProperty id, bool value)
        SetPropertyValueBool(self, String name, bool value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueBool(*args)

    def SetPropertyValueString(*args):
        """
        SetPropertyValueString(self, PGProperty id, String value)
        SetPropertyValueString(self, String name, String value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueString(*args)

    def SetPropertyValueWxObjectPtr(*args):
        """
        SetPropertyValueWxObjectPtr(self, PGProperty id, Object value)
        SetPropertyValueWxObjectPtr(self, String name, Object value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueWxObjectPtr(*args)

    def SetPropertyValueDatetime(*args):
        """
        SetPropertyValueDatetime(self, PGProperty id, DateTime value)
        SetPropertyValueDatetime(self, String name, DateTime value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueDatetime(*args)

    def SetPropertyValuePoint(*args):
        """
        SetPropertyValuePoint(self, PGProperty id, Point value)
        SetPropertyValuePoint(self, String name, Point value)
        """
        return _propgrid.PropertyGrid_SetPropertyValuePoint(*args)

    def SetPropertyValueSize(*args):
        """
        SetPropertyValueSize(self, PGProperty id, Size value)
        SetPropertyValueSize(self, String name, Size value)
        """
        return _propgrid.PropertyGrid_SetPropertyValueSize(*args)

    def SetPropertyUnspecified(*args):
        """
        SetPropertyUnspecified(self, PGProperty id)
        SetPropertyUnspecified(self, String name)
        """
        return _propgrid.PropertyGrid_SetPropertyUnspecified(*args)

    def SetVerticalSpacing(*args, **kwargs):
        """SetVerticalSpacing(self, int vspacing)"""
        return _propgrid.PropertyGrid_SetVerticalSpacing(*args, **kwargs)

    def ShowLowPriority(*args, **kwargs):
        """ShowLowPriority(self)"""
        return _propgrid.PropertyGrid_ShowLowPriority(*args, **kwargs)

    def ShowPropertyError(*args):
        """
        ShowPropertyError(self, PGProperty id, String msg)
        ShowPropertyError(self, String name, String msg)
        """
        return _propgrid.PropertyGrid_ShowPropertyError(*args)

    def Sort(*args):
        """
        Sort(self)
        Sort(self, PGProperty id)
        Sort(self, String name)
        """
        return _propgrid.PropertyGrid_Sort(*args)

    def SetWindowStyleFlag(*args, **kwargs):
        """
        SetWindowStyleFlag(self, long style)

        Sets the style of the window. Please note that some styles cannot be
        changed after the window creation and that Refresh() might need to be
        called after changing the others for the change to take place
        immediately.
        """
        return _propgrid.PropertyGrid_SetWindowStyleFlag(*args, **kwargs)

    def SetDefaultPriority(*args, **kwargs):
        """SetDefaultPriority(self, int priority)"""
        return _propgrid.PropertyGrid_SetDefaultPriority(*args, **kwargs)

    def ResetDefaultPriority(*args, **kwargs):
        """ResetDefaultPriority(self)"""
        return _propgrid.PropertyGrid_ResetDefaultPriority(*args, **kwargs)

    def EditorsValueWasModified(*args, **kwargs):
        """EditorsValueWasModified(self)"""
        return _propgrid.PropertyGrid_EditorsValueWasModified(*args, **kwargs)

    def EditorsValueWasNotModified(*args, **kwargs):
        """EditorsValueWasNotModified(self)"""
        return _propgrid.PropertyGrid_EditorsValueWasNotModified(*args, **kwargs)

    def IsEditorsValueModified(*args, **kwargs):
        """IsEditorsValueModified(self) -> bool"""
        return _propgrid.PropertyGrid_IsEditorsValueModified(*args, **kwargs)

    def GenerateEditorButton(*args, **kwargs):
        """GenerateEditorButton(self, Point pos, Size sz) -> Window"""
        return _propgrid.PropertyGrid_GenerateEditorButton(*args, **kwargs)

    def FixPosForTextCtrl(*args, **kwargs):
        """FixPosForTextCtrl(self, Window ctrl)"""
        return _propgrid.PropertyGrid_FixPosForTextCtrl(*args, **kwargs)

    def GenerateEditorTextCtrl(*args, **kwargs):
        """
        GenerateEditorTextCtrl(self, Point pos, Size sz, String value, Window secondary, 
            int extraStyle=0, int maxLen=0) -> Window
        """
        return _propgrid.PropertyGrid_GenerateEditorTextCtrl(*args, **kwargs)

    def GenerateEditorTextCtrlAndButton(*args, **kwargs):
        """
        GenerateEditorTextCtrlAndButton(self, Point pos, Size sz, Window psecondary, int limited_editing, 
            PGProperty property) -> Window
        """
        return _propgrid.PropertyGrid_GenerateEditorTextCtrlAndButton(*args, **kwargs)

    def GetGoodEditorDialogPosition(*args, **kwargs):
        """GetGoodEditorDialogPosition(self, PGProperty p, Size sz) -> Point"""
        return _propgrid.PropertyGrid_GetGoodEditorDialogPosition(*args, **kwargs)

    def ExpandEscapeSequences(*args, **kwargs):
        """ExpandEscapeSequences(String dst_str, String src_str) -> String"""
        return _propgrid.PropertyGrid_ExpandEscapeSequences(*args, **kwargs)

    ExpandEscapeSequences = staticmethod(ExpandEscapeSequences)
    def CreateEscapeSequences(*args, **kwargs):
        """CreateEscapeSequences(String dst_str, String src_str) -> String"""
        return _propgrid.PropertyGrid_CreateEscapeSequences(*args, **kwargs)

    CreateEscapeSequences = staticmethod(CreateEscapeSequences)
    def GetEditorControl(*args, **kwargs):
        """GetEditorControl(self) -> Window"""
        return _propgrid.PropertyGrid_GetEditorControl(*args, **kwargs)

    def GetEditorControlSecondary(*args, **kwargs):
        """GetEditorControlSecondary(self) -> Window"""
        return _propgrid.PropertyGrid_GetEditorControlSecondary(*args, **kwargs)


class PropertyGridPtr(PropertyGrid):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyGrid
_propgrid.PropertyGrid_swigregister(PropertyGridPtr)

def PrePropertyGrid(*args, **kwargs):
    """PrePropertyGrid() -> PropertyGrid"""
    val = _propgrid.new_PrePropertyGrid(*args, **kwargs)
    val.thisown = 1
    return val

def PropertyGrid_AutoGetTranslation(*args, **kwargs):
    """PropertyGrid_AutoGetTranslation(bool enable)"""
    return _propgrid.PropertyGrid_AutoGetTranslation(*args, **kwargs)

def PropertyGrid_RegisterValueType(*args, **kwargs):
    """PropertyGrid_RegisterValueType(PGValueType valueclass, bool noDefCheck=False) -> PGValueType"""
    return _propgrid.PropertyGrid_RegisterValueType(*args, **kwargs)

def PropertyGrid_RegisterEditorClass(*args, **kwargs):
    """PropertyGrid_RegisterEditorClass(PGEditor valueclass, String name, bool noDefCheck=False) -> PGEditor"""
    return _propgrid.PropertyGrid_RegisterEditorClass(*args, **kwargs)

def PropertyGrid_ExpandEscapeSequences(*args, **kwargs):
    """PropertyGrid_ExpandEscapeSequences(String dst_str, String src_str) -> String"""
    return _propgrid.PropertyGrid_ExpandEscapeSequences(*args, **kwargs)

def PropertyGrid_CreateEscapeSequences(*args, **kwargs):
    """PropertyGrid_CreateEscapeSequences(String dst_str, String src_str) -> String"""
    return _propgrid.PropertyGrid_CreateEscapeSequences(*args, **kwargs)

class PropertyGridEvent(_core.CommandEvent):
    """Proxy of C++ PropertyGridEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGridEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=0, int id=0) -> PropertyGridEvent"""
        newobj = _propgrid.new_PropertyGridEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PropertyGridEvent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Clone(*args, **kwargs):
        """Clone(self) -> Event"""
        return _propgrid.PropertyGridEvent_Clone(*args, **kwargs)

    def EnableProperty(*args, **kwargs):
        """EnableProperty(self, bool enable=True)"""
        return _propgrid.PropertyGridEvent_EnableProperty(*args, **kwargs)

    def DisableProperty(*args, **kwargs):
        """DisableProperty(self)"""
        return _propgrid.PropertyGridEvent_DisableProperty(*args, **kwargs)

    def GetMainParent(*args, **kwargs):
        """GetMainParent(self) -> PGProperty"""
        return _propgrid.PropertyGridEvent_GetMainParent(*args, **kwargs)

    def GetProperty(*args, **kwargs):
        """GetProperty(self) -> PGProperty"""
        return _propgrid.PropertyGridEvent_GetProperty(*args, **kwargs)

    def GetPropertyLabel(*args, **kwargs):
        """GetPropertyLabel(self) -> String"""
        return _propgrid.PropertyGridEvent_GetPropertyLabel(*args, **kwargs)

    def GetPropertyName(*args, **kwargs):
        """GetPropertyName(self) -> String"""
        return _propgrid.PropertyGridEvent_GetPropertyName(*args, **kwargs)

    def GetPropertyClientData(*args, **kwargs):
        """GetPropertyClientData(self) -> void"""
        return _propgrid.PropertyGridEvent_GetPropertyClientData(*args, **kwargs)

    def GetPropertyValue(self):
        return self.GetProperty().GetValue()

    def HasProperty(*args, **kwargs):
        """HasProperty(self) -> bool"""
        return _propgrid.PropertyGridEvent_HasProperty(*args, **kwargs)

    def IsPropertyEnabled(*args, **kwargs):
        """IsPropertyEnabled(self) -> bool"""
        return _propgrid.PropertyGridEvent_IsPropertyEnabled(*args, **kwargs)


class PropertyGridEventPtr(PropertyGridEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyGridEvent
_propgrid.PropertyGridEvent_swigregister(PropertyGridEventPtr)

PG_BASE_EVT_PRE_ID = _propgrid.PG_BASE_EVT_PRE_ID
wxEVT_PG_SELECTED = _propgrid.wxEVT_PG_SELECTED
wxEVT_PG_CHANGED = _propgrid.wxEVT_PG_CHANGED
wxEVT_PG_HIGHLIGHTED = _propgrid.wxEVT_PG_HIGHLIGHTED
wxEVT_PG_RIGHT_CLICK = _propgrid.wxEVT_PG_RIGHT_CLICK
wxEVT_PG_PAGE_CHANGED = _propgrid.wxEVT_PG_PAGE_CHANGED
wxEVT_PG_ITEM_COLLAPSED = _propgrid.wxEVT_PG_ITEM_COLLAPSED
wxEVT_PG_ITEM_EXPANDED = _propgrid.wxEVT_PG_ITEM_EXPANDED
wxEVT_PG_CREATECTRL = _propgrid.wxEVT_PG_CREATECTRL
wxEVT_PG_DESTROYCTRL = _propgrid.wxEVT_PG_DESTROYCTRL
wxEVT_PG_LEFT_DCLICK = _propgrid.wxEVT_PG_LEFT_DCLICK
class PropertyGridPopulator(object):
    """Proxy of C++ PropertyGridPopulator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGridPopulator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, PropertyGrid pg=(wxPropertyGrid *) NULL, PGProperty popRoot=((wxPGProperty *) NULL)) -> PropertyGridPopulator"""
        newobj = _propgrid.new_PropertyGridPopulator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PropertyGridPopulator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AddChoices(*args, **kwargs):
        """
        AddChoices(self, wxPGChoicesId choicesId, wxArrayString choiceLabels, 
            wxArrayInt choiceValues=wxArrayInt_wxPG_EMPTY)
        """
        return _propgrid.PropertyGridPopulator_AddChoices(*args, **kwargs)

    def AppendByClass(*args, **kwargs):
        """
        AppendByClass(self, String classname, String label, String name=wxString_wxPG_LABEL, 
            String value=wxEmptyString, String attributes=wxEmptyString, 
            wxPGChoicesId choicesId=(wxPGChoicesId) 0, 
            wxArrayString choiceLabels=wxArrayString_wxPG_EMPTY, 
            wxArrayInt choiceValues=wxArrayInt_wxPG_EMPTY) -> PGProperty
        """
        return _propgrid.PropertyGridPopulator_AppendByClass(*args, **kwargs)

    def AppendByType(*args, **kwargs):
        """
        AppendByType(self, String valuetype, String label, String name=wxString_wxPG_LABEL, 
            String value=wxEmptyString, String attributes=wxEmptyString, 
            wxPGChoicesId choicesId=(wxPGChoicesId) 0, 
            wxArrayString choiceLabels=wxArrayString_wxPG_EMPTY, 
            wxArrayInt choiceValues=wxArrayInt_wxPG_EMPTY) -> PGProperty
        """
        return _propgrid.PropertyGridPopulator_AppendByType(*args, **kwargs)

    def GetCurrentParent(*args, **kwargs):
        """GetCurrentParent(self) -> PGProperty"""
        return _propgrid.PropertyGridPopulator_GetCurrentParent(*args, **kwargs)

    def HasChoices(*args, **kwargs):
        """HasChoices(self, wxPGChoicesId id) -> bool"""
        return _propgrid.PropertyGridPopulator_HasChoices(*args, **kwargs)

    def SetGrid(*args, **kwargs):
        """SetGrid(self, PropertyGrid pg)"""
        return _propgrid.PropertyGridPopulator_SetGrid(*args, **kwargs)

    def BeginChildren(*args, **kwargs):
        """BeginChildren(self) -> bool"""
        return _propgrid.PropertyGridPopulator_BeginChildren(*args, **kwargs)

    def EndChildren(*args, **kwargs):
        """EndChildren(self)"""
        return _propgrid.PropertyGridPopulator_EndChildren(*args, **kwargs)


class PropertyGridPopulatorPtr(PropertyGridPopulator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyGridPopulator
_propgrid.PropertyGridPopulator_swigregister(PropertyGridPopulatorPtr)

class PGEditor(object):
    """Proxy of C++ PGEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_propgrid.delete_PGEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGEditor_GetName(*args, **kwargs)

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window psecondary) -> Window
        """
        return _propgrid.PGEditor_CreateControls(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, PGProperty property, Window ctrl)"""
        return _propgrid.PGEditor_UpdateControl(*args, **kwargs)

    def DrawValue(*args, **kwargs):
        """DrawValue(self, DC dc, PGProperty property, Rect rect)"""
        return _propgrid.PGEditor_DrawValue(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """
        OnEvent(self, PropertyGrid propgrid, PGProperty property, Window wnd_primary, 
            Event event) -> bool
        """
        return _propgrid.PGEditor_OnEvent(*args, **kwargs)

    def CopyValueFromControl(*args, **kwargs):
        """CopyValueFromControl(self, PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGEditor_CopyValueFromControl(*args, **kwargs)

    def SetValueToUnspecified(*args, **kwargs):
        """SetValueToUnspecified(self, Window ctrl)"""
        return _propgrid.PGEditor_SetValueToUnspecified(*args, **kwargs)

    def SetControlStringValue(*args, **kwargs):
        """SetControlStringValue(self, Window ctrl, String txt)"""
        return _propgrid.PGEditor_SetControlStringValue(*args, **kwargs)

    def SetControlIntValue(*args, **kwargs):
        """SetControlIntValue(self, Window ctrl, int value)"""
        return _propgrid.PGEditor_SetControlIntValue(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, Window ctrl, String label, int index) -> int"""
        return _propgrid.PGEditor_InsertItem(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(self, Window ctrl, int index)"""
        return _propgrid.PGEditor_DeleteItem(*args, **kwargs)

    def OnFocus(*args, **kwargs):
        """OnFocus(self, PGProperty property, Window wnd)"""
        return _propgrid.PGEditor_OnFocus(*args, **kwargs)

    def CanContainCustomImage(*args, **kwargs):
        """CanContainCustomImage(self) -> bool"""
        return _propgrid.PGEditor_CanContainCustomImage(*args, **kwargs)


class PGEditorPtr(PGEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGEditor
_propgrid.PGEditor_swigregister(PGEditorPtr)

class PGTextCtrlEditor(PGEditor):
    """Proxy of C++ PGTextCtrlEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGTextCtrlEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGTextCtrlEditor_GetName(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGTextCtrlEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window psecondary) -> Window
        """
        return _propgrid.PGTextCtrlEditor_CreateControls(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, PGProperty property, Window ctrl)"""
        return _propgrid.PGTextCtrlEditor_UpdateControl(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """
        OnEvent(self, PropertyGrid propgrid, PGProperty property, Window primary, 
            Event event) -> bool
        """
        return _propgrid.PGTextCtrlEditor_OnEvent(*args, **kwargs)

    def CopyValueFromControl(*args, **kwargs):
        """CopyValueFromControl(self, PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGTextCtrlEditor_CopyValueFromControl(*args, **kwargs)

    def SetValueToUnspecified(*args, **kwargs):
        """SetValueToUnspecified(self, Window ctrl)"""
        return _propgrid.PGTextCtrlEditor_SetValueToUnspecified(*args, **kwargs)

    def DrawValue(*args, **kwargs):
        """DrawValue(self, DC dc, PGProperty property, Rect rect)"""
        return _propgrid.PGTextCtrlEditor_DrawValue(*args, **kwargs)

    def SetControlStringValue(*args, **kwargs):
        """SetControlStringValue(self, Window ctrl, String txt)"""
        return _propgrid.PGTextCtrlEditor_SetControlStringValue(*args, **kwargs)

    def OnFocus(*args, **kwargs):
        """OnFocus(self, PGProperty property, Window wnd)"""
        return _propgrid.PGTextCtrlEditor_OnFocus(*args, **kwargs)

    def OnTextCtrlEvent(*args, **kwargs):
        """
        OnTextCtrlEvent(PropertyGrid propgrid, PGProperty property, Window ctrl, 
            Event event) -> bool
        """
        return _propgrid.PGTextCtrlEditor_OnTextCtrlEvent(*args, **kwargs)

    OnTextCtrlEvent = staticmethod(OnTextCtrlEvent)
    def CopyTextCtrlValueFromControl(*args, **kwargs):
        """CopyTextCtrlValueFromControl(PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGTextCtrlEditor_CopyTextCtrlValueFromControl(*args, **kwargs)

    CopyTextCtrlValueFromControl = staticmethod(CopyTextCtrlValueFromControl)

class PGTextCtrlEditorPtr(PGTextCtrlEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGTextCtrlEditor
_propgrid.PGTextCtrlEditor_swigregister(PGTextCtrlEditorPtr)

def PGTextCtrlEditor_OnTextCtrlEvent(*args, **kwargs):
    """
    PGTextCtrlEditor_OnTextCtrlEvent(PropertyGrid propgrid, PGProperty property, Window ctrl, 
        Event event) -> bool
    """
    return _propgrid.PGTextCtrlEditor_OnTextCtrlEvent(*args, **kwargs)

def PGTextCtrlEditor_CopyTextCtrlValueFromControl(*args, **kwargs):
    """PGTextCtrlEditor_CopyTextCtrlValueFromControl(PGProperty property, Window ctrl) -> bool"""
    return _propgrid.PGTextCtrlEditor_CopyTextCtrlValueFromControl(*args, **kwargs)

class PGChoiceEditor(PGEditor):
    """Proxy of C++ PGChoiceEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGChoiceEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGChoiceEditor_GetName(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGChoiceEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window psecondary) -> Window
        """
        return _propgrid.PGChoiceEditor_CreateControls(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, PGProperty property, Window ctrl)"""
        return _propgrid.PGChoiceEditor_UpdateControl(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """
        OnEvent(self, PropertyGrid propgrid, PGProperty property, Window primary, 
            Event event) -> bool
        """
        return _propgrid.PGChoiceEditor_OnEvent(*args, **kwargs)

    def CopyValueFromControl(*args, **kwargs):
        """CopyValueFromControl(self, PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGChoiceEditor_CopyValueFromControl(*args, **kwargs)

    def SetValueToUnspecified(*args, **kwargs):
        """SetValueToUnspecified(self, Window ctrl)"""
        return _propgrid.PGChoiceEditor_SetValueToUnspecified(*args, **kwargs)

    def SetControlIntValue(*args, **kwargs):
        """SetControlIntValue(self, Window ctrl, int value)"""
        return _propgrid.PGChoiceEditor_SetControlIntValue(*args, **kwargs)

    def SetControlStringValue(*args, **kwargs):
        """SetControlStringValue(self, Window ctrl, String txt)"""
        return _propgrid.PGChoiceEditor_SetControlStringValue(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, Window ctrl, String label, int index) -> int"""
        return _propgrid.PGChoiceEditor_InsertItem(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(self, Window ctrl, int index)"""
        return _propgrid.PGChoiceEditor_DeleteItem(*args, **kwargs)

    def CanContainCustomImage(*args, **kwargs):
        """CanContainCustomImage(self) -> bool"""
        return _propgrid.PGChoiceEditor_CanContainCustomImage(*args, **kwargs)

    def CreateControlsBase(*args, **kwargs):
        """
        CreateControlsBase(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, long extraStyle) -> Window
        """
        return _propgrid.PGChoiceEditor_CreateControlsBase(*args, **kwargs)


class PGChoiceEditorPtr(PGChoiceEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGChoiceEditor
_propgrid.PGChoiceEditor_swigregister(PGChoiceEditorPtr)

class PGComboBoxEditor(PGChoiceEditor):
    """Proxy of C++ PGComboBoxEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGComboBoxEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGComboBoxEditor_GetName(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGComboBoxEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window ??) -> Window
        """
        return _propgrid.PGComboBoxEditor_CreateControls(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, PGProperty property, Window ctrl)"""
        return _propgrid.PGComboBoxEditor_UpdateControl(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """
        OnEvent(self, PropertyGrid propgrid, PGProperty property, Window ctrl, 
            Event event) -> bool
        """
        return _propgrid.PGComboBoxEditor_OnEvent(*args, **kwargs)

    def CopyValueFromControl(*args, **kwargs):
        """CopyValueFromControl(self, PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGComboBoxEditor_CopyValueFromControl(*args, **kwargs)

    def OnFocus(*args, **kwargs):
        """OnFocus(self, PGProperty ??, Window wnd)"""
        return _propgrid.PGComboBoxEditor_OnFocus(*args, **kwargs)


class PGComboBoxEditorPtr(PGComboBoxEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGComboBoxEditor
_propgrid.PGComboBoxEditor_swigregister(PGComboBoxEditorPtr)

class PGChoiceAndButtonEditor(PGChoiceEditor):
    """Proxy of C++ PGChoiceAndButtonEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGChoiceAndButtonEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGChoiceAndButtonEditor_GetName(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGChoiceAndButtonEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window psecondary) -> Window
        """
        return _propgrid.PGChoiceAndButtonEditor_CreateControls(*args, **kwargs)


class PGChoiceAndButtonEditorPtr(PGChoiceAndButtonEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGChoiceAndButtonEditor
_propgrid.PGChoiceAndButtonEditor_swigregister(PGChoiceAndButtonEditorPtr)

class PGCheckBoxEditor(PGEditor):
    """Proxy of C++ PGCheckBoxEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGCheckBoxEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetName(*args, **kwargs):
        """GetName(self) -> wxChar"""
        return _propgrid.PGCheckBoxEditor_GetName(*args, **kwargs)

    def __del__(self, destroy=_propgrid.delete_PGCheckBoxEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateControls(*args, **kwargs):
        """
        CreateControls(self, PropertyGrid propgrid, PGProperty property, Point pos, 
            Size sz, Window psecondary) -> Window
        """
        return _propgrid.PGCheckBoxEditor_CreateControls(*args, **kwargs)

    def UpdateControl(*args, **kwargs):
        """UpdateControl(self, PGProperty property, Window ctrl)"""
        return _propgrid.PGCheckBoxEditor_UpdateControl(*args, **kwargs)

    def OnEvent(*args, **kwargs):
        """
        OnEvent(self, PropertyGrid propgrid, PGProperty property, Window primary, 
            Event event) -> bool
        """
        return _propgrid.PGCheckBoxEditor_OnEvent(*args, **kwargs)

    def CopyValueFromControl(*args, **kwargs):
        """CopyValueFromControl(self, PGProperty property, Window ctrl) -> bool"""
        return _propgrid.PGCheckBoxEditor_CopyValueFromControl(*args, **kwargs)

    def SetValueToUnspecified(*args, **kwargs):
        """SetValueToUnspecified(self, Window ctrl)"""
        return _propgrid.PGCheckBoxEditor_SetValueToUnspecified(*args, **kwargs)

    def DrawValue(*args, **kwargs):
        """DrawValue(self, DC dc, PGProperty property, Rect rect)"""
        return _propgrid.PGCheckBoxEditor_DrawValue(*args, **kwargs)

    def SetControlIntValue(*args, **kwargs):
        """SetControlIntValue(self, Window ctrl, int value)"""
        return _propgrid.PGCheckBoxEditor_SetControlIntValue(*args, **kwargs)


class PGCheckBoxEditorPtr(PGCheckBoxEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGCheckBoxEditor
_propgrid.PGCheckBoxEditor_swigregister(PGCheckBoxEditorPtr)

PG_ESCAPE = _propgrid.PG_ESCAPE
PG_SUBID1 = _propgrid.PG_SUBID1
PG_SUBID2 = _propgrid.PG_SUBID2
class PGPaintData(object):
    """Proxy of C++ PGPaintData class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGPaintData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    m_parent = property(_propgrid.PGPaintData_m_parent_get, _propgrid.PGPaintData_m_parent_set)
    m_choiceItem = property(_propgrid.PGPaintData_m_choiceItem_get, _propgrid.PGPaintData_m_choiceItem_set)
    m_drawnWidth = property(_propgrid.PGPaintData_m_drawnWidth_get, _propgrid.PGPaintData_m_drawnWidth_set)
    m_drawnHeight = property(_propgrid.PGPaintData_m_drawnHeight_get, _propgrid.PGPaintData_m_drawnHeight_set)

class PGPaintDataPtr(PGPaintData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGPaintData
_propgrid.PGPaintData_swigregister(PGPaintDataPtr)

class CustomPropertyClass(PGPropertyWithChildren):
    """Proxy of C++ CustomPropertyClass class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCustomPropertyClass instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_propgrid.delete_CustomPropertyClass):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def DoSetValue(*args, **kwargs):
        """DoSetValue(self, PGVariant value)"""
        return _propgrid.CustomPropertyClass_DoSetValue(*args, **kwargs)

    def DoGetValue(*args, **kwargs):
        """DoGetValue(self) -> PGVariant"""
        return _propgrid.CustomPropertyClass_DoGetValue(*args, **kwargs)

    def SetValueFromString(*args, **kwargs):
        """SetValueFromString(self, String text, int flags) -> bool"""
        return _propgrid.CustomPropertyClass_SetValueFromString(*args, **kwargs)

    def GetValueAsString(*args, **kwargs):
        """GetValueAsString(self, int argFlags) -> String"""
        return _propgrid.CustomPropertyClass_GetValueAsString(*args, **kwargs)

    def GetImageSize(*args, **kwargs):
        """GetImageSize(self) -> Size"""
        return _propgrid.CustomPropertyClass_GetImageSize(*args, **kwargs)

    def OnCustomPaint(*args, **kwargs):
        """OnCustomPaint(self, DC dc, Rect rect, PGPaintData paintdata)"""
        return _propgrid.CustomPropertyClass_OnCustomPaint(*args, **kwargs)

    def SetValueFromInt(*args, **kwargs):
        """SetValueFromInt(self, long value, int ??) -> bool"""
        return _propgrid.CustomPropertyClass_SetValueFromInt(*args, **kwargs)

    def GetChoiceInfo(*args, **kwargs):
        """GetChoiceInfo(self, wxPGChoiceInfo choiceinfo) -> int"""
        return _propgrid.CustomPropertyClass_GetChoiceInfo(*args, **kwargs)

    def SetAttribute(*args, **kwargs):
        """SetAttribute(self, int id, wxVariant value)"""
        return _propgrid.CustomPropertyClass_SetAttribute(*args, **kwargs)


class CustomPropertyClassPtr(CustomPropertyClass):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CustomPropertyClass
_propgrid.CustomPropertyClass_swigregister(CustomPropertyClassPtr)

class PGStringTokenizer(object):
    """Proxy of C++ PGStringTokenizer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGStringTokenizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String str, wxChar delimeter) -> PGStringTokenizer"""
        newobj = _propgrid.new_PGStringTokenizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PGStringTokenizer):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def HasMoreTokens(*args, **kwargs):
        """HasMoreTokens(self) -> bool"""
        return _propgrid.PGStringTokenizer_HasMoreTokens(*args, **kwargs)

    def GetNextToken(*args, **kwargs):
        """GetNextToken(self) -> String"""
        return _propgrid.PGStringTokenizer_GetNextToken(*args, **kwargs)


class PGStringTokenizerPtr(PGStringTokenizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PGStringTokenizer
_propgrid.PGStringTokenizer_swigregister(PGStringTokenizerPtr)


def PGConstructSpinCtrlEditorClass(*args, **kwargs):
    """PGConstructSpinCtrlEditorClass() -> PGEditor"""
    return _propgrid.PGConstructSpinCtrlEditorClass(*args, **kwargs)

def PGConstructDatePickerCtrlEditorClass(*args, **kwargs):
    """PGConstructDatePickerCtrlEditorClass() -> PGEditor"""
    return _propgrid.PGConstructDatePickerCtrlEditorClass(*args, **kwargs)
PG_COLOUR_WEB_BASE = _propgrid.PG_COLOUR_WEB_BASE
PG_COLOUR_CUSTOM = _propgrid.PG_COLOUR_CUSTOM
class ColourPropertyValue(_core.Object):
    """Proxy of C++ ColourPropertyValue class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourPropertyValue instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    m_type = property(_propgrid.ColourPropertyValue_m_type_get, _propgrid.ColourPropertyValue_m_type_set)
    m_colour = property(_propgrid.ColourPropertyValue_m_colour_get, _propgrid.ColourPropertyValue_m_colour_set)
    def __init__(self, *args):
        """
        __init__(self) -> ColourPropertyValue
        __init__(self, Colour colour) -> ColourPropertyValue
        __init__(self, unsigned int type) -> ColourPropertyValue
        __init__(self, unsigned int type, Colour colour) -> ColourPropertyValue
        """
        newobj = _propgrid.new_ColourPropertyValue(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ColourPropertyValuePtr(ColourPropertyValue):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourPropertyValue
_propgrid.ColourPropertyValue_swigregister(ColourPropertyValuePtr)


def FontProperty(*args, **kwargs):
    """FontProperty(String label, String name=wxString_wxPG_LABEL, Font value=wxFONT_wxPG_NORMAL_FONT) -> PGProperty"""
    return _propgrid.FontProperty(*args, **kwargs)

def SystemColourProperty(*args, **kwargs):
    """SystemColourProperty(String label, String name=wxString_wxPG_LABEL, ColourPropertyValue value=wxCPV_wxPG_EMPTY) -> PGProperty"""
    return _propgrid.SystemColourProperty(*args, **kwargs)

def CursorProperty(*args, **kwargs):
    """CursorProperty(String label, String name=wxString_wxPG_LABEL, int value=CURSOR_NONE) -> PGProperty"""
    return _propgrid.CursorProperty(*args, **kwargs)

def DateProperty(*args, **kwargs):
    """DateProperty(String label, String name=wxString_wxPG_LABEL, DateTime value=wxDateTime()) -> PGProperty"""
    return _propgrid.DateProperty(*args, **kwargs)

def ImageFileProperty(*args, **kwargs):
    """ImageFileProperty(String label, String name=wxString_wxPG_LABEL, String value=wxEmptyString) -> PGProperty"""
    return _propgrid.ImageFileProperty(*args, **kwargs)

def ColourProperty(*args, **kwargs):
    """ColourProperty(String label, String name=wxString_wxPG_LABEL, Colour value=wxColour_BLACK) -> PGProperty"""
    return _propgrid.ColourProperty(*args, **kwargs)

def MultiChoiceProperty(*args, **kwargs):
    """
    MultiChoiceProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString choices=wxArrayString(), 
        wxArrayInt value=wxArrayInt_wxPG_EMPTY) -> PGProperty
    """
    return _propgrid.MultiChoiceProperty(*args, **kwargs)
class PropertyGridPage(_core.EvtHandler,PropertyContainerMethods):
    """Proxy of C++ PropertyGridPage class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGridPage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PropertyGridPage"""
        newobj = _propgrid.new_PropertyGridPage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_propgrid.delete_PropertyGridPage):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AppendIn(*args):
        """
        AppendIn(self, PGProperty id, PGProperty property) -> PGProperty
        AppendIn(self, String name, PGProperty property) -> PGProperty
        """
        return _propgrid.PropertyGridPage_AppendIn(*args)

    def GetStatePtr(*args, **kwargs):
        """GetStatePtr(self) -> wxPropertyGridState"""
        return _propgrid.PropertyGridPage_GetStatePtr(*args, **kwargs)

    def Insert(*args):
        """
        Insert(self, PGProperty id, int index, PGProperty property) -> PGProperty
        Insert(self, String name, int index, PGProperty property) -> PGProperty
        """
        return _propgrid.PropertyGridPage_Insert(*args)

    def Init(*args, **kwargs):
        """Init(self)"""
        return _propgrid.PropertyGridPage_Init(*args, **kwargs)

    def IsHandlingAllEvents(*args, **kwargs):
        """IsHandlingAllEvents(self) -> bool"""
        return _propgrid.PropertyGridPage_IsHandlingAllEvents(*args, **kwargs)


class PropertyGridPagePtr(PropertyGridPage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyGridPage
_propgrid.PropertyGridPage_swigregister(PropertyGridPagePtr)

class PropertyGridManager(_windows.Panel,PropertyContainerMethods):
    """Proxy of C++ PropertyGridManager class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGridManager instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=(0), 
            wxChar name=wxPyPropertyGridManagerNameStr) -> PropertyGridManager
        """
        newobj = _propgrid.new_PropertyGridManager(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        
        self._setOORInfo(self)
        self.DoDefaultTypeMappings()
        self.edited_objects = {}
        self.DoDefaultValueTypeMappings()
        if not hasattr(self.__class__,'_vt2setter'):
            self.__class__._vt2setter = {}
    

    def AddPage(*args, **kwargs):
        """
        AddPage(self, String label=wxEmptyString, Bitmap bmp=wxBitmap_NULL, 
            PropertyGridPage pageObj=(wxPropertyGridPage *) NULL) -> int
        """
        return _propgrid.PropertyGridManager_AddPage(*args, **kwargs)

    def AppendCategory(*args, **kwargs):
        """AppendCategory(self, String label, String name=wxString_wxPG_LABEL) -> PGProperty"""
        return _propgrid.PropertyGridManager_AppendCategory(*args, **kwargs)

    def Append(*args):
        """
        Append(self, PGProperty property) -> PGProperty
        Append(self, String label, String name, String value=wxEmptyString) -> PGProperty
        Append(self, String label, String name, int value) -> PGProperty
        Append(self, String label, String name, double value) -> PGProperty
        Append(self, String label, String name, bool value) -> PGProperty
        """
        return _propgrid.PropertyGridManager_Append(*args)

    def AppendIn(*args):
        """
        AppendIn(self, PGProperty id, PGProperty property) -> PGProperty
        AppendIn(self, String name, PGProperty property) -> PGProperty
        """
        return _propgrid.PropertyGridManager_AppendIn(*args)

    def CanClose(*args, **kwargs):
        """CanClose(self) -> bool"""
        return _propgrid.PropertyGridManager_CanClose(*args, **kwargs)

    def ClearModifiedStatus(*args):
        """
        ClearModifiedStatus(self, PGProperty id)
        ClearModifiedStatus(self)
        """
        return _propgrid.PropertyGridManager_ClearModifiedStatus(*args)

    def ClearPage(*args, **kwargs):
        """ClearPage(self, int page)"""
        return _propgrid.PropertyGridManager_ClearPage(*args, **kwargs)

    def ClearPropertyValue(*args):
        """
        ClearPropertyValue(self, PGProperty id) -> bool
        ClearPropertyValue(self, String name) -> bool
        """
        return _propgrid.PropertyGridManager_ClearPropertyValue(*args)

    def Collapse(*args):
        """
        Collapse(self, PGProperty id) -> bool
        Collapse(self, String name) -> bool
        """
        return _propgrid.PropertyGridManager_Collapse(*args)

    def CollapseAll(*args, **kwargs):
        """CollapseAll(self) -> bool"""
        return _propgrid.PropertyGridManager_CollapseAll(*args, **kwargs)

    def Compact(*args, **kwargs):
        """Compact(self, bool compact) -> bool"""
        return _propgrid.PropertyGridManager_Compact(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=(0), 
            wxChar name=wxPropertyGridManagerNameStr) -> bool
        """
        return _propgrid.PropertyGridManager_Create(*args, **kwargs)

    def DisableProperty(*args):
        """
        DisableProperty(self, PGProperty id) -> bool
        DisableProperty(self, String name) -> bool
        """
        return _propgrid.PropertyGridManager_DisableProperty(*args)

    def EnableCategories(*args, **kwargs):
        """EnableCategories(self, bool enable) -> bool"""
        return _propgrid.PropertyGridManager_EnableCategories(*args, **kwargs)

    def EnableProperty(*args):
        """
        EnableProperty(self, PGProperty id, bool enable=True) -> bool
        EnableProperty(self, String name, bool enable=True) -> bool
        """
        return _propgrid.PropertyGridManager_EnableProperty(*args)

    def EnsureVisible(*args):
        """
        EnsureVisible(self, PGProperty id) -> bool
        EnsureVisible(self, String name) -> bool
        """
        return _propgrid.PropertyGridManager_EnsureVisible(*args)

    def Expand(*args):
        """
        Expand(self, PGProperty id) -> bool
        Expand(self, String name) -> bool
        """
        return _propgrid.PropertyGridManager_Expand(*args)

    def ExpandAll(*args, **kwargs):
        """ExpandAll(self) -> bool"""
        return _propgrid.PropertyGridManager_ExpandAll(*args, **kwargs)

    def GetChildrenCount(*args):
        """
        GetChildrenCount(self) -> size_t
        GetChildrenCount(self, int pageIndex) -> size_t
        GetChildrenCount(self, PGProperty id) -> size_t
        GetChildrenCount(self, String name) -> size_t
        """
        return _propgrid.PropertyGridManager_GetChildrenCount(*args)

    def GetFirst(*args, **kwargs):
        """GetFirst(self) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetFirst(*args, **kwargs)

    def GetFirstCategory(*args, **kwargs):
        """GetFirstCategory(self) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetFirstCategory(*args, **kwargs)

    def GetFirstProperty(*args, **kwargs):
        """GetFirstProperty(self) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetFirstProperty(*args, **kwargs)

    def GetGrid(*args, **kwargs):
        """GetGrid(self) -> PropertyGrid"""
        return _propgrid.PropertyGridManager_GetGrid(*args, **kwargs)

    def GetLastChild(*args):
        """
        GetLastChild(self, PGProperty id) -> PGProperty
        GetLastChild(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGridManager_GetLastChild(*args)

    def GetNextCategory(*args, **kwargs):
        """GetNextCategory(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetNextCategory(*args, **kwargs)

    def GetNextProperty(*args, **kwargs):
        """GetNextProperty(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetNextProperty(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self, unsigned int ind) -> PropertyGridPage"""
        return _propgrid.PropertyGridManager_GetPage(*args, **kwargs)

    def GetPageByName(*args, **kwargs):
        """GetPageByName(self, wxChar name) -> int"""
        return _propgrid.PropertyGridManager_GetPageByName(*args, **kwargs)

    def GetPageByState(*args, **kwargs):
        """GetPageByState(self, wxPropertyGridState pstate) -> int"""
        return _propgrid.PropertyGridManager_GetPageByState(*args, **kwargs)

    def GetPageCount(*args, **kwargs):
        """GetPageCount(self) -> size_t"""
        return _propgrid.PropertyGridManager_GetPageCount(*args, **kwargs)

    def GetPageName(*args, **kwargs):
        """GetPageName(self, int index) -> String"""
        return _propgrid.PropertyGridManager_GetPageName(*args, **kwargs)

    def GetPageRoot(*args, **kwargs):
        """GetPageRoot(self, int index) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetPageRoot(*args, **kwargs)

    def GetPrevProperty(*args, **kwargs):
        """GetPrevProperty(self, PGProperty id) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetPrevProperty(*args, **kwargs)

    def GetPrevSibling(*args):
        """
        GetPrevSibling(self, PGProperty id) -> PGProperty
        GetPrevSibling(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGridManager_GetPrevSibling(*args)

    def GetPropertyByLabel(*args, **kwargs):
        """GetPropertyByLabel(self, String name, wxPropertyGridState ppState=(wxPropertyGridState **) NULL) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetPropertyByLabel(*args, **kwargs)

    def GetPropertyCategory(*args):
        """
        GetPropertyCategory(self, PGProperty id) -> PGProperty
        GetPropertyCategory(self, String name) -> PGProperty
        """
        return _propgrid.PropertyGridManager_GetPropertyCategory(*args)

    def GetPropertyColour(*args):
        """
        GetPropertyColour(self, PGProperty id) -> Colour
        GetPropertyColour(self, String name) -> Colour
        """
        return _propgrid.PropertyGridManager_GetPropertyColour(*args)

    def GetRoot(*args, **kwargs):
        """GetRoot(self) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetRoot(*args, **kwargs)

    def GetSelectedPage(*args, **kwargs):
        """GetSelectedPage(self) -> int"""
        return _propgrid.PropertyGridManager_GetSelectedPage(*args, **kwargs)

    def GetSelectedProperty(*args, **kwargs):
        """GetSelectedProperty(self) -> PGProperty"""
        return _propgrid.PropertyGridManager_GetSelectedProperty(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _propgrid.PropertyGridManager_GetSelection(*args, **kwargs)

    def GetTargetPage(*args, **kwargs):
        """GetTargetPage(self) -> int"""
        return _propgrid.PropertyGridManager_GetTargetPage(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar(self) -> wxToolBar"""
        return _propgrid.PropertyGridManager_GetToolBar(*args, **kwargs)

    def InitAllTypeHandlers(*args, **kwargs):
        """InitAllTypeHandlers(self)"""
        return _propgrid.PropertyGridManager_InitAllTypeHandlers(*args, **kwargs)

    def Insert(*args):
        """
        Insert(self, PGProperty id, int index, PGProperty property) -> PGProperty
        Insert(self, String name, int index, PGProperty property) -> PGProperty
        """
        return _propgrid.PropertyGridManager_Insert(*args)

    def InsertPage(*args, **kwargs):
        """InsertPage(self, int index, String label, Bitmap bmp=wxNullBitmap, PropertyGridPage pageObj=(wxPropertyGridPage *) NULL) -> int"""
        return _propgrid.PropertyGridManager_InsertPage(*args, **kwargs)

    def IsAnyModified(*args, **kwargs):
        """IsAnyModified(self) -> bool"""
        return _propgrid.PropertyGridManager_IsAnyModified(*args, **kwargs)

    def IsFrozen(*args, **kwargs):
        """IsFrozen(self) -> bool"""
        return _propgrid.PropertyGridManager_IsFrozen(*args, **kwargs)

    def IsPageModified(*args, **kwargs):
        """IsPageModified(self, size_t index) -> bool"""
        return _propgrid.PropertyGridManager_IsPageModified(*args, **kwargs)

    def LimitPropertyEditing(*args):
        """
        LimitPropertyEditing(self, PGProperty id, bool limit=True)
        LimitPropertyEditing(self, String name, bool limit=True)
        """
        return _propgrid.PropertyGridManager_LimitPropertyEditing(*args)

    def Refresh(*args, **kwargs):
        """
        Refresh(self, bool eraseBackground=True, Rect rect=(wxRect const *) NULL)

        Mark the specified rectangle (or the whole window) as "dirty" so it
        will be repainted.  Causes an EVT_PAINT event to be generated and sent
        to the window.
        """
        return _propgrid.PropertyGridManager_Refresh(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(self, int page) -> bool"""
        return _propgrid.PropertyGridManager_RemovePage(*args, **kwargs)

    def SelectPage(*args):
        """
        SelectPage(self, int index)
        SelectPage(self, wxChar name)
        """
        return _propgrid.PropertyGridManager_SelectPage(*args)

    def SelectProperty(*args):
        """
        SelectProperty(self, PGProperty id, bool focus=False) -> bool
        SelectProperty(self, String name, bool focus=False) -> bool
        """
        return _propgrid.PropertyGridManager_SelectProperty(*args)

    def SetDescription(*args, **kwargs):
        """SetDescription(self, String label, String content)"""
        return _propgrid.PropertyGridManager_SetDescription(*args, **kwargs)

    def SetCurrentCategory(*args):
        """
        SetCurrentCategory(self, PGProperty id)
        SetCurrentCategory(self, String name)
        """
        return _propgrid.PropertyGridManager_SetCurrentCategory(*args)

    def SetDescBoxHeight(*args, **kwargs):
        """SetDescBoxHeight(self, int ht, bool refresh=True)"""
        return _propgrid.PropertyGridManager_SetDescBoxHeight(*args, **kwargs)

    def SetDefaultPriority(*args, **kwargs):
        """SetDefaultPriority(self, int priority)"""
        return _propgrid.PropertyGridManager_SetDefaultPriority(*args, **kwargs)

    def ResetDefaultPriority(*args, **kwargs):
        """ResetDefaultPriority(self)"""
        return _propgrid.PropertyGridManager_ResetDefaultPriority(*args, **kwargs)

    def SetPropertyAttributeAll(*args, **kwargs):
        """SetPropertyAttributeAll(self, int attrid, wxVariant value)"""
        return _propgrid.PropertyGridManager_SetPropertyAttributeAll(*args, **kwargs)

    def SetPropertyLabel(*args):
        """
        SetPropertyLabel(self, PGProperty id, String newlabel)
        SetPropertyLabel(self, String name, String newlabel)
        """
        return _propgrid.PropertyGridManager_SetPropertyLabel(*args)

    def SetPropertyColour(*args):
        """
        SetPropertyColour(self, PGProperty id, Colour col)
        SetPropertyColour(self, String name, Colour col)
        """
        return _propgrid.PropertyGridManager_SetPropertyColour(*args)

    def SetPropertyColourToDefault(*args):
        """
        SetPropertyColourToDefault(self, PGProperty id)
        SetPropertyColourToDefault(self, String name)
        """
        return _propgrid.PropertyGridManager_SetPropertyColourToDefault(*args)

    def _SetPropertyValueArrstr(*args, **kwargs):
        """_SetPropertyValueArrstr(self, PGProperty id, wxArrayString value)"""
        return _propgrid.PropertyGridManager__SetPropertyValueArrstr(*args, **kwargs)

    def SetPropertyValueLong(*args):
        """
        SetPropertyValueLong(self, PGProperty id, long value)
        SetPropertyValueLong(self, String name, long value)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueLong(*args)

    def SetPropertyValueDouble(*args):
        """
        SetPropertyValueDouble(self, PGProperty id, double value)
        SetPropertyValueDouble(self, String name, double value)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueDouble(*args)

    def SetPropertyValueBool(*args):
        """
        SetPropertyValueBool(self, PGProperty id, bool value)
        SetPropertyValueBool(self, String name, bool value)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueBool(*args)

    def SetPropertyValueString(*args):
        """
        SetPropertyValueString(self, PGProperty id, String value)
        SetPropertyValueString(self, String name, String value)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueString(*args)

    def SetPropertyValueWxObjectPtr(*args):
        """
        SetPropertyValueWxObjectPtr(self, PGProperty id, Object value)
        SetPropertyValueWxObjectPtr(self, String name, Object value)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueWxObjectPtr(*args)

    def _SetPropertyValueArrint(*args, **kwargs):
        """_SetPropertyValueArrint(self, PGProperty id, wxArrayInt _av1_)"""
        return _propgrid.PropertyGridManager__SetPropertyValueArrint(*args, **kwargs)

    def SetPropertyValueDatetime(*args):
        """
        SetPropertyValueDatetime(self, PGProperty id, DateTime _av1_)
        SetPropertyValueDatetime(self, String name, DateTime _av1_)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueDatetime(*args)

    def SetPropertyValuePoint(*args):
        """
        SetPropertyValuePoint(self, PGProperty id, Point _av1_)
        SetPropertyValuePoint(self, String name, Point _av1_)
        """
        return _propgrid.PropertyGridManager_SetPropertyValuePoint(*args)

    def SetPropertyValueSize(*args):
        """
        SetPropertyValueSize(self, PGProperty id, Size _av1_)
        SetPropertyValueSize(self, String name, Size _av1_)
        """
        return _propgrid.PropertyGridManager_SetPropertyValueSize(*args)

    def SetPropertyUnspecified(*args):
        """
        SetPropertyUnspecified(self, PGProperty id)
        SetPropertyUnspecified(self, String name)
        """
        return _propgrid.PropertyGridManager_SetPropertyUnspecified(*args)

    def SetSplitterLeft(*args, **kwargs):
        """SetSplitterLeft(self, bool subProps=False, bool allPages=True)"""
        return _propgrid.PropertyGridManager_SetSplitterLeft(*args, **kwargs)

    def SetSplitterPosition(*args, **kwargs):
        """SetSplitterPosition(self, int newx, bool refresh=True)"""
        return _propgrid.PropertyGridManager_SetSplitterPosition(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(self, wxChar name)"""
        return _propgrid.PropertyGridManager_SetStringSelection(*args, **kwargs)

    def SetTargetPage(*args):
        """
        SetTargetPage(self, int index)
        SetTargetPage(self, wxChar name)
        """
        return _propgrid.PropertyGridManager_SetTargetPage(*args)

    def Sort(*args):
        """
        Sort(self)
        Sort(self, PGProperty id)
        Sort(self, String name)
        """
        return _propgrid.PropertyGridManager_Sort(*args)

    def ClearSelection(*args, **kwargs):
        """ClearSelection(self) -> bool"""
        return _propgrid.PropertyGridManager_ClearSelection(*args, **kwargs)

    def GetValuesFromPage(self,page,dict_=None,as_strings=False):
        """\
        Same as GetValues, but returns values from specific page only.

        For argument descriptions, see GetValues.
        """

        if dict_ is None:
            dict_ = {}
        elif hasattr(dict_,'__dict__'):
            dict_ = dict_.__dict__

        if not as_strings:
            getter = self.GetPropertyValue
        else:
            getter = self.GetPropertyValueAsString

        root = self.GetPageRoot(page)
        self._GetValues(root,self.GetFirstChild(root),dict_,getter)

        return dict_


    def GetValues(self,dict_=None,as_strings=False):
        """\
        Returns values in the grid.

        dict_: if not given, then a new one is created. dict_ can be
          object as well, in which case it's __dict__ is used.
        as_strings: if True, then string representations of values
          are fetched instead of native types. Useful for config and such.

        Return value: dictionary with values. It is always a dictionary,
        so if dict_ was object with __dict__ attribute, then that attribute
        is returned.
        """

        if dict_ is None:
            dict_ = {}
        elif hasattr(dict_,'__dict__'):
            dict_ = dict_.__dict__

        if not as_strings:
            getter = self.GetPropertyValue
        else:
            getter = self.GetPropertyValueAsString

        for page in range(0,self.GetPageCount()):
            root = self.GetPageRoot(page)
            self._GetValues(root,self.GetFirstChild(root),dict_,getter)

        return dict_

    GetPropertyValues = GetValues


    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _propgrid.PropertyGridManager_DoGetBestSize(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, int winid)

        Sets the identifier of the window.  Each window has an integer
        identifier. If the application has not provided one, an identifier
        will be generated. Normally, the identifier should be provided on
        creation and should not be modified subsequently.
        """
        return _propgrid.PropertyGridManager_SetId(*args, **kwargs)

    def Freeze(*args, **kwargs):
        """
        Freeze(self)

        Freezes the window or, in other words, prevents any updates from
        taking place on screen, the window is not redrawn at all. Thaw must be
        called to reenable window redrawing.  Calls to Freeze/Thaw may be
        nested, with the actual Thaw being delayed until all the nesting has
        been undone.

        This method is useful for visual appearance optimization (for example,
        it is a good idea to use it before inserting large amount of text into
        a wxTextCtrl under wxGTK) but is not implemented on all platforms nor
        for all controls so it is mostly just a hint to wxWindows and not a
        mandatory directive.
        """
        return _propgrid.PropertyGridManager_Freeze(*args, **kwargs)

    def Thaw(*args, **kwargs):
        """
        Thaw(self)

        Reenables window updating after a previous call to Freeze.  Calls to
        Freeze/Thaw may be nested, so Thaw must be called the same number of
        times that Freeze was before the window will be updated.
        """
        return _propgrid.PropertyGridManager_Thaw(*args, **kwargs)

    def SetExtraStyle(*args, **kwargs):
        """
        SetExtraStyle(self, long exStyle)

        Sets the extra style bits for the window.  Extra styles are the less
        often used style bits which can't be set with the constructor or with
        SetWindowStyleFlag()
        """
        return _propgrid.PropertyGridManager_SetExtraStyle(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """
        SetFont(self, Font font) -> bool

        Sets the font for this window.
        """
        return _propgrid.PropertyGridManager_SetFont(*args, **kwargs)

    def SetWindowStyleFlag(*args, **kwargs):
        """
        SetWindowStyleFlag(self, long style)

        Sets the style of the window. Please note that some styles cannot be
        changed after the window creation and that Refresh() might need to be
        called after changing the others for the change to take place
        immediately.
        """
        return _propgrid.PropertyGridManager_SetWindowStyleFlag(*args, **kwargs)


class PropertyGridManagerPtr(PropertyGridManager):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PropertyGridManager
_propgrid.PropertyGridManager_swigregister(PropertyGridManagerPtr)

def PrePropertyGridManager(*args, **kwargs):
    """PrePropertyGridManager() -> PropertyGridManager"""
    val = _propgrid.new_PrePropertyGridManager(*args, **kwargs)
    val.thisown = 1
    return val


def FontDataProperty(*args, **kwargs):
    """FontDataProperty(String label, String name=wxString_wxPG_LABEL, FontData value=wxFontData()) -> PGProperty"""
    return _propgrid.FontDataProperty(*args, **kwargs)

def PointProperty(*args, **kwargs):
    """PointProperty(String label, String name=wxString_wxPG_LABEL, Point value=wxPoint(0,0)) -> PGProperty"""
    return _propgrid.PointProperty(*args, **kwargs)

def SizeProperty(*args, **kwargs):
    """SizeProperty(String label, String name=wxString_wxPG_LABEL, Size value=wxSize(0,0)) -> PGProperty"""
    return _propgrid.SizeProperty(*args, **kwargs)

def DirsProperty(*args, **kwargs):
    """DirsProperty(String label, String name=wxString_wxPG_LABEL, wxArrayString value=wxArrayString()) -> PGProperty"""
    return _propgrid.DirsProperty(*args, **kwargs)

def ArrayDoubleProperty(*args, **kwargs):
    """ArrayDoubleProperty(String label, String name=wxString_wxPG_LABEL, wxArrayDouble value=wxArrayDouble()) -> PGProperty"""
    return _propgrid.ArrayDoubleProperty(*args, **kwargs)
EVT_PG_CHANGED = wx.PyEventBinder( wxEVT_PG_CHANGED, 1 )
EVT_PG_SELECTED = wx.PyEventBinder( wxEVT_PG_SELECTED, 1 )
EVT_PG_HIGHLIGHTED = wx.PyEventBinder( wxEVT_PG_HIGHLIGHTED, 1 )
EVT_PG_RIGHT_CLICK = wx.PyEventBinder( wxEVT_PG_RIGHT_CLICK, 1 )
EVT_PG_PAGE_CHANGED = wx.PyEventBinder( wxEVT_PG_PAGE_CHANGED, 1 )
EVT_PG_CREATECTRL = wx.PyEventBinder( wxEVT_PG_CREATECTRL, 1 )
EVT_PG_DESTROYCTRL = wx.PyEventBinder( wxEVT_PG_DESTROYCTRL, 1 )
EVT_PG_LEFT_DCLICK = wx.PyEventBinder( wxEVT_PG_LEFT_DCLICK, 1 )



