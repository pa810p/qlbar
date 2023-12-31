#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <vector>
#include <string.h>

#include "constants.h"
#include "qlbar.h"
#include "qlconf.h"

#include "qllogger.h"

#include "../config.h"



using namespace std;

extern QLLogger qllogger;


/**
 * Functions displays program header, version number and program name
 */
void Header ()
{
	printf("This is %s written by %s (%s).\n", PACKAGE_STRING, QLBAR_AUTHOR, PACKAGE_BUGREPORT);
	printf("Distributed under terms of GNU General Public License\n\n");
}


/**
 * Function displays usage information on the stdout
 */
void Usage ()
{
	printf("Usage: qlbar [OPTIONS]\n");
	printf("      --bar-color       color of the bar\n");
	printf("      --bar-height      height of the bar in pixels\n");
	printf("      --bar-width       width of the bar in pixels\n");
	printf("  -c, --config          configuration file\n");
	printf("  -h  --help            this help screen\n");
	printf("      --hoffset         horizontal toolbar offset from corner defined in --position option\n");
	printf("  -H, --horizontal      toolbar will be placed horizontaly\n");
	printf("      --icon-height     height of the icons\n");
	printf("      --icon-width      width of the icons\n");
	printf("  -l, --layout          layout, may be horizontal or vertical, same as -H or -V options\n");
	printf("  -m, --menu            file containing toolbar icon information\n");
	printf("  -p, --position        position of a toolbar: n, ne, e, se, s, sw, w, nw\n");
	printf("  -t, --bar-time        time after bar will hide\n");
	printf("  -L, --log             application will produce default (warn level) logs\n");
    printf("      --loglevel        the log level: trace, debug, info, warn, error, fatal\n");
    printf("      --logfile         log filename, default: stderr\n");
	printf("      --voffset         vertical toolbar offset from corner defined in --position option\n");
	printf("  -V, --vertical        toolbar will be placed verticaly\n"); 
//	printf("available from version 0.1.2:\n");
//	printf("  -b, --balloon       tooltip balloon type: rect, round\n");
	printf("      --balloon-color   background color of tooltip balloon\n");
	printf("      --font-color      tooltip balloon font color\n");
	printf("  -f  --font-name       font used on balloon tip\n");
	printf("  -s  --font-size       tooltip balloon font size\n");
	printf("  -d  --font-dir        directory that contains TTF fonts\n");
	printf("  -B  --show-balloon    [true, false] show or not baloon, default balloon is shown\n");
    printf("  -D  --daemonize       qlbar will run in the background\n");
    printf("      --double          [true, false] double click to execute command\n");
    printf("  -i  --click-interval  interval between button clicks [msec]\n");
	printf("\n");

}


/**
 * ISO c++ forbids passing member method as a parameter, so
 * this is a wrapper function that pointer can be passed as
 * parameter to another function.
 * This function is used to pass reading configuration request
 * to a configuration object.
 *
 * @param cfg configutation object
 * @param config_file configuration file path
 * @return 0 on success, -1 on error
 */
int loadConfig(QLConf* cfg, const char* config_file) {
    return cfg->ReadConfigFile(config_file);
}


/**
 * ISO c++ forbids passing member method as a parameter, so
 * this is a wrapper function that pointer can be passed as
 * parameter to another function.
 * This function is used to pass reading config menu request
 * to a configuration object.
 *
 * @param cfg configutation object
 * @param config_file configuration file path
 * @return 0 on success, -1 on error
 */
int loadMenu(QLConf* cfg, const char* config_file) {
    return cfg->ReadMenuFile(config_file);
}


/**
 * This function is used to load configuration files. 
 *
 * @param cfg configuration object
 * @param loader pointer to a wrapper method that will call reading on
 *        configuration object
 * @param directory configuration qlbar directory
 * @param filename coniguration filename
 *
 * @return 0 on success, -1 on error
 */
