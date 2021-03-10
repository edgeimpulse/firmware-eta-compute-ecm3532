import os
import sys

replace     = sys.argv[1]
replacement    = sys.argv[2]

for dname, dirs, files in os.walk("../"):
    for fname in files:
        fpath = os.path.join(dname, fname)
        print(fpath)
        string = "sed -i 's/"+replace+"/"+replacement+"/g' " + fpath
        print(string)
        os.system(string)

exit(0)