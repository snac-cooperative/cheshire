
from baseObjects import DocumentGroup
from document import StringDocument
import re, os, c3errors, tarfile, cStringIO, sys

class StringDocumentGroup(DocumentGroup):
    start = None
    doctexts = []

    def __init__(self, data):
        self.start = re.compile("<([a-zA-Z1-9_]+:)?([a-zA-Z1-9_]+)[ >]")
        self.doctexts = []
        start = None

        indoc = 0
        idx = -1
        endtag = ""
        lines = data.split('\n')
        lines.append(None)
        txt = []
        line = lines[0]
        while line <> None:
            if not indoc:
                m = self.start.search(line)
                if m:
                    endtag = "</%s>" % m.group()[1:-1]
                    st = m.start()
                    end = line.find(endtag)
                    if (end > st):
                        txt = line[st:end+len(endtag)]
                        self.doctexts.append(txt)
                        line = line[end+len(endtag):]
                    else:
                        txt = [line[st:]]
                        idx += 1
                        line = lines[idx+1]
                        indoc = 1
                else:
                    idx += 1
                    line = lines[idx+1]
            else:
                end = line.find(endtag)
                if end > -1:
                    indoc = 0
                    txt.append(line[:end+len(endtag)])
                    doc = '\n'.join(txt)
                    self.doctexts.append(doc)
                    line = line[end+len(endtag):]
                else:
                    txt.append(line)
                    idx += 1
                    line = lines[idx+1]

    def get_length(self, session):
        return len(self.doctexts)
    def get_document(self, session, idx):
        if (idx < len(self.doctexts)):
            return StringDocument(self.doctexts[idx])
        else:
            raise(IndexError)

class FileDocumentGroup(StringDocumentGroup):

    def __init__(self, data):

        if (not os.path.exists(data)):
            raise c3errors.FileDoesNotExistException("FileDocumentGroup data file does not exist.")
        f = file(data)
        docs = f.read()
        f.close()
        StringDocumentGroup.__init__(self, docs)

class BigFileDocumentGroup(StringDocumentGroup):
    # Read in incrementally, rather than explode memory
    # Parent for basic code

    def __init__(self, data):
        self.start = re.compile("<([a-zA-Z1-9_]+:)?([a-zA-Z1-9_]+)[ >]")

        if (not os.path.exists(data)):
            raise c3errors.FileDoesNotExistException("FileDocumentGroup data file is unreadable.")

        fileh = file(data)
        indoc = 0
        line = fileh.readline()
        while line:
            if not indoc:
                m = self.start.match(line)
                if m:
                    self.endtag = m.group()[1:-1]
                    start = fileh.tell() + m.start() - len(line)
                    # Check that endtag's not in the same line
                    if line.find("</" + self.endtag + ">") > -1:
                        # XXX
                        pass
                    indoc = 1
            else:
                if line.find("</" + self.endtag + ">") > -1:
                    indoc = 0
                    end = fileh.tell()
                    fileh.seek(start)
                    txt = fileh.read(end - start)
                    self.doctexts.append(txt)
                    
            line = fileh.readline()


class TarDocumentGroup(StringDocumentGroup):
    def __init__(self, data):
        buffer = cStringIO.StringIO(data)
        tar = tarfile.open(mode="r|", fileobj=buffer)
        for tarinfo in tar:
            f = tar.extractfile(tarinfo)
            fdata = f.read()
            f.close()
            self.doctexts.append(fdata)
        tar.close()
        buffer.close()

class ZipDocumentGroup(StringDocumentGroup):
    def __init__(self, data):
        buffer = cStringIO.StringIO(data)
        zip = zipfile.ZipFile(buffer, mode="r")
        for info in zip.namelist():
            fdata = zip.read(info)
            self.doctexts.append(fdata)
        zip.close()
        buffer.close()

class ClusterDocumentGroup(FileDocumentGroup):
    # Take a raw cluster file, create documents from it.

    sortPath = "/usr/local/bin/sort"

    def __init__(self, data):
        if (not os.path.exists(data)):
            raise c3errors.FileDoesNotExistException("ClusterDocumentGroup data file does not exist.")
        sort = self.sortPath
        if (not os.path.exists(sort)):
            raise c3errors.FileDoesNotExistException("Sort utility for ClusterDocumentGroup does not exist.")
        
        sorted = data + "_SORT"
        os.spawnl(os.P_WAIT, sort, sort, data, '-o', sorted)

        # Now construct cluster documents.
        doc = ["<cluster>"]
        f = file(sorted)
        l = f.readline()
        # term docid recstore occs (line, posn)*
        currKey = ""
        while(l):
            docdata = {}
            ldata = l.split('\x00')
            key = ldata[0]
            if (not key):
                # Data from records with no key
                l = f.readline()
                l = l[:-1]
                continue

            doc.append("<key>%s</key>\n" % (key))
            ldata = ldata[1:-1]
            for bit in range(len(ldata)/2):
                d = docdata.get(ldata[bit*2], [])                
                d.append(ldata[bit*2+1])
                docdata[ldata[bit*2]] = d

                
            l = f.readline()
            l = l[:-1]
            ldata2 = l.split('\x00')
            key2 = ldata2[0]
            while key == key2:   
                ldata2 = ldata2[1:-1]
                for bit in range(len(ldata2)/2):
                    d = docdata.get(ldata2[bit*2], [])
                    d.append(ldata2[bit*2+1])
                    docdata[ldata2[bit*2]] = d
                l = f.readline()
                l = l[:-1]
                ldata2 = l.split('\x00')
                key2 = ldata2[0]
               

            for k in docdata.keys():
                doc.append("<%s>" % (k))
                for i in docdata[k]:                    
                    doc.append("%s" % i)
                doc.append("</%s>" % (k))
            doc.append("</cluster>")
            self.doctexts.append(" ".join(doc))
            doc = ["<cluster>"]
            l = f.readline()
            l = l[:-1]
