# Makefile - makefile for target/3rdparty/windriver/wrsample
#
# modification history 
# --------------------
# 01c,13oct08,h_k  moved wrsampleApp.o to apps/Makefile.
#                  fixed EXTRA_INCLUDE path.
# 01b,02sep08,h_k  changed CC_OPTIM_NORMAL to enable optimize flags.
# 01a,21aug07,tor  created 
#
# DESCRIPTION
# This file contains the makefile rules for building the
# wrsample driver, contained in the 3rd-party driver
# library provided by Wind River
#

#
#  Do not modify TGT_DIR.  This is constant across all
#  drivers and modules.
#
TGT_DIR=$(WIND_BASE)/target

#
#  The DOC_FILES macro is used to create per-function
#  documentation for the user-accessible functions available
#  in the source files listed.  By convention, we expect that
#  each 3rd party driver will be in a separate directory, and
#  only one file will be listed here.  The documentation is
#  created when "make release" is run.
#
DOC_FILES=trrel48_ios.c rsrel48_ios.c

#
#  The LIB_BASE_NAME specifies the library in which the
#  precompiled object modules will be put.  The name of
#  the library should be the name of the company that
#  supports the driver.
#
LIB_BASE_NAME=otd37

#
#  The EXTRA_INCLUDE line is used to indicate additional
#  directories to search for include files when compiling
#  the source files in this directory.  The value here is
#  redundant, but listed to show what application code would
#  need if it uses any header file from this directory.
#
EXTRA_INCLUDE += -I$(TGT_DIR)/3rdparty/otd37/trrel48

#  
#  The OBJS_COMMON macro lists the object modules that
#  are created in this directory, independent of architecture.
#  If the driver in this directory is valid only for a
#  single architecture, then you may specify a CPU-specific
#  macro instead, e.g. OBJS_PPC32
#  
OBJS_COMMON     = trrel48_ios.o rsrel48_ios.o

#  
#  Do not modify the OBJS macro.
#
OBJS = $(OBJS_COMMON) $(OBJS_$(CPU))

FLAGS = g0
#  
#  Do not modify the following include.
#  
include $(TGT_DIR)/h/make/rules.library

#
#  Do not modify the following macro or include.
#
CC_OPTIM=$(CC_OPTIM_NORMAL)
include  $(TGT_DIR)/h/make/defs.drivers
