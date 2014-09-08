

import time, os, sys, types, traceback
from xml.dom import Node

# XXX Server Configuration
use4Suite = 1
if (use4Suite):
    try:
        from Ft.Xml.XPath import ParsedRelativeLocationPath, ParsedAbsoluteLocationPath, \
             ParsedStep, ParsedNodeTest, ParsedExpr, Compile
    except:
        os.putenv('USE_MINIDOM', '1')
        from Ft.Xml.XPath import ParsedRelativeLocationPath, ParsedAbsoluteLocationPath, \
             ParsedStep, ParsedNodeTest, ParsedExpr, Compile
else:
    from xml.xpath import *

elementType = Node.ELEMENT_NODE
textType = Node.TEXT_NODE

# --- Definitions ---

def evaluateXPath(xp, dom):
    if (use4Suite):
        context = Context.Context(dom)
        return xp.evaluateEvaluate(xp, dom, context)
    else:
        return xp.evaluate(node)

def flattenTexts(elem):
    # recurse down tree and flatten all text nodes into one string.
    # Use list + join() to avoid memory overhead in Python string addition
    texts = []
    for e in elem.childNodes:
        if e.nodeType == textType:
            texts.append(e.data)
        elif e.nodeType == elementType:
            # Recurse
            texts.append(flattenTexts(e))
    return ''.join(texts)

def getFirstElement(elem):
    """ Find first child which is an Element """
    for c in elem.childNodes:
        if c.nodeType == elementType:
            return c
    return None

def getFirstData(elem):
    """ Find first child which is Data """
    for c in elem.childNodes:
        if c.nodeType == Node.TEXT_NODE:
            return c
    return None

def getFirstElementByTagName(node, local):
    if node.nodeType == elementType and node.localName == local:
        return node;
    for child in node.childNodes:
        recNode = getFirstElementByTagName(child, local)
        if recNode:
            return recNode
    return None

def traversePath(node):

    if (isinstance(node, ParsedRelativeLocationPath.ParsedRelativeLocationPath)):
        left  = traversePath(node._left)
        right = traversePath(node._right)
        if (left == []):
            # self::node()
            return [right]
        elif (type(left[0]) == types.StringType):
            return [left, right]
        else:
            left.append(right)
            return left
    
    elif (isinstance(node, ParsedAbsoluteLocationPath.ParsedAbsoluteLocationPath)):
        left = ['/']
        right = traversePath(node._child)
        if (type(right[0]) == types.StringType):
            return [left, right]
        else:
            left.extend(right)
            return left

    elif (isinstance(node, ParsedStep.ParsedStep)):
        # XXX Check that axis is something we can parse
        a = node._axis._axis
        if (a == 'self'):
            return []
        
        n = node._nodeTest
        if use4Suite:
            local = ParsedNodeTest.LocalNameTest
            nameattr = "_name"
        else:
            local = ParsedNodeTest.NodeNameTest
            nameattr = "_nodeName"
        if (isinstance(n, local)):
            n = getattr(n, nameattr)
        elif (isinstance(n, ParsedNodeTest.QualifiedNameTest)):
            n = n._prefix + ":" + n_localName
        elif (isinstance(n, ParsedNodeTest.PrincipalTypeTest)):
            n = "*"
        else:
            raise(NotImplementedError)
            
        preds = node._predicates

        pp = []
        if (preds):
            for pred in preds:
                pp.append(traversePath(pred))

        return [a, n, pp]
        
    elif (isinstance(node, ParsedExpr.ParsedEqualityExpr) or isinstance(node, ParsedExpr.ParsedRelationalExpr)):
        # @id="fish"
        op = node._op

        # Override check for common: [position()=int]
        if (op == '=' and isinstance(node._left, ParsedExpr.FunctionCall) and node._left._name == 'position' and isinstance(node._right, ParsedExpr.ParsedNLiteralExpr)):
            return node._right._literal

        left = traversePath(node._left)
        if (type(left) == types.ListType and left[0] == "attribute"):
            left = left[1]
        right = traversePath(node._right)
        return [left, op, right]

    elif (isinstance(node, ParsedExpr.ParsedNLiteralExpr) or isinstance(node, ParsedExpr.ParsedLiteralExpr)):
        # 7 or "fish"
        return node._literal
    elif (isinstance(node, ParsedExpr.FunctionCall)):
        # XXX check for more functions!
        if (node._name == 'last'):
            # Override for last using Pythonic expr
            return -1
        else:
            raise(NotImplementedError)

    elif (isinstance(node, ParsedExpr.ParsedAndExpr)):
        return [traversePath(node._left), 'and', traversePath(node._right)]
    elif (isinstance(node, ParsedExpr.ParsedOrExpr)):
        return [traversePath(node._left), 'or', traversePath(node._right)]
    else:
        # We'll need to do full XPath vs DOM
        raise(NotImplementedError)


def verifyXPaths(paths):
    compiled = []
    for p in paths:
            allAbsolute = 1
#        try:
            plist = []
            xpObj= Compile(p)
            plist.append(xpObj)
            try:
                t = traversePath(xpObj)
                if (t[0] <> '/' and type(t[0]) == types.StringType):
                    # a single Step
                    t = [t]
                plist.append(t)
            except NotImplementedError:
                # We can't handle it!
                plist.append(None)
            compiled.append(plist)
 #       except:
 #           # Utoh, invalid path. Warn.
 #           print sys.exc_info()[2]
 #           print "Invalid XPath: " + p
 #           raise(sys.exc_info()[1])
    return compiled





