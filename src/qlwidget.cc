#include "qllogger.h"
#include "qlwidget.h"


#include <stdio.h>

QLWidget::QLWidget()
{
    qllogger.logT("QLWidget creation: [%x]", this);
}

QLWidget::~QLWidget()
{
}


/**
 * Function hides widget
 */
void QLWidget::Hide()
{
    qllogger.logT("Hide widget: [%x]", this);
 //   printf("%x\n", this);
    XSelectInput(_display, _window, 0);
    XUnmapWindow(_display, _window);
}

/**
 * Function shows widget on screen
 */
void QLWidget::Show()
{
    qllogger.logT("Show widget: [%x]", this);

    XSelectInput(_display, _window, _event_mask);

    XMapWindow(_display, _window);
}


Window QLWidget::GetWindow() const
{
    return _window;
}
