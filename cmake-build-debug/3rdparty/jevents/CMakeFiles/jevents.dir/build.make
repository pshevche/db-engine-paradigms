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
CMAKE_BINARY_DIR = /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug

# Include any dependencies generated for this target.
include 3rdparty/jevents/CMakeFiles/jevents.dir/depend.make

# Include the progress variables for this target.
include 3rdparty/jevents/CMakeFiles/jevents.dir/progress.make

# Include the compile flags for this target's objects.
include 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make

3rdparty/jevents/CMakeFiles/jevents.dir/json.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/json.c.o: ../3rdparty/jevents/json.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/json.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/json.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/json.c

3rdparty/jevents/CMakeFiles/jevents.dir/json.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/json.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/json.c > CMakeFiles/jevents.dir/json.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/json.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/json.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/json.c -o CMakeFiles/jevents.dir/json.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.o: ../3rdparty/jevents/jsmn.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/jsmn.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jsmn.c

3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/jsmn.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jsmn.c > CMakeFiles/jevents.dir/jsmn.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/jsmn.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jsmn.c -o CMakeFiles/jevents.dir/jsmn.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.o: ../3rdparty/jevents/jevents.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/jevents.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jevents.c

3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/jevents.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jevents.c > CMakeFiles/jevents.dir/jevents.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/jevents.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/jevents.c -o CMakeFiles/jevents.dir/jevents.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.o: ../3rdparty/jevents/resolve.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/resolve.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/resolve.c

3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/resolve.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/resolve.c > CMakeFiles/jevents.dir/resolve.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/resolve.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/resolve.c -o CMakeFiles/jevents.dir/resolve.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.o: ../3rdparty/jevents/cache.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/cache.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cache.c

3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/cache.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cache.c > CMakeFiles/jevents.dir/cache.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/cache.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cache.c -o CMakeFiles/jevents.dir/cache.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.o: ../3rdparty/jevents/cpustr.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/cpustr.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cpustr.c

3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/cpustr.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cpustr.c > CMakeFiles/jevents.dir/cpustr.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/cpustr.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/cpustr.c -o CMakeFiles/jevents.dir/cpustr.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.o: ../3rdparty/jevents/rawevent.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/rawevent.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rawevent.c

3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/rawevent.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rawevent.c > CMakeFiles/jevents.dir/rawevent.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/rawevent.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rawevent.c -o CMakeFiles/jevents.dir/rawevent.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.o: ../3rdparty/jevents/perf-iter.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/perf-iter.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf-iter.c

3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/perf-iter.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf-iter.c > CMakeFiles/jevents.dir/perf-iter.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/perf-iter.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf-iter.c -o CMakeFiles/jevents.dir/perf-iter.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.o: ../3rdparty/jevents/interrupts.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/interrupts.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/interrupts.c

3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/interrupts.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/interrupts.c > CMakeFiles/jevents.dir/interrupts.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/interrupts.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/interrupts.c -o CMakeFiles/jevents.dir/interrupts.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.o: ../3rdparty/jevents/rdpmc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/rdpmc.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rdpmc.c

3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/rdpmc.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rdpmc.c > CMakeFiles/jevents.dir/rdpmc.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/rdpmc.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/rdpmc.c -o CMakeFiles/jevents.dir/rdpmc.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.o: ../3rdparty/jevents/measure.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/measure.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/measure.c

3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/measure.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/measure.c > CMakeFiles/jevents.dir/measure.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/measure.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/measure.c -o CMakeFiles/jevents.dir/measure.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.o: ../3rdparty/jevents/perf_event_open.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/perf_event_open.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf_event_open.c

3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/perf_event_open.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf_event_open.c > CMakeFiles/jevents.dir/perf_event_open.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/perf_event_open.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/perf_event_open.c -o CMakeFiles/jevents.dir/perf_event_open.c.s

3rdparty/jevents/CMakeFiles/jevents.dir/session.c.o: 3rdparty/jevents/CMakeFiles/jevents.dir/flags.make
3rdparty/jevents/CMakeFiles/jevents.dir/session.c.o: ../3rdparty/jevents/session.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object 3rdparty/jevents/CMakeFiles/jevents.dir/session.c.o"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/jevents.dir/session.c.o   -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/session.c

3rdparty/jevents/CMakeFiles/jevents.dir/session.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/jevents.dir/session.c.i"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/session.c > CMakeFiles/jevents.dir/session.c.i

3rdparty/jevents/CMakeFiles/jevents.dir/session.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/jevents.dir/session.c.s"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents/session.c -o CMakeFiles/jevents.dir/session.c.s

# Object files for target jevents
jevents_OBJECTS = \
"CMakeFiles/jevents.dir/json.c.o" \
"CMakeFiles/jevents.dir/jsmn.c.o" \
"CMakeFiles/jevents.dir/jevents.c.o" \
"CMakeFiles/jevents.dir/resolve.c.o" \
"CMakeFiles/jevents.dir/cache.c.o" \
"CMakeFiles/jevents.dir/cpustr.c.o" \
"CMakeFiles/jevents.dir/rawevent.c.o" \
"CMakeFiles/jevents.dir/perf-iter.c.o" \
"CMakeFiles/jevents.dir/interrupts.c.o" \
"CMakeFiles/jevents.dir/rdpmc.c.o" \
"CMakeFiles/jevents.dir/measure.c.o" \
"CMakeFiles/jevents.dir/perf_event_open.c.o" \
"CMakeFiles/jevents.dir/session.c.o"

# External object files for target jevents
jevents_EXTERNAL_OBJECTS =

3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/json.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/jsmn.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/jevents.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/resolve.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/cache.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/cpustr.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/rawevent.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/perf-iter.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/interrupts.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/rdpmc.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/measure.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/perf_event_open.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/session.c.o
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/build.make
3rdparty/jevents/libjeventsd.a: 3rdparty/jevents/CMakeFiles/jevents.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking C static library libjeventsd.a"
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && $(CMAKE_COMMAND) -P CMakeFiles/jevents.dir/cmake_clean_target.cmake
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/jevents.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
3rdparty/jevents/CMakeFiles/jevents.dir/build: 3rdparty/jevents/libjeventsd.a

.PHONY : 3rdparty/jevents/CMakeFiles/jevents.dir/build

3rdparty/jevents/CMakeFiles/jevents.dir/clean:
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents && $(CMAKE_COMMAND) -P CMakeFiles/jevents.dir/cmake_clean.cmake
.PHONY : 3rdparty/jevents/CMakeFiles/jevents.dir/clean

3rdparty/jevents/CMakeFiles/jevents.dir/depend:
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/3rdparty/jevents /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/cmake-build-debug/3rdparty/jevents/CMakeFiles/jevents.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : 3rdparty/jevents/CMakeFiles/jevents.dir/depend
