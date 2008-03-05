#ifndef N_GUITIMEDMESSAGEWINDOW_H
#define N_GUITIMEDMESSAGEWINDOW_H
//------------------------------------------------------------------------------
/**
    @brief A ngui window that shows timed messages that dissapear after a short time

    @todo A new method AddMessage(nWidget*) can be added, to have composed messages (layouts)

    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gui/nguiwindow.h"
#include "gui/nguilinelayout.h"
#include "gui/nguitextlabel.h"

//------------------------------------------------------------------------------
class nGuiTimedMessageWindow : public nGuiWindow
{
public:

    /// constructor
    nGuiTimedMessageWindow();
    /// destructor
    virtual ~nGuiTimedMessageWindow();

    /// called when widget is becoming visible
    virtual void OnShow();
    
    /// called when widget is becoming invisible
    virtual void OnHide();

    /// called per frame
    virtual void OnFrame();

    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);

    /// Set path of nguitextlabel that will be the template for the window's messages
    void SetTemplateTextLabel(const char *);
    /// Get path of nguitextlabel that will be the template for the window's messages
    const char * GetTemplateTextLabel();

    /// Set time for messages to dissappear
    void SetMessageFadeoutTime(float);
    /// Get time for messages to dissappear
    float GetMessageFadeoutTime();

    /// Add message line
    void AddMessage(const char *);
    /// Set last message
    bool SetLastMessage(int, const char *, const char *, const char *);
    /// Get last message
    const char * GetLastMessage();

    /// Clear window
    void ClearWindow();

protected:

    // Update window messages and layout
    void UpdateWindow();

    nString templateLabelPath;
    nRef<nGuiTextLabel> refTemplateTextLabel;
    nRef<nGuiLineLayout> refLayout;

    // Max time for messages
    nTime messageFadeOutTime;

    // Times of messages
    nArray<nTime> messageTimes;

    // Counter for assigning label's names
    int childId;

private:
};
//------------------------------------------------------------------------------
#endif

