# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build

# Utility rule file for apidoc.

# Include any custom commands dependencies for this target.
include lib/mbedtls/CMakeFiles/apidoc.dir/compiler_depend.make

lib/mbedtls/CMakeFiles/apidoc:
	cd /home/lucasfrazao/pico-sdk/lib/mbedtls/doxygen && doxygen mbedtls.doxyfile

apidoc: lib/mbedtls/CMakeFiles/apidoc
apidoc: lib/mbedtls/CMakeFiles/apidoc.dir/build.make
.PHONY : apidoc

# Rule to build all files generated by this target.
lib/mbedtls/CMakeFiles/apidoc.dir/build: apidoc
.PHONY : lib/mbedtls/CMakeFiles/apidoc.dir/build

lib/mbedtls/CMakeFiles/apidoc.dir/clean:
	cd /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build/lib/mbedtls && $(CMAKE_COMMAND) -P CMakeFiles/apidoc.dir/cmake_clean.cmake
.PHONY : lib/mbedtls/CMakeFiles/apidoc.dir/clean

lib/mbedtls/CMakeFiles/apidoc.dir/depend:
	cd /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-src /home/lucasfrazao/pico-sdk/lib/mbedtls /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build/lib/mbedtls /home/lucasfrazao/Documentos/BitDogLab-C/microphone_dma/build/_deps/picotool-build/lib/mbedtls/CMakeFiles/apidoc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/mbedtls/CMakeFiles/apidoc.dir/depend

