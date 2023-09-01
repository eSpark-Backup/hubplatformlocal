#!/usr/bin/env python3

import argparse, glob, re, subprocess, datetime, os
from shutil import which
from os import path
from enum import Enum

gitIsExist = which("git") is not None

versionFileTemplate = '''#pragma once

// This file was generated automatically!

#define {module_tag}_REVISION_TAG "{revision_tag}"
#define {module_tag}_REVISION_ID "{revision_id}"
#define {module_tag}_GIT_ID "{git_id}"
#define {module_tag}_DATE "{git_date}"
'''


def executeGit(params, execDir):
	return subprocess.check_output("git {0}".format(params), shell=True, cwd=execDir).decode('utf-8').rstrip('\n')


def writeVersionInfo(moduleTag, execDir, saveFilename):
	absExecDir = os.path.abspath(execDir)
	print("Check directory: {0}".format(absExecDir))
	
	try:
		gitId = executeGit('rev-parse HEAD', absExecDir)
	except:
		gitId = "unknow"
	
	try:
		firmware = executeGit('describe --tags', absExecDir)
		splitted = firmware.split('/')
		rev_tag = splitted[0]
		rev_id = splitted[1]
	except:
		rev_tag = "unknow"
		rev_id = "unknow"
	
	try:
		date = executeGit('show -s --format=%ci', absExecDir)
	except:
		date = "unknow"
	
	print("Save to: {0}".format(saveFilename))
	versionFileHandle = open(saveFilename, "w")
	versionFileHandle.writelines(versionFileTemplate.format(module_tag=moduleTag, revision_tag=rev_tag, revision_id=rev_id, git_id=gitId, git_date=date))
	versionFileHandle.close()


def main():
	print("\nlibinfo.py begin =====================================================")
	if not gitIsExist:
		print( "Warning: git tool is not exist!" )
		return
	
	arguments_parser = argparse.ArgumentParser(description = "Generating version information.")
	arguments_parser.add_argument("--module_tag", help="Module tag", type=str, required = True)
	arguments_parser.add_argument("--exec_dir", help="Exec dir", type=str, required = True)
	arguments_parser.add_argument("--save_filename", help="Save filename", type=str, required = True)
	arguments_parser.add_argument("--skip_if_exists", help="Skip generation if filename already exists", type=str, required=True)
	args = arguments_parser.parse_args()

	if os.path.exists(args.save_filename) and args.skip_if_exists == 'ON':
		print("Skipping generation due to --skip_if_exists option.")
		return

	print("Generate info for module: {0}".format(args.module_tag))
	writeVersionInfo(args.module_tag, args.exec_dir, args.save_filename)
	
	print("libinfo.py end =======================================================\n")

if __name__ == '__main__':
	exit(main())