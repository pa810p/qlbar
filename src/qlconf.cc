#include <regex.h>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <cstring>

#include "qlconf.h"
#include "constants.h"
#include "qllogger.h"


extern QLLogger qllogger;


/**
 * ctor
 * fills members with default values;
 */
QLConf::QLConf()
{
	b_icon_width = false;
	b_icon_height = false;
	b_bar_height = false;
	b_bar_width = false;
	b_bar_color = false;
	b_position = false;
	b_layout = false;
	b_bar_time = false;
	b_hoffset = false;
	b_voffset = false;
	b_font_name = false;
	b_font_size = false;
	b_font_dir = false;
	b_font_color = false;
	b_balloon_color = false;
	b_show_balloon = false;
    b_daemonize = false;
    b_logging = false;
    b_logLevel = false;

	icon_width = 32;
	icon_height = 32;
	bar_color = 0x0000ff;
	bar_width = -1;
	bar_height = 1;
	position = BP_NORTH; // top
	layout = BAR_HORIZONTAL;
	bar_time = 1;
	hoffset = 0;
	voffset = 0;
	font_name = NULL;
	font_size = 10; // TODO: think that font size is specified by font name (!) now
	font_dir = NULL;
	font_color = 0x000000; // black
	balloon_color = 0xffff00;
	show_balloon = true;    
    daemonize = false;
    logging = true;
    logLevel = QLLogger::ERROR;
    logFile = NULL;
}

/**
 * dtor
 * clears items list
 */
QLConf::~QLConf()
{
	for (unsigned int i=0;i<items.size();i++) {
		if (items[i] != NULL) {
			delete items[i];
			items[i] = NULL;
		}
	}

	//strings
	if (font_name != NULL){
		delete [] font_name;
		font_name = NULL;
	}
}

/**
 * Function compares two strings
 * @param opt pointer to first string
 * @param pattern pointer to second string used as pattern
 * @return 0 when strings are the same
 * @todo make comparision not case sensitive
 */
int QLConf::comp(const char* opt, const char* pattern) const {
	if (strlen (opt) == strlen(pattern))
		return strncmp(opt,pattern, strlen(pattern));
	else 
		return -1;
}


/**
 * Template function copies src value into dst pointer
 * @param dst pointer to any type destination
 * @param src source any value
 * @param state pointer to state that will be modified after copy
 * @param ov boolean that say the destination value should be overriden
 */
template <class T>
void QLConf::SetT(T* dst, const T & src, bool * state, const bool ov)
{
	if ((ov == true) || (*state == false)){
		*dst = src;
		*state = true;
	}
}

/**
 * Function copies src value into dst pointer. Function allocates memory for destination.
 * If pointer to destination is not NULL, function frees memory on this pointer.
 * @param dst pointer to string (char *) destination
 * @param src pointer to source string
 * @aram state pointer to state that will be modified after copy
 * @param ov boolean that say the destination value should be overriden
 */
void QLConf::Set(char** dst, const char* src, bool * state, const bool ov)
{
	if ((ov == true) || (*state == false)){
		int len = strlen(src);
		if ((*dst) != NULL)
			delete [] *dst;

		*dst = new char [ len+1 ];
		strncpy (*dst, src, len); 
		(*dst)[len] = '\0';
		*state = true;
	}
}



/**
 * Function compares given option name with well known patterns and
 * when option name is valid config member is set with given value.
 * @param optname pointer name of the option to set
 * @param value pointer to string representation of the value to set
 * @param ov destination override flag, true means that value will be overriden
 * @return int 0 on success, -1 on failure eg. option name is not known
 */
