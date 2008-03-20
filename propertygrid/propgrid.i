/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.i
// Purpose:     Wrappers for the wxPropertyGrid.
//
// Author:      David Reyes Forniés
//
// Created:     11-Feb-2005
// Copyright:   (c) 2005 by Tragnarion Studios
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The `propgrid` provides a properties editor 'a la' .NET."
%enddef

%module(package="wx", docstring=DOCSTRING) propgrid

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
%}


%import windows.i

%constant int wxPG_AUTO_SORT;
%constant int wxPG_HIDE_CATEGORIES;
%constant int wxPG_ALPHABETIC_MODE;
%constant int wxPG_BOLD_MODIFIED;
%constant int wxPG_SPLITTER_AUTO_CENTER;
%constant int wxPG_TOOLTIPS;
%constant int wxPG_HIDE_MARGIN;
%constant int wxPG_STATIC_SPLITTER;
%constant int wxPG_STATIC_LAYOUT;
%constant int wxPG_LIMITED_EDITING;
%constant int wxPG_TOOLBAR;
%constant int wxPG_DESCRIPTION;
%constant int wxPG_COMPACTOR;
%constant int wxPG_EX_INIT_NOCAT;
%constant int wxPG_EX_NO_FLAT_TOOLBAR;
%constant int wxPG_EX_CLASSIC_SPACING;
%constant int wxPG_DEFAULT_STYLE;
%constant int wxPGMAN_DEFAULT_STYLE;

%constant int wxPG_BOOL_USE_CHECKBOX;
%constant int wxPG_FLOAT_PRECISION;
%constant int wxPG_FILE_WILDCARD;
%constant int wxPG_FILE_SHOW_FULL_PATH;
%constant int wxPG_USER_ATTRIBUTE;

%constant wxEventType wxEVT_PG_SELECTED;
%constant wxEventType wxEVT_PG_CHANGED;
%constant wxEventType wxEVT_PG_HIGHLIGHTED;
%constant wxEventType wxEVT_PG_RIGHT_CLICK;
%constant wxEventType wxEVT_PG_PAGE_CHANGED;
%constant wxEventType wxEVT_PG_LEFT_DCLICK;
%constant wxEventType wxEVT_PG_CREATECTRL;
%constant wxEventType wxEVT_PG_DESTROYCTRL;

%pythoncode {
wx = _core
EVT_PG_SELECTED  = wx.PyEventBinder( wxEVT_PG_SELECTED, 1)
EVT_PG_CHANGED = wx.PyEventBinder( wxEVT_PG_CHANGED, 1)
EVT_PG_HIGHLIGHTED        = wx.PyEventBinder( wxEVT_PG_HIGHLIGHTED, 1)
EVT_PG_RIGHT_CLICK          = wx.PyEventBinder( wxEVT_PG_RIGHT_CLICK, 1)
EVT_PG_PAGE_CHANGED      = wx.PyEventBinder( wxEVT_PG_PAGE_CHANGED, 1)
EVT_PG_LEFT_DCLICK      = wx.PyEventBinder( wxEVT_PG_LEFT_DCLICK, 1)
EVT_PG_CREATECTRL       = wx.PyEventBinder( wxEVT_PG_CREATECTRL,1)
EVT_PG_DESTROYCTRL       = wx.PyEventBinder( wxEVT_PG_DESTROYCTRL,1)
}

class wxPropertyGridEvent : public wxCommandEvent
{
public:
    wxPropertyGridEvent(wxEventType commandType=0, int id=0);
    wxPGProperty* GetPropertyPtr() const;
    const wxString& GetPropertyLabel() const;
    const wxString& GetPropertyName() const;
    wxString GetPropertyValueAsString () const;
    
/*    void SetValidator ( wxValidator& validator );
    
    wxValidator* GetValidator () const;
    */
};

wxPGProperty* wxPropertyCategory ( const wxString& , const wxString& );
wxPGProperty* wxStringProperty( const wxString&, const wxString&, const wxString& );
wxPGProperty* wxIntProperty( const wxString&, const wxString&, long = 0 );
wxPGProperty* wxFloatProperty( const wxString&, const wxString&, double = .0 );
wxPGProperty* wxBoolProperty( const wxString&, const wxString&, bool = false );
wxPGProperty* wxLongStringProperty( const wxString&, const wxString&, const wxString& );
wxPGProperty* wxFileProperty( const wxString&, const wxString&, const wxString& );
wxPGProperty* wxDirProperty( const wxString&, const wxString&, const wxString& );
wxPGProperty* wxArrayStringProperty( const wxString&, const wxString&, const wxArrayString& );

