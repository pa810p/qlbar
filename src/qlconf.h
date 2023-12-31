#ifndef _qlconf_h_
#define _qlconf_h_

#include <vector>

#include "qlitem.h"
#include "qllogger.h"

using namespace std;


class QLConf {
	private:
		int icon_width;
		bool b_icon_width;

		int icon_height;
		bool b_icon_height;

		// bar height, default 1 pixel
		int bar_height;
		bool b_bar_height;

		// width of a bar, if set to -1
		// bar_width = icons count * icon_width
		int bar_width;
		bool b_bar_width;

		// color of bar
		long bar_color;
		bool b_bar_color;

		// coolbar position, north, south, etc..
		int position;
		bool b_position;

		// horizontal or vertical layout
		int layout;	
		bool b_layout;

		// delay time after that coolbar will hide [ms]
		int bar_time;
		bool b_bar_time;

		// horizontal offset of icon from a corner of the screen
		int hoffset;
		bool b_hoffset;

		// vertical offset of icon from a corner of the screen
		int voffset;
		bool b_voffset;

		// font name to use on tooltip balloon
		char * font_name;
		bool b_font_name;

		// size of font on tooltip balloon
		int font_size;
		bool b_font_size;

		// directory where to find font
		char * font_dir;
		bool b_font_dir;

		// color of the font
		long font_color;
		bool b_font_color;

		long balloon_color;
		bool b_balloon_color;

		bool show_balloon;
		bool b_show_balloon;

        bool daemonize;
        bool b_daemonize;

        bool logging;
        bool b_logging;

        QLLogger::Level logLevel;
        bool b_logLevel;

        char * logFile;
        bool b_logFile;

        long double_click_interval;
        bool b_double_click_interval;

        bool double_click;
        bool b_double_click;


		vector <QLItem *> items;
        
        QLLogger::Level parseLogLevel(const char *);

	protected:

        bool parseTrueFalse(const char* );
		int comp (const char *, const char *) const;
//		void Set(int *dst, const int &src, bool *state, const bool ov);
//		void Set(long *dst, const long &src, bool *state, const bool ov);
		void Set(char **dst, const char *src, bool *state, const bool ov);

		template <class T> void SetT(T*,  const T&, bool * , const bool );

	public:
		QLConf();
		~QLConf();

		int		Parse( const char * , const char *, const bool override = false);

		int		ReadMenuFile( const char * filename);
		int		ReadConfigFile( const char * filename);
	
		bool	Validate(); 

		// getters
		int		GetIconWidth() const { return icon_width; }
		int		GetIconHeight() const { return icon_height; }
		int		GetBarWidth(bool asis = false) const;
		int		GetBarHeight(bool asis = false) const;
		long	GetBarColor() const { return bar_color; }
		int		GetPosition () const { return position; }
		int		GetLayout () const { return layout; }
		int		GetBarTime() const { return bar_time; };
		int		GetVOffset() const { return voffset; };
		int		GetHOffset() const { return hoffset; };

		char*	GetFontName() const { return font_name; };
		char*	GetFontDir() const { return font_dir; };
		int		GetFontSize() const { return font_size; };
		long	GetFontColor() const { return font_color; };
		long	GetBalloonColor() const { return balloon_color; };
		bool	GetShowBalloon() const { return show_balloon; };
        void    SetShowBalloon(bool show) { show_balloon = show; };
        bool    GetDaemonize() const { return daemonize; };
        QLLogger::Level     GetLogLevel() const { return logLevel; };
        char *  GetLogFile() const { return logFile; };
        long    getDoubleClickInterval() const { return double_click_interval; };
        bool    getDoubleClick() const { return double_click; };
        

		vector<QLItem*> * GetItems() { return &items; }

};


#endif
