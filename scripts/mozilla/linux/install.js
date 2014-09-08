var err = initInstall('Cheshire Z39.50',  // name for install UI
                      '/protozilla/protocol/z3950',       // registered name
                      '0.9.0.0');                             // package version

var fProfile = getFolder('Profile');
var fComponents = getFolder("Components");
var fChrome     = getFolder("Chrome");

err = addDirectory("", "components", fComponents, "");
err = addDirectory("", "chrome",     fChrome,     "");
err = addDirectory("", "protozilla", fProfile,    "protozilla");

registerChrome(PACKAGE | DELAYED_CHROME, getFolder("Chrome","z3950"), "content/z3950/");




performInstall();

