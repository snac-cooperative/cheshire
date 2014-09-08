#!/usr/bin/python

import os, time, sys, re, cgi, cgitb, string, types
cgitb.enable()
from threading import *
from ChZoom import *

# RH 7.3 and below defaulted to 1.5
oldpackagedir = "/usr/lib/python1.5/site-packages"
if os.path.exists(oldpackagedir):
    sys.path.insert(0, oldpackagedir)
import pg

print "Content-type: text/html\n"
sys.stdout.flush()

class User:
    connection = ""
    username = ""
    lastconnectip = ""
    firstname = ""
    lastname = ""
    hostmask = ""
    title = ""
    lastconnecttime = ""
    createdtime = ""
    password = ""
    email = ""
    description = ""
    flags = []

    def __init__(self, un):
        cxn = pg.connect('user_db')
        self.username = un;
        res = cxn.query("select * from users where username='" + un + "'")
        if not res.dictresult():
            print res
            raise(ValueError)
        hash = res.dictresult()[0]
        for k in hash.keys():
            setattr(self, k, hash[k])

        flagresult = cxn.query("select flagid from userflags where username='" + un + "'")
        nflags = []
        for f in flagresult.dictresult():
            # Get flag by name from flags table
            flagnameresult = cxn.query("select flagname from flags where flagid = " + str(f['flagid']))
            if flagnameresult.dictresult():
                flagname = flagnameresult.dictresult()[0]['flagname']
            else:
                # Database in error state
                raise (ValueError, "Undefined FlagID" + str(f['flagid']))
            nflags.append(flagname)
        self.flags = nflags
    def has_flag(self, flagname):
        return flagname in self.flags

class buildThread(Thread):
    """Assumes that the method for building is a 'buildnew.sh' script in the configfile directory.
    Normally this is a pretty good assumption to make however."""
    directory = ""
    def run(self):
        file = os.path.join(self.directory, "buildnew.sh")
        if not os.path.exists(file):
            raise(ValueError)
        g = os.spawnl(os.P_NOWAIT, '/bin/bash', '/bin/bash', file)
        os.wait()

class testConfigThread(Thread):
    file = ""
    def run(self):
        if not os.path.exists(self.file):
            raise(ValueError)
        # 0 is stdin, 1 is merged stdout/stderr
        g = os.popen4('/home/cheshire/cheshire/bin/test_config ' + self.file, 'r')
        lines = []
        error = 0
        for l in g[1].xreadlines():
            lines.append(l)
            if l[:11] == "Config file":
                error = 1

        if not error:
            return []
        else:
            return lines

def throwfile(filename, replaces):
    if not os.path.exists(filename):
        raise ValueError, "File does not exist"
    fh = file(filename, 'r')

    replaces['%CGI%'] = os.environ['SCRIPT_NAME']

    for line in fh.xreadlines():
        for k in replaces.keys():
            line = string.replace(line, k, replaces[k])
        sys.stdout.write(line)

def myslice(list, idx):
    new = []
    for l in list:
        if type(l) != types.ListType:
            raise(ValueError, "Non list item supplied")
        elif len(l) < idx:
            raise(ValueError, "Not enough elements to slice")
        else:
            new.append(l[idx])
    return new

