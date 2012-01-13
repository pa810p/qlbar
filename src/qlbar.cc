#include <stdio.h>
#include <X11/extensions/shape.h>
#include <sys/time.h>

#include "constants.h"
#include "qllogger.h"
#include "qlbar.h"

extern QLLogger qllogger;

/**
 * ctor creates bar and fill members with default values
 */
QLBar::QLBar ()
{
    qllogger.logT("QLBar creation: [%x]", this);
    shouldHideBar = false;
    firstClick = false;

    timeEventUsec = 0;
    timeEventSec = 1;
}

/**
 * dtor frees imlib images
 */
QLBar::~QLBar()
{

}

/**
 * Function prepares display for main window and common items windows.
 * @return int 0 on success, -1 on failure
 */
int QLBar::Prepare()
{

    qllogger.logI("Preparing display");
	XColor fg;

	display = XOpenDisplay(getenv("DISPLAY"));

	if (!display) {
        qllogger.logE("Could not open display. Abort!");
		return -1;
	}

	_defScreen = DefaultScreen(display);
	visual = DefaultVisual(display, _defScreen);
	if (!visual) {
        qllogger.logE("Could not open default visual. Abort!");
		return -1;
    }

	root = RootWindow(display, _defScreen);

	colormap = DefaultColormap(display, _defScreen);

	XColor dummy;

    XAllocNamedColor(display, colormap, "#ffffff", &fg, &dummy);

	gv.foreground = fg.pixel;
    gv.line_width = 1;
    gv.line_style = LineSolid;
	gv.function = GXcopy;

    gc = XCreateGC(display, root, GCFunction|GCForeground, &gv);
	
	//TODO: create separate structure/class for coords below
	ComputeXY(_bar_x, _bar_y, _icon_start_x, _icon_start_y, _tooltipv_x, _tooltipv_y);


	XSetWindowAttributes attrib;
	attrib.override_redirect = True;
	attrib.background_pixel = _pcfg->GetBarColor();

	window = XCreateWindow(display, DefaultRootWindow(display), 
		_bar_x, _bar_y, getBarWidth(), getBarHeight(), 0, CopyFromParent, 
		InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attrib);

    XSelectInput(display, window,
             EnterWindowMask|LeaveWindowMask|ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask);
    XMapWindow(display, window);

	XSync(display, False);

	imlib_context_set_display(display);
	imlib_context_set_visual(visual);
	imlib_context_set_colormap(colormap);
	imlib_context_set_drawable(window);
	imlib_context_set_dither(1);
	imlib_context_set_blend(1);

	// fonts
	prepareFont();
	prepareItems();	

	return 0;
}


/**
 * Function initializes font for graphics usage on imlib objects
 */
int QLBar::prepareFont()
{
	imlib_add_path_to_font_path( _pcfg->GetFontDir() );
	char font [MAX_FONT_NAME];
	snprintf(font,MAX_FONT_NAME,"%s/%d", _pcfg->GetFontName(), _pcfg->GetFontSize());

    qllogger.logI("Preparing font: %s", font);
	_font = imlib_load_font( font );

    if (NULL == _font) {
        qllogger.logE("Could not find font in: %s", _pcfg->GetFontDir());
        qllogger.logI("Disabling  balloon display");
        _pcfg->SetShowBalloon(false);
    }

	return 0;
}

/**
 * Function returns width of a bar
 * @return int width of a bar in pixels
 */
int QLBar::getBarWidth() const
{
    int barWidth = -1;

	if (_pcfg->GetBarWidth() == -1) {
        barWidth = (_pcfg->GetLayout() == BAR_HORIZONTAL) ?
			items.size() * _pcfg->GetIconWidth() : 
			1;
        qllogger.logT("Counted bar width: %d on layout: %d", barWidth, _pcfg->GetLayout());
    }
	else {
		barWidth =  _pcfg->GetBarWidth();
        qllogger.logT("Returning bar width from config: %d", barWidth);
    }

    return barWidth;
}

/**
 * Function returns height of a qlbar
 * @return int height of a bar in pixels
 */
int QLBar::getBarHeight() const
{
    int barHeight = -1;

	if (_pcfg->GetBarHeight() == -1) {
		barHeight = (_pcfg->GetLayout() == BAR_HORIZONTAL) ? 
			items.size() * _pcfg->GetIconHeight() : 
			_pcfg->GetIconWidth();
        qllogger.logT("Counted bar height: %d on layout: %d", barHeight, _pcfg->GetLayout());
    }
	else {
		barHeight = _pcfg->GetBarHeight();
        qllogger.logT("Returning bar height from config: %d", barHeight);
    }
    
    return barHeight;
}


