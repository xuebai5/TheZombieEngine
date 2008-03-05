##\file scripttextctrl.py
##\brief Text box for editing scripts with syntax hightlight

import keyword
import wx
import wx.stc


# ScriptTextCtrl class
class ScriptTextCtrl(wx.stc.StyledTextCtrl):
    """Text box for editing scripts with syntax highlight"""
    
    def __init__(self, parent):
        wx.stc.StyledTextCtrl.__init__(self, parent)
        
        # Default font
        self.set_font( wx.Font(10, wx.DEFAULT, wx.NORMAL, wx.NORMAL) )
        
        # Line numbers
        self.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
        self.SetMarginWidth(1, 30)
        
        # Adding a feature to be albe to zoom in and out for better reading
        self.CmdKeyAssign(ord('B'), wx.stc.STC_SCMOD_CTRL, wx.stc.STC_CMD_ZOOMIN)
        self.CmdKeyAssign(ord('N'), wx.stc.STC_SCMOD_CTRL, wx.stc.STC_CMD_ZOOMOUT)
    
    def __colourise(self):
        self.__set_global_styles()
        if self.GetLexer() == wx.stc.STC_LEX_LUA:
            self.__set_lua_styles()
        elif self.GetLexer() == wx.stc.STC_LEX_PYTHON:
            self.__set_python_styles()
        self.Colourise(0, -1)
    
    def __set_global_styles(self):
        # Global default styles for all languages
        self.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,     "face:%(face)s,size:%(size)d" % self.font)
        self.StyleClearAll()  # Reset all to be like the default
        
        # Global default styles for all languages
        self.StyleSetSpec(wx.stc.STC_STYLE_DEFAULT,     "face:%(face)s,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,  "back:#C0C0C0,face:%(face)s,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_STYLE_CONTROLCHAR, "face:%(face)s" % self.font)
        self.StyleSetSpec(wx.stc.STC_STYLE_BRACELIGHT,  "fore:#FFFFFF,back:#0000FF,bold")
        self.StyleSetSpec(wx.stc.STC_STYLE_BRACEBAD,    "fore:#000000,back:#FF0000,bold")
        
        self.SetCaretForeground("BLUE")
    
    def __set_lua_styles(self):
        # Same style values as those found in python styles, except for those
        # unique to LUA, which are left at its default value
        
        self.StyleSetSpec(wx.stc.STC_LUA_CHARACTER, "fore:#7F007F,face:%(face)s,size:%(size)d" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_COMMENT, "" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_COMMENTDOC, "" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_COMMENTLINE, "fore:#007F00,face:%(face)s,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_DEFAULT, "fore:#000000,face:%(face)s,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_IDENTIFIER, "fore:#000000,face:%(face)s,size:%(size)d" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_LITERALSTRING, "" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_NUMBER, "fore:#007F7F,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_OPERATOR, "bold,size:%(size)d" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_PREPROCESSOR, "" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_STRING, "fore:#7F007F,face:%(face)s,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_STRINGEOL, "fore:#000000,face:%(face)s,back:#E0C0E0,eol,size:%(size)d" % self.font)
        self.StyleSetSpec(wx.stc.STC_LUA_WORD, "fore:#00007F,bold,size:%(size)d" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_WORD2, "" % self.font)
#        self.StyleSetSpec(wx.stc.STC_LUA_WORD8, "" % self.font)
    
    def __set_python_styles(self):
        # Make some styles,  The lexer defines what each style is used for, we
        # just have to define what each style looks like.  This set is adapted from
        # Scintilla sample property files.
        
        # Python styles
        # Default 
        self.StyleSetSpec(wx.stc.STC_P_DEFAULT, "fore:#000000,face:%(face)s,size:%(size)d" % self.font)
        # Comments
        self.StyleSetSpec(wx.stc.STC_P_COMMENTLINE, "fore:#007F00,face:%(face)s,size:%(size)d" % self.font)
        # Number
        self.StyleSetSpec(wx.stc.STC_P_NUMBER, "fore:#007F7F,size:%(size)d" % self.font)
        # String
        self.StyleSetSpec(wx.stc.STC_P_STRING, "fore:#7F007F,face:%(face)s,size:%(size)d" % self.font)
        # Single quoted string
        self.StyleSetSpec(wx.stc.STC_P_CHARACTER, "fore:#7F007F,face:%(face)s,size:%(size)d" % self.font)
        # Keyword
        self.StyleSetSpec(wx.stc.STC_P_WORD, "fore:#00007F,bold,size:%(size)d" % self.font)
        # Triple quotes
        self.StyleSetSpec(wx.stc.STC_P_TRIPLE, "fore:#7F0000,size:%(size)d" % self.font)
        # Triple double quotes
        self.StyleSetSpec(wx.stc.STC_P_TRIPLEDOUBLE, "fore:#7F0000,size:%(size)d" % self.font)
        # Class name definition
        self.StyleSetSpec(wx.stc.STC_P_CLASSNAME, "fore:#0000FF,bold,underline,size:%(size)d" % self.font)
        # Function or method name definition
        self.StyleSetSpec(wx.stc.STC_P_DEFNAME, "fore:#007F7F,bold,size:%(size)d" % self.font)
        # Operators
        self.StyleSetSpec(wx.stc.STC_P_OPERATOR, "bold,size:%(size)d" % self.font)
        # Identifiers
        self.StyleSetSpec(wx.stc.STC_P_IDENTIFIER, "fore:#000000,face:%(face)s,size:%(size)d" % self.font)
        # Comment-blocks
        self.StyleSetSpec(wx.stc.STC_P_COMMENTBLOCK, "fore:#7F7F7F,size:%(size)d" % self.font)
        # End of line where string is not closed
        self.StyleSetSpec(wx.stc.STC_P_STRINGEOL, "fore:#000000,face:%(face)s,back:#E0C0E0,eol,size:%(size)d" % self.font)
    
    def set_font(self, font):
        self.font = {
            'face': font.GetFaceName(),
            'size': font.GetPointSize()
            }
        self.__colourise()
    
    def set_lua_highlighting(self):
        self.SetLexer(wx.stc.STC_LEX_LUA)
        # keywords taken from Lua reference manual
        keywords = [
            'and', 'break', 'do', 'else', 'elseif', 'false', 'for', 'function',
            'if', 'in', 'local', 'nil', 'not', 'or', 'repeat', 'return',
            'true', 'until', 'while'
            ]
        self.SetKeyWords(0, " ".join(keywords))
        self.__colourise()
    
    def set_python_highlighting(self):
        self.SetLexer(wx.stc.STC_LEX_PYTHON)
        self.SetKeyWords(0, " ".join(keyword.kwlist))
        self.__colourise()
    
    def set_text(self, text, undoable=False):
        self.SetText(text)
        if not undoable:
            self.EmptyUndoBuffer()
        self.Colourise(0, -1)
    
    def empty_undo_buffer(self):
        self.EmptyUndoBuffer()
