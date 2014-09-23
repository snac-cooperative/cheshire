#!/usr/bin/env python

"""
setup.py file for CheshirePy extension
"""

from distutils.core import setup, Extension

CheshirePy_module = Extension('_CheshirePy',sources=['CheshirePy_wrap.c','CheshirePy.c'],)

setup(name = 'CheshirePy',
      version = '0.1',
      description = """Cheshire Search Extension for Python""",
      ext_modules = [CheshirePy_module],
      py_modules = ["CheshirePy"],
      )
