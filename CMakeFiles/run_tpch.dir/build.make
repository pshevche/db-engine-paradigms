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
include CMakeFiles/run_tpch.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/run_tpch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/run_tpch.dir/flags.make

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o: CMakeFiles/run_tpch.dir/flags.make
CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o: src/hyper/codegen/operators/Map.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Map.cpp

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Map.cpp > CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.i

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Map.cpp -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.s

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o: CMakeFiles/run_tpch.dir/flags.make
CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o: src/hyper/codegen/operators/Print.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Print.cpp

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Print.cpp > CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.i

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Print.cpp -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.s

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o: CMakeFiles/run_tpch.dir/flags.make
CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o: src/hyper/codegen/operators/Scan.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Scan.cpp

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Scan.cpp > CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.i

CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/hyper/codegen/operators/Scan.cpp -o CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.s

CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o: CMakeFiles/run_tpch.dir/flags.make
CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o: src/benchmarks/tpch/run.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o -c /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/benchmarks/tpch/run.cpp

CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/benchmarks/tpch/run.cpp > CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.i

CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/src/benchmarks/tpch/run.cpp -o CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.s

# Object files for target run_tpch
run_tpch_OBJECTS = \
"CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o" \
"CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o" \
"CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o" \
"CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o"

# External object files for target run_tpch
run_tpch_EXTERNAL_OBJECTS =

run_tpch: CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Map.cpp.o
run_tpch: CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Print.cpp.o
run_tpch: CMakeFiles/run_tpch.dir/src/hyper/codegen/operators/Scan.cpp.o
run_tpch: CMakeFiles/run_tpch.dir/src/benchmarks/tpch/run.cpp.o
run_tpch: CMakeFiles/run_tpch.dir/build.make
run_tpch: libtpchd.a
run_tpch: libhyperd.a
run_tpch: libvectorwised.a
run_tpch: libcommond.a
run_tpch: 3rdparty/jevents/libjeventsd.a
run_tpch: libbenchmark_configd.a
run_tpch: libvectorwised.a
run_tpch: libcommond.a
run_tpch: libhybridd.a
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_system.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_thread.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_program_options.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_serialization.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_random.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_iostreams.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_regex.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_log.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
run_tpch: /usr/lib/x86_64-linux-gnu/libboost_log_setup.so
run_tpch: /usr/lib/x86_64-linux-gnu/libtbb.so
run_tpch: CMakeFiles/run_tpch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable run_tpch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/run_tpch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/run_tpch.dir/build: run_tpch

.PHONY : CMakeFiles/run_tpch.dir/build

CMakeFiles/run_tpch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run_tpch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run_tpch.dir/clean

CMakeFiles/run_tpch.dir/depend:
	cd /home/gurumurt/Student-Files/hiwi/db-engine-paradigms && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms /home/gurumurt/Student-Files/hiwi/db-engine-paradigms/CMakeFiles/run_tpch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run_tpch.dir/depend