int QLConf::Parse(const char * optname, const char * value, const bool ov)
{

	if (!comp(optname, "icon-width"))
		SetT(&icon_width,atoi(value),&b_icon_width, ov);

	else if (!comp(optname, "icon-height"))
		SetT(&icon_height, atoi(value),&b_icon_height, ov);

	else if (!comp(optname, "bar-width")){
		if (!comp(value, "default"))
			SetT(&bar_width, -1, &b_bar_width, ov);
		else
			SetT(&bar_width, atoi(value), &b_bar_width, ov);
	}
	else if (!comp(optname, "bar-height"))
		SetT(&bar_height, atoi(value),&b_bar_height, ov);

	else if (!comp(optname, "bar-color")){
		if (!strchr(value, 'x')){
			SetT(&bar_color, atol(value),&b_bar_color, ov);
		}
		else{
			SetT(&bar_color, strtol (value,NULL,16), &b_bar_color, ov);
		}
	}
	else if (!comp(optname, "position")) {
		if (!comp(value, "n"))
			SetT(&position, BP_NORTH, &b_position, ov);
		else if (!comp(value, "ne"))
			SetT(&position, BP_NORTH_EAST, &b_position, ov);
		else if (!comp(value, "e"))
			SetT(&position, BP_EAST, &b_position, ov);
		else if (!comp(value, "se"))
			SetT(&position, BP_SOUTH_EAST, &b_position, ov);
		else if (!comp(value, "s"))
			SetT(&position, BP_SOUTH, &b_position, ov);
		else if (!comp(value, "sw"))
			SetT(&position, BP_SOUTH_WEST, &b_position, ov);
		else if (!comp(value, "w"))
			SetT(&position, BP_WEST, &b_position, ov);
		else if (!comp(value, "nw"))
			SetT(&position, BP_NORTH_WEST, &b_position, ov);
		else
		{
			fprintf(stderr, "Unknown value for option %s = %s\n", optname, value);
			return -1;
		}
	} 
	else if (!comp(optname, "layout")) {
		if (!comp(value, "vertical"))
			SetT(&layout, BAR_VERTICAL, &b_layout, ov);
		else if (!comp(value, "horizontal"))
			SetT(&layout, BAR_HORIZONTAL, &b_layout, ov);
	}
	else if (!comp(optname, "bar-time")){
		SetT(&bar_time, atoi(value),&b_bar_time, ov);
	}
	else if (!comp(optname, "hoffset")){
		SetT(&hoffset, atoi(value), &b_hoffset, ov);
		}
	else if (!comp(optname, "voffset")){
		SetT(&voffset, atoi(value), &b_voffset, ov);
	}
	else if (!comp(optname, "font-name")){
		Set(&font_name, value, &b_font_name, ov);
	}
	else if (!comp(optname, "font-dir")){
		Set(&font_dir, value, &b_font_dir, ov);
	}
	else if (!comp(optname, "font-size")){
		SetT(&font_size, atoi(value), &b_font_size, ov);
	}
	else if (!comp(optname, "font-color")){
		if (!strchr(value, 'x')){
			SetT(&font_color, atol(value),&b_font_color, ov);
		}
		else{
			SetT(&font_color, strtol (value,NULL,16), &b_font_color, ov);
		}
	}
	else if (!comp(optname, "balloon-color")){
		if (!strchr(value, 'x')){
			SetT(&balloon_color, atol(value),&b_balloon_color, ov);
		}
		else{
			SetT(&balloon_color, strtol (value,NULL,16), &b_balloon_color, ov);
		}
	}
	else if (!comp(optname, "show-balloon")){
		if (!comp(value, "true"))
			SetT(&show_balloon, true, &b_show_balloon, ov);
		else
			SetT(&show_balloon, false, &b_show_balloon, ov);
	}
    else if (!comp(optname, "daemonize")){
        SetT(&daemonize, true, &b_daemonize, ov);
    }
    else if (!comp(optname, "log")) {
        SetT(&logging, true, &b_logging, ov);
    }
    else if (!comp(optname, "loglevel")){
        SetT(&logLevel, parseLogLevel(value), &b_logLevel, ov);
    }
    else if (!comp(optname, "logfile")) {
        Set(&logFile, value, &b_logFile, ov);
    }
	else
	{
		fprintf(stderr, "Unknown option: %s = %s\n", optname, value);
		return -1; // option unknown
	}	

	return 0;
}

