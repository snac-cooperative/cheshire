
import os, sys, dynamic
from utils import getFirstData, elementType
from bootstrap import BSParser, BootstrapDocument
from c3errors import FileDoesNotExistException, ConfigFileException

class C3Object:
    id = ""
    name = ""
    objectType = ""
    parent = None
    paths = {}
    subConfigs = {}
    objects = {}
    configStore = None
    settings = {}
    defaults = {}

    # temp storage
    _includeConfigStorePaths = []
    _configStorePaths = []
    _objectRefs = []

    def getDomFromFile(self, fileName):
        # We need to be able to read in configurations from disk

        if not os.path.exists(fileName):
            raise(FileDoesNotExistException(fileName))
        
        f = file(fileName)
        doc = BootstrapDocument(f)

        # Look on self for instantiated parser, otherwise use bootstrap
        p = self.get_path(None, 'parser')
        if (p <> None):
            # Not sure if this ever occurs...
            record = p.process_document(None, doc)
        else:
            record = BSParser.process_document(None,doc)

        dom = record.get_dom()
        return dom


    def _handleConfigNode(self, node):
        pass

    # Return parsed value (eg Int, Bool, String etc)
    # Or raise an error
    def _verifyOption(self, type, value):
        return value
    def _verifySetting(self, type, value):
        return self._verifyOption(type, value)
    def _verifyDefault(self, type, value):
        return self._verifyOption(type, value)

    def _recurseSubConfigs(self, child):
        for mod in child.childNodes:
            if mod.nodeType == elementType and mod.localName == "subConfig":
                self.subConfigs[mod.getAttribute('id')] = mod
            elif mod.nodeType == elementType and mod.localName == "path":
                if (mod.hasAttribute('type') and mod.getAttribute('type') == 'includeConfigs'):
                    # Import into our space
                    if (mod.hasAttribute('docid')):
                        self._includeConfigStorePaths.append(mod.getAttribute('docid'))
                    else:
                        path = getFirstData(mod).data
                        if  not os.path.isabs(path):
                            path = os.path.join(self.get_path(None, 'defaultPath'), path)
                        dom = self.getDomFromFile(path)
                        for child2 in dom.childNodes[0].childNodes:
                            if child2.nodeType == elementType:
                                if child2.localName == "subConfigs":
                                    self._recurseSubConfigs(child2)
                                elif (child2.localName == "objects"):
                                    # record object ref to instantiate
                                    for obj in child2.childNodes:
                                        if (obj.nodeType == elementType and obj.localName == "path"):
                                            type = obj.getAttribute('type')
                                            id = obj.getAttribute('ref')
                                            self._objectRefs.append((id, type))
                else:
                    if (mod.hasAttribute('docid')):
                        # record id in self.paths['configStore'] to retrieve
                        # Need to put into list for after building configStore!
                        self._configStorePaths.append(mod.getAttribute('docid'))
                    else:
                        path = getFirstData(mod).data
                        if  not os.path.isabs(path):
                            path = os.path.join(self.get_path(None, 'defaultPath'), path)
                        dom = self.getDomFromFile(path)
                        self.subConfigs[mod.getAttribute('id')] = dom.childNodes[0]

    def __init__(self, topNode, parentObject=None):

        self.parent = parentObject
        self.subConfigs = {}
        self.paths = {}
        self.objects = {}
        self.settings = {}

        self._objectRefs = []
        pathObjects = {}
        
        try:
            if (topNode.hasAttribute('id')):
                self.id = topNode.getAttribute('id')
        except:
            # XXX Hmmm.
            if (topNode.hasAttributeNS(None, 'id')):
                self.id = topNode.getAttributeNS(None, 'id')
                

        for child in topNode.childNodes:
            if child.nodeType == elementType:
                if child.localName == "name":
                    self.name = getFirstData(child).data
                elif (child.localName == "objectType"):
                    self.objectType = getFirstData(child).data
                elif (child.localName == "paths"):
                    # Configure self with paths
                    for child2 in child.childNodes:
                        if child2.nodeType == elementType:
                            type = child2.getAttribute('type')
                            if child2.localName == "path":
                                value = getFirstData(child2).data
                                self.paths[type] = value
                            elif child2.localName == "object":
                                value = child2.getAttribute('ref')
                                pathObjects[type] = value
                elif (child.localName == "imports"):
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

                elif (child.localName == "subConfigs"):
                    # Pointers to dom nodes for config ids
                    self._recurseSubConfigs(child)

                elif (child.localName == "objects"):
                    for obj in child.childNodes:
                        if (obj.nodeType == elementType and obj.localName == "path"):
                            type = obj.getAttribute('type')
                            id = obj.getAttribute('ref')
                            self._objectRefs.append((id, type))
                elif (child.localName == "options"):
                    # See configInfo in ZeeRex
                    for child2 in child.childNodes:
                        if (child2.nodeType == elementType):
                            type = child2.getAttribute('type')
                            if (child2.localName == "setting"):
                                value = self._verifySetting(type, getFirstData(child2).data)
                                self.settings[type] = value
                            elif (child2.localName == "default"):
                                value = self._verifyDefault(type, getFirstData(child2).data)
                                self.defaults[type] = value
                else:
                    self._handleConfigNode(child)

        if (self.paths.has_key("pythonPath")):
            sys.path.append(self.paths['pythonPath'][1])

        # Dynamically Instantiate objects. This is mindbending :}
        for o in (self._objectRefs):
            # Instantiate
            config = self.get_config(None, o[0])
            try:
                obj = dynamic.makeObjectFromDom(config, self)
            except ConfigFileException:
                # Push back up 
                raise
            except:
                raise ConfigFileException("Failed to build %s" % (o[0]))
            self.objects[o[0]] = obj

        # Add default Object types to paths
        for t in pathObjects.keys():
            self.paths[t] = self.get_object(None, pathObjects[t])

        # NB: This is an object type, not an id, so safe
        # Can be instantiated from any level
        cfgStore = self.get_path(None, 'configStore')

        if (self._includeConfigStorePaths):
            if (cfgStore <> None):
                for id in self._includeConfigStorePaths:
                    rec = cfgStore.fetch_record(None, id)
            else:
                raise(ValueError)

        # Now try and build last set of objects
        if (self._configStorePaths):
            if (cfgStore <> None):
                for id in self._configStorePaths:
                    rec = cfgStore.fetch_record(None, id)
            else:
                raise(ValueError)


    # During object instantiation session will be None
    # (As we need get_* to find authStore to get user for Session)
    # Don't raise a KeyError as there's often times when we need to
    # test existence of key

    def get_setting(self, session, id):
        if (self.settings.has_key(id)):
            return self.settings[id]
        elif (self.parent <> None):
            return self.parent.get_setting(session, id)
        else:
            return None
    
    def get_default(self, session, id): 
        if (self.defaults.has_key(id)):
            return self.defaults[id]
        elif (self.parent <> None):
            return self.parent.get_default(session, id)
        else:
            return None

    def get_object(self, session, id):
        if (self.objects.has_key(id)):
            return self.objects[id]
        elif (self.parent <> None):
            return self.parent.get_object(session, id)
        else:
            return None

    def get_config(self, session, id):
        if (self.subConfigs.has_key(id)):
            return self.subConfigs[id]
        elif (self.parent <> None):
            return self.parent.get_config(session, id)
        else:
            return None

    def get_path(self, session, id):

        if (self.paths.has_key(id)):
            path = self.paths[id]
            # Special handling for defaultPath :/
            if (id == "defaultPath" and not os.path.isabs(path)):
                p1 = self.parent.get_path(session, id)
                path = os.path.join(p1, path)
            return path
        elif (self.parent <> None):
            return self.parent.get_path(session, id)
        else:
            return None
