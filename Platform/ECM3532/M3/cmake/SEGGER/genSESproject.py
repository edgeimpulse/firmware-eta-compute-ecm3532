#!/usr/bin/env python3
import jinja2
import sys
import argparse
import json
import os
import re
from collections import namedtuple
from shutil import copyfile

File = namedtuple("File", ["path"])
Target = namedtuple("Target", ["name", "includes", "defines", "sources"])
Group = namedtuple("Group", ["name", "files", "folder_match", "optimisation"])
DataRegion = namedtuple("DataRegion", ["start", "size"])

def generate_relative_path(pathA, pathB):
    pathA = os.path.relpath(pathA, pathB)
    return pathA.replace("\\", "/")

folderGroupingTemplates = [
    Group(name="Freertos-Plus", files=[], folder_match="Thirdparty/FreeRTOS-Plus", optimisation="default"),
    Group(name="Executor", files=[], folder_match="framework/executor", optimisation="Size"),
    Group(name="Freertos", files=[], folder_match="Thirdparty/FreeRTOS", optimisation="default"),
    Group(name="CMSIS", files=[], folder_match="Thirdparty/CMSIS", optimisation="default"),
    Group(name="Util", files=[], folder_match="util", optimisation="default"),
    Group(name="CSP", files=[], folder_match="hw/csp", optimisation="Size"),
    Group(name="HW", files=[], folder_match="hw", optimisation="default"),
    Group(name="Framework", files=[], folder_match="framework", optimisation="default"),
    Group(name="App", files=[], folder_match = "Applications", optimisation="default"),
    Group(name="Library", files=[], folder_match = "pre_built", optimisation="default"),
    Group(name="Other", files=[], folder_match = "", optimisation="default")
    ]

def group_files(files, out_dir):
    other = Group(name="Other", files=[], folder_match=None, optimisation="default")
    groups = folderGroupingTemplates[:]
    for f in files:
        group_match = False
        for g in groups:
            if g.folder_match in f:
                f = generate_relative_path(f, out_dir)
                g.files.append(File(f))
                group_match = True
                break
        if not group_match:
            f = generate_relative_path(f, out_dir)
            other.files.append(File(f))

    groups.append(other)
    # Remove empty groups
    for g in groups[:]:
        if len(g.files) == 0:
            groups.remove(g)

    return groups


def main():
    input_file = sys.argv[1]
    out_dir = sys.argv[2]

    with open(input_file, "r") as f:
        config = json.load(f)
    (config["target"]["name"],extension) = os.path.splitext(os.path.basename(config["target"]["name"]))

    files = config["target"]["sources"].split(";")
    config["target"]["includes"] = [generate_relative_path(i, out_dir) for i in config["target"]["includes"].split(";")]
    config["target"]["groups"] = group_files(files, out_dir)
    s = ""

    with open("../../../Platform/ECM3532/M3/cmake/SEGGER/ses.xml", "r") as f:
        s = f.read()

    t = jinja2.Template(s)
    ses_project_temp = t.render(config)
    ses_project = ''

    groups_all = folderGroupingTemplates[:]
    for line in ses_project_temp.splitlines():
        ses_project = ses_project + line + "\n"
        m = re.search(r'folder Name="([\s\S]+)">', line)
        if m:
            name = m.group(1)
            for g in groups_all:
                if (g.name == name):
                    if (g.optimisation != "default"):
                        opt_line = "      <configuration Name=\"Common\" gcc_cl_optimization_level=\""+g.optimisation+"\" />"
                        ses_project = ses_project + opt_line + "\n"

    out_dir += "/"
    # Remove . in filename
    output_filename = out_dir + config["target"]["name"].replace(".", "_")
    project_file = output_filename + ".emProject"
    with open(project_file, "w") as f:
        f.write(ses_project)

    # Create .emSession
    # dummy_session = ['<!DOCTYPE CrossStudio_Session_File>',
    #                          '<session>',
    #                          '</session>']
    # ses_session = '\n'.join(dummy_session)

    # session_file = output_filename + ".emSession"
    # with open(session_file, "w") as f:
    #     f.write(ses_session)

    print("Wrote: " +  project_file)

if __name__ == "__main__":
    main()

