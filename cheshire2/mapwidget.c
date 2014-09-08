/* 
 * mapwidget.c --
 *
 * Author: Ray R. Larson
 *         School of Information Management and Systems
 *         University of California, Berkeley
 *         ray@sherlock.berkeley.edu
 *
 * This module implements a "map" widget for TK. It also includes a 
 * special routine to read and display WorldDataBank II map data, stored as
 * as a unix file.
 * Routines are included to identify the latitude and longitude of
 * a mouse click in the frame, as well as a number of other features.
 * Maps are basically like frames windows with a background color
 * and possibly a 3-D effect, but a map specified by latitude,
 * longitude, and a "zoom" value is drawn on the background. 
 *
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
#include <math.h>
/* #include "tkUnixDefault.h" */
#include "tk.h"

#ifdef WIN32
#define	M_PI		3.14159265358979323846
#endif

/* the following are NORMALLY defined in tkUnixDefault.h -- 
 * but for simplicity in building they are copied here...
 */
#define DEF_FRAME_BG_COLOR              NORMAL_BG
#define DEF_FRAME_BG_MONO               WHITE
#define DEF_FRAME_BORDER_WIDTH          "0"
#define DEF_FRAME_CLASS                 "Frame"
#define DEF_FRAME_COLORMAP              ""
#define DEF_FRAME_CONTAINER             "0"
#define DEF_FRAME_CURSOR                ""
#define DEF_FRAME_HEIGHT                "0"
#define DEF_FRAME_HIGHLIGHT_BG          NORMAL_BG
#define DEF_FRAME_HIGHLIGHT             BLACK
#define DEF_FRAME_HIGHLIGHT_WIDTH       "0"
#define DEF_FRAME_RELIEF                "flat"
#define DEF_FRAME_TAKE_FOCUS            "0"
#define DEF_FRAME_USE                   ""
#define DEF_FRAME_VISUAL                ""
#define DEF_FRAME_WIDTH                 "0"


#define FALSE 0
#define TRUE  1

/* WDB map drawing variables */

#define	QSPAN	90*3600
#define HSPAN	QSPAN*2
#define WSPAN	QSPAN*4
#define ADJPIX  5500

typedef struct { 
  short Code, Lat, Lon; 
} Point ;

typedef struct {
  int type;
  int colorid;
  float latitude;
  float longitude;
  long  assoc_int;
  char  *assoc_string;
  struct Pfeature *next_pfeature;
} Pfeature;

typedef struct MapObjPnt{
  int pix_x, pix_y;
  float lat, lon;
  struct MapObjPnt *next_pnt;
} MapObjPnt;


typedef struct MapObj {
  int ID;
  int type;
  int rubber;
  XColor *color;
  GC gc;
  int npoints;
  int close;
  MapObjPnt *first_pnt;
  MapObjPnt *last_pnt;
  XPoint *xpoints;
  struct MapObj *next_obj;
} MapObj;



#define MAPOBJ_PNT 0
#define MAPOBJ_LINE 1
#define MAPOBJ_RECT 2
#define MAPOBJ_FRECT 3
#define MAPOBJ_POLY 4
#define MAPOBJ_PLINE 5

#define NUMBER_COLORS 10


/*
 * A data structure of the following type is kept for each
 * map that currently exists for this process:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the map.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Display *display;		/* Display containing widget.  Used, among
				 * other things, so that resources can be
				 * freed even after tkwin has gone away. */
    Tcl_Interp *interp;		/* Interpreter associated with
				 * widget.  Used to delete widget
				 * command.  */
    Tk_3DBorder border;		/* Structure used to draw 3-D border and
				 * background. */
    int borderWidth;		/* Width of 3-D border (if any). */
    int relief;			/* 3-d effect: TK_RELIEF_RAISED etc. */
    int width;			/* Width to request for window.  <= 0 means
				 * don't request any size. */
    int height;			/* Height to request for window.  <= 0 means
				 * don't request any size. */
    char *geometry;		/* Geometry that user requested.  NULL
				 * means use width and height instead. 
				 * Malloc'ed. */
    Cursor cursor;		/* Current cursor for window, or None. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
    /* the following keep information about the current map data */
    /* and the state of the map drawing for this widget          */
    char *map_data_file_name;
    FILE *map_data_file;
    int mapfileread;            /* has map data been read? TRUE | FALSE */
    int mapfilepoints;          /* number of points in the map data */
    char *point_type;             /* type of point data  */
    Point *points;              /* Allocated array of map data   */
    Pfeature *names;            /* list of place names in Pfeature format */

    /* map information */
    char *Latitude_string;      /* in form (-)<dd>o<min>'<sec>''<sec>(N|S) */
    char *Longitude_string;     /* in form (-)<ddd>o<min>'<sec>''(E|W) */
    long  Latitude;             /* latitude in seconds */
    long  Longitude;            /* longitude in seconds */

    long  x_center_pixel;       /* the pixel y value from a mouse click */
    long  y_center_pixel;      /* the pixel x value from a mouse click */
    long  last_x_center;
    long  last_y_center;
    long  X_Scale, Y_Scale;
    long  ymax,ymin;
    long  xmax,xmin;
    long  xspan,yspan;
    double zoom;
    double aspect;
    long  detail_level;
    long  number_map_objects;
    MapObj *first_map_object;
    MapObj *last_map_object;
    MapObj *rubber_map_object; /* only one at a time */
    /* X -related items for the map widget */
    XColor *colors[NUMBER_COLORS], *old_colors[NUMBER_COLORS];
    GC gc[NUMBER_COLORS];

} Map;


/*
 * Flag bits for maps:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * CLEAR_NEEDED;		Need to clear the window when redrawing.
 */

#define REDRAW_PENDING		1
#define CLEAR_NEEDED		2
#define BACKUP_SET              4


/* indexes for identifying configspec elements  */
#define GEOMETRYSPEC   0
#define HEIGHTSPEC     1
#define WIDTHSPEC      2
#define RELIEFSPEC     3
#define BACKGROUNDSPEC 4
#define MONOBGSPEC     5
#define LATSPEC        6
#define LONSPEC        7
#define BORDWIDSPEC    8
#define BGSPEC         9
#define BORDERWIDSPEC 10
#define CURSORSPEC    11
#define MAPFILESPEC   12
#define MAPBGSPEC     13
#define COLORSBASE    14
#define COASTLINESPEC 14
#define COUNTRYSPEC   15
#define DUMMYSPEC     16
#define STATESPEC     17
#define ISLANDSPEC    18
#define LAKESPEC      19
#define RIVERSPEC     20
#define COLORSEND     21
#define ZOOMSPEC      21
#define ASPECTSPEC    22
#define DETAILSPEC    23
#define POINTTYPESPEC 24
#define MAXSPEC       24