wxPGProperty* wxEnumProperty( const wxString&, const wxString&, const wxArrayString& labels,const wxArrayInt& values, int value = 0 );
wxPGProperty* wxFontProperty( const wxString&, const wxString&, const wxFont& );
wxPGProperty* wxColourProperty( const wxString&, const wxString&, const wxColour& );

wxPGProperty* wxStringCallBackProperty( const wxString&, const wxString&, const wxString& );

wxPGProperty* wxImageFileProperty( const wxString&, const wxString& );

wxPGProperty* wxCustomProperty( const wxString&, const wxString& );

wxPGProperty* wxAdvImageFileProperty( const wxString&, const wxString& );

wxPGProperty* wxParentProperty( const wxString& label, const wxString& name );

wxPGProperty* wxFlagsProperty( const wxString&, const wxString&, const wxArrayString& labels, const wxArrayInt& values, int value = 0 );


class wxPGProperty {
public:
	
	wxString GetValueAsString( int arg_flags ) const;
	
	wxVariant GetValueAsVariant () const;	
	
	wxPGId GetId();
	
	bool SetValueFromString ( const wxString& text, int flags );
	
	void SetHelpString ( const wxString& help_string );
	
	const wxString& GetHelpString () const;
	
	void SetCallBackDialog( wxObject& dialog );
	
    const wxPGPropertyClassInfo* GetClassInfo() const;

    long GetValueAsLong();
};

class wxPropertyGrid : public wxScrolledWindow
{
public:
    wxPropertyGrid(wxWindow *parent, wxWindowID id = -1,
               			const wxPoint& pos = wxDefaultPosition,
               			const wxSize& size = wxDefaultSize,
               			long style = wxPG_DEFAULT_STYLE,
               			const wxChar* name = wxPropertyGridNameStr);
               			
    wxPGId Append ( wxPGProperty* property );
    
    wxPGId AppendBool ( const wxString& label, const wxString& name = wxPG_LABEL, bool value = false );
    
    wxPGId AppendIn( const wxPGId id, wxPGProperty* property );

    void SetPropertyAttribute ( const wxString& name, int attrid, const wxString& value, long arg_flags = 0  );
    
    void Clear();
    
    void CollapseAll();
    
    bool Collapse( const wxString& name );
    
    void ExpandAll();
    
    bool Expand( const wxString& name );
    
    void Refresh();
    
    void MakeIntegerSpinControl( const wxString& name );
    
    void MakeAnyControl( const wxString& name );
    
    void MakeIntegerSpinControlById( const wxPGId id );
    
    void MakeAnyControlById( const wxPGId id );
    
    bool IsPropertyKindOf( const wxString& name, wxPGPropertyClassInfo& info );

    /** Sets category caption background colour. */
    void SetCaptionBackgroundColour(const wxColour& col);

    /** Sets category caption text colour. */
    void SetCaptionForegroundColour(const wxColour& col);

    /** Sets default cell background colour - applies to property cells.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellBackgroundColour(const wxColour& col);

    /** Sets default cell text colour - applies to property name and value text.
        Note that appearance of editor widgets may not be affected.
    */
    void SetCellTextColour(const wxColour& col);

    /** Sets colour of lines between cells. */
    void SetLineColour(const wxColour& col);

    /** Sets background colour of margin. */
    void SetMarginColour(const wxColour& col);

    /** Sets selection background colour - applies to selected property name background. */
    void SetSelectionBackground(const wxColour& col);

    /** Sets selection foreground colour - applies to selected property name text. */
    void SetSelectionForeground(const wxColour& col);

    /** Sets background colour of property and all its children. Background brush
        cache is optimized for often set colours to be set last.
    */
    void SetPropertyColour ( wxPGId id, const wxColour& col );

    wxPGProperty* GetPropertyByCoolLabel( const wxString& label ) const;

    const wxSize GetSizeControl();

    const wxPoint GetPositionControl();
};

