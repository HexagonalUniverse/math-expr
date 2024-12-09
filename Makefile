# Makefile at <REP-NAME/>
# =======================
# Template from @HexagonalUniverse.
# Last update: TODAY. 

# By default attempts build and running the main executable.
# The target systems are: Windows or Linux, x86_64.

# ~	(brief overall specific description)
.PHONY: build_and_run build rebuild run clean full-clean test



### FLAGS ###	(makefile input)

# Indicates whether to launch the post-processing executable after the build.
POSTPROCESS	:= 1


### Parameters ###	(to be set)

MAIN_EXENAME			:= main
POSTPROCESS_EXENAME		:= nothing
LIBNAME					:= hexagonal


# Project's Directories
# ---------------------
# Relevant paths for the project.

# Organization:
#	. Source files at <src/> are isolatedly compiled and then joined into a static or dynamic library.
#	. The project's library are then linked into the compiled source-files at <drv/>: the driver source files
#	of the project.
#	. All by-product objects, libraries and executables from the compilation will be at <bin/>.
#	. <cache/>

dir_src			:= src/
dir_hdr			:= $(dir_src)/include/

dir_drv			:= drv/
dir_bin			:= bin/
dir_tests		:= tests/

dir_lib			:= $(dir_bin)
dir_obj			:= $(dir_bin)obj/
dir_exe			:= $(dir_bin)exe/

dir_data		:= data/
dir_cache		:= $(dir_data)cache/


# Files
# -----
# Source-files and executables.
# Driver source files will have <drv/> as their path.
# 

# Single directory source-files case.
# source_files			:= $(wildcard $(dir_src)*.c)
# header_files			:= $(wildcard $(dir_hdr)*.h)

# Directory hierarchy source-files. Case into which the headers will all be along the .c's.
# Recognizes up to one level of depth by default.
source_files			:= $(wildcard $(dir_src)*.c) $(wildcard $(dir_src)*/*.c)
header_files			:= $(wildcard $(dir_src)*.h) $(wildcard $(dir_src)*/*.h)

driver_sources			:= $(wildcard $(dir_drv)*.c)

# Each .o file associated with its .c. Preserves the subpath of it.
object_files			:= $(foreach object,$(subst .c,.o,$(subst $(dir_src),,$(source_files))),$(dir_obj)$(object))

# All .exe files, from the driver sources.
target_executables		:= $(foreach executables,$(subst .c,.exe,$(subst $(dir_drv),,$(driver_sources))),$(dir_exe)$(executables))
main_exe				:= $(dir_exe)$(MAIN_EXENAME).exe

# An executable that will be executed right after the building if finished.
# Intended for cases in which data-preprocessing for the application is somewhat required.
post_build_exe			:= $(dir_exe)$(POSTPROCESS_EXENAME).exe

# Just so the cache directory can properly be created at the building process.
__dummy_cache_file		:= $(dir_cache)__nothing.nothing


# Static library
# --------------

target_slibname			:= $(LIBNAME).lib
target_slib				:= $(dir_lib)$(target_slibname)


# Dynamic library
# ---------------

target_dlibname			:= $(LIBNAME).dll
target_dlib				:= $(dir_exe)$(target_dlibname)
# Contrarily from the static library, the dynamic library is
# assembled at the executables path for coesion.


# Compilation
# -----------

C_COMPILER					:= gcc
COMPILATION_WARNINGS		:= -Wall -Wextra -pedantic
COMPILATION_OPTIMIZATION	:= -O3
C_STANDARD_FLAG				:= -std=c23

# Common compilation flags.
COMPILATION_FLAGS			:= $(C_STANDARD_FLAG) $(COMPILATION_WARNINGS) $(COMPILATION_OPTIMIZATION)

# Static linking
# EXE_LINK					:= -L"$(dir_lib)" -l:"$(target_slibname)"
# Dynamic linking
EXE_LINK					:= -L"$(dir_exe)" -l:"$(target_dlibname)"

# The linking the dynamic-library compilation will need.
D_LINK						:= 


# Compilation flags for the drivers, and for the objects, respectively.
EXE_CMPF					:= $(COMPILATION_FLAGS) -I"$(dir_hdr)" $(EXE_LINK)
OBJ_CMPF					:= $(COMPILATION_FLAGS)



# Building		make build_and_run/build/rebuild
# ==============================================

build_and_run: build run


define BUILD_COMPLETE_MSG
	@echo System built.
endef

# Completing the path...
ifeq ($(OS),Windows_NT)
define MAKEDIR
	if not exist "$(@D)" mkdir "$(@D)"
endef
else
define MAKEDIR
	mkdir -p "$(@D)"
endef
endif


build: __build
	$(BUILD_COMPLETE_MSG)


# Takes care of the executables, building necessary paths, and post-processing.
__build: $(target_executables) __build_paths __post_process


__build_paths: __build_cache
__build_cache: $(__dummy_cache_file)
$(__dummy_cache_file):
	@$(MAKEDIR)


# Compiling
# ---------

