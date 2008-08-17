#ifndef _qltip_h_
#define _qltip_h_

#include <X11/Xlib.h>
#include <Imlib2.h>

#include "qlwidget.h"
#include "common.h"


#define TIP_RECT
#define TIP_ROUND

class Balloon
{
	public:
		Imlib_Font	font;
		long 		fg_color; // font color
		long 		bg_color; // background color
		int			style;
		const char* caption;
		int 		position;
		int			layout;
};



class QLTip : public QLWidget
{
	private:
		Imlib_Image _im;
		Balloon _bal;

        bool _visible;

	public:
		QLTip (Balloon bal );
		virtual ~QLTip();

		virtual void Paint();
        bool CreateUI (Display * display, Window window, const int &x, const int &y, const int & w=0, const int &h=0);

		void ShowBalloon( const int &x, const int &y);
        void HideBalloon();

};


#endif
