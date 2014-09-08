#include "tcl.h"
#include "z3950_3.h"
#include "session.h"

/* current or defaul user session pointer */
extern ZSESSION         *TKZ_User_Session;

/* map coordinate conversions */
extern int MapCNVT_dfloat_latlon();
extern int MapCNVT_latlon_dfloat();
extern int MapCNVT_dfloat_lonlat();
extern int MapCNVT_lonlat_dfloat();
extern int MapDIST_dfloat();

/* this procedure loads the user defined commands into the interpreter */
LoadPGTKMCmds_NW(interp)
Tcl_Interp *interp;
{

  /* aliases for converting float degrees to latlon */
  Tcl_CreateCommand(interp, "degrees_2_lonlat", MapCNVT_dfloat_lonlat, 
		    (ClientData) TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "degrees_to_lonlat", MapCNVT_dfloat_lonlat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "convert_degrees", MapCNVT_dfloat_lonlat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);


  Tcl_CreateCommand(interp, "degrees_to_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "degrees_2_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "convert_degrees_to_latlon", MapCNVT_dfloat_latlon, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);


  /* aliases for converting latlon to float degrees */
  Tcl_CreateCommand(interp, "lonlat_2_degrees", MapCNVT_lonlat_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "lonlat_to_degrees", MapCNVT_lonlat_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);

  Tcl_CreateCommand(interp, "latlon_2_degrees", MapCNVT_latlon_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "latlon_to_degrees", MapCNVT_latlon_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);


  /* aliases for converting latlon to float degrees */
  Tcl_CreateCommand(interp, "geo_distance_km", MapDIST_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);
  Tcl_CreateCommand(interp, "geo_distance_miles", MapDIST_dfloat, 
		    (ClientData)TKZ_User_Session, (void (*)()) NULL);


}
