QLLogger::Level QLConf::parseLogLevel(const char * strLogLevel) {
    QLLogger::Level level = QLLogger::TRACE;

    //fprintf(stderr, "optname: %s:%s", optname, value);

    if (NULL != strLogLevel) {
        if (strncmp(strLogLevel, "trace", 6) == 0) {
            level = QLLogger::TRACE;
        }
        else if (strncmp(strLogLevel, "debug", 6) == 0) {
            level = QLLogger::DEBUG;
        }
        else if (strncmp(strLogLevel, "info", 6) == 0) {
            level = QLLogger::INFO;
        }
        else if (strncmp(strLogLevel, "warn", 6) == 0) {
            level = QLLogger::WARN;
        }
        else if (strncmp(strLogLevel, "error", 6) == 0) {
            level = QLLogger::ERROR;
        }
        else if (strncmp(strLogLevel, "fatal", 6) == 0) {
            level = QLLogger::FATAL;
        }
        else {
            fprintf(stderr, "Unknown logging level, log is set to TRACE");
        }
        
    }

    return level;
}


/**
 * Function reads menu file, creates QLItems list
 * @param filename name of file to read
 * @return 0 on success, -1 on failure
 */
int QLConf::ReadMenuConfig( const char * filename )
{

	char buffer [1024];

	regex_t rexec;
	regmatch_t match[4];

	const char * pattern = 
		"^[[:blank:]]*\\[exec\\][[:blank:]]*\\((.*)\\)[[:blank:]]*\\{(.*)\\}[[:blank:]]*<(.*)>";

	if(regcomp(&rexec,pattern, REG_EXTENDED ) != 0)
	{
		fprintf(stderr, "error compile regex");
		return -1;
	}

	ifstream cfg;
	cfg.open(filename, ifstream::in);
	if (!cfg){
		fprintf(stderr, "cannot open config file: %s\n", filename);
		return -1;
	}

	while (cfg.good()){
		cfg.getline(buffer,sizeof(buffer));
		if (strlen(buffer)==0) continue;
		
		if (regexec(&rexec, buffer, 4, match, REG_NOTEOL) == 0)
		{
			char ** appdesc = new char * [3];
			for (int i=0;i<3;i++)
			{
				*(appdesc+i) = new char [match[i+1].rm_eo - match[i+1].rm_so +1];
				strncpy(*(appdesc+i), buffer+match[i+1].rm_so, match[i+1].rm_eo - match[i+1].rm_so);
				(*(appdesc+i))[match[i+1].rm_eo - match[i+1].rm_so] = '\0';
			}

			//TODO: first level validate params
			
			QLItem * item = new QLItem (*(appdesc+2), *(appdesc+1), *(appdesc));
			items.push_back(item);
			
			for (int i=0;i<3;i++)
			{
				if (*(appdesc+i) != NULL)
					delete [] *(appdesc+i);
				*(appdesc+i) = NULL;
			}

			if (*(appdesc)!=NULL)
				delete [] appdesc;
			appdesc = NULL;
		}
	}
	cfg.close();	
	regfree(&rexec);
	return 0;
}


/**
 * Function reads config 
 * @param filename name of file to read
 * @return 0 on success, -1 on failure
 */