static Tk_ConfigSpec configSpecs[] = {
  {TK_CONFIG_PIXELS, "-height", "height", "Height",
     DEF_FRAME_HEIGHT, Tk_Offset(Map, height), 0},
  {TK_CONFIG_PIXELS, "-width", "width", "Width",
     DEF_FRAME_WIDTH, Tk_Offset(Map, width), 0},
  {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
     DEF_FRAME_RELIEF, Tk_Offset(Map, relief), 0},
  {TK_CONFIG_BORDER, "-background", "background", "Background",
     "#000", Tk_Offset(Map, border), TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_BORDER, "-background", "background", "Background",
     "#fff", Tk_Offset(Map, border), TK_CONFIG_MONO_ONLY},
  {TK_CONFIG_STRING, "-latitude", "latitude", "Latitude",
     "0o0'0''N", Tk_Offset(Map, Latitude_string), 
     TK_CONFIG_NULL_OK},
  {TK_CONFIG_STRING, "-longitude", "longitude", "Longitude",
     "0o0'0''W", Tk_Offset(Map, Longitude_string), 
     TK_CONFIG_NULL_OK},
  {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
     (char *) NULL, 0, 0},
  {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
     (char *) "black", 0, 0},
  {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
     DEF_FRAME_BORDER_WIDTH, Tk_Offset(Map, borderWidth), 0},
  {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
     DEF_FRAME_CURSOR, Tk_Offset(Map, cursor), TK_CONFIG_NULL_OK},
  {TK_CONFIG_STRING, "-mapfile", "mapfile", "Mapfile",
     "", Tk_Offset(Map, map_data_file_name), 
     TK_CONFIG_NULL_OK},
  {TK_CONFIG_COLOR, "-mapbackground", "mapbackground", "MapBackground",
     "#000", Tk_Offset(Map, colors[0]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-coastlines", "coastlines", "Coastlines",
     "#0a0", Tk_Offset(Map, colors[1]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-countries", "countries", "Countries",
     "#d00", Tk_Offset(Map, colors[2]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-dummy", "dummy", "Dummy",
     "#fff", Tk_Offset(Map, colors[3]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-states", "states", "States",
     "#ff0", Tk_Offset(Map, colors[4]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-islands", "islands", "Islands",
     "#5f5", Tk_Offset(Map, colors[5]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-lakes", "lakes", "Lakes",
     "#aaf", Tk_Offset(Map, colors[6]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-rivers", "rivers", "Rivers",
     "#33f", Tk_Offset(Map, colors[7]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_COLOR, "-linecolor", "linecolor", "LineColor",
     "#fff", Tk_Offset(Map, colors[8]), 
     TK_CONFIG_NULL_OK | TK_CONFIG_COLOR_ONLY},
  {TK_CONFIG_DOUBLE, "-zoom", "zoom", "Zoom",
     "1.0", Tk_Offset(Map, zoom), 0},
  {TK_CONFIG_DOUBLE, "-aspect", "aspect", "Aspect",
     "100.0", Tk_Offset(Map, aspect), 0},
  {TK_CONFIG_INT, "-detail", "detail", "Detail",
     "0", Tk_Offset(Map, detail_level), 0},
  {TK_CONFIG_STRING, "-pointtype", "pointtype", "PointType",
     "INTEGER_MINUTES", Tk_Offset(Map, point_type), 
     TK_CONFIG_NULL_OK},
  {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
     (char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static int	ConfigureMap _ANSI_ARGS_((Tcl_Interp *interp,
		    Map *mapPtr, int argc, char **argv, int flags));
static void	DestroyMap _ANSI_ARGS_((ClientData clientData));
static void	DisplayMap _ANSI_ARGS_((ClientData clientData));
static void	MapEventProc _ANSI_ARGS_((ClientData clientData,
		    XEvent *eventPtr));
static int	MapWidgetCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv));
static void	MapMap _ANSI_ARGS_((ClientData clientData));
long      ParseMapCoord(char *coordspec);
long            GetPixLong(Map *m);
long            GetPixLat(Map *m);

int map_posted = 0;


/*
 *--------------------------------------------------------------
 *
 * Tk_MapCmd --
 *
 *	This procedure is invoked to process the "map" 
 *	Tcl command.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *      Generally, however, it behaves just like a frame.
 *
 *--------------------------------------------------------------
 */

int
Tk_MapCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin;
    Tk_Window new;
    Tk_Uid screenUid;
    char *className, *screen;
    int src, dst, i;
    register Map *mapPtr;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (clientData != NULL) 
      tkwin = (Tk_Window) clientData;
    else
      tkwin = Tk_MainWindow(interp);
    /*
     * Create the window and initialize our structures and event handlers.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *)NULL);

    if (new == NULL) {
	return TCL_ERROR;
    }

    Tk_SetClass(new, "Map");

    mapPtr = (Map *) calloc(1,sizeof(Map));
    mapPtr->tkwin = new;
    mapPtr->display = Tk_Display(tkwin);
    mapPtr->interp = interp;
    mapPtr->relief = TK_RELIEF_FLAT;
    mapPtr->cursor = None;

    Tk_CreateEventHandler(mapPtr->tkwin, ExposureMask|StructureNotifyMask,
	    MapEventProc, (ClientData) mapPtr);

    Tcl_CreateCommand(interp, Tk_PathName(mapPtr->tkwin),
	    MapWidgetCmd, (ClientData) mapPtr, (void (*)()) NULL);

    if (ConfigureMap(interp, mapPtr, argc-2, argv+2, 0) != TCL_OK) {
      /* Tk_DestroyWindow(mapPtr->tkwin); */
      return TCL_ERROR;
    }

    /* probably don't need this, but... */
    Tk_DoWhenIdle(MapMap, (ClientData) mapPtr);

    interp->result = Tk_PathName(mapPtr->tkwin);

    return TCL_OK;

}


/*
 *--------------------------------------------------------------
 *
 * MapWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a map widget.  See the user
 *	documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

static int
MapWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about map widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Map *mapPtr = (Map *) clientData;
    int result = TCL_OK;
    int length;
    char c;
    double tempdouble;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) mapPtr);
    c = argv[1][0];
    length = strlen(argv[1]);

    switch (c) {
    case 'c':
      if ((strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	  result = Tk_ConfigureInfo(interp, mapPtr->tkwin, configSpecs,
				    (char *) mapPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	  result = Tk_ConfigureInfo(interp, mapPtr->tkwin, configSpecs,
				    (char *) mapPtr, argv[2], 0);
	} else {
	  result = ConfigureMap(interp, mapPtr, argc-2, argv+2,
				TK_CONFIG_ARGV_ONLY);
	}
      } 
      else if  ((strncmp(argv[1], "centerpixel", length) == 0)) {
	int arg2, arg3;
	/* center on the specified point */
        if (argc != 4) {
	  Tcl_AppendResult(interp, "wrong # args: should be \"",
			   argv[0], " centerpixel x_pixel_num y_pixel_num\"", 
			   (char *) NULL);
	  return TCL_ERROR;
	}
	arg2 = atoi(argv[2]);
	arg3 = atoi(argv[3]);

	if (arg2 >= 0)
	    mapPtr->x_center_pixel = arg2;
	if (arg3 >= 0)
	    mapPtr->y_center_pixel = arg3;

	if (mapPtr->x_center_pixel != mapPtr->last_x_center 
	    || mapPtr->y_center_pixel != mapPtr->last_y_center) {
	  /* set lat and lon from a mouse click or whatever */
	  
	  if ( arg3 >= 0) {
	    long deg, min, sec , tmp, sign;
	    mapPtr->Latitude = GetPixLat(mapPtr);
	    if (mapPtr->Latitude < 0) 
	      sign = -1;
	    else 
	      sign = 1;
	    tmp = abs(mapPtr->Latitude);
	    deg = tmp / 3600;
            min = (tmp % 3600)/ 60;
	    sec = (tmp % 3600) % 60;

	    ckfree(mapPtr->Latitude_string); 
	    mapPtr->Latitude_string = (char *)ckalloc(15); 
	    sprintf(mapPtr->Latitude_string, "%do%d'%d''%s",
		    deg, min, sec, ((sign > 0) ? "N":"S"));

	    mapPtr->last_y_center =  mapPtr->y_center_pixel; 
	  }
	  
	  if ( arg2 >= 0) {
	    long deg, min, sec, tmp, sign;
	    mapPtr->Longitude = GetPixLong(mapPtr);
	    if (mapPtr->Longitude < 0) 
	      sign = -1;
	    else 
	      sign = 1;
	    tmp = abs(mapPtr->Longitude);
	    deg = tmp / 3600;
            min = (tmp % 3600)/ 60;
	    sec = (tmp % 3600) % 600;

	    ckfree(mapPtr->Longitude_string); 
	    mapPtr->Longitude_string = (char *)ckalloc(15); 
	    sprintf(mapPtr->Longitude_string, "%do%d'%d''%s",
		    deg, min, sec, ((sign > 0) ? "E":"W"));

	    mapPtr->last_x_center = mapPtr->x_center_pixel;
	  }
	  if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	    Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	    mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	  }
	}
      }
      else if  ((strncmp(argv[1], "create", length) == 0)) {
	int objnumber;
	char temp[20];
	/* check if this create command is OK */
        if (argc < 4) {
	  Tcl_AppendResult(interp, "wrong # args: should be \"",
			   argv[0], " create <objtype> <color> <coord set>\"", 
			   (char *) NULL);
	  return TCL_ERROR;
	}

	objnumber = Map_create_obj(clientData, interp, argc, argv);
        if (objnumber < 0) {
	  return TCL_ERROR;
	}
	/* re-draw the map */
	if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	  Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	  mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	}
	/* send back the object number */
	sprintf(temp, "%d",objnumber);
	Tcl_AppendElement(interp, temp ); 

      }

      break;

    case 'g':
      if (strncmp(argv[1], "get", length) == 0) {
	Tcl_AppendResult(interp, "argument ambiguous: should be \"",
			 argv[0], " getcenter | getlatitude | getlongitude |",
			 " getframe | getxy\"", 
			 (char *) NULL);
	return TCL_ERROR;
      }
      if (strncmp(argv[1], "getcenter", length) == 0) {
	Tcl_AppendElement(interp, mapPtr->Latitude_string ); 
	Tcl_AppendElement(interp, mapPtr->Longitude_string ); 
      }
      else if (strncmp(argv[1], "getlatitude", length) == 0) {
	Tcl_AppendElement(interp, mapPtr->Latitude_string ); 
      }
      else if (strncmp(argv[1], "getlongitude", length) == 0) {
	Tcl_AppendElement(interp, mapPtr->Longitude_string ); 
      }
      else if (strncmp(argv[1], "getframe", length) == 0) {
	long toplat,toplon, botlat, botlon;
	char temp[15];

	/* save the old values */
	mapPtr->last_x_center = mapPtr->x_center_pixel;
	mapPtr->last_y_center =  mapPtr->y_center_pixel; 

	/* get the top-left corner coordinates */
	mapPtr->x_center_pixel = 0;
	mapPtr->y_center_pixel = 0;
	toplat = GetPixLat(mapPtr);
	toplon = GetPixLong(mapPtr);

	/*convert to string */
	sprintf(temp, "%do%d'%d''%s",
		    abs(toplat / 3600), 
		    abs((toplat % 3600) / 60),
		    abs((toplat % 3600) % 60), 
		    ((toplat > 0) ? "N":"S"));
	/* append to the results */
	Tcl_AppendElement(interp, temp); 

	/* same for longitude */
	sprintf(temp, "%do%d'%d''%s",
		abs(toplon / 3600), 
		abs((toplon % 3600) / 60),
		abs((toplon % 3600) % 60), 
		((toplon > 0) ? "E":"W"));

	Tcl_AppendElement(interp, temp ); 
	
	/* now get the lower-right corner */
	mapPtr->x_center_pixel = Tk_Width(mapPtr->tkwin) - 1 ;
	mapPtr->y_center_pixel = Tk_Height(mapPtr->tkwin) - 1 ;
	botlat = GetPixLat(mapPtr);
	botlon = GetPixLong(mapPtr);

	sprintf(temp, "%do%d'%d''%s",
		    abs(botlat / 3600), 
		    abs((botlat % 3600) / 60),
		    abs((botlat % 3600) % 60), 
		    ((botlat > 0) ? "N":"S"));
	Tcl_AppendElement(interp, temp); 

	sprintf(temp, "%do%d'%d''%s",
		abs(botlon / 3600), 
		abs((botlon % 3600) / 60),
		abs((botlon % 3600) % 60), 
		((botlon > 0) ? "E":"W"));

	Tcl_AppendElement(interp, temp ); 

	/* restore the old x and y */
	mapPtr->x_center_pixel = mapPtr->last_x_center;
	mapPtr->y_center_pixel = mapPtr->last_y_center;

      }

      else if (strncmp(argv[1], "getlatlon", length) == 0) {
	long lat, lon;
	char temp[15];

	if (argc < 4) {
	  Tcl_AppendResult(interp, "Wrong # args: should be \"",
			 argv[0], " getlatlon x_pixel_num y_pixel_num\"", 
			 (char *) NULL);
	  return TCL_ERROR;
	}
	/* save the old values */
	mapPtr->last_x_center = mapPtr->x_center_pixel;
	mapPtr->last_y_center =  mapPtr->y_center_pixel; 

	/* get the top-left corner coordinates */
	mapPtr->x_center_pixel = atol(argv[2]);
	mapPtr->y_center_pixel = atol(argv[3]);
	lat = GetPixLat(mapPtr);
	lon = GetPixLong(mapPtr);

	/*convert to string */
	sprintf(temp, "%do%d'%d''%s",
		    abs(lat / 3600), 
		    abs((lat % 3600) / 60),
		    abs((lat % 3600) % 60), 
		    ((lat > 0) ? "N":"S"));
	/* append to the results */
	Tcl_AppendElement(interp, temp); 

	/* same for longitude */
	sprintf(temp, "%do%d'%d''%s",
		abs(lon / 3600), 
		abs((lon % 3600) / 60),
		abs((lon % 3600) % 60), 
		((lon > 0) ? "E":"W"));

	Tcl_AppendElement(interp, temp ); 
      }
      else if (strncmp(argv[1], "getxy", length) == 0) {
	if (argc < 4) {
	  Tcl_AppendResult(interp, "Wrong # args: should be \"",
			 argv[0], " getxy latitude longitude\"", 
			 (char *) NULL);
	  return TCL_ERROR;
	}
	GetMapXYCmd(mapPtr, argv[2], argv[3]);
       
      }
      break;

    case 'r':
    case 'd':
      if ((strncmp(argv[1], "redisplay", length) == 0)
	  ||(strncmp(argv[1], "display", length) == 0) ) {
	/* re-draw the map */
	if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	  Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	  mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	}
      }
      else if ((strncmp(argv[1], "destroy", length) == 0)) {
	if (Map_destroy_obj(clientData, interp, argc, argv) < 0) {
	  return TCL_ERROR;
	}
	/* re-draw the map */
	if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	  Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	  mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	}
      }
      else if (strncmp(argv[1], "distance", length) == 0) {
	long lat, lon;
	double D, cosD, Lat1, Lat2, Lon1, Lon2, DifLon, distance; 
	char temp[15];

	/* This widget command calculates the map distance between */
	/* two pixels on the screen in miles using the great circle */
        /* method                                                   */
	if (argc < 6) {
	  Tcl_AppendResult(interp, "Wrong # args: should be \"",
			 argv[0], " distance x_pixel_num1 y_pixel_num1"
			   " x_pixel_num2 y_pixel_num2\"", 
			 (char *) NULL);
	  return TCL_ERROR;
	}
	/* save the old values */
	mapPtr->last_x_center = mapPtr->x_center_pixel;
	mapPtr->last_y_center =  mapPtr->y_center_pixel; 

	/* get the first pixel coordinates */
	mapPtr->x_center_pixel = atol(argv[2]);
	mapPtr->y_center_pixel = atol(argv[3]);
	lat = GetPixLat(mapPtr);
	lon = GetPixLong(mapPtr);
	/* convert to decimal degrees and then radians */
#ifdef ALPHA
	Lon1 = ((float)lon /3600.0);
	Lat1 = ((float)lat /3600.0);
#else
	Lon1 = ((float)lon /3600.0)*(M_PI/180.00);
	Lat1 = ((float)lat /3600.0)*(M_PI/180.00);
#endif
	/* get the other coordinates */
	mapPtr->x_center_pixel = atol(argv[4]);
	mapPtr->y_center_pixel = atol(argv[5]);
	lat = GetPixLat(mapPtr);
	lon = GetPixLong(mapPtr);
	/* convert to decimal degrees and then radians*/
#ifdef ALPHA
	Lon2 = ((float)lon /3600.0);
	Lat2 = ((float)lat /3600.0);
#else
	Lon2 = ((float)lon /3600.0)*(M_PI/180.00);
	Lat2 = ((float)lat /3600.0)*(M_PI/180.00);
#endif

	/* get D -- the angle of arc between the points */
#ifndef ALPHA
	cosD = sin(Lat1) * sin(Lat2) + cos(Lat1) * cos(Lat2)
	  * cos(Lon1 - Lon2);
#else
	cosD = sind(Lat1) * sind(Lat2) + cosd(Lat1) * cosd(Lat2)
	  * cosd(Lon1 - Lon2);
#endif
	/* the distance along the arc is then...*/
#ifndef ALPHA       
	D = acos(cosD);
	distance = 0.5 * 7915.59 * D;
	/* Nautical = distance = 0.5 * 7915.59 * 0.86838 * D; */
#else
	distance = 0.5 * 7915.59 * (acosd(cosD) * (M_PI/180.0));
	/* Nautical = distance = 0.5 * 7915.59 * 0.86838 * (acosd(cosD) * (M_PI/180.0));*/
#endif
	
	/* return the distance */
	sprintf(temp, "%f", distance);

	Tcl_AppendElement(interp, temp ); 
      }
      else if  ((strncmp(argv[1], "rubberband", length) == 0)) {
	int objnumber;
	char temp[20];

	/* check if this command is OK */
	if ((argc == 3) && strcmp(argv[2], "end") == 0) {
	  objnumber = mapPtr->rubber_map_object->ID;
	  /* turn off rubberbanding */
	  mapPtr->rubber_map_object->rubber = 0;
	  mapPtr->rubber_map_object = NULL;
	}
        else {
	  if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
			     argv[0], " rubberband <objtype> <finalcolor> <coord set>\"", 
			     (char *) NULL);
	    return TCL_ERROR;
	  }
	  
	  if (mapPtr->rubber_map_object == NULL) {
	    objnumber = Map_create_obj(clientData, interp, argc, argv);
	    if (objnumber < 0) {
	      return TCL_ERROR;
	    }
	    mapPtr->rubber_map_object = mapPtr->last_map_object;
	    mapPtr->rubber_map_object->rubber = 1;
	  }
	  else {
	    objnumber = mapPtr->rubber_map_object->ID;
	    set_rubberband_points (mapPtr, argc, argv);
	  }
	  
	}
	/* re-draw the map */
	if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	  Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	  mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	}
	
	/* send back the object number */
	sprintf(temp, "%d",objnumber);
	Tcl_AppendElement(interp, temp ); 
	
      }      
      else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
			 "\":  must be configure, display, getcenter, ",
			 "getlatitude, getlongitude, getxy, getframe, ",
			 "centerpixel, create, rubberband, destroy or zoommap",
			 (char *) NULL);
	result = TCL_ERROR;
      }
      
      break;
      
    case 'z':
      if ((strncmp(argv[1], "zoommap", length) == 0)) {
	/* zoom in to the coordinates */
        if (argc != 3) {
	  Tcl_AppendResult(interp, "wrong # args: should be \"",
			   argv[0], " zoommap [+|-]zoomval\"", 
			   (char *) NULL);
	  return TCL_ERROR;
	}

	Tcl_GetDouble(interp, argv[2], &tempdouble);
	if (*argv[2] == '+' || *argv[2] == '-')
	  mapPtr->zoom += tempdouble;
	else 
	  mapPtr->zoom = tempdouble;

        /* fix over and under flows */
	if (mapPtr->zoom < 0.45)
	  mapPtr->zoom = 0.45;
	if (mapPtr->zoom > 500.00)
	  mapPtr->zoom = 500.00;

	if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
	  Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	  mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
	}
      }
      break;
      
    default:
      Tcl_AppendResult(interp, "bad option \"", argv[1],
		       "\":  must be configure, display, getcenter, ",
		       "getlatitude, getlongitude, getxy, getframe, ",
		       "centerpixel, create, rubberband, destroy or zoommap",
		       (char *) NULL);
      result = TCL_ERROR;
    }
    Tk_Release((ClientData) mapPtr);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyMap --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a map at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the map is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyMap(clientData)
    ClientData clientData;	/* Info about map widget. */
{
    register Map *mapPtr = (Map *) clientData;
    register int i;

    Tk_FreeOptions(configSpecs, (char *) mapPtr, mapPtr->display, 0);

    /* free the graphics contexts */
    for (i = 0; i < 8; i++)
      Tk_FreeGC(mapPtr->display, mapPtr->gc[i]);

    /* free the map data */
    if (mapPtr->points != NULL)
      ckfree((char *) mapPtr->points);

    /* free any map objects */
    free_map_obj(mapPtr,mapPtr->first_map_object);
    /* free the structure */
    ckfree((char *) mapPtr);
}

