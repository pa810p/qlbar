#include "qlwidget.h"

#include <stdio.h>

QLWidget::QLWidget()
{
}

QLWidget::~QLWidget()
{
}


/**
 * Function hides widget
 */
void QLWidget::Hide()
{
 //   printf("%x\n", this);
    XSelectInput(_display, _window, 0);
    XUnmapWindow(_display, _window);
}

/**
 * Function shows widget on screen
 */
void QLWidget::Show()
{

    XSelectInput(_display, _window, _event_mask);

   XMapWindow(_display, _window);
}


Window QLWidget::GetWindow() const
{
    return _window;
}
