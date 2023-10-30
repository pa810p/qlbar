# QLBAR
![qlbar](https://github.com/pa810p/qlbar/assets/46489402/89e348f6-6be3-4347-96de-ff3224853fbf)

## Introduction

QLBar is a Toolbar gadget for X11 that contains icons and allows to launch
associated applications quickly. Usually it appears as a one pixel high line
placed at edge of the screen, because not to cover other windows. On mouse
move on this line QLBar appears as a bar with icon images. Common configured
application is launched after mouse press over associated icon.

## Installation

### Prerequisites
You need to have installed libraries listed below:
- imlib2-1.4.0
- log4cxx-0.10.0 (:hand: optional)

### Making and Installing the QLBar application
#### 1. Run **configure** script to see available configure options:
````shell
$ ./configure --help
````
:bulb: Note that configure script uses some local environment variables to determine compiler/linker, eg.:
````shell
        Variable        Description     Example
        CC              C compiler      gcc
        CFLAGS          C flags         -O -g
        CPPFLAGS        cpp flags       -I/path/include -DFOO=42
        LDFLAGS         ld flags        -L/usr/local/lib
        LIBS            libraries       -llib
        PATH            command path    /usr/local/bin:/usr/bin:/bin
````
#### 2. Run **make** script to build qlbar:
````shell
$ make
````
#### 3. Install qlbar on your system running:
````shell
$ su root -c 'make install'
````
:bulb: this step is not obligatory qlbar can be run from your local directory. You may copy binary to e.g. ~/HOME/bin
````shell
$ cp ./qlbar ~/$HOME/bin
````
#### 4. Post-configure
a) for single user configuration copy default configuration from package directory into the user directory
````shell
$ mkdir $HOME/.qlbar
$ cp ./conf/* $HOME/.qlbar
````
b) to have default global configuration on system, copy files from conf directory to the /etc/qlbar
````shell
$ mkdir -p /etc/qlbar
$ cp ./conf/* /etc/qlbar
````
#### 5. Edit menu configuration file, to put your own icons on toolbar
````shell
$ vim $HOME/.qlbar/menu
````
Menu file describes applications that are assigned to bar.
- (application name) is in ( and ) brackets
- {execute path} is placed in { and } brackets and points to a file that will be launched icon path> is placed in </br> < and > and points to a icon (xpm, png, ...) file that will be displayed on the bar.

#### 6. Edit design configuration file, to create your own favorite design and placement
````shell
$ vim $HOME/.qlbar/config
````
- Config file contains information about display and behavior of qlbar
- Its format is as follows: attribute name = attribute value and below attributes are available for current version:

| Attribute name | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      | Default value                                               |
|----------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|
| icon_width     | integer that represents width of each icon on the toolbar in pixels                                                                                                                                                                                                                                                                                                                                                                                                                                                              | 32                                                          |
| icon_height    | integer that represents height of each icon on the toolbar in pixels                                                                                                                                                                                                                                                                                                                                                                                                                                                             | 32                                                          |
| bar-color      | integer that represents color of the toolbar in line hidden mode                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | 80                                                          |
| bar-height     | integer that represents height of the bar in hidden mode (width of a line) in pixels                                                                                                                                                                                                                                                                                                                                                                                                                                             | 1                                                           |
| bar-width      | <table><tr><td>integer that represents width of the bar in hidden mode (width of a line) in pixels</td></tr><tr><td>string default - that tells the width of the bar will be computed as a sum of widths of all icons the bar contains</td></tr></table>                                                                                                                                                                                                                                                                         | <table><tr><td>1</td></tr><tr><td>default</td></tr></table> |
| position       | string that represents placement information of the bar. Possible values are:</br>n - bar will be placed on upper edge of screen</br>ne - bar will be placed on upper-right corner of screen</br>e - bar will be placed on right edge of screen</br>se - bar will be placed on lower-right edge of screen</br>s - bar will be placed on lower edge of screen</br>sw - bar will be placed on lower-left edge of screen</br>w - bar will be placed on left edge of screen</br>nw - bar will be placed on upper-left edge of screen | n                                                           |
| bar-time       | integer that represents after how many seconds bar will hide when mouse pointer is not over the bar                                                                                                                                                                                                                                                                                                                                                                                                                              | 	1                                                          |
| double         | string true double click to execute command 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    | false                                                       |
| click-interval | interval 250 interval between button clicks [msec]                                                                                                                                                                                                                                                                                                                                                                                                                                                                               | 	250                                                        |
| layout         | string that informs the bar to be placed horizontal or vertical                                                                                                                                                                                                                                                                                                                                                                                                                                                                  | 	horizontal                                                 |
| hoffset        | integer that represents horizontal offset (in pixels) from position corner of the screen. It has meaning only when layout is horizontal and position is one of following: ne, nw, se, sw                                                                                                                                                                                                                                                                                                                                         | 	0                                                          |
| voffset        | integer that represents vertical offset (in pixels) from position corner of the screen. It has meaning only when layout is vertical and position is one of following: ne, nw, se, sw                                                                                                                                                                                                                                                                                                                                             | 	0                                                          |
| balloon-color  | integer that represents background color of a balloon tooltip 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |                                                             |
| font-color     | integer that represents foreground (text) color of a balloon tooltip                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                             | 	 
| font-name      | string that represents name of a font used by balloon tooltip 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |                                                             |
| font-size      | integer that represents size of a font used by balloon tooltip 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |                                                             |
| font-dir       | string that represents directory of TTF containing font used by tooltip balloon 	                                                                                                                                                                                                                                                                                                                                                                                                                                                |                                                             |
| show-balloon   | string true when tooltip balloon should be shown, false if balloon tooltip should not be shown. Default true                                                                                                                                                                                                                                                                                                                                                                                                                     |                                                             | 	 
| daemonize      | qlbar will be run in the background 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |                                                             |
| log            | application will produce default (warn level) logs 	                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |                                                             |
| loglevel       | the log level: trace, debug, info, warn, error, fatal                                                                                                                                                                                                                                                                                                                                                                                                                                                                            | 	warn                                                       |
| logfile        | log filename                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     | /dev/stderr                                                 |

### Building distribution packages
Scripts for creation .rpm (RedHat) and .tgz (Slackware) are located in Installation folder.

## TODO
- transparent rectangle with alpha that appears when user moves mouse over toolbar item
- sliding bar instead of sudden appear, hide
- fixed qlbar icon, with clock, reminder, or other...
- right click context menu over every item with extra features (configure)
- add configurable separator to the toolbar