int loadAlterConfig(QLConf* cfg, int (*loader) (QLConf*, const char*), 
    const char * directory, const char * filename)   {

    // prepare path to default config file
    int dir_len = strlen(directory);
    int file_len = strlen(filename);
    char * config_file = new char [ dir_len + file_len +1 ];
    strncpy(config_file, directory, dir_len);
    config_file[dir_len] = 0x0;
    strncat(config_file, filename, file_len );
    config_file[dir_len+file_len] = 0x0;

    if (loader(cfg, config_file) == -1) {
        delete [] config_file;
        config_file = NULL;
        return -1;
    }
    return 0;
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
		{"layout", 1, 0, 'l'},
		{"hoffset", 1, 0, 0},
		{"voffset", 1, 0, 0},
		{"font-name", 1, 0, 'f'},
		{"font-size", 1, 0, 's'},
		{"font-dir", 1, 0, 'd'},
		{"font-color", 1, 0, 0},
		{"show-balloon", 1, 0, 'B'},
        {"daemonize", 0, 0, 'D'},
        {"log", 0, 0, 'L'},
        {"loglevel", 1, 0, 0},
        {"logfile", 1, 0, 0},
        {"double", 1, 0, 0,},
        {"click-interval", 1, 0, 0},
	};

	// first parse options
	char * config_file = NULL;
	char * menu_config_file = NULL;
	char def_config [] = QLB_DEF_CONFIG;
	char def_menu [] = QLB_DEF_MENU;

	int i=0;
	int option_index;

	while ((i=getopt_long(argc,argv, "DhHVLl:m:c:p:t:f:s:d:B:i:", long_options, &option_index ))!=-1)
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
            case 'L':
                if (cfg.Parse("log", optarg, true) == -1)
                    return -1;
                break;
            case 'i':
                if (cfg.Parse("click-interval", optarg, true) == -1)
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

    if (NULL != config_file) {
        // got config as parameter
        if (cfg.ReadConfigFile(config_file) == -1)  {
             delete [] config_file;
             config_file = NULL;
             return -1;
        }
    }

    // load alternate configs
    config_file = new char [ home_len + 1 + qlbdir_len ];
    strncpy (config_file, home, home_len + 1);
    strncat (config_file, "/", 1);
    strncat (config_file, QLB_DEF_DIR, qlbdir_len + 1);

    if (loadAlterConfig(&cfg, loadConfig, config_file, QLB_DEF_CONFIG) == -1) { // ~/ failed
        delete [] config_file;
        config_file = NULL;
        if (loadAlterConfig(&cfg, loadConfig, QLB_DEF_USR_LOCAL_ETC_QLBAR, QLB_DEF_CONFIG) == -1) {
            if (loadAlterConfig(&cfg, loadConfig, QLB_DEF_ETC_QLBAR, QLB_DEF_CONFIG) == -1) {
                fprintf(stderr, "Could not load configs. What you need is a miracle. Bye.\n");
                return -1;
            }
        }
    }

    delete [] config_file;
    // initializing logger

    qllogger.initialize(cfg.GetLogLevel(), cfg.GetLogFile());
    qllogger.logI("Started QLBar version: %s", PACKAGE_VERSION);


	// menu config
    if (NULL != menu_config_file){
        if (cfg.ReadMenuFile(menu_config_file) == -1) {
            delete [] menu_config_file;
            menu_config_file = NULL;
            return -1;
        }
    }
	
    int len = strlen (def_menu);
	menu_config_file = new char [ home_len + 1 + qlbdir_len ];
	strncpy (menu_config_file, home, home_len + 1);
	strncat (menu_config_file, "/", 1);
	strncat (menu_config_file, QLB_DEF_DIR, qlbdir_len + 1);

    if (loadAlterConfig(&cfg, loadMenu, menu_config_file, QLB_DEF_MENU) == -1){
        delete [] menu_config_file;
        menu_config_file = NULL;
        if (loadAlterConfig(&cfg, loadMenu, QLB_DEF_USR_LOCAL_ETC_QLBAR, QLB_DEF_MENU) == -1) {
            if (loadAlterConfig(&cfg, loadMenu, QLB_DEF_ETC_QLBAR, QLB_DEF_MENU) == -1) {
                qllogger.logE("Cannot read menu config file. What you need is a miracle. Bye!");
                return -1;
            }
		}
	}

    delete [] menu_config_file;

	// post-analyze config
	if (! cfg.Validate() )  {
        qllogger.logW("QLBar is misconfigured, please fix settings. Aborting!");
		return 0;
    }
    
	QLBar qlbar;
	qlbar.SetConfig(&cfg);

	qlbar.Prepare();
	qlbar.Run();

	return 0;
}

