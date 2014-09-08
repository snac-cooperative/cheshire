
function do_search(link) {

  var box = document.getElementById("ZSB_box_sessions");
  var hosts = box.getElementsByAttribute("checked","true");
  var term = document.getElementById('ZSB_search_term').value;
  var field = document.getElementById('ZSB_search_field').selectedItem.value;
  var setid = document.getElementById('ZSB_search_resultset').value;

  if (!term) {
    alert("You must give a term to search for.");
    return 1;
  } else if ( hosts.length == 0 ) {
    alert("You must give at least one host to search.");
  } else {

    var url = "z3950://search/?stype=Search&searchfield=" + field + "&searchterm=" + term;
    for (var idx = 0; idx < hosts.length; idx++) {
      url = url + "&" + hosts[idx].id + "=srchhost";
    }

    if (setid != "") {
      url = url + "&resultsetname=" + setid
    }

    link.href = url;
    return false;
  }
  return true;
  link.href = "javascript:void(0);"
}


function do_setsearch (newValue) {
   var box = document.getElementById("ZSB_search_term");
   box.value = newValue;
}


function do_browse(link) {

  var box = document.getElementById("ZSB_box_sessions");
  var hosts = box.getElementsByAttribute("checked","true");
  var term = document.getElementById('ZSB_search_term').value;
  var field = document.getElementById('ZSB_search_field').selectedItem.value;

  if ( hosts.length == 0 ) {
    alert("You must give at least one host to search.");
  } else if (hosts.length > 1) {
    alert("You can only browse one host at once.");
  } else {

    var url = "z3950://search/?stype=Scan&searchfield=" + field + "&searchterm=" + term;
    for (var idx = 0; idx < hosts.length; idx++) {
      url = url + "&" + hosts[idx].id + "=srchhost";
    }
    link.href = url;
    return false;
  }
  link.href = "javascript:void(0);";
}

function do_host(link) {
  
  var host = document.getElementById('ZSB_go_host').value;
  var port = document.getElementById('ZSB_go_port').value;
  var db = document.getElementById('ZSB_go_db').value;
  
  if (host == "") {
    alert("You must provide a host name.");
    link.href = "javascript:void(0);";
  } else {
    
    if (port == "") {
      port = 210;
    }
    
    var url = "z3950://" + host + ":" + port + "/";
    
    if (db != "") {
      url = url + "search/" + db;
    }
    link.href = url;
    return false;
  }
}



function do_sort(link) {
  
  var tree = document.getElementById("ZSB_tree_results");

  // Thanks to XUL Tutorial
  var start = new Object();
  var end = new Object();
  var numRanges = tree.view.selection.getRangeCount();
  
  var sets = new Array();
  
  for (var t=0; t<numRanges; t++){
    tree.view.selection.getRangeAt(t,start,end);
    for (var v=start.value; v<=end.value; v++){
      sets.push(v);
    } 
  }

  var items = tree.getElementsByTagName('treeitem');

  var field = document.getElementById('ZSB_sort_field').selectedItem.value;
  var descend = document.getElementById('ZSB_sort_descend').checked;
  var ucase = document.getElementById('ZSB_sort_case').checked;
  var keyword = document.getElementById('ZSB_sort_term').value;

  if (sets.length == 0 ) {
    alert("You must select at least one result set to sort.");
  } else {
    var host = "";
    setnames = new Array();
    
    for (var idx = 0; idx < sets.length; idx++) {
      sel = sets[idx];
      var sinfo = items[sel].id;

      if (sinfo) {
	var sidx = sinfo.indexOf("^");
	if (sidx > 0 ) {
	  set = sinfo.substr(sidx +1);
	  temphost = sinfo.substr(0, sidx);
	  setnames.push(set);
	  if (!host) {
	    host = temphost;
	  } else if (host != temphost) {
	    alert("You can only combine result sets from a single database");
	    return true;
	  }
 	}
      }
    }

    var hpdarray = host.split("/");
    url = "z3950://" + hpdarray[0] +  "/sort/" + hpdarray[1];

    if (keyword != "") {
      url += ("?sortkey=" + keyword);
    } else {
      url += ("?sortfield=" + field);
    }
    url += ("&descend=" + descend + "&case=" + ucase + "&sets=" + setnames.join(","));

    link.href = url;
    return false;
  }
  link.href = "javascript:void(0);";
}

function do_delete(link) {
  var tree = document.getElementById("ZSB_tree_results");

  var tree = document.getElementById("ZSB_tree_results");
  var start = new Object();
  var end = new Object();
  var numRanges = tree.view.selection.getRangeCount();
  var sets = new Array();
  
  for (var t=0; t<numRanges; t++){
    tree.view.selection.getRangeAt(t,start,end);
    for (var v=start.value; v<=end.value; v++){
      sets.push(v);
    } 
  }

  var items = tree.getElementsByTagName('treeitem');

  if ( sets.length == 0) {
    alert("You must select at least one result set to delete.");
    link.href = "javascript:void(0);";
    return true;
  } else {

   setnames = new Array();

   for (var idx = 0; idx < sets.length; idx++) {
     sel = sets[idx];
     setnames.push(items[sel].id);
   } 

    url = "z3950://sidebar/deleteResultSets?sets=" + setnames.join(",");
    link.href= url;
    return false;
  } 
}


function try_ipc() {
  const NS_IPCSERVICE_CONTRACTID = "@mozilla.org/process/ipc-service;1";
  const NS_SYSTEMENVIRONMENT_CONTRACTID = "@mozilla.org/xpcom/system-environment;1";
  
  window.alert("Test...");

  var ipcService = Components.classes[NS_IPCSERVICE_CONTRACTID].getService(Components.interfaces.nsIIPCService);

  window.alert(ipcService);
  var command = "/usr/bin/cat /home/azaroth/azaroth.sig"

  if (!ipcService) {
    ERROR_LOG("ipc.js:exec: IPCService not available\n");
    throw Components.results.NS_ERROR_FAILURE;
  }
  
  DEBUG_LOG("ipc.js:exec: command="+command+"\n");
  
  return ipcService.exec(command);
}

function try_bookmarks() {
  // Try to generate Z bookmark tree
  var docbox = document.getElementById('ZSB_box_test');
  
  var boxEl = window.document.createElement("box");
  var templateEl = window.document.createElement("template");
  var buttonEl = window.document.createElement("button");
  
  boxEl.setAttribute("datasources", "rdf:bookmarks");
  boxEl.setAttribute("ref", "NC:BookmarksRoot");
  boxEl.setAttribute("orient", "vertical");
  buttonEl.setAttribute("uri", "rdf:*");
  buttonEl.setAttribute("label", "rdf:http://home.netscape.com/NC-rdf#Name");
  
  boxEl.appendChild(templateEl);
  templateEl.appendChild(buttonEl);
  docbox.appendChild(boxEl);
  
  //  var textEl = window.document.createElement("text");
  //  textEl.setAttribute("value", "Test");
  //  box.appendChild(textEl);
  
}

