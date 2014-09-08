
from configParser import C3Object
from baseObjects import Extracter
from utils import flattenTexts
from dateutil import parser

# We need number of occurences, so process should be return a hash

class SimpleExtracter(Extracter):
    def process_string(self, session, data):
        # Accept just text and extract bits from it.
        return {data:1}

    def process_node(self, session, data):
        # Walk a DOM structure and extract
        return {flattenTexts(data): 1}

    def process_eventList(self, session, data):
        # Step through a SAX event list and extract
        txt = []
        for e in data:
            if (e[1:5] == "text"):
                txt.append(e[6:])
        return {''.join(txt): 1}


class KeywordExtracter(SimpleExtracter):

    punctuation = ''

    def __init__(self, parser, config):
        C3Object.__init__(self, parser, config)
        self.punctuation = self.get_setting(None, 'punctuation')
        if (self.punctuation == None):
            self.punctuation = '.,;:"!?/[]{}()*|~'
        elif (self.punctuation.find(' ') > -1):
            self.punctuation = self.punctuation.split()

    def _keywordify(self, session, data):
        # XXX: Load keywording rules from config!
        kw = {}
        for d in data.keys():
            # Remove extraneous punctuation
            if d:
                # This looks Very Slow
                for p in self.punctuation:
                    d = d.replace(p, ' ')

                splitWords = d.split()
                for w in splitWords:
                    kw[w] = kw.get(w, 0) + 1

        return kw

    def process_string(self, session, data):
        data = SimpleExtracter.process_string(self, session, data)
        return self._keywordify(session, data)
    def process_node(self, session, data):
        data = SimpleExtracter.process_node(self, session, data)
        return self._keywordify(session, data)
    def process_eventList(self, session, data):
        data = SimpleExtracter.process_eventList(self, session, data)
        return self._keywordify(session, data)


class DateExtracter(SimpleExtracter):
    def __init__(self, config, parent):
        C3Object.__init__(self, parser, config)

    def _datify(session, data):
        try:
            # This will only find 1 single date.
            d = parser.parse(data, fuzzy=True)
            return {d:1}
        except:
            wds = data.split()
            # reconstruct data word by word and feed to parser.
            # Must be a better way to do this
            return {}
 
    def process_string(self, session, data):
        data = SimpleExtracter.process_string(self, session, data)
        return self._datify(session, data)
    def process_node(self, session, data):
        data = SimpleExtracter.process_node(self, session, data)
        return self._datify(session, data)
    def process_eventList(self, session, data):
        data = SimpleExtracter.process_eventList(self, session, data)
        return self._datify(session, data)
    
class GeoExtracter(SimpleExtracter):
    # XXX Actually write this too!
    def __init__(self, config, parent):
        C3Object.__init__(self, parser, config)
    def process_string(self, session, data):
        pass
    def process_node(self, session, data):
        pass
    def process_eventList(self, session, data):
        pass

class UriExtracter(SimpleExtracter):
    # (protocol):[//](user):(password)@(host)/(remains)
    pass


class ProximityExtracter(KeywordExtracter):
    # Keyword proximity
    def process_string(self, session, data):
        kw = {}
        w = 0
        lno = -1
        for p in self.punctuation:
            data = data.replace(p, ' ')
        splitWords = data.split()
        for wrd in splitWords:
            temp = kw.get(wrd, [0])
            temp[0] += 1
            temp.extend([lno, w])
            kw[wrd] = temp
            w += 1
        return kw


    def process_node(self, session, data):
        raise NotImplementedError

    def process_eventList(self, session, data):
        lno = int(data[-1].split()[-1])
        w = 0
        kw = {}
        for e in data:
            if (e[1:5] == "text"):
                line = e[6:]
                for p in self.punctuation:
                    line = line.replace(p, ' ')
                splitWords = line.split()
                for wrd in splitWords:
                    temp = kw.get(wrd, [0])
                    temp[0] += 1
                    temp.extend([lno, w])
                    kw[wrd] = temp
                    w += 1
        return kw
    


    
