#!/usr/bin/python2.6

from distutils.core import setup, Extension

module1 = Extension('ZCheshire',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include', '../header'],
                    libraries = ['client', 'z3_asn', 'cmdparse', 'netconnect', 'utils', 'list', 'diag'],
                    library_dirs = ['../lib'],
                    sources = ['ZCheshire.c'])

setup (name = 'ZCheshire',
       version = '1.0',
       description = 'Cheshire Python Z39.50 extension',
       author = 'Rob Sanderson, Ray Larson',
       author_email = 'azaroth@liv.ac.uk, ray@sherlock.berkeley.edu',
       url = 'http://cheshire.berkeley.edu/',
       long_description = '''
       Cheshire, Z39.50, bla bla bla.
       ''',
       ext_modules = [module1])