/**
 * Function prepare UI data for every item on a list
 */
void QLBar::prepareItems()
{

    qllogger.logT("Preparing items...");

	int icon_x = _icon_start_x;
	int icon_y = _icon_start_y;
	vector<QLItem *>::iterator it = items.begin();
	for ( ; it != items.end(); it++){
        qllogger.logT("(prepareItems) Will create UI on [%x]", (*it));
		(*it)->CreateUI(display, window, icon_x, icon_y, _pcfg->GetIconWidth(), _pcfg->GetIconHeight());

		if (_pcfg->GetShowBalloon()){
			// creates UI for tooltip (initialize position)
			Balloon bal;
			bal.font = _font;
			bal.fg_color = _pcfg->GetFontColor();
			bal.bg_color = _pcfg->GetBalloonColor();
			bal.position = _pcfg->GetPosition();
			bal.layout = _pcfg->GetLayout();
	
			(*it)->CreateBalloonUI(icon_x + _tooltipv_x, icon_y + _tooltipv_y, bal);
		}

		switch(_pcfg->GetLayout()){
			case BAR_HORIZONTAL:
				icon_x += _pcfg->GetIconWidth();
				break;
			case BAR_VERTICAL:
				icon_y += _pcfg->GetIconHeight();
				break;
		}
	}
}


/**
 * This method handles event that has come from xlib layer
 *
 * @param ev event to handle
 * @param tv time to next event to be generate
 * @see select (2) manual for tv description
 */
void QLBar::handleEvent(const XEvent & ev) {
	qllogger.logT("handleEvent");

	switch (ev.type){
		case Expose:
		{
			qllogger.logT("Event: expose");
			// obtain displayed item, deeply - also tooltips if enabled
			QLWidget * item = findWidget(ev.xexpose.window, true);
       
               //lock();

			if (item != NULL)
				item->Paint();

			//XClearWindow(display, window);

            firstClick = false;
			break;
		}
		case LeaveNotify:
		{
                shouldHideBar = true;

        		qllogger.logT("Event: leave");
				QLWidget * item = findWidget(ev.xcrossing.window); // obtain leaving widget
				if (item != NULL){
//                    static_cast<QLItem*>(item)->UnSelect();
                    qllogger.logT("hide balloon() [%s]", ((QLItem*)item)->GetName() );
					static_cast<QLItem*>(item)->HideBalloon();
                }
                else {
	   		    hideAllBalloons();
                    }
            firstClick = false;
            break;
        }
		case EnterNotify:

            shouldHideBar = false;
		    if (ev.xcrossing.window == window) 
			{ // enter to the bar line
			    qllogger.logT("Event: enter to the line bar");
				hideAllBalloons();

				//TODO: show animated
//				//ScaleItems(_icon_height);
				showAllItems();
			}
			else //enter to the icon
			{
                qllogger.logT("Event: enter to the icon");

		    	QLWidget * item = findWidget(ev.xcrossing.window);
				if (item != NULL){
			        qllogger.logT("Found widget [%s]", static_cast<QLItem*>(item)->GetName());
					//static_cast<QLItem*>(item)->ShowBalloon(v.xcrossing.x + 30,ev.xcrossing.y + 30);
//					static_cast<QLItem*>(item)->Select();
                    static_cast<QLItem*>(item)->Paint();
                    static_cast<QLItem*>(item)->ShowBalloon(30,30);
                }
                else {
                    qllogger.logW("Widget not found.");
                }
		    }
            qllogger.logT("End of EnterNotify event");
            
            firstClick = false;
			break;
		case ButtonPress:
		{
            qllogger.logD("Event: buttonPress: %ld", ev.xbutton.time);
            if (_pcfg->getDoubleClick() == true) {
                if (false == firstClick)    {
                    qllogger.logD("Event: buttonPress: firstClick=true"); 
                    firstClick = true;
                    clickTime = ev.xbutton.time;
                }
                else {
                    // counting difference between clicks
                    long diff = ev.xbutton.time - clickTime;
                    qllogger.logD("Event: buttonPress: diff=%ld (%ld - %ld)", diff, ev.xbutton.time, clickTime);
                    if (diff < _pcfg->getDoubleClickInterval()) {
                        processClick(ev);
                    }
                    firstClick = false;
                }
            }
            else {
                processClick(ev);
            } 
	    }
		break;
        case ButtonRelease:
            qllogger.logD("Event: ButtonRelease");
            break;
		default:
            qllogger.logT("Unrecognized event: %d", ev.type);
            firstClick = false;
		    break;
		}
}


