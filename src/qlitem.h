#ifndef _qlitem_h_
#define _qlitem_h_

#include <X11/Xlib.h>
#include <Imlib2.h>

#include "qlwidget.h"
#include "qltip.h"


using namespace std;

class QLItem : public QLWidget {

	private:
		char * _pIconPath;
		char * _pExecPath;
		char * _pName;


		bool failed;
	
		void createDefIconPath(char *&);
        char * createDirectory(const char * baseDir, const char * filename);


		QLWidget * _pTip;

        Imlib_Image origImage;

	public:
		QLItem ( const char * ip, const char * ep, const char * fn);
		virtual ~QLItem ();

		void Init();
		bool isFailed() const { return failed; }

		virtual void Paint();
        virtual bool CreateUI (Display * display, Window window, const int &x, const int &y, const int & w, const int &h);
		bool CreateBalloonUI(const int & x, const int & y, Balloon bal);

		void Scale (const int & w, const int & h);
		
		void Execute ();
		void ShowBalloon(const int &x, const int &y);
		void HideBalloon();
        
        void Select();
        void UnSelect();

		QLWidget * GetBalloon() const;

        char * GetName() const;

};



#endif
