/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "MPL"); you may not use this file
 * except in compliance with the MPL. You may obtain a copy of
 * the MPL at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the MPL is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the MPL for the specific language governing
 * rights and limitations under the MPL.
 * 
 * The Original Code is protoZilla.
 * 
 * The Initial Developer of the Original Code is Ramalingam Saravanan.
 * Portions created by Ramalingam Saravanan <svn@xmlterm.org> are
 * Copyright (C) 2000 Ramalingam Saravanan. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License (the "GPL"), in which case
 * the provisions of the GPL are applicable instead of
 * those above. If you wish to allow use of your version of this
 * file only under the terms of the GPL and not to allow
 * others to use your version of this file under the MPL, indicate
 * your decision by deleting the provisions above and replace them
 * with the notice and other provisions required by the GPL.
 * If you do not delete the provisions above, a recipient
 * may use your version of this file under either the MPL or the
 * GPL.
 */

// Module self-description constants
const PACKAGE_NAME      = "protozilla";
const REGISTRY_LOCATION = "rel:"+PACKAGE_NAME+".js";
const COMPONENT_TYPE    = "text/javascript";

// List of bundled protocols
var gBundledProtoList = ["moz", "freenet"];

// List of inherited environment variables
var gInheritedEnv = [ "ALLUSERSPROFILE", "APPDATA",
                  "COMMONPROGRAMFILES", "COMSPEC", "DISPLAY",
                  "HOME", "HOMEDRIVE", "HOMEPATH",
                  "LANG", "LANGUAGE", "LC_ALL", "LC_COLLATE",  "LC_CTYPE",
                  "LC_MESSAGES",  "LC_MONETARY", "LC_NUMERIC", "LC_TIME",
                  "LOCPATH", "LOGNAME", "LD_LIBRARY_PATH", "MOZILLA_FIVE_HOME",
                  "NLSPATH", "PATH", "PATHEXT", "PROGRAMFILES",
                  "PROTOZILLA_PASS_ENV", "PWD",
                  "SHELL", "SYSTEMDRIVE", "SYSTEMROOT",
                  "TEMP", "TMP", "TMPDIR", "TZ", "TZDIR",
                  "USER", "USERPROFILE",
                  "WINDIR" ];

// Module constants
const CLIENT_FILEHEAD      = "client";     // Executable client name (.pl, ...)
const INDEX_FILEHEAD       = "index";      // Home page (.xul or .htm or .html)
const SERVER_FILEHEAD      = "server";     // Executable server name (.pl, ...)
const STARTSERVER_FILEHEAD = "start-server";  // Server start script (.pl, ...)
const STOPSERVER_FILEHEAD  = "stop-server";   // Server start script (.pl, ...)

const NS_XPCOM_AUTOREGISTRATION_OBSERVER_ID = "xpcom-autoregistration";
const NS_XPCOM_SHUTDOWN_OBSERVER_ID         = "xpcom-shutdown";

const URI_START_PAT = /^\s*[a-z]([a-z0-9+.\-])*:/i;

// CIDs are constructed by substituting the ".." string
const CID_TEMPLATE = "8462c7..-7ab1-11d4-8f02-006008948af5";

// Maximum no. of protocol/content handlers (should be <= 256)
const CID_MAX = 32;

const PROTOCOL_HANDLER_CONTRACTID_PREFIX =
  "@mozilla.org/network/protocol;1?name=";

const CONTENT_HANDLER_CONTRACTID_PREFIX =
  "@mozilla.org/uriloader/content-handler;1?type=";

// Components provided by module
////////////////////////////////

const NS_PROTOZILLA_CONTRACTID =
  "@mozilla.org/protozilla/protozilla;1";

const NS_PROTOZILLA_CLINE_CONTRACTID =
  "@mozilla.org/commandlinehandler/general-startup;1?type=protozilla";

const NS_PROTOZILLA_CID =
  Components.ID("{8431e171-7ab1-11d4-8f02-006008948af5}");

const NS_PROTOZILLA_CLINE_CID =
  Components.ID("{8431e1F1-7ab1-11d4-8f02-006008948af5}");

const NS_PROTOCHANNEL_CID =
  Components.ID("{8431e1F2-7ab1-11d4-8f02-006008948af5}");

const NS_CGICHANNEL_CID =
  Components.ID("{8431e1F3-7ab1-11d4-8f02-006008948af5}");

// Components used in module
//////////////////////////////////
const NS_IPCSERVICE_CONTRACTID    = "@mozilla.org/process/ipc-service;1";

const NS_PIPETRANSPORT_CONTRACTID = "@mozilla.org/process/pipe-transport;1";

const NS_PIPECHANNEL_CONTRACTID   = "@mozilla.org/process/pipe-channel;1";

const NS_PROCESSINFO_CONTRACTID   = "@mozilla.org/xpcom/process-info;1";

const NS_PROCESS_CONTRACTID         = "@mozilla.org/process/util;1"
const NS_OBSERVERSERVICE_CONTRACTID = "@mozilla.org/observer-service;1";

const NS_SIMPLEURI_CONTRACTID = "@mozilla.org/network/simple-uri;1";

const NS_LOCAL_FILE_CONTRACTID = "@mozilla.org/file/local;1";

const NS_LOCALFILEOUTPUTSTREAM_CONTRACTID =
                              "@mozilla.org/network/file-output-stream;1";

const NS_HTTPPROTOCOLHANDLER_CID_STR=
"{4f47e42e-4d23-4dd3-bfda-eb29255e9ea3}";

const NS_STANDARDURL_CONTRACTID = "@mozilla.org/network/standard-url;1";

const NS_IOSERVICE_CONTRACTID   = "@mozilla.org/network/io-service;1";

/* Interfaces */
const nsIWindowMediator  = Components.interfaces.nsIWindowMediator;
const nsICategoryManager = Components.interfaces.nsICategoryManager;
const nsIContentHandler  = Components.interfaces.nsIContentHandler;
const nsIProtocolHandler = Components.interfaces.nsIProtocolHandler;
const nsIURI             = Components.interfaces.nsIURI;
const nsIChannel         = Components.interfaces.nsIChannel;
const nsIRequest         = Components.interfaces.nsIRequest;
const nsIStreamListener  = Components.interfaces.nsIStreamListener;
const nsIAppShellService = Components.interfaces.nsIAppShellService;
const nsISupports        = Components.interfaces.nsISupports;

const nsIFile            = Components.interfaces.nsIFile;
const nsILocalFile       = Components.interfaces.nsILocalFile;
const nsIFileOutputStream= Components.interfaces.nsIFileOutputStream;
const nsIProcessInfo     = Components.interfaces.nsIProcessInfo;
const nsIIOService       = Components.interfaces.nsIIOService;
const nsIObserverService = Components.interfaces.nsIObserverService;
const nsIObserver        = Components.interfaces.nsIObserver;
const nsIScriptSecurityManager = Components.interfaces.nsIScriptSecurityManager;

const nsIHttpProtocolHandler = Components.interfaces.nsIHttpProtocolHandler
const nsIStandardURL     = Components.interfaces.nsIStandardURL;
const nsIURL             = Components.interfaces.nsIURL;
const nsICmdLineHandler  = Components.interfaces.nsICmdLineHandler;

const nsIPipeConsole     = Components.interfaces.nsIPipeConsole;
const nsIPipeChannel     = Components.interfaces.nsIPipeChannel;
const nsIIPCService      = Components.interfaces.nsIIPCService;
const nsIPipeTransport   = Components.interfaces.nsIPipeTransport;
const nsIProtozilla      = Components.interfaces.nsIProtozilla;
const nsIProtozillaHandler = Components.interfaces.nsIProtozillaHandler;

///////////////////////////////////////////////////////////////////////////////

/* Command Line handler service */
function CLineService()
{}

CLineService.prototype.commandLineArgument = "-protozilla";
CLineService.prototype.prefNameForStartup  = "general.startup.protozilla";
CLineService.prototype.chromeUrlForTask    = "chrome://protozilla/content";
CLineService.prototype.helpText            = "Start with a protozilla window";
CLineService.prototype.handlesArgs         = false;
CLineService.prototype.defaultArgs         = "";
CLineService.prototype.openWindowWithArgs  = false;

/* factory for command line handler service (CLineService) */
var CLineFactory = new Object();

CLineFactory.createInstance =
function (outer, iid) {
  if (outer != null)
    throw Components.results.NS_ERROR_NO_AGGREGATION;

  if (!iid.equals(nsICmdLineHandler) && !iid.equals(nsISupports))
    throw Components.results.NS_ERROR_INVALID_ARG;

  return new CLineService();
}

///////////////////////////////////////////////////////////////////////////////
// File read/write operations

const NS_RDONLY      = 0x01;
const NS_WRONLY      = 0x02;
const NS_CREATE_FILE = 0x08;
const NS_TRUNCATE    = 0x20;
const DEFAULT_FILE_PERMS = 0600;

const FileChannel = new Components.Constructor( "@mozilla.org/network/local-file-channel;1", "nsIFileChannel" );

const InputStream = new Components.Constructor( "@mozilla.org/scriptableinputstream;1", "nsIScriptableInputStream" );


function CreateFileStream(filePath, permissions) {

  //DEBUG_LOG("protozilla.js: CreateFileStream: file="+filePath+"\n");

  try {
    var localFile = Components.classes[NS_LOCAL_FILE_CONTRACTID].createInstance(Components.interfaces.nsILocalFile);

    localFile.initWithPath(filePath);

    if (localFile.exists()) {

      if (localFile.isDirectory() || !localFile.isWritable())
         throw Components.results.NS_ERROR_FAILURE;

      if (!permissions)
        permissions = localFile.permissions;
    }

    if (!permissions)
      permissions = DEFAULT_FILE_PERMS;

    var flags = NS_WRONLY | NS_CREATE_FILE | NS_TRUNCATE;

    var fileStream = Components.classes[NS_LOCALFILEOUTPUTSTREAM_CONTRACTID].createInstance(Components.interfaces.nsIFileOutputStream);

    fileStream.init(localFile, flags, permissions, 0);

    return fileStream;

  } catch (ex) {
    ERROR_LOG("protozilla.js: CreateFileStream: Failed to create "+filePath+"\n");
    return null;
  }
}

