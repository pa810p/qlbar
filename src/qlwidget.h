#ifndef _qlwidget_h_
#define _qlwidget_h_

#include <X11/Xlib.h>
#include <Imlib2.h>


class QLWidget {
	private:

	protected:

		Imlib_Image im;
		Pixmap pix;
		Pixmap mask;

		int _width;
		int _height;
		int _x;
		int _y;

		int _event_mask;
	
		Window _window;
		Display *_display;
	
	public:
		QLWidget();
		virtual ~QLWidget();
		virtual bool CreateUI (Display * display, Window window, const int &x, const int &y, const int & w=0, const int &h=0) = 0;
		virtual void Paint() = 0;
		virtual void Show () ;
		virtual void Hide () ;
		
		Window GetWindow() const;

};


#endif