def parseExplainSingle(database):
    c = Connection('localhost', 210, databaseName='IR-Explain-1')
    ZCheshire.set('attributeSet', 'EXP1')
    qstr = 'explainCategory databaseInfo and database ' + database
    q = Query('C', qstr)
    rs = c.search(q)
    if len(rs) != 1:
        return "<b>Unknown Database</b>"

    rec = rs[0].get_raw()
    hash = rec[1]

    html = "<table width = 100%>"

    if hash.has_key('title'):
        for lang in hash['title']:
            html += "<tr><td colspan=2 align=center><b>" + hash['title'][lang] + "</b> (" + lang + ")</td></tr>"

    html += "<tr><td>Database Name:</td><td>" + hash['databaseName'] + "</td></tr>"
    if hash.has_key('nickNames') and hash['nickNames']:
        html += "<tr><td>Records Location:</td><td>" + hash['nickNames'][0] + "</td></tr>"
    if hash.has_key('recordCount') and hash['recordCount']:
        html += "<tr><td>Number of Records:</td><td>" + str(hash['recordCount']['number']) + "</td></tr>"

    descs = {'description' : "Description", 'news' : "News", 'disclaimers' : "Disclaimers", 'coverage' : "Coverage", 'hours' : "Available", 'defaultOrder' : "Default Order", 'copyrightNotice' : "Copyright Notice", 'bestTime' : "Best Access Time", 'copyrightText' : "Copyright Text"}
    descorder = ['description', 'news', 'disclaimers', 'copyrightNotice', 'copyrightText', 'coverage', 'hours', 'bestTime', 'defaultOrder']

    for dk in descorder:
        if hash.has_key(dk):
            for lang in hash[dk]:
                html += "<tr><td>" + descs[dk] + " (" + lang + "):</td><td>" + hash[dk][lang] + "</td></tr>"

    if hash.has_key('lastUpdate') and hash['lastUpdate']:
        html += "<tr><td>Last Updated:</td><td>" + hash['lastUpdate'] + "</td></tr>"

    if hash['submissionContactInfo'] or hash['supplierContactInfo'] or hash['producerContactInfo']:
        html += "<tr><td colspan=2>Contact Information:</td></tr>"
        for ci in ['producerContactInfo', 'supplierContactInfo', 'submissionContactInfo']:
            if hash[ci]:
                cihtml = "<table>"
                if hash[ci].has_key('name'):
                    cihtml += "<tr><td>Name</td><td>" + hash[ci]['name'] + "</td></tr>"
                if hash[ci].has_key('address'):
                    for lang in hash[ci]['address']:
                        cihtml += "<tr><td>Address (" + lang + "):</td><td>" + hash[ci]['address'][lang] + "</td></tr>"
                if hash[ci].has_key('email'):
                    cihtml += "<tr><td>Email Address</td><td>" + hash[ci]['email'] + "</td></tr>"
                cihtml += "</table>"
                title = ci[:-11].title()
                html += "<tr><td valign=top align=right>" + title + "</td><td>" + cihtml + "</td></tr>"
                    
                
    



    html +="</table>"

    return html

    

def parseExplain():
    c = Connection('localhost', 210, databaseName='IR-Explain-1')
    ZCheshire.set('attributeSet', 'EXP1')
    qstr = 'explainCategory databaseInfo'
    q = Query('C', qstr)
    rs = c.search(q)

    html = "<b>Total Databases: </b>" + str(len(rs)) + "<p>"
    html += '<table width="100%"><tr><td><b>Title</b></td><td><b>Database</b></td><td><b>Last Updated</b></td><td><b>Number of Records</b></td></tr>'

    for rec in rs:
        data = rec.get_raw();
        data = data[1]   # 0 is type, 1 is dictionary

        line = "<tr>"
        if data['title']:
            title = data['title'][data['title'].keys()[0]][:-1]
        else:
            title = ""
        line += "<td>" + title + "</td>"

        href = os.environ['SCRIPT_NAME'] + "?operation=display&database=" + data['databaseName']
        db = '<i><a href="' + href + '">' + data['databaseName'] + "</a></i>"
        line += "<td>" + db + "</td>"

        line += "<td>" + data['lastUpdate'][:-1] + "</td>"
        line += "<td>" + str(data['recordCount']['number']) + "</td>"

        line += "</td>"
        html += line
    return html
        
        
