#ifndef N_GUIBAR_H
#define N_GUIBAR_H
//-----------------------------------------------------------------------------
/**
    @class nGuiBar
    @ingroup Gui

    @brief Fillable GUI bars. May be filled from left to right or vice versa. Examples
    are Healthbars, Progressbars, etc.

    (C) 2005 Radon Labs GmbH
*/
#include "gui/nguiwidget.h"
#include "kernel/ncmdprotonativecpp.h"

//-----------------------------------------------------------------------------
class nGuiBar : public nGuiWidget
{
public:
    /// constructor
    nGuiBar();
    /// destructor
    virtual ~nGuiBar();

    enum FillMode
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop,
    };

    /// Set fillmode to `mode'. Default is `LeftToRight'.
    void SetFillMode(FillMode mode);
    /// Fillmode (either LeftToRight or RightToLeft).
    FillMode GetFillMode() const;

    /// Set fillstate in percent to `v' (values [0.0 .. 1.0]).
    void SetFillState(float);
    /// Fillstate in percent (values [0.0 .. 1.0]).
    float GetFillState() const;

    /// Segment fillstate into `v' parts. For v == 0, fillstate is continuous.
    void SetFillStateSegments(int v);
    /// Number of parts fillstate is segmented.
    int GetFillStateSegments() const;
    /// Is fillstate segmented rather than continuous?
    bool IsFillStateSegmented() const;

    /// Rendering.
    virtual bool Render();

private:
    /// Fill state if segmented.
    float SegmentedFillState(float v) const;

    // Data
    //FillMode fillMode;
    nGuiBar::FillMode fillMode;
    float fillState;
    int fillStateSegments;

};

//-----------------------------------------------------------------------------
#endif

