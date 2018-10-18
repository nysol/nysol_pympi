from setuptools import setup, find_packages, Extension
import re
import os
import subprocess


mpimod = Extension('nysolmpi/util/mparallel',
                    sources = ['src/meachi.c']
					)


setup(name = 'nysolmpi',
			packages=['nysolmpi/util'],
			version = '0.0.1',
			description = 'This is nysol tools',
			long_description="""\
NYSOL MPI TOOL
""",
			author='nysol',
			author_email='info@nysol.jp',
			license='AGPL3',
			url='http://www.nysol.jp/',
      classifiers=[ 
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU Affero General Public License v3',
        'Operating System :: POSIX :: Linux',
        'Operating System :: MacOS :: MacOS X',
        'Programming Language :: C',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Scientific/Engineering :: Information Analysis',
        'Topic :: Scientific/Engineering :: Mathematics',
        ],
			ext_modules =[mpimod]
		)
       