def parseConfig():
    conffile = file('/etc/zserver.conf', 'r')
    regexp = re.compile('^(\S+)\s+(.+)$')
    confs = {}
    
    for line in conffile.xreadlines():
        if line[0] != "#":
            # Strip newline
            line = line[0:-1]
            line = line.expandtabs(1)
            match = regexp.search(line)
            if match:
                val = match.group(2)
                idx = val.find("#")
                if idx > -1:
                    val = val[:idx]
                    val = val.strip()
                if (val[0] == '"' and val[-1] == '"'):
                    val = val[1:-1]
                    val = val.strip()
                confs[match.group(1)] = val

    conffile.close()
    # Esp Useful are DATABASE_CONFIGFILES and DATABASE_DIRECTORIES

    cfs = confs['DATABASE_CONFIGFILES'].split()
    dirs = confs['DATABASE_DIRECTORIES'].split()
    
    if len(cfs) != len(dirs):
        raise(ValueError, "Cannot parse server configuration -- configfiles and directories are out of sync. Please correct this by editing /etc/zserver.conf.")

    databases = []
    for i in range(len(cfs)):
        cfpath = os.path.join(dirs[i], cfs[i])
        # Check if we exist
        if (not os.path.exists(dirs[i]) or not os.path.isdir(dirs[i])):
            databases.append("Configuration Error -- " + dirs[i] + " does not exist or is not a directory.")
        elif (not os.path.exists(cfpath)):
            databases.append("Configuration Error -- " + cfpath + " does not exist.")
        else:
            # Get the database names from file.
            dbcfg = file(cfpath)
            for line in dbcfg.xreadlines():
                line = line.strip()
                if line[:9].upper() == "<FILETAG>":
                    line = line[9:]
                    # Found our line.
                    if line[-10:].upper() == "</FILETAG>":
                        line = line[:-10].strip()
                    databases.append([dirs[i], cfs[i], line])
                    dbcfg.close()
                    break
    return databases

def displayConfig():
    try:
        databases = parseConfig()
        html = "<p>The following databases are listed in your configuration file:<p><table width=100% cellpadding=3 border=1 cellspacing=0><tr><td><b>Database Name</b></td><td><b>Configuration File</b></td><td><b>Test Configuration</b></td></tr>"
        for db in databases:
            if type(db) == types.StringType:
                html += "<tr><td colspan=3><i>" + db + "</i></td></tr>"
            else:
                href = os.environ['SCRIPT_NAME'] + "?operation=test_config&database=" + db[2]
                html += "<tr><td>" + db[2] + '</td><td><i>' + os.path.join(db[0], db[1]) + '</i></td><td><a href="' + href + '">Test Database Configuration</a></td></tr>'
        html += "</table>"
    except:
        throwfile("html/err.html", {'%ERR%' : "Could not parse Explain or the Server Configuration file successfully. Please correct /etc/zserver.conf manually."})
        sys.exit()
    return html

def databaseDir(database):

    try:
        dbs = parseConfig()
    except:
        throwfile('html/err.html', {"%ERR%" : "Could not parse Server Configuration file successfully. Please correct /etc/zserver.conf manually."})
        sys.exit()
        
    dbidx = database in myslice(dbs, 2)
    if dbidx < 0:
        throwfile('html/err.html', {"%ERR%" : "Unknown database: " + database})
        sys.exit()
    else:
        db = dbs[myslice(dbs,2).index(database)]
        return db[0]


# Get remote_user out of environment variables
env = os.environ
# Move to user_db space
os.chdir("/home/cheshire/cheshire/user_db")

# Check we have permission to do anything.
if env.has_key('REMOTE_USER'):
    user = User(env['REMOTE_USER'])
    if not user.has_flag("admin"):
        throwfile("html/err.html", {'%ERR%' : "You must be logged in as an administrator to use this function."})
        sys.exit()      
else:
        # We're in an error state, Must have a user to authenticate against
        throwfile("html/err.html", {'%ERR%' : "You must be logged in as an administrator to use this function."})
        sys.exit()
        
# Parse the form, if there is one
form = cgi.FieldStorage()

if form.has_key("operation"):
    operation = form.getfirst('operation')
else:
    operation = ""

if form.has_key("database"):
    database = form.getfirst('database')
else:
    database = ""
    

