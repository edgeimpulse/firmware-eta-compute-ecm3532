# Python program to demonstrate 
# command line arguments 

import sys 
import os
import os.path
from os import path
from shutil import copyfile
import subprocess
from subprocess import call
  
# total arguments 
n = len(sys.argv) 

if n<2:
    print("Usage : python3 generateNewProject.pl <projectName>")
    exit()

projectName = sys.argv[1]
projectPath = "../../Applications/"+sys.argv[1]

if path.exists(projectPath):
    print(projectName + " already exists")
    exit()

print("Creating project : "+projectName)
os.mkdir(projectPath)
os.mkdir(projectPath+"/build")
os.mkdir(projectPath+"/src")
os.mkdir(projectPath+"/include")
os.mkdir(projectPath+"/configs")
copyfile("CMakeLists_template.txt", projectPath+"/CMakeLists.txt")
copyfile("main_template.c", projectPath+"/src/main.c")
copyfile("Kconfig_template", projectPath+"/Kconfig")
generateCommand = "bash generateForSingleProject.sh "+projectName
call(generateCommand,shell=True)
sedCmd = "sed -i \'s/Task_Name_placeholder/"+projectName+"/g\' "+ projectPath+"/src/main.c"
call(sedCmd,shell=True)