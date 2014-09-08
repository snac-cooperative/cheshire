
from c3errors import ConfigFileException
from baseObjects import Server, Database
from xml.dom import Node
from configParser import C3Object

class SimpleServer(Server):
    databases = {}

    def _handleConfigNode(self, node):
        pass

    def __init__(self, configFile="serverConfig.xml"):

        # Bootstrappage
        dom = self.getDomFromFile(configFile)
        topNode = dom.childNodes[0]
        self.databases = {}
        self.id = "DefaultServer"
        C3Object.__init__(self, topNode, None)

        # For non servers we need to use get_path()
        if not self.paths.has_key('authStore'):
            raise (ConfigFileException('No authStore set in server configFile'))
        if not self.paths.has_key('resultSetStore'):
            raise (ConfigFileException('No resultSetStore set in server configFile'))
        if not self.paths.has_key('queryStore'):
            raise (ConfigFileException('No queryStore set in server configFile'))

        # We've already built everything, look for databases.
        for o in self.objects.keys():
            if isinstance(self.objects[o], Database):
                self.databases[o] = self.objects[o]


    def handle_search(self, session, req):
        # Maybe fan out to multiple databases
        res = []
        for dbname in req.databases:
            if (not self.databases.has_key(dbname)):
                # No such DB
                pass
            else:
                res.append([db, db.handle_search(session, req)])
        return res

    def handle_sort(self, session, req):
        pass
    
