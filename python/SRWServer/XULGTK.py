#!/usr/bin/python

#  Sad.
#  I can't be bothered typing in all of the silly gtk.HBox() commands, so I'm writing an XUL parser in python to generate the appropriate structure.
#  Sort of like GLADE yes. But better cos it uses XUL :P

from xml.dom.minidom import Node, parseString
import gtk, gobject, sys, types
from gtk import TRUE, FALSE

# XULElement gets (useful) attrs:
# align class collapsed container context debug dir equalsize flex flexgroup
# height hidden id maxheight maxwidth minheight minwidth observes pack popup
# tooltip tooltiptext width 

# Signals:  http://www.gtk.org/tutorial/app-gtksignals.html

globalIds = {}
myTooltips = gtk.Tooltips()
myTooltips.set_delay(10)
functions = {}

def addId(id, object):
    if (globalIds.has_key(id)):
        raise(ValueError)
    else:
        globalIds[id] = object

def registerFunction(name, fn):
    if (functions.has_key(name)):
        raise(ValueError)
    else:
        functions[name] = fn

def firstChildData(elem):
    """ Find first child which is Data """
    for c in elem.childNodes:
        if c.nodeType == Node.TEXT_NODE:
            return c
    return None

def flattenText(elem):
    """ Find first child which is Data """
    text = ""
    for c in elem.childNodes:
        if c.nodeType == Node.TEXT_NODE:
            text = text + c.data
    return text

def getFirstElement(elem):
    """ Find first child which is an Element """
    for c in elem.childNodes:
        if c.nodeType == Node.ELEMENT_NODE:
            return c
    return None

class XGElement:
    # This sucks. Should figure out multiple inheritance properly
    xul_align = None
    xul_border = None
    xul_checked = None
    xul_class = None
    xul_collapsed = None
    xul_default = None
    xul_disabled = None
    xul_flex = None
    xul_height = None
    xul_hidden = None
    xul_id = None
    xul_increment = None
    xul_label = None
    xul_maxlength = None
    xul_maxpos = None
    xul_orient = None
    xul_pack = None
    xul_pageincrement = None
    xul_popup = None
    xul_readonly = None
    xul_selected = None
    xul_size = None
    xul_title = None
    xul_tooltiptext = None
    xul_type = None
    xul_valign = None
    xul_value = None
    xul_width = None

    xul_onblur = None
    xul_oncommand = None    # clicked, or activate for menuItems
    xul_onchange = None     # changed
    xul_onclose = None      # destroy
    xul_onfocus = None
    xul_onhide = None
    xul_onselect = None
    xul_onshow = None       

    gtkPackTo = None
    gtkPackFunction = "add"

    def __init__(self, node,parent=None):
        self.xulNode = node
        self.parentObject = parent
        for a in range(node.attributes.length):
            attr = node.attributes.item(a)
            setattr(self, "xul_" + attr.localName, attr.nodeValue)
        if (self.xul_id != None):
            addId(self.xul_id, self)


    def processCommon(self):
        if self.xul_disabled:
            self.set_sensitive(FALSE)
        if self.xul_height == None or self.xul_height == "fill":
            self.xul_height = -1
        else:
            self.xul_height = int(self.xul_height)
        if self.xul_width == None or self.xul_width == "fill":
            self.xul_width = -1
        else:
            self.xul_width = int(self.xul_width)
        self.set_size_request(self.xul_width, self.xul_height)
        if self.xul_onclose:
            self.connect('destroy', functions[self.xul_onclose])
        if self.xul_onshow:
            self.connect('show', functions[self.xul_onshow])
        if self.xul_onhide:
            self.connect('hide', functions[self.xul_hide])
        if self.xul_tooltiptext:
            myTooltips.set_tip(self, self.xul_tooltiptext)


    def buildChildren(self):
        for c in self.xulNode.childNodes:
            if c.nodeType == Node.ELEMENT_NODE:
                # Instantiate
                n = defaultFactory.instantiateNode(c, self)
                # Now pack it ... 
                if (self.gtkPackTo):
                    self.gtkPackTo.add(n)
                else:
                    self.add(n)
            elif c.nodeType == Node.TEXT_NODE:
                # Text. Uhhh... Ignore?
                pass
            elif c.nodeType == Node.COMMENT_NODE:
                # Comment. Definitely ignore
                pass