define EXE_BUILT_MSG 
	@echo Assembling $<. 
endef

define SLIB_BUILT_MSG
	@echo Library built.
endef

define DLIB_BUILT_MSG
	@echo Compiled $@
endef

$(dir_exe)%.exe: $(dir_drv)%.c $(target_slib) $(target_dlib)
	@$(MAKEDIR)
	@$(C_COMPILER) -o $@ $< $(EXE_CMPF)
	$(EXE_BUILT_MSG)

# Joining in the static library.
$(target_slib): $(object_files)
	@$(MAKEDIR)
	@ar cr $@ $^
	$(SLIB_BUILT_MSG)

# Joining the dynamic library.
$(target_dlib): $(object_files)
	@$(MAKEDIR)
	$(C_COMPILER) -shared -o $@ $^ $(D_LINK) $(COMPILATION_FLAGS)

# Assembling each object file.
$(dir_obj)%.o: $(dir_src)%.c $(header_files)
	@$(MAKEDIR)
	@$(C_COMPILER) -c $(OBJ_CMPF) $< -o $@
	$(DLIB_BUILT_MSG).


# Post-processing
# ---------------

DATAFILES_TO_PREPARE	:= $(dir_data)nothing.txt


ifeq ($(POSTPROCESS),1)	
define __EXECUTE_POSTPROCESSING
	./$(post_build_exe)
	@echo post-processed executed.
endef
else
define __EXECUTE_POSTPROCESSING
	
endef
endif

__post_process: $(post_build_exe) $(DATAFILES_TO_PREPARE)
	
$(DATAFILES_TO_PREPARE):
	@$(__EXECUTE_POSTPROCESSING)


# Re-building
# -----------

define REBUILT_MSG
	@echo "System rebuilt"
endef

rebuild: clean __build
	$(REBUILT_MSG)



# Cleaning		make clean/full-clean
# ===================================

# In case of windows, "rmdir /s /q" deletes
# all subdirectories and files of it (and finally the original directory)
# without asking for confirmation. The equivallent in unix systems
# will be -r and -f, respectively.

# Deletes automatically the path specified by the 
# firstmost dependency.
ifeq ($(OS),Windows_NT)
    CMD_REMOVE = if exist "$<" rmdir /s /q "$<"
else
    CMD_REMOVE = rm -rf "$<"
endif

# The path up to where the makefile is on the system.
# It is for guaranteeing the deletion will occur correctly.
ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))


define CLEAN_MSG
	@echo Cleaned.
endef

define FULL_CLEAN_MSG
	@echo Full cleaned.
endef


full-clean: __clean_obj __clean_cache __clean_bin
	$(FULL_CLEAN_MSG)

# By the way CMD_REMOVE works, every cleaning part, 
# is separately resolved.
clean: __clean_obj
	@(CLEAN_MSG)


# Byproduct of the library construction object's file.
__clean_obj: $(dir_obj)
	@$(CMD_REMOVE)

__clean_cache: $(dir_cache)
	@$(CMD_REMOVE)

__clean_bin: $(dir_bin)
	@$(CMD_REMOVE)


# * Each dependency of its respective cleaning part need
# not be analyzed. Their purpose is to become sort of a
# "parameter" for CMD_REMOVE, so they're hereby ignored...
$(dir_obj):
$(dir_cache):
$(dir_bin):


# Running
# =======

build_and_run: __build run

run:
	./$(main_exe)



### Testing ###

dir_test_src				:= $(dir_tests)src/
dir_test_cache				:= $(dir_cache)
dir_test_exe				:= $(dir_exe)

test_source_files			:= $(wildcard $(dir_test_src)*.c)
test_exe_files				:= $(foreach executables,$(subst .c,.exe,$(subst $(dir_test_src),,$(test_source_files))),$(dir_test_exe)$(executables))

# test_dlib					:= $(dir_test_exe)$(target_dlibname)

TST_LINK					:= -L"$(dir_test_exe)" -l:"$(target_dlibname)"
TST_CMPF					:= $(COMPILATION_FLAGS) -I"$(dir_hdr)" $(TST_LINK)


test: __build_tests __run_tests


# Building the tests requires that the default build is already done.
__build_tests: __build_tests_exe
__build_tests_exe: __build $(target_dlib) $(test_exe_files)

#$(test_dlib): $(object_files)
#	$(MAKEDIR)
#	$(C_COMPILER) -shared -o $@ $^ $(D_LINK) $(COMPILATION_FLAGS)

$(dir_test_exe)%.exe: $(dir_test_src)%.c $(target_slib) $(target_dliba)
	$(MAKEDIR)
	$(C_COMPILER) -o $@ $< $(TST_CMPF)
	$(EXE_BUILT_MSG)

__run_tests:
	cmd /c "tests\\run-tests.bat"


# Other make flags
# testing
.PHONY: __build_tests __build_tests_exe __build_test_dll

# building
.PHONY: __build __build_paths __build_cache __post_process

# cleaning
.PHONY: __clean_obj __clean_bin __clean_cache


