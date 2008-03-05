#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nGuiTimedMessageWindow_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguitimedmessagewindow.h"
#include "gui/nguiserver.h"
#include "gui/nguifadeouttextlabel.h"

nNebulaScriptClass(nGuiTimedMessageWindow, "nguiwindow");

//------------------------------------------------------------------------------
/**
*/
nGuiTimedMessageWindow::nGuiTimedMessageWindow() :
    messageFadeOutTime( 5.0f ),
    childId(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTimedMessageWindow::~nGuiTimedMessageWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTimedMessageWindow::OnShow()
{
    if ( ! this->refLayout.isvalid() )
    {
        nString layoutName( this->GetFullName() );
        layoutName.Append( "/Layout" );
        nKernelServer::Instance()->PushCwd(this);
        nGuiLineLayout * layout = static_cast<nGuiLineLayout*>( nKernelServer::Instance()->New( "nguilinelayout", layoutName.Get() ) );
        n_assert( layout );
        nKernelServer::Instance()->PopCwd();
        layout->SetOrientation( nGuiLineLayout::ORI_VERTICAL );
        this->refLayout = layout;
    }

    this->ClearWindow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTimedMessageWindow::OnHide()
{
    this->ClearWindow();
    this->refLayout->Release();
}

//------------------------------------------------------------------------------
/**
    @brief Set path of nguitextlabel that will be the template for the window's messages
*/
void
nGuiTimedMessageWindow::SetTemplateTextLabel(const char *path)
{
    n_assert( path );
    this->templateLabelPath = path;
    refTemplateTextLabel = static_cast<nGuiTextLabel *>( nKernelServer::Instance()->Lookup(path) );
    n_assert( this->refTemplateTextLabel.isvalid() );
    n_assert( this->refTemplateTextLabel.get()->IsA("nguitextlabel") );
}

//------------------------------------------------------------------------------
/**
    @brief Get path of nguitextlabel that will be the template for the window's messages
*/
const char *
nGuiTimedMessageWindow::GetTemplateTextLabel()
{
    if ( this->refTemplateTextLabel.isvalid() )
    {
        return this->templateLabelPath.Get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set time for messages to dissappear
*/
void
nGuiTimedMessageWindow::SetMessageFadeoutTime(float time)
{
    this->messageFadeOutTime = (nTime)time;
}

//------------------------------------------------------------------------------
/**
    @brief Get time for messages to dissappear
*/
float
nGuiTimedMessageWindow::GetMessageFadeoutTime()
{
    return (float)this->messageFadeOutTime;
}

//------------------------------------------------------------------------------
/**
    @brief Add message line
*/
void
nGuiTimedMessageWindow::AddMessage(const char *message)
{
    n_assert( this->refLayout.isvalid() && this->refTemplateTextLabel.isvalid() );

    nString newLabelClassName = this->refTemplateTextLabel->GetClass()->GetProperName();
    newLabelClassName.ToLower();

    nString newLabelName( this->refLayout.get()->GetFullName() );
    newLabelName.Append( "/Label" );
    newLabelName.AppendInt( this->childId ++ );

    nGuiTextLabel * newLabel = static_cast<nGuiTextLabel *>( nKernelServer::Instance()->New( newLabelClassName.Get(), newLabelName.Get() ) );
    n_assert( newLabel );

    newLabel->SetFont( this->refTemplateTextLabel->GetFont() );
    newLabel->SetAlignment( this->refTemplateTextLabel->GetAlignment() );
    newLabel->SetVCenter( this->refTemplateTextLabel->GetVCenter() );
    newLabel->SetWordBreak( this->refTemplateTextLabel->GetWordBreak() );
    newLabel->SetClipping( this->refTemplateTextLabel->GetClipping() );
    newLabel->SetColor( this->refTemplateTextLabel->GetColor() );
    newLabel->SetPressedOffset( this->refTemplateTextLabel->GetPressedOffset() );
    newLabel->SetBorder( this->refTemplateTextLabel->GetBorder() );
    newLabel->SetBlinkingColor( this->refTemplateTextLabel->GetBlinkingColor() );
    newLabel->SetText( message );
    vector2 textSize = newLabel->GetTextExtent();
    newLabel->SetMinSize( textSize );
    newLabel->SetMaxSize( textSize );

    if ( strcmp( newLabel->GetClass()->GetProperName(), "nguifadeouttextlabel") == 0 )
    {
        static_cast<nGuiFadeOutTextLabel*>( newLabel )->SetTimeOut( this->messageFadeOutTime );
    }

    this->UpdateWindow();
}

//------------------------------------------------------------------------------
/**
    @brief Set text of last message or add one if there is no last message
    @param prefixPos Begin position of prefix
    @param prefix The last message is checked to begin with this string
    @param postfix The last message is checked to contain this string
    @param message The message
    @return true if the last message was substituted, false otherwise
    
    If the last message doesn't match with the prefix or postfix or there is no last message,
    a new one is created.
*/
bool
nGuiTimedMessageWindow::SetLastMessage(int prefixPos, const char *prefix, const char *postfix, const char *message)
{
    n_assert( this->refLayout.isvalid() && this->refTemplateTextLabel.isvalid() && strlen(message) > size_t(prefixPos) );

    nGuiWidget* lastWidget = (nGuiWidget*) this->refLayout->GetTail();
    nGuiTextLabel * newLabel = 0;
    if ( lastWidget && lastWidget->IsA("nguitextlabel") )
    {
        newLabel = static_cast<nGuiTextLabel *>( lastWidget );
    }

    if ( newLabel && strlen( newLabel->GetText() ) > size_t(prefixPos) &&
                     strncmp( newLabel->GetText() + prefixPos, prefix, strlen( prefix ) ) == 0 &&
                     strstr( newLabel->GetText(), postfix ) != 0 )
    {
        newLabel->SetFont( this->refTemplateTextLabel->GetFont() );
        newLabel->SetAlignment( this->refTemplateTextLabel->GetAlignment() );
        newLabel->SetVCenter( this->refTemplateTextLabel->GetVCenter() );
        newLabel->SetWordBreak( this->refTemplateTextLabel->GetWordBreak() );
        newLabel->SetClipping( this->refTemplateTextLabel->GetClipping() );
        newLabel->SetColor( this->refTemplateTextLabel->GetColor() );
        newLabel->SetPressedOffset( this->refTemplateTextLabel->GetPressedOffset() );
        newLabel->SetBorder( this->refTemplateTextLabel->GetBorder() );
        newLabel->SetBlinkingColor( this->refTemplateTextLabel->GetBlinkingColor() );

        newLabel->SetText( message );
        vector2 textSize = newLabel->GetTextExtent();
        newLabel->SetMinSize( textSize );
        newLabel->SetMaxSize( textSize );
        this->UpdateWindow();
        if ( this->messageTimes.Size() > 0 )
        {
            this->messageTimes[ this->messageTimes.Size() - 1] += this->messageFadeOutTime;
        }

        return true;
    }
    else
    {
        this->AddMessage( message );
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Get the text of the last message
    @return The text or null if there is no last message

*/
const char *
nGuiTimedMessageWindow::GetLastMessage()
{
    n_assert( this->refLayout.isvalid() && this->refTemplateTextLabel.isvalid() );

    nGuiWidget* lastWidget = (nGuiWidget*) this->refLayout->GetTail();
    nGuiTextLabel * newLabel = 0;
    if ( lastWidget && lastWidget->IsA("nguitextlabel") )
    {
        newLabel = static_cast<nGuiTextLabel *>( lastWidget );
    }
    if ( newLabel )
    {
        return newLabel->GetText();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Clear window
*/
void
nGuiTimedMessageWindow::ClearWindow()
{
    nGuiWidget* curWidget = (nGuiWidget*) this->refLayout.get()->GetHead();
    while ( curWidget )
    {
        curWidget->Remove();
        curWidget->Release();
        curWidget = (nGuiWidget*) this->refLayout.get()->GetHead();
    }
    this->messageTimes.Clear();
}

//------------------------------------------------------------------------------
/**
    @brief Update window messages and layout
*/
void
nGuiTimedMessageWindow::UpdateWindow()
{

    // If no layout, the window is not shown
    if ( ! this->refLayout.isvalid() )
    {
        return;
    }

    bool layoutChanged = false;

    // Remove timed out messages
    nTime curTime = nTimeServer::Instance()->GetTime();
    int widgetIndex = 0;
    nGuiWidget* curWidget = (nGuiWidget*) this->refLayout->GetHead();
    while ( curWidget && widgetIndex < this->messageTimes.Size() )
    {
        if ( this->messageTimes[ widgetIndex ] <= curTime )
        {
            nGuiWidget* oldWidget = curWidget;
            curWidget = (nGuiWidget*) curWidget->GetSucc();
            oldWidget->Release();
            this->messageTimes.EraseQuick( widgetIndex );
            layoutChanged = true;
        }
        else
        {
            curWidget = (nGuiWidget*) curWidget->GetSucc();
            widgetIndex++;
        }
    }

    // Set time for new messages
    widgetIndex = 0;
    curWidget = (nGuiWidget*) this->refLayout->GetHead();
    while ( curWidget && widgetIndex < this->messageTimes.Size() )
    {
            // Jump not new messages
            curWidget = (nGuiWidget*) curWidget->GetSucc();
            widgetIndex++;
    }
    while ( curWidget )
    {
        // These are new messages, add its time stamp
        this->messageTimes.Append( curTime + this->messageFadeOutTime);
        curWidget = (nGuiWidget*) curWidget->GetSucc();
        layoutChanged = true;
    }

    if ( layoutChanged )
    {
        this->SetRect(this->GetRect());
    }

    // Remove messages until newest message is entirely visible
    const rectangle &layoutRect = this->refLayout->GetRect();
    bool changed = true;
    while ( changed )
    {
        changed = false;
        nGuiWidget* lastWidget = (nGuiWidget*) this->refLayout->GetTail();
        nGuiWidget* firstWidget = (nGuiWidget*) this->refLayout->GetHead();
        if ( lastWidget && firstWidget != lastWidget )
        {
            const rectangle &widgetRect = lastWidget->GetRect();
            if ( widgetRect.v1.y > layoutRect.height() )
            {
                firstWidget->Release();
                this->messageTimes.EraseQuick( 0 );
                changed = true;
                // Update layout
                this->SetRect(this->GetRect());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called per frame. Removes timed out messages and updates layout
*/
void
nGuiTimedMessageWindow::OnFrame()
{
    this->UpdateWindow();
}

//-----------------------------------------------------------------------------
/**
   Set screen space rectangle.
*/
void
nGuiTimedMessageWindow::OnRectChange(const rectangle& newRect)
{
    nGuiWidget::OnRectChange(newRect);

    // Update layout
//    this->refLayout->SetRect( this->refLayout->GetRect() );
    this->refLayout->SetRect( rectangle( vector2(0.0f,0.0f), vector2( this->GetRect().width(), this->GetRect().height()) ) );
}