class XGWindow(XGElement, gtk.Window):
    def __init__(self,node,parent=None):
        XGElement.__init__(self,node)
        gtk.Window.__init__(self)
        self.connect('destroy', lambda w: gtk.main_quit())

        if self.xul_title:
            self.set_title(self.xul_title)
        if self.xul_width and self.xul_height:
            self.set_default_size(int(self.xul_width), int(self.xul_height))
        if self.xul_orient == "vertical":
            # Urgh. Need to pack to a box AFAICT
            box = gtk.VBox()
            self.add(box)
            self.gtkPackTo = box

        self.buildChildren()


class XGVBox(XGElement, gtk.VBox):
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        gtk.VBox.__init__(self, homogeneous=FALSE)
        self.processCommon()
        if self.xul_border:
            self.set_border_width(int(self.xul_border))
        if self.xul_align or self.xul_valign:
            # Create an alignment + box to pack to
            if (self.xul_align == "center"):
                x = 0.5
            elif (self.xul_align == "left"):
                x = 0.0
            elif (self.xul_align == "right"):
                x = 1.0
            else:
                x = 0.0
            if (self.xul_valign == "center"):
                y = 0.5
            elif (self.xul_valign == "top"):
                y = 0.0
            elif (self.xul_valign == "bottom"):
                y = 1.0
            else:
                y = 0.0
            if (self.xul_height == "fill"):
                # Fill space
                x2 = 1.0
            else:
                x2 = 0.0
            if (self.xul_width == "fill"):
                y2 = 1.0
            else:
                y2 = 0.0

            align = gtk.Alignment(x,y,x2,y2)
            self.add(align)
            box = gtk.VBox()
            align.add(box)
            self.gtkPackTo = box


        self.buildChildren()

class XGHBox(XGElement, gtk.HBox):
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        gtk.HBox.__init__(self, FALSE)
        self.processCommon()
        if self.xul_border:
            self.set_border_width(int(self.xul_border))
        if self.xul_align or self.xul_valign:
            # Create an alignment + box to pack to
            if (self.xul_align == "center"):
                x = 0.5
            elif (self.xul_align == "left"):
                x = 0.0
            elif (self.xul_align == "right"):
                x = 1.0
            else:
                x = 0.0
            if (self.xul_valign == "center"):
                y = 0.5
            elif (self.xul_valign == "top"):
                y = 0.0
            elif (self.xul_valign == "bottom"):
                y = 1.0
            else:
                y = 0.0
            if (self.xul_height == "fill"):
                # Fill space
                x2 = 1.0
            else:
                x2 = 0.0
            if (self.xul_width == "fill"):
                y2 = 1.0
            else:
                y2 = 0.0

            align = gtk.Alignment(x,y,x2,y2)
            self.add(align)
            box = gtk.HBox()
            align.add(box)
            self.gtkPackTo = box
        self.buildChildren()


class XGButton(XGElement, gtk.Button):
    def __init__(self, node, parent):
        # This is only for NORMAL buttons
        # If type="" in XUL, then raise()

        XGElement.__init__(self,node, parent)
        if (self.xul_type):
            raise(ValueError)
        gtk.Button.__init__(self)
        if self.xul_label:
            self.set_label(self.xul_label)
        self.processCommon()
        if self.xul_oncommand:
            self.connect('clicked', functions[self.xul_oncommand])

        self.buildChildren()

# XXX: Implement. (But what is it?)
class XGToggleButton(XGElement, gtk.ToggleButton):
    pass

class XGCheckButton(XGElement, gtk.CheckButton):
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        gtk.CheckButton.__init__(self)
        self.processCommon()
        if (self.xul_checked):
            self.set_active(TRUE)
        if self.xul_label:
            self.set_label(self.xul_label)

        self.buildChildren()

class XGRadioButton(XGElement, gtk.RadioButton):
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        
        # Need to get the Group
        rp = parent
        while rp:
            if (isinstance(rp, XGRadioGroup)):
                break
            else:
                rp = rp.parentObject
        if (rp and rp.groupId):
            group = rp.groupId
        else:
            group = None
        gtk.RadioButton.__init__(self, group)
        if (rp and not rp.groupId):
            rp.groupId = self

        self.processCommon()
        if (self.xul_checked):
            self.set_active(TRUE)
        if self.xul_label:
            self.set_label(self.xul_label)
        self.buildChildren()

class XGRadioGroup(XGElement, gtk.VBox):
    groupId = None
    def __init__(self, node, parent):
        XGElement.__init__(self,node,parent)
        gtk.VBox.__init__(self)
        self.processCommon()
        self.buildChildren()


class XGLabel(XGElement, gtk.Label): 
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        if (self.xul_value):
            text = self.xul_value
        else:
            # Extract data from element
            text = flattenText(node)
        gtk.Label.__init__(self, text)
        self.processCommon()
        self.buildChildren()

