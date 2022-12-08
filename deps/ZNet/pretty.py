#!/usr/bin/env python3
import argparse
import glob
import re
import itertools
import subprocess
from multiprocessing import Pool
from os import path

fmt_tool = "clang-format"
required_fmt_version = 6
extensions_regex = re.compile(r"(.*\.h$|.*\.cpp$|.*\.c$|.*\.hpp$)")


def checkClangFormatVersion():
	cf_version = subprocess.check_output([fmt_tool, "--version"])
	version_regex = re.compile(r"clang-format version ([0-9]+)\.")
	major_version_num = int(version_regex.match(cf_version.decode("utf-8")).group(1))
	return major_version_num >= required_fmt_version


def filesListFromPaths(inputDirs):
	for directory in inputDirs:
		if path.isfile(directory):
			yield directory
			continue
		files_and_dirs = glob.glob(directory+'/'+"**", recursive=True)
		files = filter(path.isfile, files_and_dirs)
		yield from filter(extensions_regex.match, files)


def getFilesFromGit(*command):
	files = subprocess.check_output(command).splitlines()
	files = (file.decode("utf-8") for file in files)
	return filter(extensions_regex.match, files)


def branchDiffFiles():
	diff = getFilesFromGit("git", "diff", "--name-only", "--diff-filter=ACMR", "master")
	untracked = getFilesFromGit("git", "ls-files", "-o", "--exclude-standard")

	return itertools.chain(diff, untracked)


def checkFilesFormatting(files, jobs):
	nonFormattedFilesList = getFilesMustBeFormatted(files, jobs)
	if nonFormattedFilesList:
		print("Must be formatted:")
		print("\n".join(nonFormattedFilesList))
		exit(1)
	exit(0)


def checkFileFormat(file):
	out = subprocess.check_output([fmt_tool, "-output-replacements-xml", file])
	if "<replacement offset=" in str(out):
		return file


def getFilesMustBeFormatted(files, jobs):
	with Pool(processes=jobs) as pool:
		return list(filter(lambda file: file is not None, pool.imap(checkFileFormat, files, chunksize=10)))


def formatFiles(files, jobs):
	nonFormattedFilesList = getFilesMustBeFormatted(files, jobs)
	if nonFormattedFilesList:
		print("This files were formatted:")
		for file in nonFormattedFilesList:
			subprocess.run(["clang-format", "-i", file])
			print(path.relpath(file))
	exit(0)


def main():
	if not checkClangFormatVersion():
		exit("clang-format version not supported, expected >= %d" % required_fmt_version)

	arguments_parser = argparse.ArgumentParser(description="Formatting the source code.")
	arguments_parser.add_argument("--action", help="Desired action.", choices=['check', 'format'], required=True)
	arguments_parser.add_argument("--all", help="Process all source files in the repository.", action='store_true')
	arguments_parser.add_argument("--paths", nargs='+', metavar="", help="""Paths to directories in which you need to 
		format or check the files formatting. Default currently directory.""")
	arguments_parser.add_argument("--exclude", nargs='+', metavar="", help="""Paths to directories in which you need exclude from
		format or check the files formatting.""")
	arguments_parser.add_argument("-j", dest='jobs', type=int, default=1, help="Specifies the number of jobs to run simultaneously.")
	args = arguments_parser.parse_args()

	if args.all:
		files = filesListFromPaths(["."])
	elif args.paths is not None:
		files = filesListFromPaths(args.paths)
	else:
		files = branchDiffFiles()

	if args.exclude is not None:
		files = list(set(files) - set(filesListFromPaths(args.exclude)))

	if args.action == 'check':
		checkFilesFormatting(files, args.jobs)
	else:
		formatFiles(files, args.jobs)


if __name__ == '__main__':
	exit(main())
