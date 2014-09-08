
# Module wrapper into which we're going to import anything
# defined in configfiles
from utils import getFirstData, elementType
import sys

def makeObjectFromDom(topNode, parentObject):
    # Lots of indirections from xml to object
    objectType = None
    for c in topNode.childNodes:
        if (c.nodeType == elementType and c.localName == "objectType"):
            # Here's what we want to instantiate
            objectType = getFirstData(c).data
            break
    if objectType == None:
        raise(ConfigFileException('No objectType set in config file.'))

    # XXX Fix for a.b.c  (?)
    objs = objectType.split('.')
    globalImport(objs[0], [objs[1]])
    parentClass = globals()[objs[0]]
    for o in objs[1:]:
        parentClass = getattr(parentClass, o)
    return parentClass(topNode, parentObject)

def globalImport(module, objects=[], name=None):
    # With thanks to:
    # http://pleac.sourceforge.net/pleac_python/packagesetc.html

    nname = module
    loaded = __import__(module)
    for mod in (module.split(".")[1:]):
        nname = mod
        loaded = getattr(loaded, mod)
    if name <> None:
        globals()[name] = loaded
    else:
        globals()[nname] = loaded

    if (len(objects) == 1 and objects[0] == "*"):
        objects = dir(loaded)

    for o in objects:
        if (o[0] <> "_"):
            globals()[o] = getattr(loaded, o)

    return loaded