class XGEntry(XGElement, gtk.Entry):
    def __init__(self, node, parent):
        XGElement.__init__(self,node, parent)
        if self.xul_maxlength:
            gtk.Entry.__init__(self, int(self.xul_maxlength))
        else:
            gtk.Entry.__init__(self)
        self.processCommon()
        if self.xul_readonly:
            self.set_editable(FALSE)
        if self.xul_value:
            self.set_text(self.xul_value)
        if self.xul_type == "password":
            self.set_visibility(FALSE)
            self.set_invisible_char(u"*")
        if self.xul_size:
            self.set_width_chars(int(self.xul_size))
        self.buildChildren()

class XGTabbox(XGElement, gtk.Notebook):
    def __init__(self, node, parent):
        # Notebook. Need to construct, then add.
        self.tabLabels = []
        self.tabWidgets = []
        XGElement.__init__(self, node, parent)
        gtk.Notebook.__init__(self)
        self.processCommon()

        self.buildChildren()

        # Now add pages.
        for p in (range(len(self.tabWidgets))):
            text = self.tabLabels[p]
            if (not text):
                text = "Tab"
            l = gtk.Label(text)
            self.append_page(self.tabWidgets[p], l)

    def add(self, object):
        # Ignore, we're done in the constructor
        pass

class XGTabs(XGElement):
    def __init__(self, node, parent):
        # Pass down. Use for TabPanels as well
        XGElement.__init__(self, node, parent)
        self.buildChildren()
    def add(self, object):
        # Irrelevant
        pass

class XGTab(XGElement):
    def __init__(self, node, parent):
        XGElement.__init__(self,node,parent)
        rp = parent
        while rp:
            if (isinstance(rp, XGTabbox)):
                break
            else:
                rp = rp.parentObject
        if (rp == None):
            raise(ValueError)
        rp.tabLabels.append(self.xul_label)
        if (self.xul_selected):
            rp.selectedIndex = len(rp.tabLabels)

class XGTabPanel(XGElement):
    def __init__(self, node, parent):
        XGElement.__init__(self,node,parent)
        self.object = None
        rp = parent
        while rp:
            if (isinstance(rp, XGTabbox)):
                break
            else:
                rp = rp.parentObject
        if (rp == None):
            raise(ValueError)
        self.buildChildren()
        rp.tabWidgets.append(self.object)

    def add(self, object):
        
        if (self.object != None):
            box = gtk.HBox()
            box.add(self.object)
            self.object = box
            self.object.add(object)
        else:
            self.object = object


class XGList(XGElement, gtk.TreeView):
    store = None

    def __init__(self, node, parent):
        XGElement.__init__(self,node,parent)
        self.listitems = []
        self.listheads = []

        # Build kids first to gather up listitems
        self.buildChildren()

        initargs = []
        if (len(self.listitems) != 0 and type(self.listitems[0]) in [types.StringType, types.UnicodeType]):
            for h in range(len(self.listitems)):
                initargs.append(gobject.TYPE_STRING)
                if (len(self.listheads) <= h):
                    self.listheads.append("Column " + str(h))
                    
        elif (self.listitems and type(self.listitems[0]) == types.ListType):
            for h in range(len(self.listitems[0])):
                initargs.append(gobject.TYPE_STRING)
                if (len(self.listheads) <= h):
                    self.listheads.append("Column " + str(h))

        self.store = apply(gtk.ListStore, initargs)

        for it in self.listitems:
            iter = self.store.append()
            if (type(it) in [types.StringType, types.UnicodeType]):
                self.store.set(iter, 0, it)
            else:
                # need to apply()
                setargs = [iter];
                print it
                for h in range(len(it)):
                    setargs.append(h)
                    setargs.append(it[h])
                apply(self.store.set, setargs)

        gtk.TreeView.__init__(self, self.store)
        self.processCommon()
        model = self.get_model()
        renderer = gtk.CellRendererText()
        for h in range(len(self.listheads)):
            column = gtk.TreeViewColumn(self.listheads[h], renderer, text=h)
            self.append_column(column)


    def add(self, object):
        # Model is built by kids instantiation
        pass

class XGListItem(XGElement):
    def __init__(self, node, parent):
        XGElement.__init__(self,node,parent)
        rp = parent
        while rp:
            if (isinstance(rp, XGList)):
                break
            else:
                rp = rp.parentObject
        if (rp == None):
            raise(ValueError)

        self.listbox = rp
        # Get label...
        if (self.xul_label):
            text = self.xul_label
            rp.listitems.append(text)
        else:
            # Look for listcell
            self.cells = []
            self.buildChildren()
            rp.listitems.append(self.cells)

    def add(self, object):
        pass


