#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <vector>

#include "constants.h"
#include "qlbar.h"
#include "qlconf.h"


using namespace std;

/**
 * Functions displays program header, version number and program name
 */
void Header ()
{
	printf("This is QLBar %s written by %s.\n", QLBAR_VERSION, QLBAR_AUTHOR);
	printf("Distributed under terms of GNU General Public License\n\n");
}


/**
 * Function displays usage information on the stdout
 */
void Usage ()
{
	printf("Usage: qlbar [OPTIONS]\n");
	printf("      --bar-color     color of the bar\n");
	printf("      --bar-height    height of the bar in pixels\n");
	printf("      --bar-width     width of the bar in pixels\n");
	printf("  -c, --config        configuration file\n");
	printf("  -h  --help          this help screen\n");
	printf("      --hoffset       horizontal toolbar offset from corner defined in --position option\n");
	printf("  -H, --horizontal    toolbar will be placed horizontaly\n");
	printf("      --icon-height   height of the icons\n");
	printf("      --icon-width    width of the icons\n");
	printf("  -l, --layout        layout, may be horizontal or vertical, same as -H or -V options\n");
	printf("  -m, --menu          file containing toolbar icon information\n");
	printf("  -p, --position      position of a toolbar: n, ne, e, se, s, sw, w, nw\n");
	printf("  -t, --bar-time      time after bar will hide\n");
	printf("  -v, --verbose       verbose display on stderr\n");
	printf("      --voffset       vertical toolbar offset from corner defined in --position option\n");
	printf("  -V, --vertical      toolbar will be placed verticaly\n"); 
//	printf("available from version 0.1.2:\n");
//	printf("  -b, --balloon       tooltip balloon type: rect, round\n");
	printf("      --balloon-color background color of tooltip balloon\n");
	printf("      --font-color    tooltip balloon font color\n");
	printf("  -f  --font-name     font used on balloon tip\n");
	printf("  -s  --font-size     tooltip balloon font size\n");
	printf("  -d  --font-dir      directory that contains TTF fonts\n");
	printf("  -B  --show-balloon  [true, false] show or not baloon, default balloon is shown\n");
    printf("  -D  --daemonize     qlbar will run in the background\n");
	printf("\n");

}



