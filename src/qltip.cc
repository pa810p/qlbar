#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <stdio.h>

#include "constants.h"
#include "qltip.h"


/**
 * Constructor
 * @param bal Balloon parameters structure
 */
QLTip::QLTip (Balloon bal)
{
    _bal = bal;
    _width = 60;
    _height = 20;
    _visible = false;
}


/**
 * Destructor
 */
QLTip::~QLTip () 
{
    imlib_context_set_image(im);
    imlib_free_image();
}


/**
 * Function creates UI for balloon qltip.
 * @param display
 * @param window
 * @param x
 * @param y
 * @param w - ignored
 * @param h - ignored
 */
bool QLTip::CreateUI(Display * display, Window window, const int &x, const int &y, const int & w, const int &h)
{
    int r,g,b;

    _x = x;
    _y = y;

    _display = display;
    XSetWindowAttributes attrib;
    attrib.override_redirect = True;
    attrib.background_pixel = 0xffff00;

    _window = XCreateWindow(display, DefaultRootWindow(display), x, y, _width, _height, 0, CopyFromParent,
        InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attrib);

    _event_mask = ExposureMask;


    // create imlib data
    im = imlib_create_image(_width, _height);
    if (!im)
        return false;

    imlib_context_set_drawable(_window);
    imlib_context_set_image(im);

    // get width and height of given string
    imlib_context_set_font(_bal.font);
    imlib_get_text_size(_bal.caption, &_width, &_height); // no image needed
    _width+=6;
    _height+=4;

    // if qlbar is on right side - sub it's width from left-top
    // balloon coordinates
    
    if ((_bal.position == BP_EAST || _bal.position == BP_NORTH_EAST ||
        _bal.position == BP_SOUTH_EAST ) && _bal.layout == BAR_VERTICAL)
        _x -= _width;

    XMoveResizeWindow(_display, _window, _x, _y, _width, _height);

// free it and recreate larger
    imlib_free_image();
    im = imlib_create_image(_width, _height);
    imlib_context_set_image(im);

    imlib_render_image_on_drawable_at_size(0,0,_width,_height);
// 

    long2RGB(_bal.bg_color,r,g,b);
    imlib_context_set_color(r,g,b,255);
    imlib_image_fill_rectangle(1,1,_width-2,_height-2);

    imlib_context_set_color(0,0,0,255);
    imlib_image_draw_rectangle(0,0,_width,_height);

    if (!_bal.font)
        return false;

    long2RGB(_bal.fg_color,r,g,b);
    
    imlib_context_set_font(_bal.font);
    imlib_context_set_color(r,g,b,255);

    imlib_text_draw(3,2,_bal.caption);
    return true;
}


/**
 * Function paints image on the screen
 */
void QLTip::Paint()
{
    if (_visible == false)
        return;

    //DBGOUT printf("QLTip paint : %d, %d, %d, %d\n", _x, _y, _width, _height);
    DBGOUT printf("QLTip paint [%s]\n", _bal.caption);

    imlib_context_set_drawable(_window);
    imlib_context_set_image(im);
    imlib_render_image_on_drawable(0,0);
    XRaiseWindow(_display, _window);

    Show();

}


/**
 * Function shows balloon on the screen in given coordinates. Currently params are 
 * ignored and balloon is shown on coordinates that it was created. I'm not sure
 * whether implement balloons that move with mouse pointer.
 * @param x - ignored in 0.1.2 version
 * @param y - ignored in 0.1.2 version
 * @todo think about parameters
 */
void QLTip::ShowBalloon( const int & x, const int & y)
{
    _visible = true;
    this->Paint();
}


void QLTip::HideBalloon()
{
    _visible = false;
    QLWidget::Hide();
}