free_map_obj(Map *mapPtr, MapObj *obj) 
{
  if (obj == NULL) return;

  free_map_obj(mapPtr, obj->next_obj);

  Tk_FreeGC(mapPtr->display, obj->gc);

  if (obj->color != NULL) {
    Tk_FreeColor(obj->color);
  }

  if (obj->xpoints) free(obj->xpoints);
  free_obj_pnts(obj->first_pnt);

  free(obj);
}

free_obj_pnts(MapObjPnt *pnt) 
{
  if (pnt == NULL) return;

  free_obj_pnts(pnt->next_pnt);
  
  free(pnt);
}


/*
 *----------------------------------------------------------------------
 *
 * ConfigureMap --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a map widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for mapPtr;  old resources get freed, if there
 *	were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureMap(interp, mapPtr, argc, argv, flags)
     Tcl_Interp *interp;		/* Used for error reporting. */
     register Map *mapPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
     int argc;			/* Number of valid entries in argv. */
     char **argv;		/* Arguments. */
     int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
  
  XGCValues values;
  unsigned int attmask;
  XSetWindowAttributes atts;
  int i, pointsread;
  long templat, templon;
  
  if (Tk_ConfigureWidget(interp, mapPtr->tkwin, configSpecs,
			 argc, argv, (char *) mapPtr, flags) != TCL_OK) {
    return TCL_ERROR;
  }
  
  Tk_SetBackgroundFromBorder(mapPtr->tkwin, mapPtr->border);
  Tk_SetInternalBorder(mapPtr->tkwin, mapPtr->borderWidth);
  
  if (configSpecs[GEOMETRYSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED) {
    int height, width;   
    if (mapPtr->geometry) {
      if (sscanf(mapPtr->geometry, "%dx%d", &width, &height) != 2) {
	Tcl_AppendResult(interp, "bad geometry \"", mapPtr->geometry,
			 "\": expected widthxheight", (char *) NULL);
	return TCL_ERROR;
      }
      Tk_GeometryRequest(mapPtr->tkwin, width, height);
    }
  }
  if ((configSpecs[WIDTHSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED)
      || (configSpecs[HEIGHTSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED)) {
    Tk_GeometryRequest(mapPtr->tkwin, mapPtr->width,
		       mapPtr->height);
  }
  
  /* arrange for automatic backing store of window   */
  /* NOTE: this doesn't behave well on all X servers */
  if ((mapPtr->flags & BACKUP_SET) == 0) {
    attmask = CWBackingStore;
    atts.backing_store = Always;
    Tk_ChangeWindowAttributes(mapPtr->tkwin,attmask,&atts);
    
    /* force it to be displayed */
    Tk_MakeWindowExist(mapPtr->tkwin);
    mapPtr->flags |= BACKUP_SET;
  }
  
  if ((configSpecs[LATSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED)
      || (configSpecs[LONSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED)) {
    if (mapPtr->Latitude_string != NULL)
      templat = ParseMapCoord(mapPtr->Latitude_string);
    
    if (mapPtr->Longitude_string != NULL)
      templon = ParseMapCoord(mapPtr->Longitude_string);
    
    if (templon != mapPtr->Longitude || templat != mapPtr->Latitude) {
      mapPtr->Longitude = templon;
      mapPtr->Latitude = templat;
      Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
      mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
    }
  }
  
  if (configSpecs[DETAILSPEC].specFlags | TK_CONFIG_OPTION_SPECIFIED) {
    if (mapPtr->detail_level < 0) 
      mapPtr->detail_level = 0;
    if (mapPtr->detail_level > 10) 
      mapPtr->detail_level = 10;
    
  }
  
  /* set up the graphics contexts for drawing different colors */
  for (i=0; i < NUMBER_COLORS; i++) {
    if (mapPtr->colors[i] != mapPtr->old_colors[i]) {
      /* there is a new color value -- need to redraw */
      mapPtr->old_colors[i] = mapPtr->colors[i];
      if (mapPtr->gc[i] != NULL) {
	Tk_FreeGC(mapPtr->display, mapPtr->gc[i]);
      }
      values.foreground = mapPtr->colors[i]->pixel;
      mapPtr->gc[i] = Tk_GetGC(mapPtr->tkwin, GCForeground, &values);
    }
    if ((mapPtr->flags & REDRAW_PENDING) != REDRAW_PENDING) {
      Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
      mapPtr->flags |= REDRAW_PENDING | CLEAR_NEEDED ;
    }
  }
  
  if (mapPtr->points == NULL && mapPtr->mapfileread == FALSE) { 
    /* must read the file */
    if (mapPtr->map_data_file_name == NULL) {
      Tcl_AppendResult(interp, "No map data file name specified",
		       (char *) NULL);
      return TCL_ERROR;
    }
    else { /* have a file name */
      if (*mapPtr->map_data_file_name == '@') { 
	/* do postgres large object reads */
#ifdef USEPOSTGRES
	(void) PQexec("begin");
	if ((filein = p_open(argv[2], INV_READ)) < 0) {
	  Tcl_AppendResult(interp,
			   "Unable to open map database file", 
			   (char *) NULL);
	  (void) PQexec("end");
	  return TCL_ERROR;
	}
	mapPtr->points = (int) p_read(filein, p, PCount*PNTSiz);
	p_close(filein);
	(void) PQexec("end");
#else
	Tcl_AppendResult(interp,
			 "Unable to open map database file: ",
			 "POSTGRES use not enabled for map widget",
			 (char *) NULL);
	return TCL_ERROR;
#endif
      }
      else { /* read from a unix file */
	if ((mapPtr->map_data_file =
#ifndef WIN32
	     fopen(mapPtr->map_data_file_name , "r")) == NULL) {
#else
	     fopen(mapPtr->map_data_file_name , "rb")) == NULL) {
#endif
	   Tcl_AppendResult(interp,
			    "Unable to open map database file", 
			    (char *) NULL);
	   return TCL_ERROR; 
        }
      /* find out how big the file is...*/
      fseek(mapPtr->map_data_file,0, SEEK_END);
      mapPtr->mapfilepoints = 
	(ftell(mapPtr->map_data_file) / sizeof(Point));
      
      /* allocate space for the data */
      mapPtr->points = 
	(Point *) ckalloc(mapPtr->mapfilepoints*sizeof(Point)+1);
      /* read it all into the structure */
      fseek(mapPtr->map_data_file,0, SEEK_SET);
      
      pointsread = fread(mapPtr->points, sizeof(Point), 
			 mapPtr->mapfilepoints, 
			 mapPtr->map_data_file); 
      
      /* and that's it */
      fclose(mapPtr->map_data_file);
      
      if (pointsread != mapPtr->mapfilepoints) {
	Tcl_AppendResult(interp, "Map file data incomplete or inconsistant",
			 (char *) NULL);
	return TCL_ERROR;
      }
#ifdef DOENDIANCONVERSION 
      /* this doesn't work yet...                                 */
      /* if we have a bigendian machine vs a littleendian machine */
      if (mapPtr->points[0].Code != 1001) {
	for (i = 0; i < mapPtr->mapfilepoints; i++) {
	  unsigned short y;
          unsigned short tmpcode;
	  unsigned short tmplon;
          unsigned short tmplat;
	  
	  y = (unsigned short)mapPtr->points[i].Code << 8;
	  tmpcode = (unsigned short)y | (mapPtr->points[i].Code >> 8);
	  y = (unsigned short) mapPtr->points[i].Lat << 8;
	  tmplat = (unsigned short) y | (mapPtr->points[i].Lat >> 8);
	  y = (unsigned short) mapPtr->points[i].Lon << 8;
	  tmplon = (unsigned short) y | (mapPtr->points[i].Lon >> 8);
	  mapPtr->points[i].Code = tmpcode;
	  mapPtr->points[i].Lon = tmplon;
	  mapPtr->points[i].Lat = tmplat;

	}
      }
#endif

    }
    mapPtr->mapfileread = TRUE;
  }
  
  }

   if (Tk_IsMapped(mapPtr->tkwin)
     && !(mapPtr->flags & REDRAW_PENDING)) {
     Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
     mapPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
   }

  return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayMap --
 *
 *	This procedure is invoked to display a map widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the map in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayMap(clientData)
     ClientData clientData;	/* Information about widget. */
{
   register Map *mapPtr = (Map *) clientData;
   register Tk_Window tkwin = mapPtr->tkwin;
   long win_width, win_height, winw, winh;
   register int x, y;
   register Point *pp;
   Point *pend;
   int gcindex;
   int	z, LonPrv=0, LatPrv=0, xprv=0, yprv=0;
   int	newseg=0;

   Pixmap pm;
  
  /* turn off the redraw flag */
  mapPtr->flags &= ~REDRAW_PENDING;
  
  if ((mapPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
    return;
  }
  
  pm = Tk_GetPixmap(mapPtr->display, Tk_WindowId(tkwin),
		     Tk_Width(tkwin), Tk_Height(tkwin),
		     Tk_Depth(tkwin));

  
  Tk_Fill3DRectangle(tkwin, pm,
		     mapPtr->border, 0, 0, 
		     Tk_Width(tkwin), Tk_Height(tkwin),
		     mapPtr->borderWidth, mapPtr->relief);

  /* set width and height for the window */
  win_width = Tk_Width(tkwin);
  win_height = Tk_Height(tkwin);
  
  winw = win_width / 2;
  winh = win_height / 2;
  mapPtr->X_Scale = mapPtr->zoom * mapPtr->aspect/(10800.00/(float)winw);
  /* e.g. 33.75 = 10800 / 320  for 640x400*/
  mapPtr->Y_Scale = mapPtr->zoom * 100.0 /(5400.00/(float)winh);
  /* e.g. 27.00 =  5400 / 200  for 640x400*/
  mapPtr->ymax = QSPAN/mapPtr->zoom;
  mapPtr->ymin = -mapPtr->ymax;
  mapPtr->xmax = HSPAN / (mapPtr->zoom * (mapPtr->aspect / 100.0));
  mapPtr->xmin = -mapPtr->xmax;
  mapPtr->xspan = mapPtr->xmax - mapPtr->xmin;
  mapPtr->yspan = mapPtr->ymax - mapPtr->ymin;
  
  pend = mapPtr->points + mapPtr->mapfilepoints;
  
  for (pp = mapPtr->points; pp < pend; pp++) { /* do displacement	*/
    if (pp->Code < mapPtr->detail_level)
      continue;

    x = ((int)pp->Lon)*60 - mapPtr->Longitude;
    y = ((int)pp->Lat)*60 - mapPtr->Latitude;
    /* wrap around for East-West	*/
    if (x < -HSPAN)
      x += WSPAN;
    else if (x > HSPAN)
      x -= WSPAN;
    /* ignore pts outside magnified area	*/
    if((x < mapPtr->xmin || x > mapPtr->xmax 
	|| y < mapPtr->ymin || y > mapPtr->ymax)) {
      if ((int)pp->Code > 5)
	gcindex =  (int)pp->Code / 1000;
      newseg = 1;
      continue;
    }
    if (abs(x - LonPrv) >= mapPtr->xspan 
	|| abs(y - LatPrv) >= mapPtr->yspan)
      newseg = 1;
    LonPrv = x;
    LatPrv = y;
    /* scale pts w/in area of a window */
    x = winw + (x * mapPtr->X_Scale)/ADJPIX;
    y = winh - (y * mapPtr->Y_Scale)/ADJPIX;
    /* x = winw + (x * mapPtr->X_Scale)/10000; */
    /* y = winh - (y * mapPtr->Y_Scale)/10000; */
    /* ignore duplicates	*/
    if (x == xprv && y == yprv)
      continue;
    /* if over edge, set for return	*/
    if (newseg || (int)pp->Code > 5 
	|| abs(z - x) > (winw - 5)) {
      if ((int)pp->Code > 5)
	gcindex =  (int)pp->Code / 1000;
	if (gcindex > 7) {
           printf("This is a BAD data point -- is the data in inverted byte order?\n");           
        }
        else {
#ifndef WIN32 
         XDrawPoint(mapPtr->display, pm,
		     mapPtr->gc[gcindex], x, y);
#else
         /* No drawpoint implementation for Win */
         XDrawLine(mapPtr->display, pm,
		    mapPtr->gc[gcindex], x, y, x, y);
#endif
      }
      newseg = 0;
    }
    else XDrawLine(mapPtr->display, pm,
		   mapPtr->gc[gcindex], xprv, yprv, x, y);
    xprv = x;
    yprv = y;
    z = x;
  }

  Display_Map_Objects (clientData, pm);

  XClearWindow(mapPtr->display, Tk_WindowId(tkwin));


  XCopyArea(mapPtr->display, pm, Tk_WindowId(tkwin), mapPtr->gc[gcindex],
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);

  Tk_FreePixmap(mapPtr->display, pm);



}


Display_Map_Objects (clientData, pm)
    ClientData clientData;	/* Information about map widget. */
    Pixmap pm; /* the drawable pixmap */
{
  register Map *mapPtr = (Map *) clientData;
  register Tk_Window tkwin = mapPtr->tkwin;
  MapObj *mo;
  
  for (mo = mapPtr->first_map_object; mo; mo = mo->next_obj) {
    Map_set_obj_points(clientData, mo);
    Map_draw_obj(clientData, mo, pm);
  }

}


/*
 *----------------------------------------------------------------------
 *
 * Map_Create_Obj --
 *
 *	This procedure is invoked to create drawable object in a map widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the object in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */
int Map_create_obj (clientData, interp, argc, argv)
    ClientData clientData;	/* Information about map widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
  register Map *mapPtr = (Map *) clientData;
  MapObj  *obj;
  MapObjPnt *pnt;
  XColor *newPtr;
  XGCValues values;
  unsigned int attmask;
  XSetWindowAttributes atts;
  int arg2len;
  int object_id;
  int lat_sec, lon_sec;
  int type, i;
  unsigned long mask = 0;
  Tk_Uid uid;
  Pixmap Obj_fillStipple  = 0;
  Pixmap Rect_fillStipple = 0;

#define grey_width 16
#define grey_height 16
static char grey1_bits[] = {
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa};

static char grey2_bits[] = {
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44};

  arg2len = strlen(argv[2]);

  if ((strncmp(argv[2], "line", arg2len) == 0)) {
    if (argc != 8) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create line color lat1 lon1 lat2 lon2\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_LINE;
    obj->npoints = 2;
    obj->close = 0;

  }
  else if ((strncmp(argv[2], "rect", arg2len) == 0) 
	   || (strncmp(argv[2], "rectangle", arg2len) == 0)) {
    if (argc != 8) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create rect color toplat toplon botlat botlon\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_RECT;
    obj->npoints = 2;
    obj->xpoints = (XPoint *)calloc(2,sizeof(XPoint));
    obj->close = 1;

  }
  else if ((strncmp(argv[2], "filledrect", arg2len) == 0) 
	   || (strncmp(argv[2], "filledrectangle", arg2len) == 0)) {
    if (argc != 8) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create filledrectangle color toplat toplon botlat botlon\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_FRECT;
    obj->npoints = 2;
    obj->xpoints = (XPoint *)calloc(2,sizeof(XPoint));
    obj->close = 1;

  }
  else if ((strncmp(argv[2], "polygon", arg2len) == 0)) {
    if (argc < 8) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create polygon color lat1 lon1 ... latN lonN\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_POLY;
    obj->npoints = (argc - 4)/2 ;
    obj->xpoints = (XPoint *)calloc(obj->npoints,sizeof(XPoint));
    obj->close = 1;
  }
  else if ((strncmp(argv[2], "polyline", arg2len) == 0)) {
    if (argc < 8) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create polyline color lat1 lon1 lat2 lon2 ...\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_PLINE;
    obj->npoints = (argc - 4)/2 ;
    obj->xpoints = (XPoint *)calloc(obj->npoints,sizeof(XPoint));
    obj->close = 0;

  }
  else if ((strncmp(argv[2], "point", arg2len) == 0)) {
    if (argc < 4) {
      Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " create point color lat lon\"", 
		       (char *) NULL);
      return -1;
    }
    obj = (MapObj *) calloc(1,sizeof(MapObj));
    obj->type = MAPOBJ_PNT;
    obj->npoints = 1;
    obj->close = 0;

  }
  else {
    Tcl_AppendResult(interp, "object type \"",
		     argv[2], "\" not known -- should be ",
		     " point, line, rectangle, polygon, polyline",
		     (char *) NULL);
    return -1;
  }


  /* set map obj id number from map struct */
  obj->ID = mapPtr->number_map_objects++;


  /* register and allocate color and GC */
  uid = Tk_GetUid(argv[3]);

  newPtr = Tk_GetColor(interp, mapPtr->tkwin, uid);
  
  if (newPtr == NULL) {
    Tcl_AppendResult(interp, "Bad color value: ", argv[3], 
		     (char *) NULL);
    return -2;
  }
  obj->color = newPtr;
  values.foreground = obj->color->pixel;
  
  if (obj->type == MAPOBJ_POLY) {
    if (Obj_fillStipple == 0) {
      Obj_fillStipple = XCreateBitmapFromData(mapPtr->display,
					      Tk_WindowId(mapPtr->tkwin),
						grey1_bits, grey_width, 
					      grey_height);
    }
    values.stipple = Obj_fillStipple;
    values.fill_style = FillStippled;
    mask |= GCStipple|GCFillStyle|GCForeground;
  }
  else if (obj->type == MAPOBJ_FRECT) {
    if (Rect_fillStipple == 0) {
      Rect_fillStipple = XCreateBitmapFromData(mapPtr->display,
					       Tk_WindowId(mapPtr->tkwin),
					       grey2_bits, grey_width, 
					       grey_height);
    }
    values.stipple = Rect_fillStipple;
    values.fill_style = FillStippled;
    mask |= GCStipple|GCFillStyle|GCForeground;
  }
  else {
    mask = GCForeground ;
  }
  
  obj->gc = Tk_GetGC(mapPtr->tkwin, mask, &values);
  
  /* get the points */
  for (i = 4 ; i < argc ; i += 2) {
    
    pnt = (MapObjPnt *) calloc(1,sizeof(MapObjPnt));
    
    lat_sec = ParseMapCoord(argv[i]);
    lon_sec = ParseMapCoord(argv[i+1]);
    
    /* these return somewhat strange values -- convert to float */
    
    pnt->lon = (float)lon_sec /3600.0;
    pnt->lat = (float)lat_sec /3600.0;
    pnt->next_pnt = NULL;
    if (obj->first_pnt == NULL) {
      obj->first_pnt = pnt;
      obj->last_pnt = pnt;
    }
    else {
      obj->last_pnt->next_pnt = pnt;
      obj->last_pnt = pnt;
    }    
  }
  
  if (mapPtr->first_map_object == NULL) {
      mapPtr->first_map_object = obj;
      mapPtr->last_map_object = obj;
    }
  else {
    mapPtr->last_map_object->next_obj = obj;
    mapPtr->last_map_object = obj;
  }
  
  return(obj->ID);
}



int Map_destroy_obj (clientData, interp, argc, argv)
    ClientData clientData;	/* Information about map widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
  register Map *mapPtr = (Map *) clientData;
  MapObj  *obj, *del_obj, *prior_obj;
  MapObjPnt *pnt;


  int objectnum;

  
  if (argc != 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		       argv[0], " destroy <objectnumber>\"", 
		       (char *) NULL);
      return -1;
    }
  if (sscanf(argv[2],"%d",&objectnum) != 1) {
    Tcl_AppendResult(interp, "Invalid object number: should be \"",
		     argv[0], " destroy <objectnumber>\"", 
		     (char *) NULL);
    return -1;
  }
  
  del_obj = NULL;

  if (mapPtr->first_map_object 
      && mapPtr->first_map_object->ID == objectnum) {
    del_obj = mapPtr->first_map_object;
    mapPtr->first_map_object = del_obj->next_obj;
    prior_obj = mapPtr->first_map_object;
  }
  for (obj = mapPtr->first_map_object; obj ; obj = obj->next_obj) {
    if (obj->next_obj) {
      if(obj->next_obj->ID == objectnum) {
	prior_obj = obj;
	del_obj = obj->next_obj ;
	obj->next_obj = del_obj->next_obj;
	break;
      }
    }
  }

  if (del_obj == mapPtr->last_map_object) {
    if (prior_obj != NULL)
      mapPtr->last_map_object = prior_obj;
    else
      mapPtr->last_map_object = NULL;
  }
  if (del_obj) {
    del_obj->next_obj = NULL;
    free_map_obj(mapPtr, del_obj);
  }
    
  return(objectnum);
}


/*
 *----------------------------------------------------------------------
 *
 * set_rubberband_pnts --
 *
 *	This procedure is invoked to create drawable object in a map widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the object in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */
int set_rubberband_points (mapPtr, argc, argv)
     Map *mapPtr;
     int argc;			/* Number of arguments. */
     char **argv;		/* Argument strings. */
{
  MapObj  *obj;
  MapObjPnt *pnt, *tmpfirst;

  int lat_sec, lon_sec;
  int type, i;

  /* get the points */

  obj = mapPtr->rubber_map_object;

  tmpfirst = obj->first_pnt;
  obj->first_pnt = NULL;
  obj->last_pnt = NULL;

  for (i = 4 ; i < argc ; i += 2) {
    
    pnt = (MapObjPnt *) calloc(1,sizeof(MapObjPnt));
    lat_sec = ParseMapCoord(argv[i]);
    lon_sec = ParseMapCoord(argv[i+1]);
      
    /* these return somewhat strange values -- convert to float */
    pnt->lon = (float)lon_sec /3600.0;
    pnt->lat = (float)lat_sec /3600.0;
    pnt->next_pnt = NULL;
    if (obj->first_pnt == NULL) {
      obj->first_pnt = pnt;
      obj->last_pnt = pnt;
    }
    else {
      obj->last_pnt->next_pnt = pnt;
      obj->last_pnt = pnt;
    }    
  }
  free_obj_pnts(tmpfirst);

}


/*
 *----------------------------------------------------------------------
 *
 * Map_set_obj_points --
 *
 *	This procedure is invoked to display a map object -- it sets
 *      up the x and y pixel coordinates for each segment (point) of
 *      the object..
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Map_set_obj_points(clientData, obj)
    ClientData clientData;	/* Information about widget. */
    MapObj *obj;
{
  register Map *mapPtr = (Map *) clientData;
  register Tk_Window tkwin = mapPtr->tkwin;
  long win_width, win_height, winw, winh;
  register int x, y;
  register MapObjPnt *pnt;

  int	z, LonPrv=0, LatPrv=0, xprv=0, yprv=0;
  int	newseg=0;
  XPoint *xpoint;


  /* set width and height for the window */
  win_width = Tk_Width(tkwin);
  win_height = Tk_Height(tkwin);
  
  winw = win_width / 2;
  winh = win_height / 2;
  mapPtr->X_Scale = mapPtr->zoom * mapPtr->aspect/(10800.00/(float)winw);
  /* e.g. 33.75 = 10800 / 320  for 640x400*/
  mapPtr->Y_Scale = mapPtr->zoom * 100.0 /(5400.00/(float)winh);
  /* e.g. 27.00 =  5400 / 200  for 640x400*/
  mapPtr->ymax = QSPAN/mapPtr->zoom;
  mapPtr->ymin = -mapPtr->ymax;
  mapPtr->xmax = HSPAN / (mapPtr->zoom * (mapPtr->aspect / 100.0));
  mapPtr->xmin = -mapPtr->xmax;
  mapPtr->xspan = mapPtr->xmax - mapPtr->xmin;
  mapPtr->yspan = mapPtr->ymax - mapPtr->ymin;
  

  xpoint = obj->xpoints;

  for (pnt = obj->first_pnt; pnt ; pnt = pnt->next_pnt) {


    x = ((int)(pnt->lon * 3600)) - mapPtr->Longitude;
    y = ((int)(pnt->lat * 3600)) - mapPtr->Latitude;
    /* wrap around for East-West	*/
    if (x < -HSPAN)
      x += WSPAN;
    else if (x > HSPAN)
      x -= WSPAN;

    /* scale pts w/in area of the window */
    pnt->pix_x = winw + (x * mapPtr->X_Scale)/ADJPIX;
    pnt->pix_y = winh - (y * mapPtr->Y_Scale)/ADJPIX;

    if (xpoint) {
      xpoint->x = pnt->pix_x;
      xpoint->y = pnt->pix_y;
      xpoint++;
    }
  }

}

/*
 *----------------------------------------------------------------------
 *
 * Map_draw_obj --
 *
 *	This procedure is invoked to display a map object -- it does
 *      the actual drawing of the object;
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      updates display;
 *
 *----------------------------------------------------------------------
 */

Map_draw_obj(clientData, obj, pm)
    ClientData clientData;	/* Information about widget. */
    MapObj *obj;
    Pixmap pm;
{
  register Map *mapPtr = (Map *) clientData;
  register Tk_Window tkwin = mapPtr->tkwin;

 
  switch (obj->type) {
  case MAPOBJ_PNT: 
#ifndef WIN32
	XDrawPoint(mapPtr->display, pm ,
		   obj->gc, obj->first_pnt->pix_x, obj->first_pnt->pix_y);
#else 
	XDrawLine(mapPtr->display,  pm,
		  obj->gc,  obj->first_pnt->pix_x, obj->first_pnt->pix_y,
		  obj->first_pnt->pix_x, obj->first_pnt->pix_y);
#endif 
    break;
  case MAPOBJ_LINE:
	XDrawLine(mapPtr->display,  pm,
		  obj->gc,  obj->first_pnt->pix_x, obj->first_pnt->pix_y,
		  obj->last_pnt->pix_x, obj->last_pnt->pix_y);
    break;
  case MAPOBJ_RECT:
	XDrawRectangle(mapPtr->display,  pm ,
		  obj->gc,  obj->first_pnt->pix_x, obj->first_pnt->pix_y,
		  obj->last_pnt->pix_x - obj->first_pnt->pix_x, 
		  obj->last_pnt->pix_y - obj->first_pnt->pix_y);
    break;
  case MAPOBJ_FRECT:
	XFillRectangle(mapPtr->display,  pm ,
		  obj->gc,  obj->first_pnt->pix_x, obj->first_pnt->pix_y,
		  obj->last_pnt->pix_x - obj->first_pnt->pix_x + 1, 
		  obj->last_pnt->pix_y - obj->first_pnt->pix_y + 1 );
    break;
  case MAPOBJ_POLY:
	XFillPolygon(mapPtr->display, pm,
		  obj->gc, obj->xpoints, obj->npoints - 1, Complex,
		     CoordModeOrigin); 
    break;
  case MAPOBJ_PLINE:
	XDrawLines(mapPtr->display,  pm ,
		  obj->gc, obj->xpoints, obj->npoints - 1, CoordModeOrigin); 
    break;
  }
}


/*
 *--------------------------------------------------------------
 *
 * MapEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher on
 *	structure changes to a map.  For maps with 3D
 *	borders, this procedure is also invoked for exposures.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
MapEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    register XEvent *eventPtr;	/* Information about event. */
{
    register Map *mapPtr = (Map *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	if ((mapPtr->relief != TK_RELIEF_FLAT) && (mapPtr->tkwin != NULL)
		&& !(mapPtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayMap, (ClientData) mapPtr);
	    mapPtr->flags |= REDRAW_PENDING;
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(mapPtr->interp, Tk_PathName(mapPtr->tkwin));
	mapPtr->tkwin = NULL;
	if (mapPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayMap, (ClientData) mapPtr);
	}
	Tk_CancelIdleCall(MapMap, (ClientData) mapPtr);
	if (map_posted != 0) {
	  Tk_EventuallyFree((ClientData) mapPtr, (Tcl_FreeProc *)DestroyMap);
	}
    }
    /* this is set by the first event */
    map_posted = 1;

}

/*
 *----------------------------------------------------------------------
 *
 * MapMap --
 *
 *	This procedure is invoked as a when-idle handler to map a
 *	newly-created top-level map.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The map given by the clientData argument is mapped.
 *
 *----------------------------------------------------------------------
 */

static void
MapMap(clientData)
    ClientData clientData;		/* Pointer to map structure. */
{
    Map *mapPtr = (Map *) clientData;

    /*
     * Wait for all other background events to be processed before
     * mapping window.  This ensures that the window's correct geometry
     * will have been determined before it is first mapped, so that the
     * window manager doesn't get a false idea of its desired geometry.
     */

    Tk_Preserve((ClientData) mapPtr);
    while (1) {
	if (Tk_DoOneEvent(TK_IDLE_EVENTS) == 0) {
	    break;
	}

	/*
	 * After each event, make sure that the window still exists
	 * and quit if the window has been destroyed.
	 */

	if (mapPtr->tkwin == NULL) {
	    Tk_Release((ClientData) mapPtr);
	    return;
	}
    }
    Tk_MapWindow(mapPtr->tkwin);
    Tk_Release((ClientData) mapPtr);
}

/*****************************************************************
 * ParseMapCoord -- parse a latitude or longitude specification
 *
 * Results -- long value specifying lat or long in "seconds"
 *****************************************************************/
long ParseMapCoord(char *coordspec)
{
  long ldeg = 0, lmin = 0, lsec = 0;
  long value = 0;
  char dir, dummy;
  int sign;
  
  sign = 1;
  if (sscanf(coordspec,"%do%d'%d''%c", &ldeg, &lmin, &lsec, &dir) == 4) {
    if (ldeg < 0) {
      sign = -1;
      ldeg = ldeg * -1;
    }
    value = (ldeg * 3600) + (lmin * 60) + lsec;
    
    /* do some error checks and direction setting */
    
    if (dir == 'S' || dir == 's') {
      /* must be a latitude */
      sign = -1;
      if (value > (90 * 3600))
	value = 90*3600;
    }
    if (dir == 'N' || dir == 'n') {
      /* must be a latitude */
      if (value > (90 * 3600))
	value = 90*3600;
    }
    if (dir == 'W' || dir == 'w') {
      /* must be a longitude */
      sign = -1;
      if (value > (180 * 3600))
	value = 180*3600;
    }
    if (dir == 'E' || dir == 'e') {
      /* must be a longitude */
      if (value > (180 * 3600))
	value = 180*3600;
    }
    
    return (sign * value);
  }

  return(0);
}

long GetPixLong(Map *m)
{
  long winw, lonval, sign, londeg, lonmin, lonsec ;

  winw = Tk_Width(m->tkwin) / 2;

  lonval = m->Longitude + ((ADJPIX * (m->x_center_pixel - winw)) / m->X_Scale);

  /* wrap around for East-West	*/
  if (lonval < -HSPAN)
    lonval += WSPAN;
  else if (lonval > HSPAN)
    lonval -= WSPAN;

  if (lonval < 0) {
    sign = -1;
    lonval *= -1;
  }
  else 
    sign = 1;

  londeg = lonval / 3600;
  lonmin = (lonval % 3600) / 60;
  lonsec = (lonval % 3600) % 60; 

  if (londeg >= 180) {
    londeg = 180;
    lonmin = 0;
    lonsec = 0;
  }

  return (sign * ((londeg * 3600) + (lonmin * 60) + lonsec));
}

 
long GetPixLat(Map *m) 
{
  long winh, latval, sign, latdeg, latmin, latsec;
 
  winh = Tk_Height(m->tkwin) /2;

  latval = m->Latitude - ((ADJPIX * (m->y_center_pixel - winh)) / m->Y_Scale);

  if (latval < 0) {
    sign = -1;
    latval *= -1;
  }
  else 
    sign = 1;

  latdeg = latval / 3600;
  latmin = (latval % 3600) / 60;
  latsec = (latval % 3600) % 60; 

  if (latdeg >= 90) {
    latdeg = 90;
    latmin = 0;
    latsec = 0;
  }

  return (sign * ((latdeg * 3600) + (latmin * 60) + latsec));

}


int place_point_objects(Map *m, Pfeature *flist)
{

} 




/**************** window x and y from lat and long *************/
int
GetMapXYCmd(Map *m, char *latstring, char *lonstring)
{
      	int inlatdeg, inlondeg, inlatmin, inlonmin, zoomval;
	int win_width, win_height, winw, winh;
        long lonval, latval;
        double aspect;
	int i, foo;
	Tk_Window tkwin2;
	int x, y;
	char buff[50];

	/* window to render in */
	tkwin2 = m->tkwin;

        /* set width and height for the window */
        win_width = Tk_Width(tkwin2);
        win_height = Tk_Height(tkwin2);


        /* set up the drawing parameters */
	latval = ParseMapCoord(latstring);
	lonval = ParseMapCoord(lonstring);
        winw = win_width / 2;
        winh = win_height / 2;

	if (m->ymax == 0 && m->xmax == 0) {
	  m->X_Scale = m->zoom * m->aspect/(10800.00/(float)winw);
	  /* e.g. 33.75 = 10800 / 320  for 640x400*/
	  m->Y_Scale = m->zoom * 100.0 /(5400.00/(float)winh);
	  /* e.g. 27.00 =  5400 / 200  for 640x400*/
	  m->ymax = QSPAN/m->zoom;
	  m->ymin = -m->ymax;
	  m->xmax = HSPAN / (m->zoom * (m->aspect / 100.0));
	  m->xmin = -m->xmax;
	  m->xspan = m->xmax - m->xmin;
	  m->yspan = m->ymax - m->ymin;
	}


	x = lonval - m->Longitude;
	y = latval - m->Latitude;
	/* wrap around for East-West	*/
	if (x < -HSPAN)
		x += WSPAN;
	else if (x > HSPAN)
		x -= WSPAN;


	/* ignore pts outside magnified area	*/
	if((x < m->xmin || x > m->xmax || y < m->ymin || y > m->ymax)) {
	  sprintf(buff,"-1 -1");
	  Tcl_AppendResult(m->interp, buff, (char *) NULL);
	  return TCL_OK;
	  }
	/* scale pts w/in area of a 640x400 window */
	x = winw + (x * m->X_Scale)/ADJPIX;
	y = winh - (y * m->Y_Scale)/ADJPIX;


	sprintf(buff,"%d %d",x,y);
	Tcl_AppendResult(m->interp, buff, (char *) NULL);


	return TCL_OK;
}

















