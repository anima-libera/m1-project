#!/usr/bin/env python3

""" Compiles the project C++ source code.

Usage:
  {this_script} [options]

Options:
  -h   --help       Prints this docstring and halts.
  -l   --launch     Executes the bin if compiled, with what follows as args.
  -d   --debug      Standard debuging build, defines DEBUG, launches with -d.
  -c=X --compiler=X Use the X compiler, where X is g++ or clang (TODO).

Example usage for debug:
  {this_script} -d --compiler=g++ -l
"""

import sys
import os
import re

def print_blue(*args, **kwargs):
	print("\x1b[36m", end = "")
	print(*args, **kwargs)
	print("\x1b[39m", end = "", flush = True)

def print_error(error, *args, **kwargs):
	print("\x1b[1m", end = "")
	print(f"\x1b[31m{error} error:\x1b[39m ", end = "")
	print(*args, **kwargs)
	print("\x1b[22m", end = "", flush = True)

## Options

# Launch option -l
option_launch = False
for i in range(1, len(sys.argv)):
	if sys.argv[i] in ("-l", "--launch"):
		option_launch = True
		launch_index = i
		break
if option_launch:
	options = sys.argv[1:launch_index]
	launch_args = sys.argv[launch_index+1:]
else:
	options = sys.argv[1:]

def cmdline_has_option(expects_value, *option_names):
	global options
	# Find option
	found, value = None, None
	for option in options:
		option_name, *option_value = option.split("=", 1)
		if option_name in option_names:
			if expects_value and len(option_value) == 0:
				print_error("Cmdline",
					f"Argument option \"{option_name}\" expects a value " +
					f"given via the \"{option_name}=value\" syntax.")
				value = None
			elif not expects_value and len(option_value) == 1:
				print_error("Cmdline",
					f"Argument option \"{option_name}\" " +
					"doesn't expect a value.")
			elif expects_value and len(option_value) == 1:
				value = option_value[0]
			else:
				value = True
			found = option
			break
	if found == None:
		return None if expects_value else False
	# Check for duplicates
	options.remove(found)
	for option in options:
		option_name, *_ = option.split("=", 1)
		if option_name in option_names:
			print_error("Cmdline",
				f"Argument option \"{option_name}\" is redundant.")
			options.remove(option)
	return value

option_help = cmdline_has_option(False, "-h", "--help")
option_debug = cmdline_has_option(False, "-d", "--debug")
option_compiler = cmdline_has_option(True, "-c", "--compiler")
if option_compiler == None:
	option_compiler = "g++"
if option_compiler not in ("g++", ):
	print_error("Cmdline", f"The \"{option_compiler}\" compiler " +
		"is not supported.")
	sys.exit(-1)
if option_debug:
	print(f"Using compiler {option_compiler}")

for unknown_option in options:
	print_error("Cmdline", f"Unknown argument option \"{unknown_option}\".")

src_dir_name = "src"
bin_dir_name = "bin"
bin_name = "rennesimpl"

# Help message if -h
if option_help:
	this_script = sys.argv[0]
	python = "" if this_script.startswith("./") else "python3 "
	print(__doc__.strip().format(this_script = python + sys.argv[0]))
	sys.exit(0)

## Compile

# List src files
src_file_names = []
for dir_name, _, file_names in os.walk(src_dir_name):
	for file_name in file_names:
		if file_name.split(".")[-1] == "cpp":
			src_file_names.append(os.path.join(dir_name, file_name))

# Bin directory
if not os.path.exists(bin_dir_name):
	os.makedirs(bin_dir_name)

# Build
build_command_args = [option_compiler]
for src_file_name in src_file_names:
	build_command_args.append(src_file_name)
build_command_args.append("-o")
build_command_args.append(os.path.join(bin_dir_name, bin_name))
build_command_args.append("-I" + src_dir_name)
build_command_args.append("-std=c++17")
build_command_args.append("-Wall")
build_command_args.append("-Wextra")
build_command_args.append("-pedantic")
#build_command_args.append("-Wno-unused-result")
#build_command_args.append("-Wno-unused-function")
#if option_compiler == "gcc":
#	build_command_args.append("-Wno-maybe-uninitialized")
if option_debug:
	build_command_args.append("-DDEBUG")
	build_command_args.append("-Og")
else:
	build_command_args.append("-O3")
	build_command_args.append("-no-pie")
	build_command_args.append("-fno-stack-protector")
	build_command_args.append("-flto")
	build_command_args.append("-s")
#build_command_args.append("-lm")
build_command = " ".join(build_command_args)
print(("DEBUG" if option_debug else "RELEASE") + " BUILD")
print_blue(build_command)
build_exit_status = os.system(build_command)

## Launch

if option_launch and build_exit_status == 0:
	launch_command_args = ["./" + bin_name]
	if option_debug:
		launch_command_args.append("-d")
	for launch_arg in launch_args:
		launch_command_args.append(launch_arg)
	launch_command = " ".join(launch_command_args)
	os.chdir(bin_dir_name)
	print_blue(launch_command)
	launch_exit_status_raw = os.system(launch_command)
	launch_exit_status = launch_exit_status_raw >> 8
	if bin_dir_name != ".":
		os.chdir("..")
	if launch_exit_status != 0:
		print_blue("exit status {}".format(launch_exit_status))