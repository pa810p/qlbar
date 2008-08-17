#include <stdio.h>
#include <X11/extensions/shape.h>
#include <pthread.h>

#include "constants.h"
#include "qlbar.h"

/**
 * ctor creates bar and fill members with default values
 */
QLBar::QLBar ()
{
	active = false;
	if (pthread_create(&hidding_thread, NULL, hideBar, this) != 0)
		fprintf(stderr, "Could not create thread\n");
    pthread_detach(hidding_thread);

	if (pthread_mutex_init(&graph_mutex, NULL) != 0)
		fprintf(stderr, "Could not init mutex");

}

/**
 * dtor frees imlib images
 */
QLBar::~QLBar()
{
	pthread_mutex_destroy(&graph_mutex);
}

/**
 * Function prepares display for main window and common items windows.
 * @return int 0 on success, -1 on failure
 */
int QLBar::Prepare()
{

	XColor fg;

	display = XOpenDisplay(getenv("DISPLAY"));

	if (!display) {	
		fprintf(stderr, "Could not open display");
		return -1;
	}

	_defScreen = DefaultScreen(display);
	visual = DefaultVisual(display, _defScreen);
	if (!visual)
		return -1;

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
	_font = imlib_load_font( font );

	return 0;
}

/**
 * Function returns width of a bar
 * @return int width of a bar in pixels
 */
int QLBar::getBarWidth() const
{

	if (_pcfg->GetBarWidth() == -1)
		return (_pcfg->GetLayout() == BAR_HORIZONTAL) ?
			items.size() * _pcfg->GetIconWidth() : 
			1;
	else
		return _pcfg->GetBarWidth();

}

/**
 * Function returns height of a qlbar
 * @return int height of a bar in pixels
 */
int QLBar::getBarHeight() const
{
	if (_pcfg->GetBarHeight() == -1)
		return (_pcfg->GetLayout() == BAR_HORIZONTAL) ? 
			items.size() * _pcfg->GetIconHeight() : 
			_pcfg->GetIconWidth();

	else
		return _pcfg->GetBarHeight();
}


/**
 * Function prepare UI data for every item on a list
 */
void QLBar::prepareItems()
{

	int icon_x = _icon_start_x;
	int icon_y = _icon_start_y;
	vector<QLItem *>::iterator it = items.begin();
	for ( ; it != items.end(); it++){
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
 * Function receive events from created windows
 * return bool true on success, false on failure
 */
int QLBar::Run()
{
	hideAllItems();

	XEvent ev;
	while (1) {
		XNextEvent(display, &ev);
		//DBGOUT printf("Event: %d\n", ev.type);
		switch (ev.type){
			case Expose:
				{
				DBGOUT printf("expose\n");
				// obtain displayed item, deeply - also tooltips if enabled
				QLWidget * item = findWidget(ev.xexpose.window, true);
	
				if (pthread_mutex_lock(&graph_mutex) != 0){
					DBGOUT fprintf(stderr, "Cannot lock mutex\n");
				}

				if (item != NULL)
					item->Paint();

				//XClearWindow(display, window);

				pthread_mutex_unlock(&graph_mutex);
				break;
				}
			case LeaveNotify:
				{
					DBGOUT printf("leave\n");
					QLWidget * item = findWidget(ev.xcrossing.window); // obtain leaving widget
					
					if (item != NULL){
         //               static_cast<QLItem*>(item)->UnSelect();
                        DBGOUT printf ("hide balloon() [%s]", ((QLItem*)item)->GetName() );
						static_cast<QLItem*>(item)->HideBalloon();
                    }
                    else {
					    hideAllBalloons();
                    }

					active = true;
					pthread_create(&hidding_thread, NULL, hideBar, this);
                    pthread_detach(hidding_thread);
				}
				break;
			case EnterNotify:
				if (ev.xcrossing.window == window) 
				{ // enter to the bar line
				    DBGOUT printf("enter\n");
					hideAllBalloons();
					//TODO: show animated
					pthread_cancel(hidding_thread);
//					ScaleItems(_icon_height);
					showAllItems();
				}
				else //enter to the icon
				{
		//			active = findWidget(ev.xcrossing.window);
					pthread_cancel(hidding_thread);
					//TODO: create balloon for item
					QLWidget * item = findWidget(ev.xcrossing.window);
				    DBGOUT printf("enter [%s]\n", static_cast<QLItem*>(item)->GetName());
					if (item != NULL){
						//static_cast<QLItem*>(item)->ShowBalloon(v.xcrossing.x + 30,ev.xcrossing.y + 30);
//						static_cast<QLItem*>(item)->Select();
//                      static_cast<QLItem*>(item)->Paint();
						static_cast<QLItem*>(item)->ShowBalloon(30,30);
                    }
				}

				break;
			case ButtonPress:
				{
					QLWidget * item = findWidget(ev.xcrossing.window);
					if (item!=NULL)
						static_cast<QLItem*>(item)->Execute();
				}
				break;
			default:
				break;
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
void QLBar::hideAllItems() 
{
	pthread_mutex_lock(&graph_mutex);

	vector<QLItem *>::const_iterator it = items.begin();
	for (; it != items.end() ; it++)
    {
        (*it)->HideBalloon();
		(*it)->Hide();
    }

	XSync(display, False);

	pthread_mutex_unlock(&graph_mutex);
}

/**
 * Function iterates through vector containing qlitems and show each of them
 */
void QLBar::showAllItems()
{
	pthread_mutex_lock(&graph_mutex);

	vector<QLItem *>::const_iterator it = items.begin();
	for (; it != items.end() ; it++)
    {
		(*it)->Show();
    }
	XSync(display, False);

	pthread_mutex_unlock(&graph_mutex);
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

bool QLBar::GetActive() const 
{
	return active;
}

void QLBar::SetConfig(QLConf * cfg)
{
	if (cfg == NULL)
		return;

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


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// thread function //
void* hideBar(void * thid)
{
    DBGOUT printf("will hide %d\n", pthread_self());
	QLBar * qb = static_cast<QLBar *> (thid);
	if (qb == NULL) {
        DBGOUT printf("qb==NULL\n");
		pthread_exit(0);
	}

	if (!qb->GetActive()){
        DBGOUT printf("!GetActive()\n");
		pthread_exit(0);
    }

	sleep(qb->GetBarTime()); // wait a second and then hide all items
	if (qb!=NULL)
		qb->hideAllItems();

	pthread_exit(0);
}
