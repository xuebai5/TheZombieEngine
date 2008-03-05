#ifndef N_GUIBRUSHLABEL_H
#define N_GUIBRUSHLABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiBrushLabel
    @ingroup NebulaGuiSystem
    @brief Uses the gfx server to render gui brushes depending on characters in a string
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "gui/nguilabel.h"
#include "resource/nresourceserver.h"

//------------------------------------------------------------------------------
class nGuiBrushLabel : public nGuiLabel
{
public:
    /// Brush alignments
    enum Alignment
    {
        Left,
        Center,
        Right,
    };

    /// constructor
    nGuiBrushLabel();
    /// destructor
    virtual ~nGuiBrushLabel();
    /// render the widget
    virtual bool Render();
    /// set the text as string
    virtual void SetText(const char* text);
    /// get the text as string
    virtual const char* GetText() const;
    /// set text alignment
    void SetAlignment(Alignment a);
    /// get text alignment
    Alignment GetAlignment() const;
    /// set the text as integer
    void SetInt(int i);
    /// get the text as integer
    int GetInt() const;
    /// computes the text extent for this widget
    vector2 GetTextExtent();
    /// Get the character separation
    float GetCharSeparation() const;
    /// Set the character separation
    void SetCharSeparation(float);
    /// set fake facetype name
    void SetTypeFaceName(const char *);
    /// get fake facetype name
    const char * GetTypeFaceName();
    /// set size of character brush in pixels
    void SetBrushSize(int xs, int ys);
    /// get size of character brush in pixels
    void GetBrushSize(int& xs, int& ys);
    /// Add character brushes definitions
    void AddBrushDefinition(const char * startChar, const char * endChar, vector2 uvPos, vector2 uvSize, const char * imageFile);
protected:

    /// Struct for character definition set
    struct CharacterDefinitionSet
    {
        char startChar;
        char endChar;
        vector2 uvPos;
        vector2 uvSize;
        nString imageFile;
        nArray<nGuiBrush> characterBrushes;
    };

    /// render the label text
    virtual void RenderCharacters();

    nString text;               // the displayed text 
    Alignment align;

    /// fake typeface name
    nString typeFaceName;

    /// brush size in pixels
    int brushSizeX;
    int brushSizeY;

    // Character separation
    float charSeparation;

    // brushes definitions
    nArray<CharacterDefinitionSet> brushesDefinitions;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiBrushLabel::SetCharSeparation(float sep)
{
    this->charSeparation = sep;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiBrushLabel::GetCharSeparation() const
{
    return this->charSeparation;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiBrushLabel::SetInt(int i)
{
    char buf[128];
    sprintf(buf, "%d", i);
    this->SetText(buf);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiBrushLabel::GetInt() const
{
    return atoi(this->GetText());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiBrushLabel::SetAlignment(Alignment a)
{
    this->align = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGuiBrushLabel::Alignment
nGuiBrushLabel::GetAlignment() const
{
    return this->align;
}

//------------------------------------------------------------------------------
#endif

