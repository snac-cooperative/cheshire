# this script shows some objects drawn on a map widget called .map
# to run this script run cheshire2 then source map3.tcl (to create
# the widget) then source this file to draw these objects.

.map create line white 0o0'0''N 179o0'0''W 0o0'0''N 179o59'59''E

.map create polygon white 50o0'0''N 0o0'0''W  0o0'0''N 50o0'0''W 0o0'0''N 50o0'0''E 50o0'0''N 0o0'0''W

.map create polygon blue 23o0'0''N 120o0'0''W 24o0'0''N 120o0'0''W 25o0'0''N 22o0'0''W 20o0'0''N 122o0'0''W 23o0'0''N 120o0'0''W

.map create polyline yellow 23o0'0''N 120o0'0''W 24o0'0''N 120o0'0''W 25o0'0''N 22o0'0''W 20o0'0''N 122o0'0''W 23o0'0''N 120o0'0''W

.map create point white 23o0'0''N 120o0'0''W

.map create rectangle violet 50o0'0''N 70o0'0''W  10o0'0''N 20o0'0''W

.map create filledrectangle violet 55o0'0''N 75o0'0''W  5o0'0''N 15o0'0''W
