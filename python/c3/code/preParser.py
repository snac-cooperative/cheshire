
from baseObjects import PreParser
from document import StringDocument
import re, gzip, cStringIO as StringIO
import httplib, mimetypes

class SgmlPreParser(PreParser):
    entities = {}
    emptyTags = []
    attr_re = None
    elem_re = None

    def __init__(self, server, config):
        self.parent = server
        self.attr_re = re.compile('([a-zA-Z0-9_]+)[ ]*=[ ]*([-:_.a-zA-Z0-9]+)')
        self.elem_re = re.compile('(<[/]?)([a-zA-Z0-9_]+)')
        self.empty_re = re.compile('<(a-z0-9_]+[^>]*)>')

        # Check config for emptyTags and entities

    def _lowerElement(self, match):
        #return match.groups()[0] + match.groups()[1].lower()
        return "%s%s" % (match.group(1), match.group(2).lower())
    def _attributeFix(self, match):
        #return match.groups()[0].lower() + '="' + match.groups()[1] + '"'
        return '%s="%s"' % (match.group(1).lower(), match.group(2))
    def _emptyElement(self, match):
        return "</%s>" % (match.group(1))

    def process_document(self, session, doc):
        txt = doc.get_raw()

        for e in self.entities.keys():
            txt = txt.replace("&%s;" % (e), self.entities[e])
        txt = txt.replace('& ', '&amp; ')
        txt = self.attr_re.sub(self._attributeFix, txt)
        txt = self.elem_re.sub(self._lowerElement, txt)
        for t in self.emptyTags:
            txt = self.empty_re.sub(self._emptyElement, txt)
        
        doc = StringDocument(txt)
        return doc


class GzipPreParser(PreParser):
    def process_document(self, session, doc):
        buffer = StringIO.StringIO(doc.get_raw())
        zfile = gzip.GzipFile(mode = 'rb', fileobj=buffer)
        data = zfile.read()
        new = StringDocument(data)
        return new

class RemoteUrlPreParser(PreParser):
    """ Implementation adapted from ASPN Recipe """

    def post_multipart(self, host, selector, fields, files):
        content_type, body = self.encode_multipart_formdata(fields, files)
        h = httplib.HTTPConnection(host)
        headers = {'content-type': content_type}
        h.request('POST', selector, body, headers)
        resp = h.getresponse()
        return resp.read()

    def encode_multipart_formdata(self, fields, files):
        BOUNDARY = '----------ThIs_Is_tHe_bouNdaRY_$'
        CRLF = '\r\n'
        L = []
        for (key, value) in fields:
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"' % key)
            L.append('')
            L.append(value)
        for (key, filename, value) in files:
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"; filename="%s"' % (key, filename))
            L.append('Content-Type: %s' % self.get_content_type(filename))
            L.append('')
            L.append(value)
        L.append('--' + BOUNDARY + '--')
        L.append('')
        body = CRLF.join(L)
        content_type = 'multipart/form-data; boundary=%s' % BOUNDARY
        return content_type, body

    def get_content_type(self, filename):
        return mimetypes.guess_type(filename)[0] or 'application/octet-stream'

    def send_request(self, session, data=None):
        url = self.get_path(session, 'RemoteURL')
        if (url[:7] == "http://"):
            url = url[7:]
        hlist = url.split('/', 1)
        host = hlist[0]
        if (len(hlist) == 2):
            selector = hlist[1]
        else:
            selector = ""
        # XXX  Remove dependency
        fields = ()
        files = [("file", "foo.doc", data)]
        return self.post_multipart(host, selector, fields, files)
    
class WordPreParser(RemoteUrlPreParser):

    def process_document(self, session, doc):
        data = doc.get_raw()
        return self.send_request(session, data)

try:
    import bz2

    class BzipPreParser(PreParser):
        def process_document(self, session, doc):
            buffer = StringIO.StringIO(doc.get_raw())
            zfile = bz2.BZ2File(mode = 'rb', fileobj=buffer)
            data = zfile.read()
            new = StringDocument(data)
            return new

except ImportError:
    # We might not have this lib
    pass

        
