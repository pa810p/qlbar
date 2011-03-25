#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

//#define DEBUG

// #define QLBAR_VERSION	"0.1.4"
#define QLBAR_AUTHOR	"Pawel Prokop"


#ifdef DEBUG
	#define DBGOUT 
#else
	#define DBGOUT  if (0) 
#endif


#define MAX_OPT_LEN 	1024
#define MAX_DIR_SIZE	1024
#define MAX_FONT_NAME	128

//places of bar
#define BP_NORTH		0x00
#define BP_NORTH_EAST	0x02
#define BP_EAST			0x04
#define BP_SOUTH_EAST	0x08
#define BP_SOUTH		0x10
#define BP_SOUTH_WEST	0x20
#define BP_WEST			0x40
#define BP_NORTH_WEST	0x80

//bar display
#define BAR_HORIZONTAL	0x0
#define BAR_VERTICAL	0x1

#define QLB_DEF_DIR		".qlbar/"
#define QLB_DEF_ICON	"qlbar.png"
#define QLB_DEF_CONFIG 	"config"
#define QLB_DEF_MENU	"menu"

#endif
