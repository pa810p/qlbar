#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <X11/extensions/shape.h>
#include <string.h>
#include <stdlib.h>

#include "qllogger.h"
#include "constants.h"
#include "qlitem.h"

extern QLLogger qllogger;

/**
 * ctor fills members with default value, reads icons
 * and creates images
 */
QLItem::QLItem ( char * ip, char * ep, char * fn)
{
	_width = 64;
	_height = 64;

	failed = false;

	_x = 0;
	_y = 0;
	int iip = strlen (ip);
	_pIconPath = new char [ iip + 1 ];
	strncpy(_pIconPath, ip, iip);
	_pIconPath[iip] = '\0';

	iip = strlen (ep);
	_pExecPath = new char [ iip + 1 ];
	strncpy(_pExecPath, ep, iip);
	_pExecPath[iip] = '\0';

	iip = strlen (fn);
	_pName = new char [ iip +1 ];
	strncpy(_pName, fn, iip);
	_pName[iip] = '\0';

    //TODO:
    // move initialization to further stage 
    // or display creation
	Init();
	_pTip = NULL;

    qllogger.logT("QLItem creation: [%x]", this);

}


void QLItem::createDefIconPath(char * &dst)
{
	if (dst != NULL)
		delete [] dst;

	const char * home = getenv("HOME");
	int home_len = strlen(home);
  	int qlbdir_len = strlen(QLB_DEF_DIR);
	
	int len = strlen ( QLB_DEF_ICON );
	dst = new char [ home_len + qlbdir_len + len + 2 ];
	strncpy (dst, home, home_len + 1);
	strncat (dst, "/", 1);
	strncat (dst, QLB_DEF_DIR, qlbdir_len + 1);
	strncat (dst, QLB_DEF_ICON, len+1);
	
}

/**
 * Function initializes and validates item's data
 *
 */
void QLItem::Init()
{
	// check paths
	struct stat st;
	stat(_pIconPath, &st);

	if (!S_ISREG(st.st_mode)){
		fprintf(stderr, "Could not find image: %s\nUsing default\n", _pIconPath);
		createDefIconPath(_pIconPath);
		}


	origImage = imlib_load_image(_pIconPath);
	if (origImage == NULL) {
		qllogger.logW("Could not load image: %s\nUsing default\n", _pIconPath);
		char * dst = NULL;
		createDefIconPath(dst);
		origImage = imlib_load_image(dst);
		if (origImage == NULL) {
			qllogger.logW("Could load default icon file: %s\n", dst);
			failed = true;
		}

		if (dst != NULL){
			delete [] dst;
			dst = NULL;
		}
			
	}

    //TODO:
    //this should not be done before CreateUI
    //or at least _width/_height variables initialization
	if (failed == true) // failed
		return;
}

/**
 * dtor frees images
 */
QLItem::~QLItem ()
{
	if (_pIconPath != NULL) {
		delete [] _pIconPath;
		_pIconPath = NULL;
	}

	if (_pExecPath != NULL) {
		delete [] _pExecPath;
		_pExecPath = NULL;
	}

	if (_pName != NULL) {
		delete [] _pName;
		_pName = NULL;
	}

	if (failed == false){
		imlib_context_set_image(im);
		imlib_free_image();
	}

    if (_pTip != NULL){
        delete _pTip;
        _pTip = NULL;
    }
}

/**
 * Function paints icon on a screen
 */
void QLItem::Paint()
{
//    Imlib_Image rect = imlib_create_image(_width,_height);
//    imlib_context_set_image(rect);
//    imlib_context_set_color(0,0,0,255);
//    imlib_image_fill_rectangle(0,0,_width,_height);


	qllogger.logT("QLItem paint : %d, %d, %d, %d", _x, _y, _width, _height);
	imlib_context_set_image(im);
	imlib_context_set_drawable(_window);
	imlib_image_set_has_alpha(1);
	imlib_image_set_irrelevant_alpha(0);

	imlib_render_pixmaps_for_whole_image_at_size( &pix, &mask, _width, _height);

  //  imlib_blend_image_onto_image(rect,1,0,0,_width,_height,0,0,_width,_height);

	XSetWindowBackgroundPixmap(_display, _window, pix);
	XShapeCombineMask(_display, _window, ShapeBounding, 0, 0, mask, ShapeSet);

	XRaiseWindow(_display, _window);
	XClearWindow(_display, _window);
}

