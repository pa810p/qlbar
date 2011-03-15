#ifndef _qlbar_h_
#define _qlbar_h_

#include <X11/Xlib.h>
#include <stdlib.h>

#include "constants.h"
#include "qlitem.h"
#include "qlconf.h"

#include <vector>

void* hideBar(void * thid);

class QLBar {
	
	private:
		Display *display;
		Visual *visual;
		Window window, root;
		Colormap colormap;
		GC gc;
		int _defScreen;

		XGCValues gv;
		
		QLConf *_pcfg;
		vector <QLItem *> items;

		int _icon_width;
		int _icon_height;
		// position of qlbar
		int _bar_x; 
		int _bar_y;

		int _icon_start_x;
		int _icon_start_y;

		// tool tip offset vector from right upper corner of icon
		int _tooltipv_x;
		int _tooltipv_y;

		Imlib_Font _font;

		void prepareItems();
		int prepareFont();
		int getBarWidth() const;
		int getBarHeight() const;

		QLWidget * findWidget (const Window window, const bool deep = false) const;

		void ComputeXY(int &x, int &y, int &ic_x, int &ic_y, int &ttv_x, int & ttv_y) ;

        struct timeval tv;


        bool shouldHideBar;
	public:
		QLBar ();
		~QLBar ();

		int Prepare ();
		int Run ();

		int AddItem (QLItem * pItem);
		int AddItem (const int & index, QLItem * pItem);

		void SetConfig (QLConf *cfg);

		int GetBarX() const;
		bool GetActive() const;
		
		void hideAllItems() ;
		void showAllItems() ;
		void ScaleItems();
        void hideAllBalloons();

		int GetBarTime () const;

        void lock();
        void unlock();

        void handleEvent(const XEvent &ev);
};

#endif