class XGListCols(XGElement):
    def __init__(self, node, parent):
        # Uhhh....
        pass
    def add(self, object):
        pass

class XGListHead(XGElement):
    def __init__(self,node,parent):
        # Irrelevant wrapper, pass down to items
        XGElement.__init__(self,node,parent)
        self.buildChildren()

    def add(self, object):
        pass
        

class XGListHeadItem(XGElement):
    def __init__(self,node,parent):
        XGElement.__init__(self,node,parent)
        rp = parent
        while rp:
            if (isinstance(rp, XGList)):
                break
            else:
                rp = rp.parentObject
        if (rp == None):
            raise(ValueError)

        if (self.xul_label):
            text = self.xul_label
            rp.listheads.append(text)

    def add(self, object):
        pass

class XGListCell(XGElement):
    def __init__(self,node,parent):
        XGElement.__init__(self,node,parent)
        rp = parent
        while rp:
            if (isinstance(rp, XGListItem)):
                break
            else:
                rp = rp.parentObject
        if (rp == None):
            raise(ValueError)
        if (self.xul_label):
            text = self.xul_label
            rp.cells.append(text)
        else:
            rp.cells.append("")
    def add(self, object):
        pass

# def __init__(xalign=0.0, yalign=0.0, xscale=0.0, yscale=0.0)
# It has four settings: xscale, yscale, xalign, and yalign: The scale settings specify how much the child widget should expand to fill the space allocated to the Gtk::Alignment. The values can range from 0 (meaning the child doesn't expand at all) to 1 (meaning the child expands to fill all of the available space). The align settings place the child widget within the available area. The values range from 0 (top or left) to 1 (bottom or right). Of course, if the scale settings are both set to 1, the alignment settings have no effect.

class XGSpacer(XGElement, gtk.Alignment):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.Alignment.__init__(self)
        self.processCommon()
        
class XGProgressMeter(XGElement, gtk.ProgressBar):
    def __init__(self, node, parent):
        # Very little to do.
        XGElement.__init__(self, node, parent)
        gtk.ProgressBar.__init__(self)
        
class XGVScrollBar(XGElement, gtk.VScrollbar):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.VScrollbar.__init__(self)
        if (self.xul_increment and self.xul_pageincrement):
            self.set_increments(int(self.xul_increment), int(self.xul_pageincrement))
        if (self.xul_maxpos):
            self.set_range(0, int(self.xul_maxpos))

class XGHScrollBar(XGElement, gtk.VScrollbar):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.VScrollbar.__init__(self)
        if (self.xul_increment and self.xul_pageincrement):
            self.set_increments(int(self.xul_increment), int(self.xul_pageincrement))
        if (self.xul_maxpos):
            self.set_range(0, int(self.xul_maxpos))

# Menus

class XGMenuBar(XGElement, gtk.MenuBar):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.MenuBar.__init__(self)
        self.buildChildren()


class XGMenuItem(XGElement, gtk.MenuItem):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        if (self.xul_label):
            text = self.xul_label
        else:
            text = "Item?"
        gtk.MenuItem.__init__(self, text)

class XGMenuPopup(XGElement):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        self.buildChildren()

    def add(self, object):
        self.parentObject.add(object)

        
class XGMenu(XGElement, gtk.MenuItem):
    menuObject = None

    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        if (self.xul_label):
            text = self.xul_label
        else:
            text = "Menu"
        gtk.MenuItem.__init__(self, text)
        menu = gtk.Menu()
        self.menuObject = menu
        self.buildChildren()
        self.set_submenu(menu)

    def add(self, object):
        if (not isinstance(object, XGMenuPopup)):
            self.menuObject.add(object)
    

class XGMenuSeparator(XGElement, gtk.SeparatorMenuItem):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.SeparatorMenuItem.__init__(self)

class XGSeparator(XGElement, gtk.Separator):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.Separator.__init__(self)


# Toolbox is just another box
class XGToolBox(XGElement, gtk.VBox):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)
        gtk.VBox.__init__(self)
        self.buildChildren()
        
