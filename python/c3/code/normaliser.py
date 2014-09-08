
from baseObjects import Normaliser
from c3errors import ConfigFileException

class SimpleNormaliser(Normaliser):
    def process_string(self, session, data):
        # normalise string into single appropriate form (eg '1' -> 1)
        return data

class CaseNormaliser(SimpleNormaliser):
    def process_string(self, session, data):
        return data.lower()

class ArticleNormaliser(SimpleNormaliser):
    def process_string(self, session, data):
        d = data.lower()
        if (d[:4] == "the "):
            return data[4:]
        elif (d[:2] == "a "):
            return data[2:]
        elif (d[:3] == "an "):
            return data[3:]
        else:
            return data

class PossessiveNormaliser(SimpleNormaliser):
    def process_string(self, session, data):
        if (data[-2:] == "s'"):
            return data[:-1]
        elif (data[-2:] == "'s"):
            return data[:-2]
        else:
            return data

class IntNormaliser(SimpleNormaliser):
    def process_string(self, session, data):
        try:
            return long(data)
        except:
            return None
        
class StringIntNormaliser(SimpleNormaliser):
    def process_string(self, session, data):
        try:
            d = long(data)
            return "%012d" % (d)
        except:
            return None


try:
    # We require PyStemmer from
    # http://sourceforge.net/projects/pystemmer/
    import Stemmer

    class StemNormaliser(SimpleNormaliser):
        stemmer = None

        def __init__(self, config, parent):
            SimpleNormaliser.__init__(self, config, parent)
            lang = self.get_setting(None, 'language')
            if lang == None:
                lang = 'english'
            try:
                st = Stemmer.Stemmer(lang)
            except:
                raise(ConfigFileException("Unknown stemmer language: %s" % (lang)))

            cache = self.get_setting(None, 'cacheSize')
            if (cache <> None):
                cache = int(cache)
            else:
                cache = 5000
            st.setCacheSize(cache)
            self.stemmer = st

        def process_string(self, session, data):
            return self.stemmer.stem(data)

except:
    class StemNormaliser(SimpleNormaliser):
        def __init__(self, config, parent):
            raise(ConfigFileException('Stemmer library not available'))
        
