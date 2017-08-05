import sys
import os
sys.path.append('fat16gen')

from genfs import vFileSystem

rootdir = 'testfiles'
vfiles = [{'path' : os.path.join(rootdir,'a virtual file.txt'), 
           'size' : 1024, 
           'func' : 'vfile_1', 
           'operand' : '(void *) 0'
           }
          ]

fs = vFileSystem(rootdir,vfiles)
fs.gensys(sys.stdout,'filesys','VIRTUAL DISK')
