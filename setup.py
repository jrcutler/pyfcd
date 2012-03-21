#!/usr/bin/env python
from distutils.core import setup, Extension

pyfcd = Extension(
	'pyfcd',
	libraries = ['fcd'],
	sources = ['pyfcd.c']
)

setup(
	name = 'pyfcd',
	version = '1.0',
	description = 'Python interface to FUNcube dongles (via libfcd)',
	ext_modules = [pyfcd]
)
