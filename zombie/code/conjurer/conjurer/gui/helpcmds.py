##\file helpcmds.py
##\brief Help menu commands

import wx

import aboutdlg
import iehelpwindow
import menu


# Help menu IDs
ID_Wiki = wx.NewId()
ID_Nebula2 = wx.NewId()
ID_Trac = wx.NewId()
ID_About = wx.NewId()
ID_Summoner = wx.NewId()
ID_Export = wx.NewId()
ID_Conjurer = wx.NewId()
ID_Manual = wx.NewId()

# HelpMenu class
class HelpMenu(menu.Menu):
    """Err... The help menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
    
    def create(self):
        # menu items
        self.Append( ID_Wiki, "&Wiki Help" )
        self.Append( ID_Nebula2, "&Nebula2 Help" )
        self.Append( ID_Trac, "&Trac Bug Help" )
        self.AppendSeparator()
        self.Append( ID_Export, "&Exporter Manual" )
        self.Append( ID_Manual, "&Conjurer Manual" )
        self.AppendSeparator()
        self.Append( ID_Conjurer, "&Conjurer Keys" )
        self.Append( ID_Summoner, "&Summoner Keys" )
        self.AppendSeparator()
        self.Append( ID_About, "A&bout Conjurer..." )
        
        # bindings
        self.bind_function(ID_Wiki, self.on_wiki_help)
        self.bind_function(ID_Nebula2, self.on_nebula2_help)
        self.bind_function(ID_Trac, self.on_trac_help)
        self.bind_function(ID_Summoner, self.on_summoner)
        self.bind_function(ID_Conjurer, self.on_conjurer)
        self.bind_function(ID_Export, self.on_export)
        self.bind_function(ID_Manual, self.on_manual)
        self.bind_function(ID_About, self.on_about)        

    def on_conjurer (self, evt):
        iehelpwindow.CreateWindow(self.get_frame(),
                                  "http://thezombieengine.sourceforge.net/SWD+Nebula+Conjurer+Conjurer")

    def on_summoner (self,evt):
        iehelpwindow.CreateWindow(self.get_frame(),
                                  "http://thezombieengine.sourceforge.net/SWD+Nebula+Conjurer+Summoner")

    def on_export (self,evt):
        iehelpwindow.CreateWindow(self.get_frame(),
                                  "http://thezombieengine.sourceforge.net/SWD+Nebula+MAX+Exporter")

    def on_manual (self,evt):
        iehelpwindow.CreateWindow(self.get_frame(),
                                  "http://thezombieengine.sourceforge.net/SWD+Nebula+Conjurer")

    def on_wiki_help(self,evt):
        dlg = wx.TextEntryDialog(self.get_frame(),
                                 'Put the word you want to search?',
                                 'Wiki Search Engine')
        
        # Get the clipboard content
        #do = wx.TextDataObject()
        #wx.TheClipboard.Open()
        #success = wx.TheClipboard.GetData(do)
        #wx.TheClipboard.Close()


        #if (success):
        #    dlg.SetValue(do.GetText())
        #else:
        dlg.SetValue("")

        if dlg.ShowModal() == wx.ID_OK:
            iehelpwindow.CreateWindow(self.get_frame(),
                                      "http://thezombieengine.wiki.sourceforge.net/"\
                                      "tiki-searchresults.php?words=",
                                      "&where=wikis&search=go", dlg.GetValue(),
                                      "http://thezombieengine.wiki.sourceforge.net/")

        dlg.Destroy()

    def on_trac_help(self,evt):
        dlg = wx.TextEntryDialog(self.get_frame(),
                                 'Put the word you want to search?',
                                 'Trac Search Engine')

        dlg.SetValue("")

        if dlg.ShowModal() == wx.ID_OK:
            iehelpwindow.CreateWindow(self.get_frame(),
                                      "http://sourceforge.net/tracker/?group_id=219911"\
                                      "search?q=",
                                      "&wiki=on&changeset=on&ticket=on",
                                      dlg.GetValue(),
                                      "http://sourceforge.net/tracker/?group_id=219911")
        dlg.Destroy()

    def on_nebula2_help(self,evt):
        dlg = wx.TextEntryDialog(self.get_frame(),
                                 'Put the word you want to search?',
                                 'Trac Search Engine')
        if dlg.ShowModal() == wx.ID_OK:            
            iehelpwindow.CreateWindow(self.get_frame(),
                                      "http://thezombieengine.sourceforge.net/"\
                                      "nightlybuild/doc/nebula2/html/class",
                                      ".html",
                                      dlg.GetValue(),
                                      "http://thezombieengine.sourceforge.net/"\
                                      "nightlybuild/doc/nebula2/"\
                                      "html/index.html")
        dlg.Destroy()
    
    def on_about(self,evt):
        """Show an about dialog"""
        dlg = aboutdlg.create_window(self.get_frame())
        dlg.ShowModal()
        dlg.Destroy()
