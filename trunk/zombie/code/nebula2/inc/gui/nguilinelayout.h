#ifndef N_GUILINELAYOUT_H
#define N_GUILINELAYOUT_H
//------------------------------------------------------------------------------
/**
    @brief A line layout aligns child widgets in a vertical or horizontal line

    (C) 2006 Conjurer Services, S.A.
*/
#include "gui/nguiformlayout.h"

//------------------------------------------------------------------------------
class nGuiLineLayout : public nGuiFormLayout
{
public:

    enum Orientation {
        ORI_HORIZONTAL = 0,
        ORI_VERTICAL,
    };

    enum Order {
        ORDER_NORMAL = 0,
        ORDER_REVERSED = 0,
    };

    /// constructor
    nGuiLineLayout();
    /// destructor
    virtual ~nGuiLineLayout();
    /// Set horizontal or vertical orientation of layout
    void SetOrientation( Orientation );
    /// Set normal or reversed order of child widgets
    void SetOrder( Order );
    /// Set aligned flag
    void SetAligned( bool );
    /// Get aligned flag
    bool GetAligned();
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget position or size changes
    virtual void OnRectChange(const rectangle& newRect);

protected:

    void UpdateLayout(const rectangle& newRect);

    /// horizontal or vertical orientation of layout
    Orientation orientation;
    /// order of child widgets (normal or reversed)
    Order order;
    /// alignment flag
    bool aligned;

private:
};
//------------------------------------------------------------------------------
#endif

