
import time

resultSets = []
authenticationTokens = []
authenticationTokenValues = {}

def sort_rset(a, b):
    at = a['time'] + a['ttl']
    bt = b['time'] + b['ttl']
    if at > bt:
        return 1
    elif at == bt:
        return 0
    else:
        return -1

def sort_at(a,b):
    at = a.time + a.ttl
    bt = b.time + b.ttl
    if at > bt:
        return 1
    elif at == bt:
        return 0
    else:
        return -1    

def expire_rsat():
    global resultSets, authenticationTokens, authenticationTokenValues
    while resultSets and resultSets[0]['time'] < (time.time() - resultSets[0]['ttl']):
        resultSets[0]['config'].expireResultSet(resultSets[0]['resultSetName'])
        resultSets = resultSets[1:]
    while authenticationTokens and authenticationTokens[0].time < (time.time() - authenticationTokens[0].ttl):
        if authenticationTokens[0].config.logLevel > 2:
            print "AuthToken:  Expiring " + authenticationTokens[0].value
        del authenticationTokenValues[authenticationTokens[0].value]
        authenticationTokens = authenticationTokens[1:]

def expire_rs_from_config(conf):
    global resultSets
    for set in resultSets:
        if set['config'] == self:
            resultSets = resultSets.remove(set)