// Execute file (detached)
function ExecuteFile(executable, args) {
  WRITE_LOG("Protozilla: ExecuteFile: ("+args.length+") "+executable.path+" "+args.join(" ")+"\n");

  if (!executable.exists()) {
    ERROR_LOG("Protozilla: ExecuteFile: Error - executable '"+executable.path+"' not found\n");
    return false;
  }

  try {
    var process = Components.classes[NS_PROCESS_CONTRACTID].createInstance(Components.interfaces.nsIProcess);

    process.init(executable);

    var pid = new Object();
    process.run(false, args, args.length, pid);

    DEBUG_LOG("Protozilla: ExecuteFile: pid="+pid.value+"\n");

  } catch (ex) {
    ERROR_LOG("Protozilla: ExecuteFile: Error in executing "+executable.path+" \n");
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////

var gLogLevel = 3;     // Output only errors/warnings by default
var gLogFileStream = null;

function WRITE_LOG(str) {
  dump(str);

  if (gLogFileStream) {
    gLogFileStream.write(str, str.length);
    gLogFileStream.flush();
  }
}

function DEBUG_LOG(str) {
  if (gLogLevel >= 4)
    WRITE_LOG(str);
}

function WARNING_LOG(str) {
  if (gLogLevel >= 3)
    WRITE_LOG(str);
}

function ERROR_LOG(str) {
  if (gLogLevel >= 2)
    WRITE_LOG(str);
}

function CONSOLE_LOG(str) {
  if (gLogLevel >= 3)
    WRITE_LOG(str);

  pzilla.console.write(str);
}

// Object for invoking anonymous Javascript functions
// (Not thread-safe?)
var _protozillaFunctionObject = new Object();

// Global functions available to Javascript CGI:
//   write(arg1, arg2, ...)   : writes to "stdout"
//   writeln(arg1, arg2, ...) : writes to "stdout" with newline
//   getEnv(name)  : return value of environment variable or null string
//   execSh(command) : executes command and returns its stdout (requires ipcserv)
//   load(args)    : defined to be a no-op (to allow "loading" of protoEnv.js)

// Define load to be a no-op (to allow "loading" of protoEnv.js)
function load(args) {
}

function write(args) {
  for (var j=0; j<arguments.length; j++) {
    _protozillaFunctionObject._writeArray.push(arguments[j].toString());
  }
}

function writeln(args) {
  for (var j=0; j<arguments.length; j++) {
    _protozillaFunctionObject._writeArray.push(arguments[j].toString());
  }
  _protozillaFunctionObject._writeArray.push("\n");
}

function getEnv(name) {
  var value = _protozillaFunctionObject._environ[name];
  return value ? value : "";
}

function execSh(command) {
  DEBUG_LOG("protozilla:execSh: command="+command+"\n");

  if (!pzilla || !pzilla.ipcservice)
      throw Components.results.NS_ERROR_FAILURE;

  return pzilla.ipcservice.execSh(command);
}

var gFileInterpreter = { 'class': 'java',
                         'pl':    'perl',
                         'perl':  'perl',
                         'py':    'python'};

function GetCommandLine(localFile, extraArg) {
  if (!localFile.exists() || localFile.isDirectory())
    return null;
  
  var executable = null;
  var args = [];
  
  var unixFlag = pzilla.unix;
  
  try {
    var fileHeader = pzilla.readFileContents(localFile, 255);
    
    var interpEx = /^\s*#!\s*([^\r\n]*)/;
    
    var matches = fileHeader.match(interpEx);
    
    if (matches && matches[1]) {
      if (matches[1].substr(0, 9) == "chrome://") {
        var chromeDir = pzilla.getFileOfProperty("AChrom").path;
        var chromeFile = matches[1].substr(9);
        var bits = chromeFile.split(/\//);
        if (unixFlag) {
          slash = "/";
        } else {
          slash = "\\";
        }
	
        // XXX Semiportable code
	
        executable  = chromeDir + (slash + bits[0] + slash + bits[1] + slash + bits[0] + slash + bits[2]);
        args.push(localFile.path);
        CONSOLE_LOG("Chrome executable should be:  " + executable + "\n");
	
      } else {
	
	var comps = matches[1].split(/\s+/);
	
	if ((comps.length > 0) && comps[0] && (comps[0].search(/^\//) != -1)) {
	  // Found interpreter path
	  
	  if (unixFlag) {
	    var interpFile = Components.classes[NS_LOCAL_FILE_CONTRACTID].createInstance(nsILocalFile);
	    
	    interpFile.initWithPath(comps[0]);
	    
	    if (interpFile.exists()) {
	      // Interpreter file exists
	      executable = localFile.path;
	    }
	  }
	  
	  if (!executable) {
	    // Interpreter file not found; extract its name
	    var interpName = comps[0].match(/\/([^\/]+)$/);
	    
	    if (interpName && interpName[1] && (interpName[1] != "env")) {
	      if (unixFlag) {
		executable = "/usr/bin/env";
		args.push(interpName[1]);
	      } else {
		executable = interpName[1];
	      }
	      
	      if (interpName[1] == "perl") {
		for (k=1; k<comps.length; k++) {
		  if (comps[k] == "-T")
		    args.push(comps[k]);
		}
	      }
	      
	      args.push(localFile.path);
	    }
	  }
	}
      }
    }
  } catch (ex) {
  }

  if (!executable && !unixFlag) {
    // Win32: try to determine executable from file extension
    var leafName = localFile.leafName;

    var dotOffset = leafName.lastIndexOf(".");

    var extension = (dotOffset == -1) ? "" : leafName.substr(dotOffset+1);

    if (!extension && gFileInterpreter[extension]) {
      executable = gFileInterpreter[extension];
      args.push(localFile.path);
    }
  }

  if (extraArg)
    args.push(extraArg);

  if (!executable)
    executable = localFile.path;

  WRITE_LOG("Protozilla: GetCommandLine: "+executable+" "+args.join(" ")+"\n");

  return {executable: executable, args: args};
}

function GetEnvObject(env) {
  var envObj = new Object();
  if (env) {
    for (var j=0; j<env.length; j++) {
       var splitOffset = env[j].indexOf("=");
       if (splitOffset > 0)
          envObj[env[j].substr(0,splitOffset)] = env[j].substr(splitOffset+1);
    }
  }
  return envObj;
}

var gProcessInfo = null;
try {
  gProcessInfo = Components.classes[NS_PROCESSINFO_CONTRACTID].getService();
  gProcessInfo = gProcessInfo.QueryInterface(nsIProcessInfo);
} catch (ex) {
}

function GetSysEnv(name) {
  //DEBUG_LOG("Protozilla: GetSysEnv: "+name+"\n")
  return gProcessInfo.getEnv(name);
}

function ApplyJSFunc(jsFunc, env) {
  DEBUG_LOG("Protozilla: ApplyJSFunc: \n")

  if ((typeof jsFunc) != "function") {
    CONSOLE_LOG("Protozilla: Error - Invalid Javascript function body\n");
      throw Components.results.NS_ERROR_FAILURE;
  }

  _protozillaFunctionObject._environ = GetEnvObject(env);

  _protozillaFunctionObject._writeArray = [];

  var returnVal;

  // Not thread-safe?
  try {
    returnVal = jsFunc.apply(_protozillaFunctionObject);
  } catch (ex) {
    CONSOLE_LOG("Protozilla: Error - Javascript execution error\n");
    throw ex;
  }

  var jsOutput = _protozillaFunctionObject._writeArray.join("");

  return jsOutput;
}


function ExecuteCommand(command, env, substitute) {

  if (substitute) {
    // Substitute environment variables in command line
    var envObj = GetEnvObject(env);

    var cmdComps = command.split(/\$/);
    for (var k=1; k<cmdComps.length; k++) {
       var cmdComp = cmdComps[k];
       if (cmdComps[k-1].search(/\\$/) != -1) {
         // Escaped dollar sign
         cmdComps[k] = "$"+cmdComp;

       } if (cmdComp.search(/^[a-zA-Z_]/) != 0) {
         // Dollar sign not followed by identifier
         cmdComps[k] = "$"+cmdComp;

       } else {
         var nameLen = cmdComp.search(/\W/);
         if (nameLen == -1) nameLen = cmdComp.length;
         var name = cmdComp.substr(0,nameLen);
         var value = envObj[name];
         if ((typeof value) != "string") {
           if (pzilla.ipcService) {
             value = GetSysEnv(name);
           } else {
             value = "";
           }
         }
         cmdComps[k] = value + cmdComp.substr(nameLen);
       }
    }

    // Reconstruct command line after substitution
    command = cmdComps.join("");
  }

  // Strip trailing CRLF or whitespace
  command = command.replace(/\r?\n$/,"");
  command = command.replace(/\s+$/,"");

  // Parse command line
  var args = command.split(/\s+/);
  var executable = args.shift();

  if (!executable) {
    ERROR_LOG("Protozilla: ExecuteCommand: Error - empty command line\n")
    throw Components.results.NS_ERROR_FAILURE;
  }

  var execPath = "";
  if (isAbsolutePath(executable)) {
    // Absolute path
    execPath = executable;

  } else if (pzilla.ipcService) {
    // Resolve relative path using PATH environment variable
    var envPath = GetSysEnv("PATH");
    execPath = ResolvePath(executable, envPath);

  } else {
    CONSOLE_LOG("Protozilla: ExecuteCommand: Error - unable to resolve relative path '"+executable+"'; IPCServ module needed\n")
    throw Components.results.NS_ERROR_FAILURE;
  }

  if (!execPath) {
    ERROR_LOG("Protozilla: ExecuteCommand: Error - executable '"+executable+"' not found\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var execFile = Components.classes[NS_LOCAL_FILE_CONTRACTID].createInstance(nsILocalFile);

  execFile.initWithPath(execPath);

  ExecuteFile(execFile, args);

  return;
}


function NewChannel(aSpec, aBaseURI, aOriginalSpec) {
  var ioServ = Components.classes[NS_IOSERVICE_CONTRACTID].getService();
  ioServ = ioServ.QueryInterface(nsIIOService);

  DEBUG_LOG("Protozilla: NewChannel: "+aSpec+", base="+(aBaseURI?aBaseURI.spec:"")+", orig="+aOriginalSpec+"\n")

  var channel = ioServ.newChannel(aSpec, "", aBaseURI);

  if (aOriginalSpec) {
     var uri = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
     uri.spec = aOriginalSpec;
     channel.originalURI = uri;
  }

  return channel;
}

function NewJSChannel(aURI, env, funcBody) {
  // Evaluate anonymous javascript function
  DEBUG_LOG("Protozilla: NewJSChannel: "+aURI.spec+"\n")
  if (!pzilla.ipcService) {
    CONSOLE_LOG("Protozilla: Error - IPCService module needed for javascript clients\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  // TODO: this should be cached
  var jsFunc;
  try {
    jsFunc = Function(funcBody);
  } catch (ex) {
    CONSOLE_LOG("Protozilla: Error - Invalid Javascript function body\n");
    throw ex;
  }

  if ((typeof jsFunc) != "function") {
    CONSOLE_LOG("Protozilla: Error - Invalid Javascript function body\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var jsOutput = ApplyJSFunc(jsFunc, env);

  if (jsOutput.length == 0) {
    CONSOLE_LOG("Protozilla: Error - Javascript returned no data\n");
      throw Components.results.NS_ERROR_FAILURE;
  }

  var regex = /^content-type:\s*(\S+)\s*\r?\n\r?\n/i;

  var mimeType = "text/plain";
  if (jsOutput.search(regex) != -1) {
      jsOutput = jsOutput.replace(regex,"");
      mimeType = RegExp.lastParen;
  }

  DEBUG_LOG("NewJSChannel: mimeType="+mimeType+",jsOutput="+jsOutput);

  return pzilla.ipcService.newStringChannel(aURI, mimeType, "", jsOutput);
}

function NewPipeChannel(aURI, clientFile, clientData, env, restricted) {
  DEBUG_LOG("Protozilla: NewPipeChannel: "+aURI.spec+", "+clientFile.path+", "+clientData+", "+restricted+"\n")

  if (!clientFile.exists()) {
    CONSOLE_LOG("Protozilla: Error - Client file "+clientFile.path+" not found!\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var clientExtn = pzilla.getFileExtn(clientFile).toLowerCase();

  if (clientExtn == "js") {
    var fileContents = pzilla.readFileContents(clientFile, -1);

    if (fileContents.search(/^\s*#!/) != -1) {
      // Skip shell path line
      var lineLen = fileContents.indexOf("\n")+1;
      fileContents = fileContents.substr(lineLen);
    }

    return NewJSChannel(aURI, env, fileContents);
  }

  if (pzilla.unix && !clientFile.isExecutable()) {
    CONSOLE_LOG("Protozilla: Error - client file "+clientFile.path+" not executable!\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  if (!pzilla.ipcService) {
    CONSOLE_LOG("Protozilla: Error - IPCService module needed for executing "+clientFile.path+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  // Create pipe channel
  var pipeChannel = Components.classes[NS_PIPECHANNEL_CONTRACTID].createInstance();

  DEBUG_LOG("NewPipeChannel: pipeChannel = " + pipeChannel + "\n");

  pipeChannel = pipeChannel.QueryInterface(nsIPipeChannel);
  DEBUG_LOG("NewPipeChannel: pipeChannel = " + pipeChannel + "\n");

  // Initialize channel
  var cline = GetCommandLine(clientFile, clientData);
  var noMimeHeaders = false;
  var mergeStderr = false;

  try {
    pipeChannel.init(aURI, cline.executable, cline.args, cline.args.length,
                     env, env.length,
                     0, "", noMimeHeaders, mergeStderr, restricted, null);
  } catch (ex) {

    CONSOLE_LOG("Protozilla: Error - Failed to execute file "+clientFile.path+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  return pipeChannel;
}

function GetContractIDFromCID(cid_str) {
   var contractid = null;
   try {
      var compMgr = Components.manager.QueryInterface(Components.interfaces.nsIComponentRegistrar);
      contractid = compMgr.CIDToContractID(Components.ID(cid_str));
   } catch (ex) {
   }

   var retObj = null;

   if (contractid) {
      retObj = new Object();

      retObj.contractID = contractid;

      var eqOffset = contractid.lastIndexOf("=");

      if (eqOffset >= 0) {
         retObj.handlerName = contractid.substr(eqOffset+1);

         var contractidPrefix = contractid.substr(0,eqOffset+1);

         if (contractidPrefix == PROTOCOL_HANDLER_CONTRACTID_PREFIX) {
            retObj.handlerType = "protocol";
         } else if (contractidPrefix == CONTENT_HANDLER_CONTRACTID_PREFIX) {
            retObj.handlerType = "content";
         }
      }

      //DEBUG_LOG("PZilla.GetContractIDFromCID: cid="+cid_str+", "+retObj.contractID+"\n");
   }

   return retObj;
}


var newProtoChannel =
function (aURI)
{
  DEBUG_LOG("newProtoChannel: URI='"+aURI.spec+"'\n");

  // Refresh protocol handlers
  pzilla.refresh();

  var spec;
  if (aURI.spec == aURI.scheme+":console") {
    // Display protozilla console messages
    spec = "chrome://protozilla/content/protoConsole.htm";

  } else if (aURI.spec == aURI.scheme+":examples") {
    // Display protozilla console messages
    spec = "chrome://protozilla/content/protoExamples.htm";

  } else if (aURI.spec == aURI.scheme+":about") {
    // About Protozilla
    spec = "chrome://protozilla/content/protoAbout.htm";

  } else {
    // Display protozilla home page
    spec = "chrome://protozilla/content/protozilla.xul";
  }

  // ***NOTE*** Creating privileged channel
  return pzilla.newPrivilegedChannel(spec, null, aURI.spec);
}

function isAbsolutePath(filePath) {
  // Check if absolute path
  if (pzilla.unix) {
    return (filePath.search(/^\//) == 0)
  } else {
    return (filePath.search(/^\w+:\\/) == 0);
  }
}

function ResolvePath(filePath, envPath) {
  DEBUG_LOG("protozilla:ResolvePath: filePath="+filePath+"\n");

  if (isAbsolutePath(filePath))
    return filePath;

  if (!envPath)
     return null;

  var retValue = null;
  var pathDirs = envPath.split(pzilla.unix ? ":" : ";");
  for (var j=0; j<pathDirs.length; j++) {
     var pathDir = Components.classes[NS_LOCAL_FILE_CONTRACTID].createInstance(nsILocalFile);

     pathDir.initWithPath(pathDirs[j]);

     if (pathDir.exists() && pathDir.isDirectory()) {
        pathDir.appendRelativePath(filePath);

        if (pathDir.exists()) {
           retValue = pathDir.path;
           break;
        }
     }
  }

  DEBUG_LOG("protozilla:ResolvePath: return value="+retValue+"\n");
  return retValue;
}

function CreateEnv(aURI, scriptDir, pathInfo) {
  // Set up common environment variables for CGI from URL
  var env = ["SERVER_SOFTWARE=Protozilla/0.1",
            "SERVER_NAME=127.0.0.1",
            "GATEWAY_INTERFACE=CGI/1.1",
            "SERVER_PROTOCOL=HTTP/1.0",
            "REMOTE_ADDR=127.0.0.1"
  ];

  env.push("SCRIPT_DIRECTORY="+scriptDir);

  // Inherited environment variables
  for (var j=0; j<pzilla.passEnv.length; j++)
    env.push(pzilla.passEnv[j]);

  if (!aURI)
    return env;

  var uriSpec = aURI.spec;
  var colonOffset = uriSpec.indexOf(":");
  var uriData  = uriSpec.substr(colonOffset+1);

   // Additional environment variables: URI_SPEC, URI_DATA, URI_HOST, URI_PORT

  env.push("URI_SPEC="+uriSpec);
  env.push("URI_DATA="+uriData);

  if (uriSpec.indexOf("/") == colonOffset+1) {
    // URI with hierarchical path (RFC 2396); create a nsIURL instance

    var url = Components.classes[NS_STANDARDURL_CONTRACTID].createInstance();
    url = url.QueryInterface(nsIStandardURL);

    // init(urlType:standard, defaultPort, initialSpec, initialBaseURI)
    url.init(1, -1, uriSpec, "", null);

    url = url.QueryInterface(nsIURL);

    // Additional variables for URLs with hierarchical path info
    env.push("SCRIPT_NAME="+url.filePath);

    if (url.query) {
      env.push("QUERY_STRING="+url.query);
    } else {
      env.push("QUERY_STRING=");
    }

    if (pathInfo != null) {
        env.push("PATH_INFO="+pathInfo);
    } else {
        env.push("PATH_INFO="+url.filePath);
    }

    try {
      if (url.host)
        env.push("URI_HOST="+url.host);

      if (url.port >= 0)
        env.push("URI_PORT="+url.port);
    } catch (ex) {
    }

  } else {
    // URI without hierarchical path (RFC 2396)
    env.push("QUERY_STRING=" + uriData.replace(/^\?/,"") );
  }

  DEBUG_LOG("CreateEnv: "+env+"\n");

  return env;
}


var newCGIChannel =
function (aURI)
{
  DEBUG_LOG("newCGIChannel: URI='"+aURI.spec+"'\n");

  // Ensure that the CGI URI has privilege (else an exception will occur)
  var plainSpec = pzilla.removePrivilegeFromSpec(aURI.spec, true);
   
  if (plainSpec.toLowerCase() == "cgi+:") {
    // Display CGI+ home page
    return NewChannel("chrome://protozilla/content/protoCGI.xul", null,
                      plainSpec);
  }

  var colonOffset = plainSpec.indexOf(":");
  var schemeName = plainSpec.substr(0,colonOffset).toLowerCase();
  var uriData = plainSpec.substr(colonOffset+1);

  if (schemeName != "cgi+")  // assertion
  throw Components.results.NS_ERROR_FAILURE;

  // Create URL from plain spec
  DEBUG_LOG("newCGIChannel: creating URL\n");
  var url = Components.classes[NS_STANDARDURL_CONTRACTID].createInstance();
  url = url.QueryInterface(nsIStandardURL);

  // init(urlType, defaultPort, initialSpec, charSet, initialBaseURI)
  url.init(1, -1, plainSpec, "", null);

  url = url.QueryInterface(nsIURL);

  var filePath = url.filePath

  var fileName;
  var pathInfo;
  var slashOffset = filePath.indexOf("/",1);
  if (slashOffset > 1) {
    // Extract first path component as file name
    fileName = filePath.substr(1,slashOffset-1);
    pathInfo = filePath.substr(slashOffset+1);
  } else {
    fileName = filePath.substr(1);
    pathInfo = "";
  }

  DEBUG_LOG("newCGIChannel: fileName="+fileName+"\n");
  DEBUG_LOG("newCGIChannel: pathInfo="+pathInfo+"\n");

  if (!fileName) // assertion
  throw Components.results.NS_ERROR_FAILURE;

  // For extra security, ensure that filename does not contain ".."
  if (fileName.search(/\.\./) != -1) {
    CONSOLE_LOG("Protozilla: Error - filename contains '..'\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  // CGI file object
  var cgiFile = pzilla.packageDir.clone();
  cgiFile.append("cgi+");

  var scriptDir = cgiFile.path;

  cgiFile.append(fileName);
  DEBUG_LOG("cgiFile = "+cgiFile.path+"\n");

  // Create simple URI from plain spec
  uri = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
  uri.spec = plainSpec;

  var env = CreateEnv(uri, scriptDir, pathInfo);

  env.push("REQUEST_METHOD=GET");   // POST etc. not yet implemented

  DEBUG_LOG("newCGIChannel:env= "+env+"\n");

  return NewPipeChannel(uri, cgiFile, url.query, env, true);
}


function PredefProtocol(cid_str, newChannelFunc)
{
  this.cid_str = cid_str;
  this.newChannelFunc = newChannelFunc;
}

function PZilla(registeringModule)
{

  DEBUG_LOG("PZilla: START "+registeringModule+"\n");
  this.registeringModule = registeringModule;

  var httpHandler = Components.classesByID[NS_HTTPPROTOCOLHANDLER_CID_STR].createInstance();
  httpHandler = httpHandler.QueryInterface(nsIHttpProtocolHandler);

  this.oscpu = httpHandler.oscpu;
  DEBUG_LOG("oscpu="+this.oscpu+"\n");

  this.platform = httpHandler.platform;
  DEBUG_LOG("platform="+this.platform+"\n");

  this.unix = (this.platform == "X11");

  this.passEnv = new Array();

  try {
    var ipcService = Components.classes[NS_IPCSERVICE_CONTRACTID].getService();
    ipcService = ipcService.QueryInterface(nsIIPCService);
    WRITE_LOG("ipcService="+ipcService+"\n");

    var nspr_log_modules = GetSysEnv("NSPR_LOG_MODULES");

    var matches = nspr_log_modules.match(/protozilla:(\d+)/);

    if (matches && (matches.length > 1)) {
      gLogLevel = matches[1];
      WARNING_LOG("Protozilla: gLogLevel="+gLogLevel+"\n");
    }

    if (0) {
        // TEST STUFF
        WRITE_LOG("ipcService execSh="+ipcService.execSh("ls -l")+"\n\n");

        var inputData = "TEST DATA\n";
        var outputObj = new Object();
        var errorObj = new Object();
        var outCountObj = new Object();
        var errCountObj = new Object();

        WRITE_LOG("ipcService execPipe="+ipcService.execPipe("cat > test.dat",
                   true, "", inputData, inputData.length, [], 0,
                   outputObj, outCountObj, errorObj, errCountObj)+"\n");
        WRITE_LOG("ipcService outputObj.value = "+outputObj.value+"\n");
        WRITE_LOG("ipcService errorObj.value = "+errorObj.value+"\n\n");

        WRITE_LOG("ipcService execAsync="+ipcService.execAsync("ls -l", true,
                  null, 0, [], 0, null, null, null)+"\n\n");
    }

    this.ipcService = ipcService;

    this.console = ipcService.console;
    this.cookie = ipcService.cookie;

    // Copy inherited environment variables
    var envList = [];
    envList = envList.concat(gInheritedEnv);

    var passList = GetSysEnv("PROTOZILLA_PASS_ENV");
    if (passList) {
      var passNames = passList.split(":");
      for (var k=0; k<passNames.length; k++)
        envList.push(passNames[k]);
    }

    var definedEnv = new Object();
    for (var j=0; j<envList.length; j++) {
       var name = envList[j];

       if (!definedEnv[name]) {
         definedEnv[name] = true;

         var value = GetSysEnv(name);
         if (value)
           this.passEnv.push(name+"="+value);
       }
    }

  } catch (ex) {
    // Create console
    this.console = new PipeConsole();
    this.console.open(60, 80, false);

    // Nine digit random number
    var rand = 1000000000 + Math.floor(Math.random()*1.0e9);
    var ranDigits = rand.toString().substr(1);

    // Extract 5 "random" digits from the least significant digits of
    // the current time (in ms since 1970)
    var curTime = new Date();
    var timeStr = curTime.getTime().toString();
    var timeDigits = timeStr.substr(timeStr.length-5);

    this.cookie = ranDigits + timeDigits;
  }

  if (gLogLevel >= 4)
    gLogFileStream = CreateFileStream("protdbg1.txt");

  DEBUG_LOG("PZilla: cookie="+this.cookie+"\n");

  var compMgr = Components.manager.QueryInterface(Components.interfaces.nsIComponentRegistrar);

  var compMgrObsolete = Components.manager.QueryInterface(Components.interfaces.nsIComponentManagerObsolete);

  this.moduleFile = compMgrObsolete.specForRegistryLocation(REGISTRY_LOCATION);
  this.registryLocation = REGISTRY_LOCATION;
  this.componentType    = COMPONENT_TYPE;

  // Pre-defined protocols
  this.predef = new Object();
  this.predef["protozilla"]= new PredefProtocol(NS_PROTOCHANNEL_CID, newProtoChannel);
  this.predef["cgi+"] = new PredefProtocol(NS_CGICHANNEL_CID, newCGIChannel);

  for (var protoName in this.predef) {
    // Register pre-defined protocols, if not already registered

    var protoObj = this.predef[protoName];
    var contractid = PROTOCOL_HANDLER_CONTRACTID_PREFIX+protoName;

    var oldContractid = "";
    try {
      oldContractid = compMgr.CIDToContractID(Components.ID(protoObj.cid_str));
    } catch (ex) {
    }

    if (this.registeringModule || (oldContractid != contractid)) {
      compMgr.registerFactoryLocation(Components.ID(protoObj.cid_str),
                                      protoName+" protocol handler",
                                      contractid,
                                      this.moduleFile,
                                      this.registryLocation,
                                      this.componentType);
      DEBUG_LOG("PZilla: ***Registered "+contractid+": "+protoObj.cid_str+"\n");
    } else {
      DEBUG_LOG("PZilla: ***"+contractid+": already registered\n");
    }
  }

  // Handler info
  this.handlerNumberByName = new Object();
  this.handlerNumberByCID  = new Object();

  this.CIDArray     = new Array(CID_MAX);
  this.handlerArray = new Array(CID_MAX);

  // Create a bunch of CIDs for user defined protocols
  var j;
  var registeredProtoList = new Object();
  var freeCIDArray = new Array();

  for (j=0; j<CID_MAX; j++) {
    var protoNo = j.toString(16);
    if (protoNo.length == 1) protoNo = "0" + protoNo;

    var cid_str = "{"+CID_TEMPLATE.replace("\.\.", protoNo)+"}";
    this.CIDArray[j]                 = cid_str;
    this.handlerArray[j]             = null;
    this.handlerNumberByCID[cid_str] = j+1;

    // Check if CID is already in use
    var idObj = GetContractIDFromCID(cid_str);

    if (idObj) {
      if (this.registeringModule) {
        // Re-register component (just in case registryLocation has changed)
        compMgr.registerFactoryLocation(Components.ID(cid_str),
                                        idObj.handlerName+" "+idObj.handlerType+
                                        " handler",
                                        idObj.contractID,
                                        this.moduleFile,
                                        this.registryLocation,
                                        this.componentType);
      }

      if (idObj.handlerType == "protocol")
        registeredProtoList[idObj.handlerName] = cid_str;

      DEBUG_LOG("Protozilla: Re-registered "+idObj.contractID+"\n");

    } else {
      // List of free CIDs
      freeCIDArray.push(cid_str);
    }
  }

  // Placeholder registration for bundled protocols (if not already registered)
  j = 0;
  for (var k=0; k<gBundledProtoList.length; k++) {
    var bundledProto = gBundledProtoList[k];

    if (!registeredProtoList[bundledProto] && freeCIDArray.length) {

      // Get first available CID
      var cid_avail = freeCIDArray.shift();

      compMgr.registerFactoryLocation(
                  Components.ID(cid_avail),
                  bundledProto+" protocol handler",
                  PROTOCOL_HANDLER_CONTRACTID_PREFIX+bundledProto,
                  this.moduleFile, this.registryLocation,
                  this.componentType);

      WRITE_LOG("Protozilla: Registering bundled protocol "+bundledProto+"\n");
    }
  }

  DEBUG_LOG("PZilla: END\n");
}

PZilla.prototype.registeringModule = false;
PZilla.prototype.ipcService = null;
PZilla.prototype.console = null;
PZilla.prototype.xulOwner = null;
PZilla.prototype.predef = null;
PZilla.prototype.packageDir = null;
PZilla.prototype.initialized = false;
PZilla.prototype.moduleFile = null;
PZilla.prototype.registryLocation = "";
PZilla.prototype.componentType = "";
PZilla.prototype.altConsole = "";
PZilla.prototype.platform = "";
PZilla.prototype.passEnv = null;

PZilla.prototype.QueryInterface =
function (iid) {

  //DEBUG_LOG("PZilla.QueryInterface:\n");
  if (!iid.equals(nsIProtozilla) && !iid.equals(nsISupports))
  throw Components.results.NS_ERROR_NO_INTERFACE;

  return this;
}

PZilla.prototype.startup =
function () {

  DEBUG_LOG("PZilla.startup: START\n");

  if (this.initialized) return;

  // Open temporary XUL channel
  var temChannel = NewChannel("chrome://protozilla/content/dummy.xul",
                              null, "");

  // Get owner of XUL channel
  var xulOwner = temChannel.owner;

  // Cancel XUL request and release channel
  temChannel.cancel(Components.results.NS_BINDING_ABORTED);
  temChannel = null;

  DEBUG_LOG("PZilla.startup: xulOwner="+xulOwner+"\n");

  if (!xulOwner) {
    ERROR_LOG("PZilla.startup: Error - Null XUL owner\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  this.xulOwner = xulOwner;

  // Process directory
  var procDir = this.getFileOfProperty("CurProcD");
  procDir.normalize();

  // Application registry directory
  var regDir = this.getFileOfProperty("AppRegD");
  regDir.normalize();

  // User profile directory
  var profDir = this.getFileOfProperty("ProfD");
  profDir.normalize();

  // Package directory
  var packageDir = profDir.clone();
  packageDir.append(PACKAGE_NAME);

  DEBUG_LOG("PZilla.startup: packageDir = "+packageDir.path+"\n");

  if (!packageDir.exists()) {
    CONSOLE_LOG("Protozilla: Error - Package directory "+packageDir.path+" not found\n");
  }

  this.packageDir = packageDir;

  // Register any new user-defined handlers
  this.refresh();

  // Register to observe XPCOM shutdown
  var obsServ = Components.classes[NS_OBSERVERSERVICE_CONTRACTID].getService();
  obsServ = obsServ.QueryInterface(nsIObserverService);

  var observer = new TopicObserver();
  obsServ.addObserver(observer, NS_XPCOM_SHUTDOWN_OBSERVER_ID, false);

  // Set initialization flag
  this.initialized = true;

  DEBUG_LOG("PZilla.startup: END\n");
}

PZilla.prototype.uninstall =
function () {
  DEBUG_LOG("PZilla.uninstall: START\n");

  // Shutdown handlers
  this.shutdown();

  var compMgr = Components.manager.QueryInterface(Components.interfaces.nsIComponentRegistrar);

  // Unregister handlers
  for (var j=0; j<CID_MAX; j++) {
    var cid_str = this.CIDArray[j];

    var idObj = GetContractIDFromCID(cid_str);

    if (idObj) {
      WRITE_LOG("PZilla.uninstall: Unregistering "+idObj.contractID+"\n");
      compMgr.unregisterFactoryLocation(Components.ID(cid_str),
                                        this.moduleFile);
    }
  }

  DEBUG_LOG("PZilla.uninstall: END\n");
}

PZilla.prototype.shutdown =
function () {
  // Shutdown registered handlers
  DEBUG_LOG("PZilla.shutdown: START\n");

  for (var j=0; j<CID_MAX; j++) {
    var cid_str = this.CIDArray[j];

    var handlerNumber = this.handlerNumberByCID[cid_str];
    var handlerObj = this.handlerArray[handlerNumber-1];

    if (handlerObj) {
      // Shutdown handler
      this.shutdownHandler(handlerObj);
    }
  }

  DEBUG_LOG("PZilla.shutdown: END\n");
}

PZilla.prototype.shutdownHandler =
function (handlerObj) {
  // Shutdown handler
  DEBUG_LOG("PZilla.shutdownHandler: "+handlerObj.name+"\n");

  if (handlerObj.serverTransport != null) {
    // Terminate server
    CONSOLE_LOG("Protozilla: Terminating server for protocol "+handlerObj.name+"\n");
    handlerObj.serverTransport.terminate();
    handlerObj.serverTransport = null;
  }

  if (handlerObj.serverActive) {
    handlerObj.serverActive = false;

    if (handlerObj.stopServerLeafName) {
      // Run server stop script
      var stopServerFile = this.packageDir.clone();
      stopServerFile.append(handlerObj.clientType);

      if (handlerObj.schemeDirName)
        stopServerFile.append(handlerObj.schemeDirName);

      stopServerFile.append(handlerObj.stopServerLeafName);
      ExecuteFile(stopServerFile, []);
    }
  }
}


PZilla.prototype.refresh =
function () {
  // Refresh registration of user-supplied protocol/content handlers

  DEBUG_LOG("PZilla.refresh: START\n");

  // Refresh handlers that are already registered
  for (var j=0; j<CID_MAX; j++) {
    var cid_str = this.CIDArray[j];
    //DEBUG_LOG("PZilla.startup: j="+j+", cid="+cid_str+"\n");

    var idObj = GetContractIDFromCID(cid_str);

    if (idObj) {
      if (idObj.handlerName && idObj.handlerType)
        this.refreshHandler(idObj.handlerName, idObj.handlerType, cid_str);
    }
  }

  // Protocol directory
  var protocolDir = this.packageDir.clone();
  protocolDir.append("protocol");
  DEBUG_LOG("protocolDir = "+protocolDir.path+"\n");

  // Iterate through entries in protocol directory
  var fileList = null;
  if (protocolDir.exists())
  fileList = protocolDir.directoryEntries;

  while (fileList && fileList.hasMoreElements()) {

    var protoFile = fileList.getNext().QueryInterface(nsILocalFile);

    // Protocol name (canonicalized to lower case)
    var protoName = this.getFileName(protoFile).toLowerCase();

    if (protoName.search(/^\.|~$/) != -1)
       continue

    DEBUG_LOG("PZilla.refresh: protoName="+protoName+", protoFile.path="+protoFile.path+"\n");

    if (!this.isValidSchemeName(protoName)) {
      CONSOLE_LOG("Protozilla: **ERROR** Invalid character(s) in protocol name '"+protoName+"'\n");

    } else if (this.predef[protoName]) {
      CONSOLE_LOG("Protozilla: **ERROR** Protocol name '"+protoName+"' conflicts with pre-defined protocol\n");

    } else if (!this.handlerNumberByName["protocol:"+protoName]) {
      // Create protocol handler
      this.refreshHandler(protoName, "protocol", null);
    }
  }

  if (this.initialized) {
    // Start servers for protocols, if not already started
  }

  DEBUG_LOG("PZilla.refresh: END\n");
}

PZilla.prototype.refreshHandler =
function (handlerName, clientType, cid_str) {

  DEBUG_LOG("PZilla.refreshHandler: name="+handlerName+",type="+clientType+",cid_str='"+cid_str+"'\n");

  var alreadyRegistered = (cid_str != null);

  // Ensure that the client executable is available
  var protoDir = this.packageDir.clone();
  protoDir.append(clientType);

  var handler = null;
  try {
    handler = this.findFileInDir(protoDir, handlerName);

  } catch (ex) {
    CONSOLE_LOG("Protozilla: Error - Ambigous handler for scheme "+handlerName+": in "+protoDir.path+"\n");
  }

  var schemeDirName = null;

  var clientLeafName = "";
  var indexLeafName = "";
  var startServerLeafName = "";
  var serverLeafName      = "";
  var stopServerLeafName  = "";

  if (handler && handler.exists()) {

    if (handler.isDirectory()) {
      schemeDirName = handler.leafName;

      try {
        // Locate executable "client.*" file in handler directory
        var clientFile = this.findFileInDir(handler, CLIENT_FILEHEAD);
        if (clientFile)
        clientLeafName = clientFile.leafName;

        // Locate index file "index.*" in protocol directory
        var indexFile = this.findFileInDir(handler, INDEX_FILEHEAD);
        if (indexFile)
        indexLeafName = indexFile.leafName;

        // Locate script "start-server.*" in protocol directory
        var startServerFile = this.findFileInDir(handler, STARTSERVER_FILEHEAD);
        if (startServerFile)
        startServerLeafName = startServerFile.leafName;

        // Locate server file "server.*" in protocol directory
        var serverFile = this.findFileInDir(handler, SERVER_FILEHEAD);
        if (serverFile)
        serverLeafName = serverFile.leafName;

        // Locate script "stop-server.*" in protocol directory
        var stopServerFile = this.findFileInDir(handler, STOPSERVER_FILEHEAD);
        if (stopServerFile)
        stopServerLeafName = stopServerFile.leafName;

      } catch (ex) {
        CONSOLE_LOG("Protozilla: Error - Ambigous client/index/server filename in "+handler.path+"\n");
        clientLeafName = "";
      }

    } else {
      // Handler is a simple file
      schemeDirName = "";
      clientLeafName = handler.leafName;
    }
  }

  if (!clientLeafName && !cid_str)
  return;

  var unregister = false;
  var handlerObj = null;
  var oldHandlerNumber = null;

  if (alreadyRegistered) {
    oldHandlerNumber = this.handlerNumberByCID[cid_str];
    handlerObj = this.handlerArray[oldHandlerNumber-1];

    if (handlerObj) {
      // Handler exists
      if (handlerObj.clientType != clientType) {
        unregister = true;

      } else  if (handlerObj.serverLeafName != serverLeafName) {
        unregister = true;
      }

      if (handlerObj.serverTransport &&
          !handlerObj.serverTransport.isAttached()) {
            // Shutdown "zombie" server process
            CONSOLE_LOG("Protozilla: Server for protocol "+handlerObj.name+" appears to have died!\n");
            handlerObj.serverTransport.terminate();
            handlerObj.serverTransport = null;

            this.shutdownHandler(handlerObj);
          }
    }
  }

  var compMgr = Components.manager.QueryInterface(Components.interfaces.nsIComponentRegistrar);

  if (!clientLeafName || unregister) {
    // Unregister handler, and return if not client file
    DEBUG_LOG("Protozilla: Unregistering handler "+handlerName+
              " with cid='"+cid_str+"'\n");

    compMgr.unregisterFactoryLocation(Components.ID(cid_str),
                                      this.moduleFile);

    if (handlerObj) {
      // Shutdown handler
      this.shutdownHandler(handlerObj);

      // Delete handler from all hashes
      delete this.handlerNumberByName[handlerObj.clientType+":"+handlerObj.name];

      this.handlerArray[oldHandlerNumber-1] = null;

      handlerObj = null;
    }

    DEBUG_LOG("PZilla.refreshHandler: ***Unregistration completed\n");

    if (!clientLeafName)
    return;
  }

  if (!cid_str) {
    // Obtain free CID
    var j;
    for (j=0; j<CID_MAX; j++)
    if (this.handlerArray[j] == null) break;

    if (j >= CID_MAX) {
      ERROR_LOG("PZilla.refresh: Error - More than "+CID_MAX+" protocols/content handlers!!!\n");
      return;
    }

    cid_str = this.CIDArray[j];
  }

  if (!handlerObj) {
    // Create new handler
    handlerObj = new PZHandler(handlerName, clientType, cid_str);
  }

  // Refresh handler
  var handlerNumber = this.handlerNumberByCID[cid_str];
  this.handlerNumberByName[clientType+":"+handlerName] = handlerNumber;

  this.handlerArray[handlerNumber-1] = handlerObj;

  DEBUG_LOG("PZilla.refreshHandler: handlerNumber="+handlerNumber+", cid="+cid_str+"\n");

  handlerObj.schemeDirName  = schemeDirName;
  handlerObj.clientLeafName = clientLeafName;
  handlerObj.indexLeafName  = indexLeafName;
  handlerObj.startServerLeafName = startServerLeafName;
  handlerObj.serverLeafName      = serverLeafName;
  handlerObj.stopServerLeafName  = stopServerLeafName;

  if (!handlerObj.serverActive) {
    handlerObj.serverActive = true;

    if (startServerLeafName) {
      // Run server startup script
      var startFile = this.packageDir.clone();
      startFile.append(handlerObj.clientType);

      if (handlerObj.schemeDirName)
        startFile.append(handlerObj.schemeDirName);

      startFile.append(handlerObj.startServerLeafName);

      ExecuteFile(startServerFile, []);
    }

    if (serverLeafName && pzilla.ipcService) {
      // Startup server
      var pipetrans = Components.classes[NS_PIPETRANSPORT_CONTRACTID].createInstance();

      pipetrans = pipetrans.QueryInterface(nsIPipeTransport);
      DEBUG_LOG("PZilla.refreshHandler: pipetrans = " + pipetrans + "\n");

      var serverFileObj = protoDir.clone();
      if (schemeDirName)
        serverFileObj.append(schemeDirName);
      serverFileObj.append(serverLeafName);

      var executable = serverFileObj.path;
      var args = [];
      var env = CreateEnv(null, serverFileObj.parent.path, null);

      CONSOLE_LOG("Protozilla: Starting server "+executable+"\n");

      var serverExtn = pzilla.getFileExtn(serverFileObj).toLowerCase();

      var cline = GetCommandLine(serverFile, null);

      var noProxy = false;
      var mergeStderr = false;

      try {
          pipetrans.init(cline.executable, cline.args, cline.args.length,
                         env, env.length,
                         0, "", noProxy, mergeStderr,
                         pzilla.ipcService.console);

          pipetrans.loggingEnabled = true;

          pipetrans.asyncRead(null, null, 0, Number.MAX_VALUE, 0);

          handlerObj.serverTransport = pipetrans;
      } catch (ex) {
          CONSOLE_LOG("Protozilla: Error - Failed to start server for protocol "+handlerName+"\n");
      }
    }
  }

  if (!alreadyRegistered) {
    // Register handler
    var replace = true;
    var persist = true;

    if (this.isOverride(handlerObj.contractid)) {
        // Overriding system protocol
        handlerObj.override = true;
        CONSOLE_LOG("Protozilla: **Overriding system protocol "+
                    handlerObj.name+ ":\n");
    }

    compMgr.registerFactoryLocation(
                  Components.ID(handlerObj.cid_str),
                  handlerObj.name+" "+handlerObj.clientType+" handler",
                  handlerObj.contractid,
                  this.moduleFile, this.registryLocation,
                  this.componentType);
    DEBUG_LOG("PZilla.refreshHandler: ***Registered "+handlerObj.contractid+": "+handlerObj.cid_str+"\n");
  }

  DEBUG_LOG("PZilla.refreshHandler: END\n");
}


PZilla.prototype.isOverride =
function (contractid) {
  var jscid = Components.classes[contractid];
  if (!jscid)
  return false;

  // Get CID
  var cidStr = jscid.number ? jscid.number.toString() : jscid.id.toString();
  var cidMatch = cidStr.search(new RegExp(CID_TEMPLATE));

  return (cidMatch == -1);
}

PZilla.prototype.getHandlerByCID =
function (cid_str) {
  DEBUG_LOG("PZilla:getHandlerByCID: cid="+cid_str+"\n");

  var handlerNumber = this.handlerNumberByCID[cid_str];

  if (!handlerNumber || !this.handlerArray[handlerNumber-1])
  return "";

  return this.handlerArray[handlerNumber-1];
}

PZilla.prototype.getHandlerByName =
function (handlerName, handlerType) {
  DEBUG_LOG("PZilla:getHandlerByName: "+handlerName+", "+handlerType+"\n");

  var handlerNumber = this.handlerNumberByName[handlerType+":"+handlerName];
  if (!handlerNumber)
  return null;

  DEBUG_LOG("PZilla:getHandlerByName: number="+handlerNumber+"\n");

  var handlerObj = this.handlerArray[handlerNumber-1];

  return handlerObj;
}

PZilla.prototype.getHandlerNamesByType =
function (handlerType) {
  DEBUG_LOG("PZilla:getHandlerNamesByType: type="+handlerType+"\n");

  var handlerNames = [];
  for (j=0; j<CID_MAX; j++) {
    var handlerObj = this.handlerArray[j];
    if (handlerObj && (handlerObj.clientType == handlerType)) {
      handlerNames.push(handlerObj.name);
    }
  }

  return handlerNames.sort();
}

PZilla.prototype.copyHandlerFromURL =
function (handlerURL, handlerType) {
  DEBUG_LOG("PZilla:copyHandlerFromURL: '"+handlerURL+"', "+handlerType+"\n");

  var colonOffset = handlerURL.indexOf(":");
  var schemeName = handlerURL.substr(0,colonOffset).toLowerCase();

  if (schemeName != "file")
  return "Error - Only file: URLs can be dragged and dropped\n";

  var filePath = handlerURL.substr(colonOffset+1);
  filePath = filePath.replace(/^\/\//,"");

  DEBUG_LOG("PZilla:copyHandlerFromURL: filePath="+filePath+"\n");

  var handlerFile = Components.classes[NS_LOCAL_FILE_CONTRACTID].createInstance(nsILocalFile);

  handlerFile.initWithPath(filePath);

  if (!handlerFile.exists())
  return "Error - File "+handlerFile.path+" does not exist\n";

  // Protocol name (canonicalized to lower case)
  var protoName = this.getFileName(handlerFile).toLowerCase();

  DEBUG_LOG("PZilla.copyHandlerFromURL: protoName="+protoName+", handlerFile.path="+handlerFile.path+"\n");

  if (!this.isValidSchemeName(protoName))
  return "Error - Invalid character(s) in protocol name '"+protoName+"'\n";

  var handlerNumber = this.handlerNumberByName["protocol:"+protoName];
  if (handlerNumber)
  return "Error - Protocol handler "+protoName+" already exists; please delete it\n";

  // Copy handler file
  CONSOLE_LOG("Protozilla: Copying "+handlerType+" handler from "+handlerFile.path+" ...\n");
  var protocolDir = this.packageDir.clone();
  protocolDir.append("protocol");

  try {
    handlerFile.copyToFollowingLinks(protocolDir, null);
  } catch (ex) {
    return "Error - Unable to copy handler "+handlerFile.path+
           " to directory "+protocolDir+"\n";
  }

  return protoName;
}

PZilla.prototype.execSh =
function (command) {
  DEBUG_LOG("PZilla::execSh: "+command+"\n");
  if (!this.ipcService) {
    CONSOLE_LOG("Protozilla: Error - IPCService module needed for executing commands\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  return this.ipcService.execSh(command);
}

PZilla.prototype.addPrivilegeToSpec =
function (aSpec) {
  DEBUG_LOG("PZilla::addPrivilegeToSpec: "+aSpec+"\n");

  var colonOffset = aSpec.indexOf(':');
  if (colonOffset < 0)
  throw Components.results.NS_ERROR_FAILURE;

  var newSpec;

  if (aSpec.indexOf("/") == colonOffset+1) {
    // URI with hierarchical path (RFC 2396); create a nsIURL instance

    var url =Components.classes[NS_STANDARDURL_CONTRACTID].createInstance();
    url = url.QueryInterface(nsIStandardURL);

    // init(urlType:standard, defaultPort, charSet,initialSpec, initialBaseURI)
    url.init(1, -1, aSpec, "", null);

    url = url.QueryInterface(nsIURL);

    newSpec = url.scheme + ":";

    if (aSpec.indexOf("/",colonOffset+2) == colonOffset+2) {
      // Net path (begins with double slash)
      newSpec += "//x:" + this.cookie + "@np";
    } else {
      // Absolute path (begins with single slash)
      newSpec += "//x:" + this.cookie + "@ap";
    }

    if (url.host) {
      newSpec += url.host;
      if (url.port >= 0)
      newSpec += ":" + url.port;
    }

    newSpec += url.path;

  } else {
    // URI without hierarchical path (RFC 2396)
    newSpec = aSpec.substr(0,colonOffset+1) +
    "//x:" + this.cookie + "@nh/" +
    aSpec.substr(colonOffset+1);
  }


  return newSpec;
}

PZilla.prototype.removePrivilegeFromSpec =
function (aPrivilegedSpec, allowNoData) {
  DEBUG_LOG("PZilla::removePrivilegeFromSpec: "+aPrivilegedSpec+", "+allowNoData+"\n");

  if (aPrivilegedSpec.indexOf(":") == aPrivilegedSpec.length-1) {
    // Special case: URI has no data portion, only scheme portion
    if (allowNoData)
      return aPrivilegedSpec;

    CONSOLE_LOG("Protozilla: Error - Unable to load/reload restricted URI "+aPrivilegedSpec+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }


  var url = Components.classes[NS_STANDARDURL_CONTRACTID].createInstance();
  url = url.QueryInterface(nsIStandardURL);

  // init(urlType:authority, defaultPort, charSet, initialSpec, initialBaseURI)
  url.init(2, -1, aPrivilegedSpec, "", null);

  url = url.QueryInterface(nsIURL);

  var userPass = url.userPass;
  var host     = url.host;
  var path     = url.path;

  if (!host || (host.length < 2) ) {
    ERROR_LOG("PZilla::removePrivilegeFromSpec: Error - Invalid host for "+aPrivilegedSpec+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var userPassCompare = "x:"+this.cookie.toLowerCase();

  if (!userPass || (userPass.toLowerCase() != userPassCompare)) {
    CONSOLE_LOG("Protozilla: Error - Unable to load/reload restricted URI "+aPrivilegedSpec+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var newSpec = url.scheme + ":";

  if (host == "nh") {
    // Non-hierarchical URI
    if (path)
    newSpec += path.substr(1);

  } else {
    // Hierarchical URI

    if (host.substr(0,2) == "np")
    newSpec += "//";

    newSpec += host.substr(2);

    if (url.port >= 0)
    newSpec += ":" + url.port;

    if (path)
    newSpec += path;
  }

  DEBUG_LOG("PZilla::removePrivilegeFromSpec: newspec="+newSpec+"\n");

  return newSpec;
}


// Creates a channel with the system principal (use carefully!)
PZilla.prototype.newPrivilegedChannel =
function (aSpec, aBaseURI, aOriginalSpec) {

  // Get URI scheme
  var scheme = "";

  if (aBaseURI) {
    scheme = aBaseURI.scheme;

  } else {
    var colonOffset = aSpec.indexOf(':');
    if (colonOffset > 0)
    scheme = aSpec.substr(0,colonOffset);
  }

  scheme = scheme.toLowerCase();

  if ((scheme != "file") && (scheme != "chrome") && (scheme != "resource")) {
    ERROR_LOG("PZilla::newPrivilegedChannel: Error - Scheme "+scheme+" cannot be privileged\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var privChannel = NewChannel(aSpec, aBaseURI, aOriginalSpec);

  // Make new channel owned by XUL owner
  privChannel.owner = this.xulOwner

  return privChannel;
}

PZilla.prototype.isValidSchemeName =
function (schemeName) {
  // Check for valid scheme name per RFC 2396, Sec. 3.1
  return (schemeName.search(/^[a-z]([a-z0-9+.\-])*$/) != -1);
}

PZilla.prototype.getFileOfProperty =
function (prop) {
  var dscontractid = "@mozilla.org/file/directory_service;1";
  var ds = Components.classes[dscontractid].getService();

  var dsprops = ds.QueryInterface(Components.interfaces.nsIProperties);
  DEBUG_LOG("PZilla.getFileOfProperty: prop="+prop+"\n");
  var file = dsprops.get(prop, nsIFile);
  return file;
}

PZilla.prototype.getFileName =
function (fileObj) {
  var leafName = fileObj.leafName;

  var dotOffset = leafName.lastIndexOf(".");

  return (dotOffset == -1) ? leafName : leafName.substr(0,dotOffset);
}

PZilla.prototype.getFileExtn =
function (fileObj) {
  var leafName = fileObj.leafName;

  var dotOffset = leafName.lastIndexOf(".");

  return (dotOffset == -1) ? "" : leafName.substr(dotOffset+1)
}

// Find file in directory with name fileHead and any extension,
// and return the associated nsILocalFile object if successful
// or null otherwise.
// Throws an exception if multiple matches occur.
// Ignores filenames starting with "." or ending with tilde (~)
// (Not case-sensitive)
PZilla.prototype.findFileInDir =
function (dir, fileHead)
{
  if (!dir.exists() || !dir.isDirectory())
  return null;

  DEBUG_LOG("PZilla.findFileInDir: "+dir.path+", "+fileHead+"\n");

  var nameCompare = fileHead.toLowerCase();
  var fileList = dir.directoryEntries;
  var fileFound = null;
  while (fileList.hasMoreElements()) {

    var fileObj = fileList.getNext().QueryInterface(nsILocalFile);

    var leafName = fileObj.leafName;

    // Ignore filenames starting with "." or ending with tilde (~)
    if (leafName.search(/^\.|~$/) != -1)
      continue;

    var dotOffset = leafName.lastIndexOf(".");

    var fName = (dotOffset == -1) ? leafName : leafName.substr(0,dotOffset);

    //DEBUG_LOG("PZilla.findFileInDir: file="+leafName+"\n");

    if (fName.toLowerCase() == nameCompare) {
      if (fileFound)
      throw Components.results.NS_ERROR_FAILURE;

      fileFound = fileObj;
    }
  }

  return fileFound;
}

// maxBytes == -1 => read whole file
PZilla.prototype.readFileContents =
function (localFile, maxBytes) {
  DEBUG_LOG("Protozilla: readFileContents: file="+localFile.leafName+", "+
                                           maxBytes+"\n");
  var fileChannel = new FileChannel();

  if (!localFile.exists() || !localFile.isReadable())
    throw Components.results.NS_ERROR_FAILURE;

  fileChannel.init(localFile, NS_RDONLY, 0);

  var rawInStream = fileChannel.open();

  var scriptableInStream = new InputStream();    
  scriptableInStream.init(rawInStream);

  if ((maxBytes < 0) || (maxBytes > localFile.fileSize))
    maxBytes = localFile.fileSize;

  var fileContents = scriptableInStream.read(maxBytes);

  rawInStream.close();

  return fileContents;
}

PZilla.prototype.writeFileContents =
function (localFile, data, permissions) {
  DEBUG_LOG("Protozilla: writeFileContents: file="+localFile.leafName+"\n");

  if (!permissions)
    permissions = DEFAULT_FILE_PERMS;

  if (localFile.exists()) {
    if (localFile.isDirectory() || !localFile.isWritable())
    throw Components.results.NS_ERROR_FAILURE;
    permissions = localFile.permissions;
  }

  var flags = NS_WRONLY | NS_CREATE_FILE | NS_TRUNCATE;

  var fileOutStream = Components.classes[NS_LOCALFILEOUTPUTSTREAM_CONTRACTID].createInstance(nsIFileOutputStream);

  fileOutStream.init(localFile, flags, permissions, 0);

  if (data.length) {
    if (fileOutStream.write(data, data.length) != data.length)
      throw Components.results.NS_ERROR_FAILURE;

    fileOutStream.flush();
  }
  fileOutStream.close();

  return;
}

// nsIPipeConsole implementation (without the native methods)

function PipeConsole()
{
}

PipeConsole.prototype = {

  _data: "",
  _newOffset: 0,
  _maxLines: 0,
  _maxCols: 0,
  joinable: false,
  overflowed: false,

  QueryInterface: function (iid) {
    if (!iid.equals(nsIPipeConsole) && !iid.equals(nsISupports))
      throw Components.results.NS_ERROR_NO_INTERFACE;
    return this;
  },

  open: function (maxRows, maxCols, joinable) {
    DEBUG_LOG("PipeConsole::open: maxLines="+maxRows+"\n");
    this._maxLines = maxRows;
    this._maxCols = maxCols >= 3 ? maxCols : 3;
    this._data = "";
    this._newOffset = 0;
  },

  observe: function (observer, context) {
    DEBUG_LOG("PipeConsole::observe:\n");
  },

  join: function () {
    DEBUG_LOG("PipeConsole::join:\n");
  },

  shutdown: function () {
    DEBUG_LOG("PipeConsole::shutdown:\n");
    this._data = "";
  },

  write: function (str) {
    // Append new data (line wrapping not yet implemented!)
    this._data += str;

    // Count complete lines
    var searchOffset = this._data.length;
    var lineCount = 0;

    for (;;) {
      searchOffset = this._data.lastIndexOf("\n",searchOffset-1);
      if (searchOffset < 0) break;

      lineCount++;
      if (lineCount > this._maxLines) {
        // Too many lines; delete oldest lines
        this.overflowed = true;
        this._data = this._data.substr(searchOffset+1);
        this._newOffset -= searchOffset+1;
        if (this._newOffset < 0) this._newOffset = 0;
        break;
      }
    }
  },

  getByteData: function (count) {
    // Append new data (line wrapping not yet implemented!)
    var dataStr = this._data.substr(this._newOffset);
    count.value = dataStr.length;
    return dataStr;
  },


  getNewData: function ()
  {
    this._newOffset = this._data.length;
    return this._data.substr(this._newOffset); },

  getData: function ()
  {
    this._newOffset = this._data.length;
    return this._data;
   },

  hasNewData: function()
  { 
    return (this._newOffset < this._data.length);
  },

// nsIStreamListener
  onStartRequest: function (channel, ctxt)
  {
    DEBUG_LOG("PipeConsole: onStartRequest\n");
  },

    onStopRequest: function (channel, ctxt, status)
  {
    DEBUG_LOG("PipeConsole: onStopRequest\n");
  },

  onDataAvailable: function (channel, ctxt, inStream, sourceOffset, count)
  {
    DEBUG_LOG("PipeConsole: onDataAvailable (ignored)\n");
  }
}

///////////////////////////////////////////////////////////////////////////////

function PZHandler(name, clientType, cid_str)
{
  DEBUG_LOG("PZHandler:CTOR "+name+", "+clientType+", "+cid_str+"\n");

  this.name       = name;
  this.clientType = clientType;
  this.cid_str    = cid_str;

  if (clientType == "protocol") {
    this.contractid = PROTOCOL_HANDLER_CONTRACTID_PREFIX+name;
  } else {
    this.contractid = CONTENT_HANDLER_CONTRACTID_PREFIX+name;
  }
}

PZHandler.prototype.name            = "";  // Protocol or MIME type name
PZHandler.prototype.clientType      = "";  // "protocol" or "content"
PZHandler.prototype.cid_str         = "";
PZHandler.prototype.contractid      = "";
PZHandler.prototype.override        = false;
PZHandler.prototype.schemeDirName   = "";  // Protocol directory name
PZHandler.prototype.clientLeafName  = "";  // CGI client name (.pl, ...)
PZHandler.prototype.indexLeafName   = "";  // Home "page" (.htm, .html, .xul)
PZHandler.prototype.startServerLeafName = "";
PZHandler.prototype.serverLeafName      = "";
PZHandler.prototype.stopServerLeafName  = "";
PZHandler.prototype.serverActive        = false;
PZHandler.prototype.serverTransport     = null;

PZHandler.prototype.QueryInterface =
function (iid) {

  if (!iid.equals(nsIProtozillaHandler) && !iid.equals(nsISupports))
  throw Components.results.NS_ERROR_NO_INTERFACE;

  return this;
}

///////////////////////////////////////////////////////////////////////////////

/* Topic observer */
function TopicObserver()
{
}

TopicObserver.prototype.QueryInterface =
function (iid) {

  if (!iid.equals(nsIObserver) && !iid.equals(nsISupports))
  throw Components.results.NS_ERROR_NO_INTERFACE;

  return this;
}

TopicObserver.prototype.observe =
function (aSubject, aTopic, someData)
{
  DEBUG_LOG("TopicObserver: aTopic = "+aTopic+"\n");
  DEBUG_LOG("TopicObserver: someData = "+someData+"\n");
  DEBUG_LOG("TopicObserver: aSubject = "+aSubject+"\n");

  if (aTopic == NS_XPCOM_SHUTDOWN_OBSERVER_ID)
    pzilla.shutdown();
}

///////////////////////////////////////////////////////////////////////////////

function testPrivURL() {

  var privC = pzilla.newPrivilegedChannel("file:///home/svn/m.htm");
  WRITE_LOG("testPrivURL: privC="+privC+"\n");

  try {
    var privC2 = pzilla.newPrivilegedChannel("http://localhost/m.htm");
    WRITE_LOG("testPrivURL: privC2="+privC2+"\n");
  } catch (ex) {
  }

  var url;
  url = "abcd:/efg";
  WRITE_LOG("testPrivURL: url="+url+"\n");
  url = pzilla.addPrivilegeToSpec(url);
  WRITE_LOG(url+"\n");
  WRITE_LOG("testPrivURL: new="+pzilla.removePrivilegeFromSpec(url,false)+"\n\n");

  url = "abcd:///mno";
  WRITE_LOG("testPrivURL: url="+url+"\n");
  url = pzilla.addPrivilegeToSpec(url);
  WRITE_LOG(url+"\n");
  WRITE_LOG("testPrivURL: new="+pzilla.removePrivilegeFromSpec(url,false)+"\n\n");

  url = "abcd://host.edu/jkl";
  WRITE_LOG("testPrivURL: url="+url+"\n");
  url = pzilla.addPrivilegeToSpec(url);
  WRITE_LOG(url+"\n");
  WRITE_LOG("testPrivURL: new="+pzilla.removePrivilegeFromSpec(url,false)+"\n\n");

  url = "abcd://user@host.edu/jkl";
  WRITE_LOG("testPrivURL: url="+url+"\n");
  url = pzilla.addPrivilegeToSpec(url);
  WRITE_LOG(url+"\n");
  WRITE_LOG("testPrivURL: new="+pzilla.removePrivilegeFromSpec(url,false)+"\n\n");

  url = "abcd:e@f";
  WRITE_LOG("testPrivURL: url="+url+"\n");
  url = pzilla.addPrivilegeToSpec(url);
  WRITE_LOG(url+"\n");
  WRITE_LOG("testPrivURL: new="+pzilla.removePrivilegeFromSpec(url,false)+"\n\n");

  //WRITE_LOG("testPrivURL: rem="+pzilla.removePrivilegeFromSpec("abcd://host.edu/mmm",false)+"\n");
  //WRITE_LOG("testPrivURL: rem="+pzilla.removePrivilegeFromSpec("abcd:e@f",false)+"\n");
}

// Global Protozilla "service"
var pzilla = null;

var ProtoModule = {
  registerSelf: function (compMgr, moduleFile, registryLocation, componentType)
  {
    WRITE_LOG("Protozilla: Registering self\n");

    WRITE_LOG("Protozilla: moduleFile="+moduleFile.path+"\n");
    WRITE_LOG("Protozilla: registryLocation="+registryLocation+"\n");
    WRITE_LOG("Protozilla: componentType="+componentType+"\n");

    compMgr = compMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar);

    // Check validity of module self-description constants

    if (registryLocation != REGISTRY_LOCATION) {
      CONSOLE_LOG("Protozilla: Error - Please update value of REGISTRY_LOCATION to '"+registryLocation+"'\n");
      throw Components.results.NS_ERROR_FAILURE;
    }

    if (componentType != COMPONENT_TYPE) {
      CONSOLE_LOG("Protozilla: Error - Please update value of COMPONENT_TYPE to "+componentType+"\n");
      throw Components.results.NS_ERROR_FAILURE;
    }

    var compMgrObsolete = compMgr.QueryInterface(Components.interfaces.nsIComponentManagerObsolete);

    if (moduleFile.path != compMgrObsolete.specForRegistryLocation(registryLocation).path) {
      CONSOLE_LOG("Protozilla: Error - Please check REGISTRY_LOCATION; it does not seem to map to the correct module file path "+moduleFile.path+"\n");
      throw Components.results.NS_ERROR_FAILURE;
    }

    if (pzilla == null) {
      // Create Protozilla "service" (delay initialization)
      pzilla = new PZilla(true);
    }

    compMgr.registerFactoryLocation( NS_PROTOZILLA_CID,
                                     "Protozilla",
                                     NS_PROTOZILLA_CONTRACTID,
                                     moduleFile, registryLocation,
                                     componentType);
    DEBUG_LOG("ProtoModule: ***Registered Protozilla\n");

    //testPrivURL();

    compMgr.registerFactoryLocation( NS_PROTOZILLA_CLINE_CID,
                                     "Protozilla CommandLine Service",
                                     NS_PROTOZILLA_CLINE_CONTRACTID,
                                     moduleFile, registryLocation,
                                     componentType);
    
    catman = Components.classes["@mozilla.org/categorymanager;1"]
    .getService(nsICategoryManager);
    catman.addCategoryEntry("command-line-argument-handlers",
                            "protozilla command line handler",
                            NS_PROTOZILLA_CLINE_CONTRACTID, true, true);

    DEBUG_LOG("ProtoModule: ***Registered -protozilla handler.\n");

    // Register to observe end of autoregistration
    var obsServ = Components.classes[NS_OBSERVERSERVICE_CONTRACTID].getService();
    obsServ = obsServ.QueryInterface(nsIObserverService);

    var observer = new TopicObserver();
    obsServ.addObserver(observer, NS_XPCOM_AUTOREGISTRATION_OBSERVER_ID, false );
  },

  unregisterSelf: function(componentManager, moduleFile, registryLocation)
  {
    DEBUG_LOG("ProtoModule: unRegisterSelf\n");

    compMgr.unregisterFactoryLocation(NS_PROTOZILLA_CID,
                                               moduleFile);

    compMgr.unregisterFactoryLocation(NS_PROTOZILLA_CLINE_CID,
                                               moduleFile);

    catman = Components.classes["@mozilla.org/categorymanager;1"]
    .getService(nsICategoryManager);
    catman.deleteCategoryEntry("command-line-argument-handlers",
                               NS_PROTOZILLA_CLINE_CONTRACTID, true);
  },

  getClassObject: function (compMgr, cid, iid) {
    DEBUG_LOG("ProtoModule.getClassObject: cid="+cid+"\n");

    if (!iid.equals(Components.interfaces.nsIFactory))
    throw Components.results.NS_ERROR_NOT_IMPLEMENTED;

    if (cid.equals(NS_PROTOZILLA_CLINE_CID))
    return CLineFactory;

    if (pzilla == null) {
      // Create Protozilla "service"
      pzilla = new PZilla(false);
    }

    if (!pzilla.initialized) {
      // Initialize user-defined protocols etc.
      pzilla.startup();
    }

    if (cid.equals(NS_PROTOZILLA_CID)) {
      return new GenericFactory(pzilla);
    }

    for (var protoName in pzilla.predef) {
      // Check if predefined protocol
      var protoObj = pzilla.predef[protoName];

      if (cid.equals(protoObj.cid_str)) {
        var factory = new HandlerFactory(protoName, "protocol",
                                         protoObj.newChannelFunc);
        return factory;
      }
    }

    var handlerObj = pzilla.getHandlerByCID(cid.toString());
    if (!handlerObj)
    throw Components.results.NS_ERROR_FAILURE;

    DEBUG_LOG("ProtoModule.getClassObject: handlerName="+handlerObj.name+"\n");

    return new HandlerFactory(handlerObj.name, handlerObj.clientType, null);
  },

  canUnload: function(compMgr)
  {
    DEBUG_LOG("ProtoModule.canUnload:\n");
    return true;
  }
};

/* Module entry point */
function NSGetModule(compMgr, moduleFile) {
  DEBUG_LOG("NSGetModule:\n");
  return ProtoModule;
}

///////////////////////////////////////////////////////////////////////////////

function GenericFactory(instance)
{
  DEBUG_LOG("GenericFactory: instance="+instance+"\n");
  this._instance = instance
    }

GenericFactory.prototype = {
  _instance: null,

  QueryInterface: function (iid) {

    //DEBUG_LOG("GenericFactory.QueryInterface:"+iid+"\n");
    if (!iid.equals(Components.interfaces.nsIFactory) &&
        !iid.equals(nsISupports))
    throw Components.results.NS_ERROR_NO_INTERFACE;

    return this;
  },

  createInstance: function (outer, iid) {
    DEBUG_LOG("GenericFactory.createInstance: "+this._instance+"\n");
    var instance = this._instance.QueryInterface(iid);
    return instance;
  }
}

function HandlerFactory(name, type, handlerFunc)
{
  //DEBUG_LOG("HandlerFactory: handlerFunc="+handlerFunc+"\n");
  this.name = name;
  this.type = type;
  this.handlerFunc = handlerFunc;
}

HandlerFactory.prototype = {
  name: "",
  type: "",
  locked: false,
  handlerFunc: null,

  QueryInterface: function (iid) {

    //DEBUG_LOG("HandlerFactory.QueryInterface:"+iid+"\n");
    if (!iid.equals(Components.interfaces.nsIFactory) &&
        !iid.equals(nsISupports))
    throw Components.results.NS_ERROR_NO_INTERFACE;

    return this;
  },

  createInstance: function (outer, iid) {
    DEBUG_LOG("HandlerFactory.createInstance:\n");

    if (outer != null)
    throw Components.results.NS_ERROR_NO_AGGREGATION;

    DEBUG_LOG("HandlerFactory.createInstance: this.name = "+this.name+
              ", this.type="+this.type+"\n");

    if (this.type == "protocol") {
      if (!iid.equals(nsIProtocolHandler) && !iid.equals(nsISupports))
      throw Components.results.NS_ERROR_INVALID_ARG;

      return new GenericProtocolHandler(this.name, this.handlerFunc);

    } else {
      if (!iid.equals(nsIContentHandler) && !iid.equals(nsISupports))
      throw Components.results.NS_ERROR_INVALID_ARG;

      return new GenericContentHandler(this.name);
    }
  },

  lockFactory: function (lock) {
    DEBUG_LOG("HandlerFactory.lockFactory: "+lock+"\n");
    this.locked = lock;
  }
}


///////////////////////////////////////////////////////////////////////////////

/* Generic protocol handler component */
function GenericProtocolHandler(scheme, newChannelFunc)
{
  this.scheme = scheme;

  if (newChannelFunc)
    this.newChannel = newChannelFunc;
}

GenericProtocolHandler.prototype.QueryInterface =
function (iid) {

  //DEBUG_LOG("GenericProtocolHandler: QueryInterface\n");
  if (!iid.equals(nsIProtocolHandler) && !iid.equals(nsISupports))
  throw Components.results.NS_ERROR_NO_INTERFACE;

  return this;
}

GenericProtocolHandler.prototype.scheme = "";
GenericProtocolHandler.prototype.defaultPort = -1;
GenericProtocolHandler.prototype.URIType =                                      Components.interfaces.nsIProtocolHandler.URI_NORELATIVE;       


GenericProtocolHandler.prototype.allowPort =
function (port, scheme)
{
  DEBUG_LOG("GenericProtocolHandler.allowPort: port='"+port+"', scheme="+scheme+"\n");

  return false;
}

GenericProtocolHandler.prototype.newURI =
function (aSpec, aCharset, aBaseURI)
{
  DEBUG_LOG("GenericProtocolHandler.newURI: aSpec='"+aSpec+"', base="+(aBaseURI?aBaseURI.spec:"")+"\n");

  var spec = aSpec;
  if (aBaseURI) {
    try {
      // Resolve relative to base URI
      var baseUrl = aBaseURI.QueryInterface(nsIURL);
      spec = baseUrl.resolve(aSpec);
      DEBUG_LOG("new URL spec="+spec+"\n");

    } catch(ex) {
      // Append relative spec to base URI spec
      spec = aBaseURI.spec + aSpec;
      DEBUG_LOG("new URI spec="+spec+"\n");
    }
  }

  var colonOffset = spec.indexOf(":");
  var schemeName = spec.substr(0,colonOffset).toLowerCase();
  var uriData = spec.substr(colonOffset+1);

  this.scheme = schemeName;

  var uri;    
  if (uriData.search(/^\/\//) == -1) {
    // URI spec data portion doesn't begin with a double slash,
    // i.e., no net_path; treat rest of URI as opaque
    // (See Section 3 of RFC 2396 describing URIs)

    uri = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
    uri.spec = spec;

  } else {
    // URI has a net_path in spec; treat as URL

    var url = Components.classes[NS_STANDARDURL_CONTRACTID].createInstance();
    url = url.QueryInterface(nsIStandardURL);

    // init(urlType, defaultPort, initialSpec, charSet, initialBaseURI)
    url.init(1, -1, spec, "", null);

    uri = url.QueryInterface(nsIURI);
  }

  DEBUG_LOG("GenericProtocolHandler.newURI: uri.spec="+uri.spec+"\n");

  return uri;
}

GenericProtocolHandler.prototype.newChannel =
function (aURI)
{
  var uri = aURI;
  var uriSpec = aURI.spec;
  var colonOffset = uriSpec.indexOf(":");
  var schemeName  = uriSpec.substr(0,colonOffset);

  DEBUG_LOG("GenericProtocolHandler.newChannel: URI='"+uriSpec+"'\n");

  var handlerNumber = pzilla.handlerNumberByName["protocol:"+schemeName];

  if (!handlerNumber)
  throw Components.results.NS_ERROR_FAILURE;

  var handlerObj = pzilla.handlerArray[handlerNumber-1];
  if (!handlerObj)
  throw Components.results.NS_ERROR_FAILURE;

  var indexLeafName = handlerObj.indexLeafName;

  var indexAvailable = indexLeafName ? true : false;

  var restricted = false;
  if (schemeName.search(/\+$/) >= 0) {
    // Scheme name ends with '+';
    // Ensure that the URI has privilege (else an exception will occur)
    restricted = true;
    uriSpec = pzilla.removePrivilegeFromSpec(uriSpec, indexAvailable);

    uri = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
    uri.spec = uriSpec;
  }

  var uriData = uriSpec.substr(colonOffset+1);

  // Get protocol directory object
  var protoDir = pzilla.packageDir.clone();
  protoDir.append("protocol");

  if ((uriData.length == 0) && indexAvailable) {
    // No query data; re-direct to protocol index document
    var schemeDir = handlerObj.schemeDirName;

    var indexFile = protoDir.clone();
    if (schemeDir)
      indexFile.append(schemeDir);

    indexFile.append(indexLeafName);
    DEBUG_LOG("indexFile = "+indexFile.path+"\n");

    var indexSpec = "file:"+indexFile.path;

    var indexChannel;
    if (indexLeafName.search(/\.xul$/) != -1) {
      // ***NOTE*** Creating privileged channel for XUL index file
      indexChannel = pzilla.newPrivilegedChannel(indexSpec, null, uriSpec);
    } else {
      indexChannel = NewChannel(indexSpec, null, uriSpec);
    }

    DEBUG_LOG("GenericProtocolHandler.newChannel: indexChannel="+indexChannel+"\n");
    DEBUG_LOG("GenericProtocolHandler.newChannel: indexChannel.owner="+indexChannel.owner+"\n");

    return indexChannel;
  }

  //return new TestChannel(uri);

  // Client file object
  var clientFile = protoDir.clone();

  var dirName = handlerObj.schemeDirName;
  if (dirName) {
    clientFile.append(dirName);
  }

  var scriptDir = clientFile.path;

  var leafName = handlerObj.clientLeafName;
  clientFile.append(leafName);
  DEBUG_LOG("GenericProtocolHandler.newChannel: clientFile = "+clientFile.path+"\n");

  var clientExtn = pzilla.getFileExtn(clientFile).toLowerCase();

  if (!clientFile.exists()) {
    CONSOLE_LOG("Protozilla: Error - client file "+clientFile.path+" not found!\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  var env = CreateEnv(uri, scriptDir, null);

  env.push("REQUEST_METHOD=GET");   // POST etc. not yet implemented

  DEBUG_LOG("GenericProtocolHandler.newChannel:env= "+env+"\n");

  if ((clientExtn != "cmd") && (clientExtn != "url")) {
    // Not command exec or URL re-direction; use pipe to execute client
    return NewPipeChannel(uri, clientFile, uriData, env, restricted);
  }

  // Handle command execution and URL prefix redirection
  // TODO: this should be cached
  var fileData = pzilla.readFileContents(clientFile, -1);

  var executeFile = false;

  if (fileData.search(/^\s*#!/) == 0) {
    // Skip shell path line
    var lineLen = fileData.indexOf("\n")+1;
    fileData = fileData.substr(lineLen);
    executeFile = true;
  }

  if (fileData.search(/^\s*\/\//) == 0) {
    // File starts with Javascript comment; assume executable
    executeFile = true;
  }

  if (executeFile) {
    // Compile JS function
    // TODO: this should be cached
    var jsFunc;
    try {
      jsFunc = Function(fileData);
    } catch (ex) {
      CONSOLE_LOG("Protozilla: Error - Invalid Javascript function body\n");
      throw ex;
    }

    // Execute function call
    fileData = ApplyJSFunc(jsFunc, env);
  }

  if (clientExtn == "cmd") {
    // Execute command
    ExecuteCommand(fileData, env, !executeFile)

    // Throw special exception for nsDocShell::DoURILoad to catch (doesn't work)
    //throw 0x80530000 + 1013;
    //throw Components.results.NS_ERROR_DOM_RETVAL_UNDEFINED;

    throw Components.results.NS_ERROR_FAILURE;
  }

  // URL re-direction
  if (fileData.search(URI_START_PAT) == -1) {
    CONSOLE_LOG("Protozilla: Error - Invalid URL "+fileData+"\n");
    throw Components.results.NS_ERROR_FAILURE;
  }

  // Construct modified URL
  var modSpec;

  if (executeFile) {
    modSpec = fileData;

  } else {
    // Replace CRLF sequences with spaces
    var uriPrefix = fileData.replace(/\r?\n/g," ");

    // Strip anything past a white space (comment)
    uriPrefix = uriPrefix.replace(/\s[^\n]*$/,"");

    // If prefix ends with '/' and data begins with '/' ...
    if ( (uriPrefix.search(/\/$/) != -1) &&
         (uriData.search(/^\//)   != -1) )
          uriData = uriData.substr(1);

    // Modified URI spec
    modSpec = uriPrefix + uriData;
  }

  // Strip any leading spaces from URI
  modSpec = modSpec.replace(/^\s*/,"");

  DEBUG_LOG("GenericProtocolHandler.newChannel: modSpec = "+modSpec+"\n");

  var channel = NewChannel(modSpec, null, uriSpec);

  return channel;

  //var wrapperChannel = new WrapperChannel(uriSpec, uriSpec, channel);

  //return wrapperChannel;
}


///////////////////////////////////////////////////////////////////////////////

/* Generic content handler */
function GenericContentHandler(contentType)
{
  this.contentType = contentType;
}

GenericContentHandler.prototype.QueryInterface =
function (iid) {

  //DEBUG_LOG("GenericContentHandler: QueryInterface\n");
  if (!iid.equals(nsIContentHandler) && !iid.equals(nsISupports))
  throw Components.results.NS_ERROR_NO_INTERFACE;

  return this;
}

GenericContentHandler.prototype.contentType = "";

GenericContentHandler.prototype.handleContent =
function (aContentType, aCommand, aWindowTarget, aSourceContext, aChannel)
{
  DEBUG_LOG("GenericContentHandler.handleContent:" + aContentType + ", " +
            aCommand + ", " + aWindowTarget + ", " + aSourceContext + ", " +
            aChannel.URI.spec + ")\n");
}


///////////////////////////////////////////////////////////////////////////////

/* Test channel used by protocol handler */
function TestChannel(aURI)
{
  this.URI = aURI;
  this._originalURI = aURI;

  var pipetrans = Components.classes[NS_PIPETRANSPORT_CONTRACTID].createInstance();

  pipetrans = pipetrans.QueryInterface(nsIPipeTransport);
  DEBUG_LOG("TestChannel: pipetrans = " + pipetrans + "\n");

  DEBUG_LOG("this = "+this+"\n");

  var cmd = '/home/svn/mysrc/pipella/proto/protozilla/cgi+/testcgi.pl';

  var env = ["REQUEST_METHOD=GET",
             "QUERY_STRING=query=testquerystring"];

  var noProxy = false;
  var mergeStderr = false;
  pipetrans.initCommand(cmd, false, env, env.length,
                 0, "", noProxy, mergeStderr, null);

  this.pipeTransport = pipetrans;
}

TestChannel.prototype = {

  QueryInterface: function (iid) {

    if (!iid.equals(nsIChannel) && !iid.equals(nsIRequest) &&
        !iid.equals(nsIStreamListener) && !iid.equals(nsISupports))
       throw Components.results.NS_ERROR_NO_INTERFACE;

    return this;
  },

/* nsIChannel */
  URI: null,
  get originalURI() { return this._originalURI; },
  set originalURI(val) { this._originalURI = val;
                         WRITE_LOG("TestChannel:setOriginalURI="+val.spec+"\n"); },

  _originalURI: null,
  transferOffset: 0,
  transferCount: 0,
  loadFlags: null,
  contentType: "text/plain",
  contentLength: -1,
  owner: null,
  loadGroup: null,
  notificationCallbacks: null,
  securityInfo: null,
  bufferSegmentSize: 0,
  bufferMaxSize: 0,
  shouldCache: false,
  pipeliningAllowed: false,

  open: function ()
  {
    DEBUG_LOG("TestChannel: open\n");
    return this.pipeTransport.openInputStream(0, Number.MAX_VALUE, 0);
  },

  asyncOpen: function (listener, ctxt)
  {
    DEBUG_LOG("TestChannel: asyncOpen\n");

    this.listener = listener;
    this.pipeRequest = this.pipeTransport.asyncRead(this, ctxt,
                                                    0, Number.MAX_VALUE, 0);

    return this.pipeRequest;
  },

/* nsIRequest */
  isPending: function ()
  {
    DEBUG_LOG("TestChannel: isPending\n");
    return true;
  },

  status: Components.results.NS_OK,

  cancel: function (aStatus)
  {
    DEBUG_LOG("TestChannel: cancel "+aStatus+"\n");
    this.status = aStatus;
    return this.pipeRequest.cancel(aStatus);
  },

  suspend: function ()
  {
    DEBUG_LOG("TestChannel: suspend\n");
    throw Components.results.NS_ERROR_NOT_IMPLEMENTED;
  },

  resume: function ()
  {
    DEBUG_LOG("TestChannel: resume\n");
    throw Components.results.NS_ERROR_NOT_IMPLEMENTED;
  },

// nsIStreamListener
  onStartRequest: function (channel, ctxt)
  {
    DEBUG_LOG("TestChannel: onStartRequest\n");

    DEBUG_LOG("loadGroup = "+this.loadGroup+"\n");

    //this.loadGroup.addChannel(channel, ctxt);

    return this.listener.onStartRequest(this, ctxt);
  },

    onStopRequest: function (channel, ctxt, statusg)
  {
    DEBUG_LOG("TestChannel: onStopRequest\n");

    //this.loadGroup.removeChannel(channel, ctxt, 0, "STATUSyyy");

    var rv = this.listener.onStopRequest(this, ctxt, status);

    // Release pipeTransport, pipeRequest and listener
    this.pipeTransport = null;
    this.pipeRequest = null;
    this.listener = null;
  },

  onDataAvailable: function (channel, ctxt, inStream, sourceOffset, count)
  {
    DEBUG_LOG("TestChannel: onDataAvailable\n");
    return this.listener.onDataAvailable(this, ctxt, inStream,
                                         sourceOffset, count);
  }
}

/* Wrapper channel (used for testing) */
function WrapperChannel(aURISpec, aOriginalURISpec, aChannel)
{
  this._URI = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
  this._URI.spec = aURISpec;

  this._originalURI = Components.classes[NS_SIMPLEURI_CONTRACTID].createInstance(nsIURI);
  this._originalURI.spec = aOriginalURISpec;

  this._channel = aChannel;

  WRITE_LOG("WrapperChannel: "+aURISpec+", "+aOriginalURISpec+", "+aChannel.URI.spec+"\n");
}

WrapperChannel.prototype = {

  _URI: null,
  _originalURI: null,
  _listener: null,

  QueryInterface: function (iid) {

    WRITE_LOG("WrapperChannel: QueryInterface\n");

    if (!iid.equals(nsIChannel) && !iid.equals(nsIRequest) &&
        !iid.equals(nsIStreamListener) && !iid.equals(nsISupports))
       throw Components.results.NS_ERROR_NO_INTERFACE;

    return this;
  },

/* nsIChannel */
  get URI()    { return this._URI; },

  get originalURI()    { return this._originalURI; },
  set originalURI(val) { //this._originalURI = val;
                         WRITE_LOG("WrapperChannel:setOriginalURI="+val.spec+"\n"); },

  get transferOffset()    { return this._channel.transferOffset; },
  set transferOffset(val) { this._channel.transferOffset = val; },

  get transferCount()    { return this._channel.transferCount; },
  set transferCount(val) { this._channel.transferCount = val; },

  get contentType()    { return this._channel.contentType; },
  set contentType(val) { this._channel.contentType = val; },

  get contentLength()    { return this._channel.contentLength; },
  set contentLength(val) { this._channel.contentLength = val; },

  get owner()    { return this._channel.owner; },
  set owner(val) { this._channel.owner = val; },

  get notificationCallbacks()    { return this._channel.notificationCallbacks; },
  set notificationCallbacks(val) { this._channel.notificationCallbacks = val; },

  get securityInfo()    { return this._channel.securityInfo; },
  set securityInfo(val) { this._channel.securityInfo = val; },

  get bufferSegmentSize()    { return this._channel.bufferSegmentSize; },
  set bufferSegmentSize(val) { this._channel.bufferSegmentSize = val; },

  get bufferMaxSize()    { return this._channel.bufferMaxSize; },
  set bufferMaxSize(val) { this._channel.bufferMaxSize = val; },

  get shouldCache()    { return this._channel.shouldCache; },
  set shouldCache(val) { this._channel.shouldCache = val; },

  get pipeliningAllowed()    { return this._channel.pipeliningAllowed; },
  set pipeliningAllowed(val) { this._channel.pipeliningAllowed = val; },

  open: function ()
  {
    WRITE_LOG("WrapperChannel: open\n");
    return this._channel.open();
  },

  asyncOpen: function (listener, ctxt)
  {
    WRITE_LOG("WrapperChannel: asyncOpen\n");

    this._listener = listener;
    return this._channel.asyncOpen(this, ctxt);
  },

/* nsIRequest */
  isPending: function ()
  {
    WRITE_LOG("WrapperChannel: isPending\n");
    return this._channel.isPending();
  },

  get status()    { return this._channel.status; },
  set status(val) { this._channel.status = val; },

  cancel: function (aStatus)
  {
    WRITE_LOG("WrapperChannel: cancel "+aStatus+"\n");
    return this._channel.cancel(aStatus);
  },

  suspend: function ()
  {
    WRITE_LOG("WrapperChannel: suspend\n");
    return this._channel.suspend();
  },

  resume: function ()
  {
    WRITE_LOG("WrapperChannel: resume\n");
    return this._channel.resume();
  },

  get loadFlags()    { return this._channel.loadFlags; },
  set loadFlags(val) { this._channel.loadFlags = val; },

  get loadGroup()    { return this._channel.loadGroup; },
  set loadGroup(val) { this._channel.loadGroup = val; },

// nsIStreamListener
  onStartRequest: function (channel, ctxt)
  {
    WRITE_LOG("WrapperChannel: onStartRequest\n");
    return this._listener.onStartRequest(this, ctxt);
  },

    onStopRequest: function (channel, ctxt, status)
  {
    WRITE_LOG("WrapperChannel: onStopRequest\n");
    return this._listener.onStopRequest(this, ctxt, status);
  },

  onDataAvailable: function (channel, ctxt, inStream, sourceOffset, count)
  {
    WRITE_LOG("WrapperChannel: onDataAvailable\n");
    return this._listener.onDataAvailable(this, ctxt, inStream, sourceOffset, count);
  }
}
