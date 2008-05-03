# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

"""
The `propgrid` provides a properties editor 'a la' .NET.
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
wxPG_AUTO_SORT = _propgrid.wxPG_AUTO_SORT
wxPG_HIDE_CATEGORIES = _propgrid.wxPG_HIDE_CATEGORIES
wxPG_ALPHABETIC_MODE = _propgrid.wxPG_ALPHABETIC_MODE
wxPG_BOLD_MODIFIED = _propgrid.wxPG_BOLD_MODIFIED
wxPG_SPLITTER_AUTO_CENTER = _propgrid.wxPG_SPLITTER_AUTO_CENTER
wxPG_TOOLTIPS = _propgrid.wxPG_TOOLTIPS
wxPG_HIDE_MARGIN = _propgrid.wxPG_HIDE_MARGIN
wxPG_STATIC_SPLITTER = _propgrid.wxPG_STATIC_SPLITTER
wxPG_STATIC_LAYOUT = _propgrid.wxPG_STATIC_LAYOUT
wxPG_LIMITED_EDITING = _propgrid.wxPG_LIMITED_EDITING
wxPG_TOOLBAR = _propgrid.wxPG_TOOLBAR
wxPG_DESCRIPTION = _propgrid.wxPG_DESCRIPTION
wxPG_COMPACTOR = _propgrid.wxPG_COMPACTOR
wxPG_EX_INIT_NOCAT = _propgrid.wxPG_EX_INIT_NOCAT
wxPG_EX_NO_FLAT_TOOLBAR = _propgrid.wxPG_EX_NO_FLAT_TOOLBAR
wxPG_EX_CLASSIC_SPACING = _propgrid.wxPG_EX_CLASSIC_SPACING
wxPG_DEFAULT_STYLE = _propgrid.wxPG_DEFAULT_STYLE
wxPGMAN_DEFAULT_STYLE = _propgrid.wxPGMAN_DEFAULT_STYLE
wxPG_BOOL_USE_CHECKBOX = _propgrid.wxPG_BOOL_USE_CHECKBOX
wxPG_FLOAT_PRECISION = _propgrid.wxPG_FLOAT_PRECISION
wxPG_FILE_WILDCARD = _propgrid.wxPG_FILE_WILDCARD
wxPG_FILE_SHOW_FULL_PATH = _propgrid.wxPG_FILE_SHOW_FULL_PATH
wxPG_USER_ATTRIBUTE = _propgrid.wxPG_USER_ATTRIBUTE
wxEVT_PG_SELECTED = _propgrid.wxEVT_PG_SELECTED
wxEVT_PG_CHANGED = _propgrid.wxEVT_PG_CHANGED
wxEVT_PG_HIGHLIGHTED = _propgrid.wxEVT_PG_HIGHLIGHTED
wxEVT_PG_RIGHT_CLICK = _propgrid.wxEVT_PG_RIGHT_CLICK
wxEVT_PG_PAGE_CHANGED = _propgrid.wxEVT_PG_PAGE_CHANGED
wxEVT_PG_LEFT_DCLICK = _propgrid.wxEVT_PG_LEFT_DCLICK
wxEVT_PG_CREATECTRL = _propgrid.wxEVT_PG_CREATECTRL
wxEVT_PG_DESTROYCTRL = _propgrid.wxEVT_PG_DESTROYCTRL
wx = _core
EVT_PG_SELECTED  = wx.PyEventBinder( wxEVT_PG_SELECTED, 1)
EVT_PG_CHANGED = wx.PyEventBinder( wxEVT_PG_CHANGED, 1)
EVT_PG_HIGHLIGHTED        = wx.PyEventBinder( wxEVT_PG_HIGHLIGHTED, 1)
EVT_PG_RIGHT_CLICK          = wx.PyEventBinder( wxEVT_PG_RIGHT_CLICK, 1)
EVT_PG_PAGE_CHANGED      = wx.PyEventBinder( wxEVT_PG_PAGE_CHANGED, 1)
EVT_PG_LEFT_DCLICK      = wx.PyEventBinder( wxEVT_PG_LEFT_DCLICK, 1)
EVT_PG_CREATECTRL       = wx.PyEventBinder( wxEVT_PG_CREATECTRL,1)
EVT_PG_DESTROYCTRL       = wx.PyEventBinder( wxEVT_PG_DESTROYCTRL,1)

class wxPropertyGridEvent(_core.CommandEvent):
    """Proxy of C++ wxPropertyGridEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGridEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=0, int id=0) -> wxPropertyGridEvent"""
        newobj = _propgrid.new_wxPropertyGridEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPropertyPtr(*args, **kwargs):
        """GetPropertyPtr(self) -> wxPGProperty"""
        return _propgrid.wxPropertyGridEvent_GetPropertyPtr(*args, **kwargs)

    def GetPropertyLabel(*args, **kwargs):
        """GetPropertyLabel(self) -> String"""
        return _propgrid.wxPropertyGridEvent_GetPropertyLabel(*args, **kwargs)

    def GetPropertyName(*args, **kwargs):
        """GetPropertyName(self) -> String"""
        return _propgrid.wxPropertyGridEvent_GetPropertyName(*args, **kwargs)

    def GetPropertyValueAsString(*args, **kwargs):
        """GetPropertyValueAsString(self) -> String"""
        return _propgrid.wxPropertyGridEvent_GetPropertyValueAsString(*args, **kwargs)


class wxPropertyGridEventPtr(wxPropertyGridEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPropertyGridEvent
_propgrid.wxPropertyGridEvent_swigregister(wxPropertyGridEventPtr)


def wxPropertyCategory(*args, **kwargs):
    """wxPropertyCategory(String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxPropertyCategory(*args, **kwargs)

def wxStringProperty(*args, **kwargs):
    """wxStringProperty(String ??, String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxStringProperty(*args, **kwargs)

def wxIntProperty(*args, **kwargs):
    """wxIntProperty(String ??, String ??, long ??=0) -> wxPGProperty"""
    return _propgrid.wxIntProperty(*args, **kwargs)

def wxFloatProperty(*args, **kwargs):
    """wxFloatProperty(String ??, String ??, double ??=.0) -> wxPGProperty"""
    return _propgrid.wxFloatProperty(*args, **kwargs)

def wxBoolProperty(*args, **kwargs):
    """wxBoolProperty(String ??, String ??, bool ??=False) -> wxPGProperty"""
    return _propgrid.wxBoolProperty(*args, **kwargs)

def wxLongStringProperty(*args, **kwargs):
    """wxLongStringProperty(String ??, String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxLongStringProperty(*args, **kwargs)

def wxFileProperty(*args, **kwargs):
    """wxFileProperty(String ??, String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxFileProperty(*args, **kwargs)

def wxDirProperty(*args, **kwargs):
    """wxDirProperty(String ??, String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxDirProperty(*args, **kwargs)

def wxArrayStringProperty(*args, **kwargs):
    """wxArrayStringProperty(String ??, String ??, wxArrayString ??) -> wxPGProperty"""
    return _propgrid.wxArrayStringProperty(*args, **kwargs)

def wxEnumProperty(*args, **kwargs):
    """
    wxEnumProperty(String ??, String ??, wxArrayString labels, wxArrayInt values, 
        int value=0) -> wxPGProperty
    """
    return _propgrid.wxEnumProperty(*args, **kwargs)

def wxFontProperty(*args, **kwargs):
    """wxFontProperty(String ??, String ??, Font ??) -> wxPGProperty"""
    return _propgrid.wxFontProperty(*args, **kwargs)

def wxColourProperty(*args, **kwargs):
    """wxColourProperty(String ??, String ??, Colour ??) -> wxPGProperty"""
    return _propgrid.wxColourProperty(*args, **kwargs)

def wxStringCallBackProperty(*args, **kwargs):
    """wxStringCallBackProperty(String ??, String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxStringCallBackProperty(*args, **kwargs)

def wxImageFileProperty(*args, **kwargs):
    """wxImageFileProperty(String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxImageFileProperty(*args, **kwargs)

def wxCustomProperty(*args, **kwargs):
    """wxCustomProperty(String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxCustomProperty(*args, **kwargs)

def wxAdvImageFileProperty(*args, **kwargs):
    """wxAdvImageFileProperty(String ??, String ??) -> wxPGProperty"""
    return _propgrid.wxAdvImageFileProperty(*args, **kwargs)

def wxParentProperty(*args, **kwargs):
    """wxParentProperty(String label, String name) -> wxPGProperty"""
    return _propgrid.wxParentProperty(*args, **kwargs)

def wxFlagsProperty(*args, **kwargs):
    """
    wxFlagsProperty(String ??, String ??, wxArrayString labels, wxArrayInt values, 
        int value=0) -> wxPGProperty
    """
    return _propgrid.wxFlagsProperty(*args, **kwargs)
class wxPGProperty(object):
    """Proxy of C++ wxPGProperty class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPGProperty instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetValueAsString(*args, **kwargs):
        """GetValueAsString(self, int arg_flags) -> String"""
        return _propgrid.wxPGProperty_GetValueAsString(*args, **kwargs)

    def GetValueAsVariant(*args, **kwargs):
        """GetValueAsVariant(self) -> wxVariant"""
        return _propgrid.wxPGProperty_GetValueAsVariant(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> wxPGId"""
        return _propgrid.wxPGProperty_GetId(*args, **kwargs)

    def SetValueFromString(*args, **kwargs):
        """SetValueFromString(self, String text, int flags) -> bool"""
        return _propgrid.wxPGProperty_SetValueFromString(*args, **kwargs)

    def SetHelpString(*args, **kwargs):
        """SetHelpString(self, String help_string)"""
        return _propgrid.wxPGProperty_SetHelpString(*args, **kwargs)

    def GetHelpString(*args, **kwargs):
        """GetHelpString(self) -> String"""
        return _propgrid.wxPGProperty_GetHelpString(*args, **kwargs)

    def SetCallBackDialog(*args, **kwargs):
        """SetCallBackDialog(self, Object dialog)"""
        return _propgrid.wxPGProperty_SetCallBackDialog(*args, **kwargs)

    def GetClassInfo(*args, **kwargs):
        """GetClassInfo(self) -> wxPGPropertyClassInfo"""
        return _propgrid.wxPGProperty_GetClassInfo(*args, **kwargs)

    def GetValueAsLong(*args, **kwargs):
        """GetValueAsLong(self) -> long"""
        return _propgrid.wxPGProperty_GetValueAsLong(*args, **kwargs)


class wxPGPropertyPtr(wxPGProperty):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPGProperty
_propgrid.wxPGProperty_swigregister(wxPGPropertyPtr)

class wxPropertyGrid(_windows.ScrolledWindow):
    """Proxy of C++ wxPropertyGrid class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPropertyGrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxPG_DEFAULT_STYLE, 
            wxChar name=wxPropertyGridNameStr) -> wxPropertyGrid
        """
        newobj = _propgrid.new_wxPropertyGrid(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Append(*args, **kwargs):
        """Append(self, wxPGProperty property) -> wxPGId"""
        return _propgrid.wxPropertyGrid_Append(*args, **kwargs)

    def AppendBool(*args, **kwargs):
        """AppendBool(self, String label, String name=wxPG_LABEL, bool value=False) -> wxPGId"""
        return _propgrid.wxPropertyGrid_AppendBool(*args, **kwargs)

    def AppendIn(*args, **kwargs):
        """AppendIn(self, wxPGId id, wxPGProperty property) -> wxPGId"""
        return _propgrid.wxPropertyGrid_AppendIn(*args, **kwargs)

    def SetPropertyAttribute(*args, **kwargs):
        """SetPropertyAttribute(self, String name, int attrid, String value, long arg_flags=0)"""
        return _propgrid.wxPropertyGrid_SetPropertyAttribute(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _propgrid.wxPropertyGrid_Clear(*args, **kwargs)

    def CollapseAll(*args, **kwargs):
        """CollapseAll(self)"""
        return _propgrid.wxPropertyGrid_CollapseAll(*args, **kwargs)

    def Collapse(*args, **kwargs):
        """Collapse(self, String name) -> bool"""
        return _propgrid.wxPropertyGrid_Collapse(*args, **kwargs)

    def ExpandAll(*args, **kwargs):
        """ExpandAll(self)"""
        return _propgrid.wxPropertyGrid_ExpandAll(*args, **kwargs)

    def Expand(*args, **kwargs):
        """Expand(self, String name) -> bool"""
        return _propgrid.wxPropertyGrid_Expand(*args, **kwargs)

    def Refresh(*args, **kwargs):
        """
        Refresh(self)

        Mark the specified rectangle (or the whole window) as "dirty" so it
        will be repainted.  Causes an EVT_PAINT event to be generated and sent
        to the window.
        """
        return _propgrid.wxPropertyGrid_Refresh(*args, **kwargs)

    def MakeIntegerSpinControl(*args, **kwargs):
        """MakeIntegerSpinControl(self, String name)"""
        return _propgrid.wxPropertyGrid_MakeIntegerSpinControl(*args, **kwargs)

    def MakeAnyControl(*args, **kwargs):
        """MakeAnyControl(self, String name)"""
        return _propgrid.wxPropertyGrid_MakeAnyControl(*args, **kwargs)

    def MakeIntegerSpinControlById(*args, **kwargs):
        """MakeIntegerSpinControlById(self, wxPGId id)"""
        return _propgrid.wxPropertyGrid_MakeIntegerSpinControlById(*args, **kwargs)

    def MakeAnyControlById(*args, **kwargs):
        """MakeAnyControlById(self, wxPGId id)"""
        return _propgrid.wxPropertyGrid_MakeAnyControlById(*args, **kwargs)

    def IsPropertyKindOf(*args, **kwargs):
        """IsPropertyKindOf(self, String name, wxPGPropertyClassInfo info) -> bool"""
        return _propgrid.wxPropertyGrid_IsPropertyKindOf(*args, **kwargs)

    def SetCaptionBackgroundColour(*args, **kwargs):
        """SetCaptionBackgroundColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetCaptionBackgroundColour(*args, **kwargs)

    def SetCaptionForegroundColour(*args, **kwargs):
        """SetCaptionForegroundColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetCaptionForegroundColour(*args, **kwargs)

    def SetCellBackgroundColour(*args, **kwargs):
        """SetCellBackgroundColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetCellBackgroundColour(*args, **kwargs)

    def SetCellTextColour(*args, **kwargs):
        """SetCellTextColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetCellTextColour(*args, **kwargs)

    def SetLineColour(*args, **kwargs):
        """SetLineColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetLineColour(*args, **kwargs)

    def SetMarginColour(*args, **kwargs):
        """SetMarginColour(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetMarginColour(*args, **kwargs)

    def SetSelectionBackground(*args, **kwargs):
        """SetSelectionBackground(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetSelectionBackground(*args, **kwargs)

    def SetSelectionForeground(*args, **kwargs):
        """SetSelectionForeground(self, Colour col)"""
        return _propgrid.wxPropertyGrid_SetSelectionForeground(*args, **kwargs)

    def SetPropertyColour(*args, **kwargs):
        """SetPropertyColour(self, wxPGId id, Colour col)"""
        return _propgrid.wxPropertyGrid_SetPropertyColour(*args, **kwargs)

    def GetPropertyByCoolLabel(*args, **kwargs):
        """GetPropertyByCoolLabel(self, String label) -> wxPGProperty"""
        return _propgrid.wxPropertyGrid_GetPropertyByCoolLabel(*args, **kwargs)

    def GetSizeControl(*args, **kwargs):
        """GetSizeControl(self) -> Size"""
        return _propgrid.wxPropertyGrid_GetSizeControl(*args, **kwargs)

    def GetPositionControl(*args, **kwargs):
        """GetPositionControl(self) -> Point"""
        return _propgrid.wxPropertyGrid_GetPositionControl(*args, **kwargs)


class wxPropertyGridPtr(wxPropertyGrid):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = wxPropertyGrid
_propgrid.wxPropertyGrid_swigregister(wxPropertyGridPtr)