int QLConf::ReadConfigFile(const char * filename)
{

	char buffer [1024];

	regex_t rexec;
	regmatch_t match [3];

	const char * pattern = "^([a-zA-Z-]+)[[:blank:]]*=[[:blank:]]([/a-zA-Z0-9-]*)";

	if(regcomp(&rexec,pattern, REG_EXTENDED ) != 0) {
		fprintf(stderr, "error compile regex");
		return -1;
	}  

	ifstream cfg;
	cfg.open(filename, ifstream::in);
	if (!cfg){
		fprintf(stderr, "cannot open config file: %s\n", filename);
		return -1;
	}

	while (cfg.good()){
		cfg.getline(buffer, sizeof(buffer));
		if (strlen(buffer) == 0)
			continue;
		if (*buffer == '#') // skip comment 
			continue;

		if (regexec(&rexec, buffer, 3, match, 0) == 0){
			// matched
			char * opt_name = new char [ match[1].rm_eo - match[1].rm_so + 1];
			strncpy(opt_name, buffer + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
			opt_name[match[1].rm_eo - match[1].rm_so] = '\0';

			char * value = new char [ match[2].rm_eo - match[2].rm_so + 1];
			strncpy(value, buffer + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
			value[match[2].rm_eo - match[2].rm_so] = '\0';

			Parse(opt_name, value, false);

			delete [] opt_name;
			opt_name = NULL;
			delete [] value;
			value = NULL;
		}
	}

	cfg.close();
	return 0;
}


/**
 * Function returns width of the bar.
 * @param asis bar width is not computed. It's returned AS IS 
 * @return int widht of the bar
 */
int QLConf::GetBarWidth(bool asis) const
{
	if ((asis == false || bar_width == -1) && layout == BAR_HORIZONTAL)
		return icon_width * items.size();
	else 
		return bar_width;
}


/**
 * Function returns height of the bar. 
 * @param asis bar height is not computed. It's returned AS IS
 * @return int height of the bar
 */
int QLConf::GetBarHeight(bool asis) const
{
	if ((asis == false || bar_height == -1) && layout == BAR_VERTICAL)
		return icon_height * items.size();
	else
		return bar_height;
}


/**
 * Function puts string given by parameter onto standard error when
 * second parameter is true
 * @param msg message string
 * @param verbose if true message is displayed on stderr, nothing happen else
 */
void QLConf::Error(const char * msg, const bool verbose) const
{
    qllogger.logE(msg);
    
	if (verbose)
		fprintf(stderr, msg);
}


/**
 * Function validates if config's data is correct.
 * @param verbose boolean true if error information should be displayed on stderr
 * @return bool true if config's data is valid, false on invalid
 */
bool QLConf::Validate(const bool verbose) 
{
    qllogger.logT("Validating...");

	int valid = true;
	switch (this->GetLayout()){
		case BAR_HORIZONTAL:
			switch (this->GetPosition()){
				case BP_EAST:
				case BP_WEST:
					valid = false;
					Error("Bar position EAST or WEST cannot be set with layout HORIZONTAL\n", verbose);
					break;
				default:
					break;
			}
			if (this->GetVOffset() != 0){
				valid = false;
			 	Error("Vertical offset cannot be set with layout HORIZONTAL\n", verbose);
			}
			break;
		case BAR_VERTICAL:
			switch (this->GetPosition()){
				case BP_NORTH:
				case BP_SOUTH:
					valid = false;
					Error("Bar position NORTH or SOUTH cannot be set with layout VERTICAL\n", verbose);
					break;
				default:
					break;
			}
			if (this->GetHOffset() != 0){
				valid = false;
				Error("Horizontal offset cannot be set with layout VERTICAL\n", verbose);
			}
		break;
	}

	if (this->GetHOffset() < 0){
		valid = false;
		Error("Horizontal offset cannot be lower than 0\n", verbose);
	}
	
	if (this->GetVOffset() < 0){
		valid = false;
		Error("Vertical offset cannot be lower than 0\n", verbose);
	}

	if (this->GetShowBalloon() == true){
		if (NULL != this->GetFontDir() && strlen(this->GetFontDir()) == 0){
			Error("Font directory path length. Balloon disabled\n", verbose);
			this->show_balloon = false;
		}

		if (NULL != this->GetFontName() && strlen(this->GetFontName()) == 0){
			Error("Font name is empty. Balloon disabled\n", verbose);
			this->show_balloon = false;
		}
	}
		

	return valid;
}