# Toolbar is a handleBox
class XGHandleBox(XGElement, gtk.HandleBox):
    def __init__(self,node,parent):
        XGElement.__init__(self, node, parent)
        gtk.HandleBox.__init__(self)
        # Only one child
        if self.xul_align or self.xul_valign:
            # Create an alignment + box to pack to
            if (self.xul_align == "center"):
                x = 0.5
            elif (self.xul_align == "left"):
                x = 0.0
            elif (self.xul_align == "right"):
                x = 1.0
            else:
                x = 0.0
            if (self.xul_valign == "center"):
                y = 0.5
            elif (self.xul_valign == "top"):
                y = 0.0
            elif (self.xul_valign == "bottom"):
                y = 1.0
            else:
                y = 0.0
            align = gtk.Alignment(x,y,0.0,0.0)
            self.add(align)
            box = gtk.HBox()
            align.add(box)
            self.gtkPackTo = box
        else:
            box = gtk.HBox()
            self.add(box)
            self.gtkPackTo = box
        self.buildChildren()

# Frame is how to put labels in box frames
class XGGroupBox(XGElement, gtk.Frame):
    def __init__(self, node, parent):
        XGElement.__init__(self, node, parent)

        # Label might be in @label, but more likely to be ./caption@label
        fnode = getFirstElement(node)
        if (fnode and fnode.localName == "caption" and fnode.hasAttribute('label')):
            label = fnode.getAttribute('label')
            # Pull out of the tree
            node.removeChild(fnode)
        else:
            # Is allowed to be None
            label = self.xul_label

        gtk.Frame.__init__(self, label)

        if self.xul_border:
            self.set_border_width(int(self.xul_border))
        else:
            self.set_border_width(3)
        # Only one child
        box = gtk.VBox()
        self.add(box)
        self.gtkPackTo = box
        self.buildChildren()

# VPaned and HPaned are boxes with splitters.
# paned.add1(object) to add top/left, add2 for bottom/right
# XUL has <splitter> which is a different model

# XUL's tooltip object can contain anything.  Can't do this in GTK.

class XGFactory:
    def __init__(self):
        self.objectMap = {"window" : XGWindow, "vbox": XGVBox, "hbox" : XGHBox, "checkbox" : XGCheckButton, "description" : XGLabel, "label" : XGLabel, "textbox" : XGEntry, "radiogroup": XGRadioGroup, "radio" : XGRadioButton, "listbox" : XGList, "listitem": XGListItem, "listcols" : XGListCols, "listhead" : XGListHead, "listheader" : XGListHeadItem, "listcell" : XGListCell, "spacer" : XGSpacer, "tabbox" : XGTabbox, "tabs" : XGTabs, "tab" : XGTab, "tabpanels" : XGTabs, "tabpanel" : XGTabPanel, "menubar" : XGMenuBar, "menuitem" : XGMenuItem, "menupopup": XGMenuPopup, "menu" : XGMenu, "menuseparator" : XGMenuSeparator, "separator" : XGSeparator, "toolbar" : XGHandleBox, "toolbarbutton" : XGButton, "toolbox" : XGToolBox, "groupbox": XGGroupBox}

    def instantiateNode(self, node, parent):
        print "Trying to instantiate: " + node.localName
        if self.objectMap.has_key(node.localName):
            return self.objectMap[node.localName](node, parent)
        elif node.localName == "button":
            # check type attribute
            if not node.hasAttribute('type'):
                return XGButton(node, parent)
            else:
                type = node.getAttribute('type')
                if (type == 'checkbox'):
                    # ToggleButton, not a checkbox
                    return XGToggleButton(node,parent)
                elif (type == 'menu' or type == "menu-button"):
                    pass
                elif (type == 'radio'):
                    return XGRadioButton(node,parent)
                else:
                    # Uhoh
                    raise(ValueError)
        elif (node.localName == "scrollbar"):
            if (not node.hasAttribute("orient") or node.getAttribute("orient") != "horizontal"):
                return XGVScrollBar(node, parent)
            else:
                return XGHScrollBar(node, parent)
        else:
            raise(NotImplementedError)


defaultFactory = XGFactory()


class XULLoader:
    xulns = "http://www.mozilla.org/keymaster/gatekeeper/there.is.only.xul"
    xhtmlns = "http://www.w3.org/1999/xhtml"

    topElement = None

    def __init__(self, xulfile="window.xul"):
        self.idHash = {}

        # Now lets build it
        inh = file(xulfile)
        text = inh.read()
        
        try:
            dom = parseString(text)
        except:
            print "Could not parse xul file."
            sys.exit()
            
        # Top level should be 'window' If not, then something is fubar
        if dom.documentElement.localName != "window":
            print "Not an XUL document"
            sys.exit()
            
        # Now walk the tree and build objects. Fun fun fun.
        window = defaultFactory.instantiateNode(dom.documentElement, None)
        # Don't show in case we want to do things to the structure.
        self.topElement =window

if __name__ == '__main__':
    f = XULLoader('client.xul')
    f.topElement.show_all()
    gtk.main()