/**
 * This method is used to execute application under clicked icon.
 *
 * @param ev reference to an event that occured
 */
void QLBar::processClick( const XEvent &ev ) {

    QLWidget * item = findWidget(ev.xcrossing.window);

    if (NULL != item) {
        qllogger.logT("Found widget [%s]", static_cast<QLItem*>(item)->GetName());
        static_cast<QLItem*>(item)->Execute();
    }
    else {
        qllogger.logW("Widget not found");
    }
}


/**
 * Function receive events from created windows
 * return bool true on success, false on failure
 */
int QLBar::Run()
{
    int x11_fd;
    fd_set in_fds;

    x11_fd = ConnectionNumber(display);

    XFlush(display);
    int pending =0;
	hideAllItems();

	XEvent ev;

    struct timeval tv;

	while (1) {
    
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        tv.tv_usec = timeEventUsec;
        tv.tv_sec = timeEventSec;

        if (select(x11_fd+1, &in_fds, 0, 0, &tv)) {
        }
        else { // time event occured
            if (true == shouldHideBar) {
                hideAllItems();
                shouldHideBar = false;
            }
            firstClick = false;
        }

        qllogger.logD("Event: %d: %d", tv.tv_sec, tv.tv_usec);

        while(pending=XPending(display)) {
       		XNextEvent(display, &ev);
            handleEvent(ev);
        }

	};
    return 0;
}


/**
 * Function iterates through vector containing qlitems and compares their
 * window property.
 * @param window pattern window
 * @return QLItem pointer to the item found, NULL when not found
 */
QLWidget * QLBar::findWidget(const Window window, const bool deep) const
{
	vector<QLItem *>::const_iterator it = items.begin();

	for (;it != items.end(); it++)
	{
		if ( (*it)->GetWindow() == window)
			return (*it);
		else if (deep == true) {// check balloon	
			QLWidget * pWidget = (*it)->GetBalloon();
			if (pWidget != NULL)
				if( pWidget->GetWindow() == window)
					return pWidget;
		}

	}
	return NULL;
}


/**
 * This function hides all balloons
 */
void QLBar::hideAllBalloons()
{

	vector<QLItem *>::const_iterator it = items.begin();
	for (; it != items.end() ; it++)
    {
        (*it)->HideBalloon();
    }
}


/**
 * Function iterates through vector containing qlitems and hide each of them
 */
void QLBar::hideAllItems() {

	vector<QLItem *>::const_iterator it = items.begin();
	for (; it != items.end() ; it++)
    {
        (*it)->HideBalloon();
		(*it)->Hide();
    }

	XSync(display, False);

}

/**
 * Function iterates through vector containing qlitems and show each of them
 */
void QLBar::showAllItems()
{

	vector<QLItem *>::const_iterator it = items.begin();
	for (; it != items.end() ; it++)
    {
		(*it)->Show();
    }
	XSync(display, False);
}


/**
 * Function appends item onto the end of a list
 * @param pItem pointer to newly created item
 * @return int as size of a vector containing items
 */
int QLBar::AddItem ( QLItem * pItem )
{
	items.push_back(pItem);
	return items.size();
}


/**
 * Function inserts item at a given index
 * @param index integer index of inserting element
 * @param pItem pointer to a newly created item
 * @return int as size of a vector containing items
 */
int QLBar::AddItem ( const int & index, QLItem * pItem )
{
    qllogger.logT("Adding item of index: %d", index);
	int i = 0;
	vector<QLItem *>::iterator it = items.begin();
	for ( ; it != items.end(); it++,i++){
		if (i==index)
			break;
	}

	if (it != items.begin())
		items.insert(it, pItem);
	else 
		items.push_back(pItem);

	return items.size();
}

/**
 * Function computes coordinates of bar.
 * Coordinates depends on settings: position, layout, and screen width and height
 * @param x reference to x where x computed coordinate will be placed
 * @param y reference to y where y copmuted coordinate will be placed
 * @param ic_x reference to x position of first icon on bar
 * @param ic_y reference to y position of first icon on bar
 * @param ttv_x reference to tool tip vector x coordinate
 * @param ttv_y reference to tool tip vector y coordinate
 * @todo implement ttv
 */
