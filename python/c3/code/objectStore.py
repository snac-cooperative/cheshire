
from baseObjects import ObjectStore
from configParser import C3Object
from recordStore import BdbRecordStore
from utils import getFirstData, elementType
import dynamic
from c3errors import ConfigFileException

class BdbObjectStore(BdbRecordStore):
    # Store XML records in RecordStore
    # Retrieve and instantiate
    
    def create_object(self, session, object=None):
        # Need to implement object -> config xml for all objects!
        # Check doesn't exist, then call store_object
        raise(NotImplementedError)

    def delete_object(self, session, id):
        return self.delete_record(session, id)

    def fetch_object(self, session, id):
        rec = self.fetch_record(session, id)
        if (not rec):
            return None

        dom = rec.get_dom()
        topNode = dom.childNodes[1]
        # Need to import stuff first, possibly
        for child in topNode.childNodes:
            if child.nodeType == elementType:
                if (child.localName == "imports"):
                    # Do this now so we can reference
                    for mod in child.childNodes:
                        if mod.nodeType == elementType and mod.localName == "module":
                            name, objects, withname = ('', [], None)
                            for n in mod.childNodes:
                                if (n.nodeType == elementType):
                                    if (n.localName == 'name'):
                                        name = getFirstData(n).data
                                    elif (n.localName == 'object'):
                                        objects.append(getFirstData(n).data)
                                    elif (n.localName == 'withName'):
                                        withname = getFirstData(n).data
                            if (name):
                                dynamic.globalImport(name, objects, withname)
                            else:
                                raise(ConfigFileException('No name given for module to import in configFile for %s' % (self.id)))

        object = dynamic.makeObjectFromDom(topNode, self)
        return object

    def fetch_objectList(self, session, req):
        # Scan like interface?
        raise(NotImplementedError)

    def store_object(self, session, object):
        raise(NotImplementedError)