/**
 * Function creates Window for the item and registers events.
 * @param display parents display pointer
 * @param window parents window
 * @param x coordinate x relative to parent
 * @param y coordinate y relative to parent
 * @param w width of icon
 * @param h height of icon
 */
bool QLItem::CreateUI (Display * display, Window window, 
	const int & x, const int & y, const int &w, const int &h)
{
	qllogger.logT("prepare QLItem [%x] (%s): %d, %d, %d, %d", this, GetName(), x, y, w, h);
	_x = x;
	_y = y;
	_width = w;
	_height = h;


	imlib_context_set_image(origImage);
	int width = imlib_image_get_width();
	int height = imlib_image_get_height();

	im = imlib_create_cropped_scaled_image(0,0,width,height,_width,_height);

    imlib_context_set_image(im);
    qllogger.logT("Init image size after scale: %d, %d, (%d, %d)", imlib_image_get_width(), imlib_image_get_height(), width, height);
    imlib_context_set_image(origImage);

    origImage = imlib_clone_image();
	imlib_free_image();


	_display = display;

	XSetWindowAttributes attrib;
 	attrib.override_redirect = True;
	attrib.background_pixel = 1;

	// create dedicated window for item
	_window = XCreateWindow(display, DefaultRootWindow(display),
      _x, _y, _width, _height, 0, CopyFromParent,
      InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attrib);

	_event_mask = EnterWindowMask|LeaveWindowMask|ExposureMask
					|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask;

	Show();

	return true;
}


/**
 * Function creates UI for item balloon
 * @param base_x x coordinate 
 * @param base_y y coordinate
 * @return true on success, false on failure
 */
bool QLItem::CreateBalloonUI(const int & base_x, const int & base_y, Balloon bal)
{
	//security issue, but probably _pTip should be null
	//because CreateUI is called only on bar creation
	if (_pTip != NULL){
		delete _pTip;
	}
	
	bal.caption = _pName;
	_pTip = new QLTip(bal);
	qllogger.logT("Created Balloon UI: [%x] on qlitem [%x]", _pTip, this);
	
	//let it be balloon
	if (!  _pTip->CreateUI(_display, _window, base_x, base_y) )
		return false;;// , _width*4, _height*2);
	
	return true;
}


void QLItem::Scale(const int & w, const int &h)
{
	_width = w;
	_height = h;
}


/**
 * Function executes items application as separated
 */
void QLItem::Execute() 
{
	qllogger.logT("Execute");
 	pid_t pid;

   if (!(pid = fork())) 
	{
      setsid();
      switch (fork()) 
		{
      	case 0:
        		execlp("/bin/sh", "sh", "-c", _pExecPath, NULL);
      	default:
         	_exit(0);
      }
   }

   if (pid > 0)
      wait(NULL);

}


void QLItem::ShowBalloon(const int &x, const int &y)
{
    qllogger.logT("QLItem show balloon() [%s] [%x]", GetName(), _pTip );

	if (_pTip != NULL)
		static_cast<QLTip*>(_pTip)->ShowBalloon(x,y);
	
}

void QLItem::HideBalloon()
{
	if (_pTip != NULL)
    {   
        qllogger.logT("QLItem will hide the balloon() -> cast to qltip");
        static_cast<QLTip*>(_pTip)->HideBalloon();
        //delete _pTip;
        //_pTip = NULL;
    }
}


/**
 * Function returns balloon pointer
 * @return balloon
 */
QLWidget * QLItem::GetBalloon() const
{
	return _pTip;
}


void QLItem::Select()
{
	qllogger.logT("QLItem select : %d, %d, %d, %d", _x, _y, _width, _height);

    imlib_context_set_image(im);
    imlib_context_set_color(255,255,255,50);
    imlib_image_fill_rectangle(0,0, imlib_image_get_width(), imlib_image_get_height());
    imlib_render_image_on_drawable_at_size(0,0,imlib_image_get_width(),imlib_image_get_height());

    int width = imlib_image_get_width();
    int height = imlib_image_get_height();
    qllogger.logT("QLItem select image size: %d, %d", width, height);
}

void QLItem::UnSelect()
{
	qllogger.logT("QLItem Unselect : %d, %d, %d, %d", _x, _y, _width, _height);

    imlib_context_set_image(origImage);
    imlib_render_image_on_drawable_at_size(0,0,_width,_height);
}

char * QLItem::GetName() const
{
    return _pName;
}