if not form.has_key("operation"):
    # No request, display information
    try:
        html = parseExplain()
        html += '</table><p><a href="' + env['SCRIPT_NAME'] + '?operation=parse_config">Display Server Configuration File</a>'
    except:
        # Utoh, no Explain...
        html = displayConfig()

elif operation == "parse_config":
    html = displayConfig()

elif not form.has_key('database'):
        throwfile('html/err.html', {"%ERR%" : "Database is required for '" + form['operation'] + "' operation."});
        sys.exit()

elif operation == "display":
    # Display form['database'] operations.
    throwfile('html/globalheader.html', {})
    throwfile('html/dbops.html', {"%DB%" : database})
    throwfile('html/globalfooter.html', {})
    sys.exit()

elif operation == "explain":
    try:
        html = parseExplainSingle(database)
    except:
        html = displayConfig()


elif operation == "test_config":
    # Test database configuration
    try:
        dbs = parseConfig()
    except:
        throwfile('html/err.html', {"%ERR%" : "Could not parse Server Configuration file successfully. Please correct /etc/zserver.conf manually."})

    validdb = database in myslice(dbs, 2)
    if not validdb:
        throwfile('html/err.html', {"%ERR%" : "Unknown database: " + database})
    else:
        db = dbs[myslice(dbs,2).index(database)]
        cfpath = os.path.join(db[0], db[1])
        f = testConfigThread()
        f.file = cfpath
        errorList = f.start()

        if errorList:
            html = "<p>Errors testing the configuration file:<p><pre>"
            for l in errorList:
                html += l
        else:
            html = "<p>Configuration file successfully parsed."

elif operation == "rebuild":

    directory = databaseDir(database)
    f = buildThread()
    f.directory = directory
    f.start()
        
    throwfile("html/globalheader.html", {});
    print "<p>Rebuilding database: " + database
    print "<p>Starting at " + time.ctime()
    sys.stdout.flush()
    while f.isAlive():
        time.sleep(5)
        print "<br> ..."
        sys.stdout.flush()
    print "<br>Finished at " + time.ctime()

    # Read INDEX_LOGFILE for errors
    data = file(os.path.join(directory, "INDEX_LOGFILE")).read()
    lines = data.split('\n')
    idx = -1
    lastline = lines[idx]
    while lastline == "":
        idx -= 1
        lastline = lines[idx]

    if lastline.find(' -- Memory') != -1 or lastline.find('Last cluster record processed') != -1:
        print "<p><b>Indexing was successfull.</b>"
    else:
        print "<p><b>Indexing failed. Reason given:</b><pre>"
        print lines[idx - 1]
        print "</pre>"
        
    print '<p><a href="' + env['SCRIPT_NAME'] + "?operation=display&database=" + database + '">Options for ' + database + ' database</a>'
    print "</body></html>"
    sys.exit();

elif operation == "list_files":
    # Find data dir. Generate list of files.

    directory = os.path.join(databaseDir(database), "DATA")
    if not os.path.exists(directory) or not os.path.isdir(directory):
        throwfile('html/err.html', {"%ERR%" : "Data directory does not exist for reading: " + directory})
        sys.exit()

    filelist = os.listdir(directory)
    filelist.sort()
    html = "<p>File list for database: " + database + "<p>"

    html += "<table width = 100%><tr><td align=center><table border = 1 cellspacing=0 cellpadding=3><tr><td><b>Filename</b></td><td><b>Size</b></td><td><b>View</b></td><td><b>Remove</b></td></tr>"

    l = len(filelist)/ 2
    for x in range(0, l):
        f = filelist[x]
        basehref = env['SCRIPT_NAME'] + "?database=" + database + "&filename=" + cgi.escape(f)
        html += "<tr><td>" + f+ "</td><td>" + str(os.stat(os.path.join(directory,f)).st_size) + "</td><td><a><a href=\"" + basehref + "&operation=view_file\">view</a></td><td><a href=\"" + basehref + "&operation=remove_file\">remove</a></td></tr>"
    html += "</table></td><td align=center><table border= 1 cellspacing=0 cellpadding=3><tr><td><b>Filename</b></td><td><b>Size</b></td><td><b>View</b></td><td><b>Remove</b></td></tr>"

    for x in range(l, len(filelist)):
        f = filelist[x]
        basehref = env['SCRIPT_NAME'] + "?database=" + database + "&filename=" + cgi.escape(f)
        html += "<tr><td>" + f+ "</td><td>" + str(os.stat(os.path.join(directory,f)).st_size) + "</td><td><a><a href=\"" + basehref + "&operation=view_file\">view</a></td><td><a href=\"" + basehref + "&operation=remove_file\">remove</a></td></tr>"
    html += "</table></td></tr></table>"