void QLBar::ComputeXY(int & x, int & y, int & ic_x, int & ic_y, int & ttv_x, int & ttv_y) 
{
	int bw = this->getBarWidth(); // this depended, not config
	int bh = this->getBarHeight();
	int dw = DisplayWidth(display, _defScreen);
	int dh = DisplayHeight(display, _defScreen);
	int ih = _pcfg->GetIconHeight();
	int iw = _pcfg->GetIconWidth();
	int layout = _pcfg->GetLayout();
	int ho = _pcfg->GetHOffset();
	int vo = _pcfg->GetVOffset();

	switch(_pcfg->GetPosition()){
		case BP_NORTH:
			if (layout == BAR_HORIZONTAL){
				y = ic_y = 0;
				x = ic_x = dw / 2 - bw / 2;
				ttv_x = 0;
				ttv_y = (int)(1.5 * ih);
			}
			else {
				// not available
			}
			break;
		case BP_SOUTH:
			if (layout == BAR_HORIZONTAL){
				y = dh - bw;
				x = ic_x = dw / 2 - bw / 2;
				ic_y = y - ih;
				ttv_x = 0;
				ttv_y =  -ih;
			}
			else {
				// not available
			}
			break;
		case BP_NORTH_EAST:
			y = ic_y = 0;
			x = dw - bw;
			if (layout == BAR_HORIZONTAL){
				x -= ho;
				ic_x = x;
				ttv_x = 0;
				ttv_y = (int)(1.5 * ih);
			}
			else {
				y += vo;
				ic_x = dw - iw;
				ttv_x = -iw;
				ttv_y = 0;
			}
			
			break;
		case BP_EAST:
			if (layout == BAR_VERTICAL){
				x = dw - bw;
				y = ic_y = dh / 2 - bh / 2;
				ic_x = dw - iw;
				ttv_y = 0;
				ttv_x = -iw; // east so, need to sub text length
			}
			else {
				// not available
			}
			break;
		case BP_SOUTH_EAST:
			x = dw - bw; 
			if (layout == BAR_HORIZONTAL){
				x -= ho;
				y = dh - bw;
				ic_x = x;
				ic_y = y - ih;
				ttv_x = 0;
				ttv_y = -ih;
			}
			else{
				y = dh - bh - ho;
				ic_x = dw - iw;
				ic_y = dh - bh;
				ttv_x = -2 * iw; // east so, need to sub text length
				ttv_y = 0;
			}
			break;
		case BP_NORTH_WEST:
			if (layout == BAR_HORIZONTAL){
				y = ic_y = 0;
				x = ic_x = ho;
				ttv_x = 0;
				ttv_y = (int)(1.5 * ih);
			}
			else {
				y = ic_y = vo;
				x = ic_x = 0;
				ttv_x = 0;
				ttv_y = -ih;
			}
			break;
		case BP_WEST:
			if (layout == BAR_VERTICAL){
				x = ic_x = 0;
				y = ic_y = dh / 2 - bh / 2;
				ttv_x = 2 * iw;
				ttv_y = 0;
			}
			else {
				// not available
			}
			break;
		case BP_SOUTH_WEST:
			y = dh;
			if (layout == BAR_HORIZONTAL){
				x = ic_x = ho;	  
				ic_y = y - ih;
				ttv_x = 0;
				ttv_y = -ih;
			}
			else {
				x = ic_x = 0;
				ic_y = dw - bh - vo;
				ttv_x = 2 * iw;
				ttv_y = 0;
			}
			break;
	}

    qllogger.logD("ComputedXY: %d, %d, %d, %d, %d, %d", x, y, ic_x, ic_y, ttv_x, ttv_y);

    if (x < 0 || y < 0) 
        qllogger.logW("Bar start position is out of screen: [%d,%d]", x, y);
    

}


void QLBar::ScaleItems()
{
	vector<QLItem *>::iterator it = items.begin();
	for ( ; it != items.end(); it++){
		(*it)->Scale(_icon_width, _icon_height);
	}
}

int QLBar::GetBarTime() const
{
	return _pcfg->GetBarTime();
}


void QLBar::SetConfig(QLConf * cfg)
{
	if (cfg == NULL) {}
    else {
        _pcfg = cfg;
        // and create valid items lise
        vector<QLItem*> * pV = cfg->GetItems();
        vector<QLItem*>::iterator it = pV->begin();
        for (; it!=pV->end(); it++){
            if (!(*it)->isFailed()){ // destroy unnessesary icons
                AddItem(*it);
            }
        }
    }
}



