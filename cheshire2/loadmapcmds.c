#include "tk.h"


/* Tcl/Tk functions in external modules */
/* extern int PGTK_SourceCmd(); */

/* Map widget definition routines */
extern int Tk_MapCmd();

/* map coordinate conversions */
extern int MapCNVT_dfloat_latlon();
extern int MapCNVT_latlon_dfloat();
extern int MapCNVT_dfloat_lonlat();
extern int MapCNVT_lonlat_dfloat();
extern int MapDIST_dfloat();

/* this procedure loads the user defined commands into the interpreter */
LoadPGTKMCmds(interp)
Tcl_Interp *interp;
{
  Tk_Window  w;

  w = Tk_MainWindow(interp);

  /* the map widget command */
  Tcl_CreateCommand(interp, "map", Tk_MapCmd, (ClientData) w,
		    (void (*)()) NULL);

  /* aliases for converting float degrees to latlon */
  Tcl_CreateCommand(interp, "degrees_2_lonlat", MapCNVT_dfloat_lonlat, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "degrees_to_lonlat", MapCNVT_dfloat_lonlat, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "convert_degrees", MapCNVT_dfloat_lonlat, 
		    (ClientData) w, (void (*)()) NULL);


  Tcl_CreateCommand(interp, "degrees_to_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "degrees_2_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "convert_degrees_to_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData) w, (void (*)()) NULL);


  /* aliases for converting latlon to float degrees */
  Tcl_CreateCommand(interp, "lonlat_2_degrees", MapCNVT_lonlat_dfloat, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "lonlat_to_degrees", MapCNVT_lonlat_dfloat, 
		    (ClientData) w, (void (*)()) NULL);

  Tcl_CreateCommand(interp, "latlon_2_degrees", MapCNVT_latlon_dfloat, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "latlon_to_degrees", MapCNVT_latlon_dfloat, 
		    (ClientData) w, (void (*)()) NULL);

  /* aliases for map distances functions */
  Tcl_CreateCommand(interp, "geo_distance_km", MapDIST_dfloat, 
		    (ClientData) w, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "geo_distance_miles", MapDIST_dfloat, 
		    (ClientData) w, (void (*)()) NULL);


}
















