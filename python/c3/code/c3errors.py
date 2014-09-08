
import exceptions

class C3Exception(Exception):
    text = ""

    def __init__(self, text="None"):
        self.reason = text

    def __str__(self):
        return "Cheshire3 Exception: " + self.reason

class ConfigFileException(C3Exception):
    pass

class FileDoesNotExistException(C3Exception):
    pass
