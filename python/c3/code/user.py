
from baseObjects import User
from utils import getFirstData

class SimpleUser(User):
    simpleNodes = ["username", "password", "email", "address", "tel", "realName", "description"]
    username = ""
    password = ""
    email = ""
    address =""
    tel = ""
    realName = ""
    description = ""

    resultSetIds = []

    def _handleConfigNode(self, node):
        if (node.localName in self.simpleNodes):
            setattr(self, node.localName, getFirstData(node).data)
        elif (node.localName == "rights"):
            # Extract Rights info
            pass
        elif (node.localName == "groups"):
            # Extract group list
            pass
        elif (node.localName == "history"):
            # Extract user history
            pass
        elif (node.localName == "hostmask"):
            # Extract hostmask list
            pass