elif operation == "view_file":

    if not form.has_key('filename'):
        throwfile("html/err.html", {"%ERR%" : "File to view not specified."})
        sys.exit()

    dir = os.path.join(databaseDir(database), "DATA")
    filepath = os.path.join(dir, form.getfirst('filename'));
    if os.path.normpath(filepath) != filepath:
        throwfile("html/err.html", {"%ERR%" : "Relative paths are not allowed."})
        sys.exit()

    if not os.path.exists(filepath):
        throwfile("html/err.html", {"%ERR%" : "File to view does not exist."})
        sys.exit()

    fh = file(filepath)
    data = fh.read()
    data = data.replace("<", "&lt;")
    data = data.replace(">", "&gt;")
    
    html = "<p>File: " + filepath + "<p><pre>" + data + "</pre>"

elif operation == "remove_file":
    if not form.has_key('filename'):
        throwfile("html/err.html", {"%ERR%" : "File to view not specified."})
        sys.exit()

    dir = databaseDir(database)
    filepath = os.path.join(dir,"DATA", form.getfirst('filename'));
    if os.path.normpath(filepath) != filepath:
        throwfile("html/err.html", {"%ERR%" : "Relative paths are not allowed."})
        sys.exit()

    if not os.path.exists(filepath):
        throwfile("html/err.html", {"%ERR%" : "Specified file does not exist."})
        sys.exit()

    newpath = os.path.join(dir, "old_data")

    if not os.path.exists(newpath):
        os.mkdir(newpath)

    newfilepath = os.path.join(newpath, form.getfirst('filename'));
              
    if os.path.exists(newfilepath):
        throwfile("html/err.html", {"%ERR%" : "Backup file already exists."})
        sys.exit()

    os.rename(filepath, newfilepath)
    html = "Removed " + filepath + ". You should now rebuild the database immediately."

elif operation == "upload_record":
    # First find the directory that we need to put this into.
    # Make assumption that DATA in directory is place.
    # If not dir or not exist, fail.

    directory = databaseDir(database)
        
    if form.has_key('filename'):

        # We've been sent a file, lucky us.
        item = form['filename']
        if item.file:
            filename = user.username + "-" + str(int(time.time())) + ".sgml"
            filepath = os.path.join(directory, filename)
            if os.path.exists(filepath):
                throwfile('html/err.html', {"%ERR%" : "File already exists in Data directory."})
                sys.exit()
            data = item.file.read()
            out = file(filepath, 'w')
            out.writelines(data)
            out.close()
            html = "Wrote file to " + filepath + "."

        else:
            throwfile('html/err.html', {"%ERR%" : "You must submit a file to upload."})
            sys.exit()
        
    else:
        # No file, throw request.  This sends back database and trusts response, but there's no significant loss or risk in doing this.
        throwfile('html/globalheader.html', {})
        throwfile('html/upload.html', {"%DB%" : database})
        throwfile('html/globalfooter.html', {})
        sys.exit()

            
else:
    throwfile('html/err.html', {"%ERR%" : "Unknown Operation requested" + operation})
    sys.exit()


throwfile('html/globalheader.html', {})
print html
if database and operation != "display":
    print '<p><a href="' + env['SCRIPT_NAME'] + "?operation=display&database=" + database + '">Options for ' + database + ' database</a>'

throwfile('html/globalfooter.html', {})