int main (int argc, char ** argv)
{
	// show header
	Header();

	QLConf cfg;

	// options
	static struct option long_options[] = {
		{"menu", 1, 0, 'm'},
		{"config", 1, 0, 'c'},
		{"icon-width", 1, 0, 0},
		{"icon-height", 1, 0, 0},
		{"bar-height", 1, 0, 0},
		{"bar-width", 1, 0, 0},
		{"bar-color", 1, 0, 0},
		{"help", 0, 0, 'h'},
		{"position", 1, 0, 'p'},
		{"horizontal", 0, 0, 'H'},
		{"vertical", 0, 0, 'V'},
		{"bar-time", 1, 0, 't'},
		{"verbose", 0, 0, 'v'},
		{"layout", 1, 0, 'l'},
		{"hoffset", 1, 0, 0},
		{"voffset", 1, 0, 0},
		{"font-name", 1, 0, 'f'},
		{"font-size", 1, 0, 's'},
		{"font-dir", 1, 0, 'd'},
		{"font-color", 1, 0, 0},
		{"show-balloon", 1, 0, 'B'},
        {"daemonize", 0, 0, 'D'}
	};

	// first parse options
	char * config_file = NULL;
	char * menu_config_file = NULL;
	char def_config [] = QLB_DEF_CONFIG;
	char def_menu [] = QLB_DEF_MENU;

	int i=0;
	int option_index;

	while ((i=getopt_long(argc,argv, "DhHVvl:m:c:p:t:f:s:d:B:", long_options, &option_index ))!=-1)
	{
		switch(i)
		{
			case 'm':
				{
					// menu file
					int fnsize = strlen(optarg);
					if (fnsize<MAX_OPT_LEN)
					{
						menu_config_file = new char [fnsize+1];
						strncpy(menu_config_file, optarg, fnsize+1);
					}
				}
				break;
			case 'c':
				{
					// alternative config file
					int fnsize = strlen(optarg);
					if (fnsize<MAX_OPT_LEN)
					{
						config_file = new char [fnsize+1];
						strncpy(config_file, optarg, fnsize+1);
					}
				}
				break;
			case 'h':
				Usage();
				return 0;
			case 'p':
				if (cfg.Parse("position", optarg, true) == -1)
					return -1;
				break;
			case 'H':
				if (cfg.Parse("layout", "horizontal", true) == -1)
					return -1;
				break;
			case 'V':
				if (cfg.Parse("layout", "vertical", true) == -1)
					return -1;
				break;
			case 'l':
				if (cfg.Parse("layout", optarg, true) == -1)
					return -1;
				break;
			case 't':
				if (cfg.Parse("bar-time", optarg, true) == -1)
					return -1;
				break;
			case 'f':
				if (cfg.Parse("font-name", optarg, true) == -1)
					return -1;
				break;
			case 's':
				if (cfg.Parse("font-size", optarg, true) == -1)
					return -1;
				break;
			case 'd':
				if (cfg.Parse("font-dir", optarg, true) == -1)
					return -1;
				break;
			case 'B':
				if (cfg.Parse("show-balloon", optarg, true) == -1)
					return -1;
				break;
            case 'D':
                if (cfg.Parse("daemonize", optarg, true) == -1)
                    return -1;
                break;
			case 0:
				{
					if (cfg.Parse(long_options[option_index].name, optarg, true) == -1)
						return -1;
				}
				break;
			default:
				Usage();
				return -1;
		} 
	}

    pid_t child;
    if (cfg.GetDaemonize() == true)
    {
        child = fork();
        if (child == (pid_t) -1)
            return -1;
        else if (child > (pid_t) 0 )
            return 0; // parent
        
    }

	// configs	
	const char * home = getenv("HOME");
	int home_len = strlen(home);
	int qlbdir_len = strlen(QLB_DEF_DIR);

	// menu config
	if (menu_config_file == NULL) {
		int len = strlen (def_menu);
		menu_config_file = new char [ home_len + qlbdir_len + len + 2 ];
		strncpy (menu_config_file, home, home_len + 1);
		strncat (menu_config_file, "/", 1);
		strncat (menu_config_file, QLB_DEF_DIR, qlbdir_len + 1);
		strncat (menu_config_file, def_menu, len+1);
	}

	if (menu_config_file != NULL){
		if (cfg.ReadMenuConfig(menu_config_file) == -1){
			delete [] menu_config_file; // config filename is no longer needed
			menu_config_file = NULL;
			fprintf(stderr, "Cannot read menu config file. Abort\n");
			return -1;
		}
		delete [] menu_config_file;
		menu_config_file = NULL;
	}

	// main config
	if (config_file == NULL) {
		int len = strlen (def_config);
		config_file = new char [ home_len + qlbdir_len + len+2 ];
		strncpy (config_file, home, home_len + 1);
		strncat (config_file, "/", 1);
		strncat (config_file, QLB_DEF_DIR, qlbdir_len + 1);
		strncat (config_file, def_config, len+1);
	}


	if (config_file != NULL) {
		if (cfg.ReadConfigFile(config_file) == -1){
			delete [] config_file; // config filename is no longer needed
			config_file = NULL;
			fprintf(stderr, "Cannot read config file. Abort\n");
			return -1;
		}
		delete [] config_file;
		config_file = NULL;
	}

	// post-analyze config
	if (! cfg.Validate(true) )
		return 0;

	QLBar qlbar;
	qlbar.SetConfig(&cfg);

	qlbar.Prepare();
	qlbar.Run();

	return 0;
}

