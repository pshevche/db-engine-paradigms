# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/gurumurt/clion/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/gurumurt/clion/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gurumurt/Student-Files/hiwi/db-engine-paradigms

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gurumurt/Student-Files/hiwi/db-engine-paradigms

# Include any dependencies generated for this target.
include 3rdparty/jevents/CMakeFiles/listevents.dir/depend.make

# Include the progress variables for this target.
include 3rdparty/jevents/CMakeFiles/listevents.dir/progress.make

# Include the compile flags for this target's objects.
include 3rdparty/jevents/CMakeFiles/listevents.dir/flags.make

3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.o: 3rdparty/jevents/CMakeFiles/listevents.dir/flags.make
3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.o: 3rdparty/jevents/listevents.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object 3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/listevents.dir/listevents.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/listevents.c

3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/listevents.dir/listevents.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/listevents.c > CMakeFiles/listevents.dir/listevents.c.i

3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/listevents.dir/listevents.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/listevents.c -o CMakeFiles/listevents.dir/listevents.c.s

# Object files for target listevents
listevents_OBJECTS = \
"CMakeFiles/listevents.dir/listevents.c.o"

# External object files for target listevents
listevents_EXTERNAL_OBJECTS =

3rdparty/jevents/listevents: 3rdparty/jevents/CMakeFiles/listevents.dir/listevents.c.o
3rdparty/jevents/listevents: 3rdparty/jevents/CMakeFiles/listevents.dir/build.make
3rdparty/jevents/listevents: 3rdparty/jevents/libjeventsd.a
3rdparty/jevents/listevents: 3rdparty/jevents/CMakeFiles/listevents.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable listevents"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/listevents.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
3rdparty/jevents/CMakeFiles/listevents.dir/build: 3rdparty/jevents/listevents

.PHONY : 3rdparty/jevents/CMakeFiles/listevents.dir/build

3rdparty/jevents/CMakeFiles/listevents.dir/clean:
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents && $(CMAKE_COMMAND) -P CMakeFiles/listevents.dir/cmake_clean.cmake
.PHONY : 3rdparty/jevents/CMakeFiles/listevents.dir/clean

3rdparty/jevents/CMakeFiles/listevents.dir/depend:
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/CMakeFiles/listevents.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : 3rdparty/jevents/CMakeFiles/listevents.dir/depend

