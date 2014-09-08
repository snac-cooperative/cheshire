
from baseObjects import Document

class FileDocument(Document):
    handle = None
    text = ""

    def __init__(self, fileHandle):
        # This means we have hanging file handles, but less memory usage...
        self.handle = fileHandle

    def get_raw(self):
        if (self.text):
            return self.text
        elif (self.handle <> None):
            self.text =  self.handle.read()
            self.handle.close()
            return self.txt
        else:
            return None

class StringDocument(Document):
    text = ""

    def __init__(self, text):
        self.text = text

    def get_raw(self):
        return self.text

