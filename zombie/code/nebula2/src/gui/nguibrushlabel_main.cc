#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguibrushlabel_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguibrushlabel.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiBrushLabel, "nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiBrushLabel::nGuiBrushLabel() :
    align(Center),
    brushSizeX(0),
    brushSizeY(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiBrushLabel::~nGuiBrushLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiBrushLabel::SetText(const char* text)
{
    n_assert(text);
    this->text = text;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiBrushLabel::GetText() const
{
    return this->text.Get();
}

//------------------------------------------------------------------------------
/**
*/
vector2
nGuiBrushLabel::GetTextExtent()
{
    n_assert( this->brushSizeX > 0 && this->brushSizeY > 0 );
    vector2 brushScreenSize( float(this->brushSizeX / nGfxServer2::Instance()->GetDisplayMode().GetWidth()),
                             float(this->brushSizeY / nGfxServer2::Instance()->GetDisplayMode().GetHeight()) );
    float aspect = brushScreenSize.x / brushScreenSize.y;
    brushScreenSize.y = this->rect.height();
    brushScreenSize.x = brushScreenSize.y * aspect;

    brushScreenSize.x *= this->text.Length();
    return brushScreenSize;
}

//------------------------------------------------------------------------------
/**
    @brief Render the character brushes
*/
void
nGuiBrushLabel::RenderCharacters()
{
    if (this->text.IsEmpty())
    {
        // no text, nothing to render
        return;
    }

    vector4 drawColor;
    if (this->blinking)
    {
        nTime time = nGuiServer::Instance()->GetTime();
        if (fmod(time, this->blinkRate) > this->blinkRate/3.0)
        {
            drawColor = this->blinkColor;
        }
        else
        {
            drawColor = this->color;
        }
    }
    else
    {
        drawColor = this->color;
    }

    n_assert( this->brushSizeX > 0 && this->brushSizeY > 0 );

    rectangle screenSpaceRect = this->GetScreenSpaceRect();

    vector2 brushScreenSize( float(this->brushSizeX / nGfxServer2::Instance()->GetViewport().width),
                             float(this->brushSizeY / nGfxServer2::Instance()->GetViewport().height) );
    float aspect = brushScreenSize.x / brushScreenSize.y;
    brushScreenSize.y = screenSpaceRect.height();
    brushScreenSize.x = brushScreenSize.y * aspect;

    rectangle brushRect( screenSpaceRect.v0, vector2( screenSpaceRect.v0 + brushScreenSize ) );
    for ( int charIndex = 0; charIndex < this->text.Length(); charIndex++ )
    {
        char character = this->text[ charIndex ];
        for ( int charDef = 0; charDef < this->brushesDefinitions.Size(); charDef ++ )
        {
            CharacterDefinitionSet & charDefInfo = brushesDefinitions[ charDef ];
            if ( charDefInfo.startChar <= character && charDefInfo.endChar >= character )
            {
                if ( this->colorSet )
                {
                    nGuiServer::Instance()->DrawBrush( brushRect, charDefInfo.characterBrushes[ character - charDefInfo.startChar ], drawColor );
                }
                else
                {
                    nGuiServer::Instance()->DrawBrush( brushRect, charDefInfo.characterBrushes[ character - charDefInfo.startChar ] );
                }
                brushRect.v0.x += brushScreenSize.x * ( 1.0f + this->charSeparation);
                brushRect.v1.x += brushScreenSize.x * ( 1.0f + this->charSeparation);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nGuiBrushLabel::Render()
{
    if (this->IsShown())
    {
        // render the background image, if defined
        nGuiLabel::Render();

        // render the text
        this->RenderCharacters();
        return true;
    }
    return false;
}


//------------------------------------------------------------------------------
/**
    @brief set size of character brush in pixels
*/
void
nGuiBrushLabel::SetBrushSize(int xs, int ys)
{
    this->brushSizeX = xs;
    this->brushSizeY = ys;
}

//------------------------------------------------------------------------------
/**
    @brief get size of character brush in pixels
*/
void
nGuiBrushLabel::GetBrushSize(int & xs, int & ys)
{
    xs = this->brushSizeX;
    ys = this->brushSizeY;
}

//------------------------------------------------------------------------------
/**
    @brief Add character brushes definitions
*/
void
nGuiBrushLabel::AddBrushDefinition(const char * startChar, const char * endChar, vector2 uvPos, vector2 uvSize, const char * imageFile)
{
    n_assert( brushSizeX > 0 && brushSizeY );
    n_assert( startChar && endChar && *startChar <= *endChar );
    n_assert( imageFile && * imageFile );

    CharacterDefinitionSet & charDef = brushesDefinitions.At( brushesDefinitions.Size() );
    charDef.startChar = *startChar;
    charDef.endChar = *endChar;
    charDef.uvPos = uvPos;
    charDef.uvSize = uvSize;
    charDef.imageFile = imageFile;
    int numChars = int(charDef.endChar) - int(charDef.startChar) + 1;
    charDef.characterBrushes.SetFixedSize( numChars );

    nGuiSkin* sysSkin = nGuiServer::Instance()->GetSkin();

    char currentChar = charDef.startChar;
    vector2 pos = charDef.uvPos;
    for ( int i = 0; i < numChars; i++ )
    {
        nString brushName = "( )";
        brushName[1] = currentChar;
        brushName = this->typeFaceName + "_" + currentChar + brushName;

        if ( !sysSkin->FindBrush( brushName.Get() ) )
        {
            sysSkin->AddBrush( brushName.Get(), charDef.imageFile.Get(), pos, charDef.uvSize, vector4(1.0f,1.0f,1.0f,1.0f) );
        }
        nGuiBrush & brush = charDef.characterBrushes[ i ];
        brush.SetName( brushName.Get() );
        pos.x += charDef.uvSize.x;
        currentChar ++;
    }

    sysSkin->EndBrushes();
}

//------------------------------------------------------------------------------
/**
    @brief set fake type face name
*/
void
nGuiBrushLabel::SetTypeFaceName(const char * name)
{
    this->typeFaceName = name;
}

//------------------------------------------------------------------------------
/**
    @brief Get fake type face name
*/
const char *
nGuiBrushLabel::GetTypeFaceName()
{
    return this->typeFaceName.Get();
}
