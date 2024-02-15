# This is free script released into the public domain.
# GNU make file v20240120 created by Truong Hy.
#
# Builds bare-metal source for the Intel Cyclone V SoC.
# Depending on the options it will output the following application files:
#   - elf (.elf)
#   - binary (.bin)
#   - U-Boot image (.uimg)
#
# For usage, type make help
#
# Linux requirements (tested only under Ubuntu):
#   - Bash command-line utilities
#
# Windows limitations:
#   - Natively supports building the C/C++ sources into elf, bin or uimg
#
# Common requirements:
#   - GCC ARM cross compiler toolchain.  The bin directory added to the search path
#   - GNU make (for Windows use xPack's build tools).  The bin directory added to the search path
#
# This makefile is already complicated, but to keep things a bit more simple:
#   - We assume the required global variables are already set
#   - We assume the required files and paths are relative to the location of this Makefile

# Optional commandline parameters
semi ?= 0
etu ?= 0
bin ?= 0
uimg ?= 0

# These variables are assumed to be set already
ifndef BM_PROGRAM_NAME1
$(error BM_PROGRAM_NAME1 environment variable is not set)
endif
ifndef BM_HOME_PATH
$(error BM_HOME_PATH environment variable is not set)
endif
ifndef BM_OUT_PATH
$(error BM_OUT_PATH environment variable is not set)
endif
ifndef BM_SRC_PATH1
$(error BM_SRC_PATH1 environment variable is not set)
endif

# Convert back-slashes
ifeq ($(OS),Windows_NT)
BM_HOME_PATH := $(subst \,/,$(BM_HOME_PATH))
BM_OUT_PATH := $(subst \,/,$(BM_OUT_PATH))
BM_SRC_PATH1 := $(subst \,/,$(BM_SRC_PATH1))
endif

# ============
# Source files
# ============

# List of source folders and files to exclude from the build
EXCLUDE_SRCS := \
	$(BM_SRC_PATH1)/hwlib/src/hwmgr/soc_a10 \
	$(BM_SRC_PATH1)/hwlib/src/hwmgr/alt_eth_phy_ksz9031.c \
	$(BM_SRC_PATH1)/hwlib/src/hwmgr/alt_ethernet.c \
	$(BM_SRC_PATH1)/hwlib/src/utils/alt_base.S \
	$(BM_SRC_PATH1)/hwlib/src/utils/alt_base.c \
	$(BM_SRC_PATH1)/hwlib/src/utils/alt_p2uart.c \
	$(BM_SRC_PATH1)/hwlib/src/utils/alt_printf.c

# Get and build a list of source file names from the file system with these locations and pattern
SRCS := \
	$(wildcard $(BM_SRC_PATH1)/*.c) \
	$(wildcard $(BM_SRC_PATH1)/startup/source/*.c) \
	$(wildcard $(BM_SRC_PATH1)/CMSIS/Core/Source/*.c) \
	$(wildcard $(BM_SRC_PATH1)/CMSIS/Device/c5soc/source/*.c) \
	$(wildcard $(BM_SRC_PATH1)/hwlib/src/hwmgr/*.c) \
	$(wildcard $(BM_SRC_PATH1)/hwlib/src/hwmgr/soc_cv_av/*.c) \
	$(wildcard $(BM_SRC_PATH1)/hwlib/src/utils/*.c) \
	$(wildcard $(BM_SRC_PATH1)/hwlib/src/utils/*.S) \
	$(wildcard $(BM_SRC_PATH1)/trulib/source/*.c) \
	$(wildcard $(BM_SRC_PATH1)/trulib_config/source/*.c) \
	$(wildcard $(BM_SRC_PATH1)/irqhandler/source/*.c)

# Remove exclude files
SRCS := $(filter-out $(EXCLUDE_SRCS),$(SRCS))

# List of header include search paths
INCS := \
	-I$(BM_SRC_PATH1) \
	-I$(BM_SRC_PATH1)/startup/include \
	-I$(BM_SRC_PATH1)/CMSIS/Core/Include \
	-I$(BM_SRC_PATH1)/CMSIS/Core/Include/a-profile \
	-I$(BM_SRC_PATH1)/CMSIS/Device/c5soc/include \
	-I$(BM_SRC_PATH1)/bsp \
	-I$(BM_SRC_PATH1)/hwlib/include \
	-I$(BM_SRC_PATH1)/hwlib/include/soc_cv_av \
	-I$(BM_SRC_PATH1)/trulib/include \
	-I$(BM_SRC_PATH1)/trulib_config/include \
	-I$(BM_SRC_PATH1)/irqhandler/include

# The linker script to use
ifeq ($(etu),1)
LINKER_SCRIPT := $(BM_SRC_PATH1)/ldscript/tru_c5_ddr_etu.ld
else
LINKER_SCRIPT := $(BM_SRC_PATH1)/ldscript/tru_c5_ddr.ld
endif

# =========================================
# Common linker and compiler build settings
# =========================================

CFLAGS := -mcpu=cortex-a9 -marm -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -std=gnu11
LDFLAGS := -Xlinker --gc-sections --specs=nosys.specs

# Compiler user symbols (defines)
CFLAGS_SYMBOL_HWLIB := -Dsoc_cv_av -DCYCLONEV
CFLAGS_SYMBOL_DEBUG_SEMI := -DSEMIHOSTING
CFLAGS_SYMBOL_ETU := -DTRU_EXIT_TO_UBOOT=1

# ================================
# Optimization and Debugging flags
# ================================

DBG_CFLAGS_OD := -g3 -O0
REL_CFLAGS_OD := -Os

# =====================================
# Create list of compiler flags (Debug)
# =====================================

# Common debug compiler flags
DBG_CFLAGS := $(DBG_CFLAGS_OD) $(CFLAGS) -DDEBUG $(CFLAGS_SYMBOL_HWLIB)
# Conditional debug compiler flags
ifeq ($(semi),1)
DBG_CFLAGS := $(DBG_CFLAGS) $(CFLAGS_SYMBOL_DEBUG_SEMI)
endif
# Conditional debug compiler flags
ifeq ($(etu),1)
DBG_CFLAGS := $(DBG_CFLAGS) $(CFLAGS_SYMBOL_ETU)
endif
# Common debug compiler flags
DBG_CFLAGS := $(DBG_CFLAGS) $(INCS)

# ===================================
# Create list of linker flags (Debug)
# ===================================

# Common debug linker flags
DBG_LDFLAGS := $(DBG_CFLAGS_OD) $(CFLAGS) $(LDFLAGS)
# Conditional debug linker flags
ifeq ($(semi),1)
DBG_LDFLAGS := $(DBG_LDFLAGS) --specs=rdimon.specs -lrdimon
endif
# Conditional debug linker flags
ifeq ($(etu),1)
#DBG_LDFLAGS := $(DBG_LDFLAGS) -nostdlib
endif
# Common debug linker flags
DBG_LDFLAGS := $(DBG_LDFLAGS) -T$(LINKER_SCRIPT)

# =======================================
# Create list of compiler flags (Release)
# =======================================

# Common release compiler flags
REL_CFLAGS := $(REL_CFLAGS_OD) $(CFLAGS) $(CFLAGS_SYMBOL_HWLIB)
# Conditional release compiler flags
ifeq ($(semi),1)
REL_CFLAGS := $(REL_CFLAGS) $(CFLAGS_SYMBOL_DEBUG_SEMI)
endif
# Conditional release compiler flags
ifeq ($(etu),1)
REL_CFLAGS := $(REL_CFLAGS) $(CFLAGS_SYMBOL_ETU)
endif
# Common release compiler flags
REL_CFLAGS := $(REL_CFLAGS) $(INCS)

# =====================================
# Create list of linker flags (Release)
# =====================================

# Common release linker flags
REL_LDFLAGS := $(REL_CFLAGS_OD) $(CFLAGS) $(LDFLAGS)
# Conditional release linker flags
ifeq ($(semi),1)
REL_LDFLAGS := $(REL_LDFLAGS) --specs=rdimon.specs -lrdimon
endif
# Conditional release linker flags
ifeq ($(etu),1)
#REL_LDFLAGS := $(REL_LDFLAGS) -nostdlib
endif
# Common release linker flags
REL_LDFLAGS := $(REL_LDFLAGS) -T$(LINKER_SCRIPT)

# ====================
# App settings (Debug)
# ====================

DBG_PATH := $(BM_OUT_PATH)/Debug
DBG_ELF := $(DBG_PATH)/$(BM_PROGRAM_NAME1).elf
DBG_CFLAGS_FILE := $(DBG_PATH)/$(BM_PROGRAM_NAME1).cflags.txt
DBG_ELF_LOAD_FILE := $(DBG_PATH)/$(BM_PROGRAM_NAME1).load.txt
DBG_ELF_ENTRY_FILE := $(DBG_PATH)/$(BM_PROGRAM_NAME1).entry.txt
DBG_BIN := $(DBG_PATH)/$(BM_PROGRAM_NAME1).bin
DBG_UIMG := $(DBG_PATH)/$(BM_PROGRAM_NAME1).uimg
DBG_OBJS := $(patsubst %.c,$(DBG_PATH)/%.o,$(SRCS))

# ======================
# App settings (Release)
# ======================

REL_PATH := $(BM_OUT_PATH)/Release
REL_ELF := $(REL_PATH)/$(BM_PROGRAM_NAME1).elf
REL_CFLAGS_FILE := $(REL_PATH)/$(BM_PROGRAM_NAME1).cflags.txt
REL_ELF_LOAD_FILE := $(REL_PATH)/$(BM_PROGRAM_NAME1).load.txt
REL_ELF_ENTRY_FILE := $(REL_PATH)/$(BM_PROGRAM_NAME1).entry.txt
REL_BIN := $(REL_PATH)/$(BM_PROGRAM_NAME1).bin
REL_UIMG := $(REL_PATH)/$(BM_PROGRAM_NAME1).uimg
REL_OBJS := $(patsubst %.c,$(REL_PATH)/%.o,$(SRCS))

# ============================
# Read elf load addr from file
# ============================

# File exists?
ifneq (,$(wildcard $(DBG_ELF_LOAD_FILE)))
DBG_ELF_LOAD_TEXT := $(file <$(DBG_ELF_LOAD_FILE))
endif
# File exists?
ifneq (,$(wildcard $(REL_ELF_LOAD_FILE)))
REL_ELF_LOAD_TEXT := $(file <$(REL_ELF_LOAD_FILE))
endif

# =============================
# Read elf entry addr from file
# =============================

# File exists?
ifneq (,$(wildcard $(DBG_ELF_ENTRY_FILE)))
DBG_ELF_ENTRY_TEXT := $(file <$(DBG_ELF_ENTRY_FILE))
endif
# File exists?
ifneq (,$(wildcard $(REL_ELF_ENTRY_FILE)))
REL_ELF_ENTRY_TEXT := $(file <$(REL_ELF_ENTRY_FILE))
endif

# ===========================
# Miscellaneous support tools
# ===========================

CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)g++
NM := $(CROSS_COMPILE)nm
OC := $(CROSS_COMPILE)objcopy
OD := $(CROSS_COMPILE)objdump
RE := $(CROSS_COMPILE)readelf
SZ := $(CROSS_COMPILE)size
MK := mkimage

# Represents an empty white space - we need it for extracting the elf entry address from readelf output
SPACE := $() $()

# ===========
# Build rules
# ===========

# Options
.PHONY: all help release debug clean cleantemp

# Default build
all: release

# Dummy force always rule
FORCE:
	

help:
	@echo "Builds the bare-metal C program"
	@echo "Usage:"
	@echo "  make [targets] [options]"
	@echo ""
	@echo "Targets:"
	@echo "  release       Build elf Release (default)"
	@echo "  debug         Build elf Debug"
	@echo "  clean         Delete all built files"
	@echo "  cleantemp     Clean except target files"
	@echo "Options to use with target:"
	@echo "  semi=1        Use Semihosting"
	@echo "  etu=1         Elf exit to U-Boot"
	@echo "  bin=1         Outputs binary from the elf"
	@echo "  uimg=1        Outputs U-Boot image from the binary"

# ===========
# Clean rules
# ===========

# Clean sublevel 1 folder
clean_1:
	@if [ -d "$(DBG_PATH)" ]; then echo rm -rf $(DBG_PATH); rm -rf $(DBG_PATH); fi
	@if [ -d "$(REL_PATH)" ]; then echo rm -rf $(REL_PATH); rm -rf $(REL_PATH); fi

# Clean root folder
clean: clean_1
	@if [ -d "$(BM_OUT_PATH)" ] && [ -z "$$(ls -A $(BM_OUT_PATH))" ]; then echo rm -df $(BM_OUT_PATH); rm -df $(BM_OUT_PATH); fi

# ===============================================================
# Clean temporary files rules (does not remove user target files)
# ===============================================================

# Clean sublevel 1 folder
cleantemp_1:
	@if [ -d "$(DBG_PATH)" ]; then \
		echo rm -rf $(DBG_PATH)/*.map; rm -rf $(DBG_PATH)/*.map; \
		echo rm -rf $(DBG_PATH)/*.objdump; rm -rf $(DBG_PATH)/*.objdump; \
	fi
	@if [ -d "$(REL_PATH)" ]; then \
		echo rm -rf $(REL_PATH)/*.map; rm -rf $(REL_PATH)/*.map; \
		echo rm -rf $(REL_PATH)/*.objdump; rm -rf $(REL_PATH)/*.objdump; \
	fi

# Clean root folder
cleantemp: cleantemp_1
	@if [ -d "$(BM_OUT_PATH)" ]; then \
		echo rm -f $(DBG_OBJS) $(DBG_CFLAGS_FILE) $(DBG_ELF_LOAD_FILE) $(DBG_ELF_ENTRY_FILE); rm -f $(DBG_OBJS) $(DBG_CFLAGS_FILE) $(DBG_ELF_LOAD_FILE) $(DBG_ELF_ENTRY_FILE); \
		echo rm -f $(REL_OBJS) $(REL_CFLAGS_FILE) $(REL_ELF_LOAD_FILE) $(REL_ELF_ENTRY_FILE); rm -f $(REL_OBJS) $(REL_CFLAGS_FILE) $(REL_ELF_LOAD_FILE) $(REL_ELF_ENTRY_FILE); \
	fi

# ===========
# Build rules
# ===========

# =================
# Top level targets
# =================

debug: $(DBG_ELF) $(DBG_CFLAGS_FILE) $(DBG_ELF_LOAD_FILE) $(DBG_ELF_ENTRY_FILE)

release: $(REL_ELF) $(REL_CFLAGS_FILE) $(REL_ELF_LOAD_FILE) $(REL_ELF_ENTRY_FILE)

ifeq ($(bin),1)
# Add additional target rule
debug: $(DBG_BIN)
release: $(REL_BIN)
endif

ifeq ($(uimg),1)
# Add additional target rule
debug: $(DBG_UIMG)
release: $(REL_UIMG)
endif

# =================================================
# Create prerequisite list for source files (Debug)
# =================================================

DBG_SRCS_PRE := %.c

# We want to FORCE build elf if the etu=1 wasn't used in previous compile
# If the file exists
ifneq (,$(wildcard $(DBG_CFLAGS_FILE)))
DBG_CFLAGS_FILE_TEXT := $(strip $(file <$(DBG_CFLAGS_FILE)))
# etu=1?
ifeq ($(etu),1)
# The etu define was not used in the previous compile?
ifeq (,$(filter $(CFLAGS_SYMBOL_ETU),$(DBG_CFLAGS_FILE_TEXT)))
DBG_SRCS_PRE := $(DBG_SRCS_PRE) FORCE
endif
else
# The etu define was used in the previous compile?
ifneq (,$(filter $(CFLAGS_SYMBOL_ETU),$(DBG_CFLAGS_FILE_TEXT)))
DBG_SRCS_PRE := $(DBG_SRCS_PRE) FORCE
endif
endif
endif

# ===============================
# Compile and linke rules (Debug)
# ===============================

# Compile source files
$(DBG_PATH)/%.o: $(DBG_SRCS_PRE)
	@mkdir -p $(@D)
	$(CC) -c $(DBG_CFLAGS) -o $@ $<
	
# Link object files
$(DBG_ELF): $(DBG_OBJS)
	$(LD) $(DBG_LDFLAGS) $(DBG_OBJS) -o $@
	$(NM) $@ > $@.map
	$(OD) -d $@ > $@.objdump

# ===================================================
# Create prerequisite list for source files (Release)
# ===================================================

REL_SRCS_PRE := %.c

# We want to FORCE build elf if the etu=1 wasn't used in previous compile
# If the file exists
ifneq (,$(wildcard $(REL_CFLAGS_FILE)))
REL_CFLAGS_FILE_TEXT := $(strip $(file <$(REL_CFLAGS_FILE)))
# etu=1?
ifeq ($(etu),1)
# The etu define was not used in the previous compile?
ifeq (,$(filter $(CFLAGS_SYMBOL_ETU),$(REL_CFLAGS_FILE_TEXT)))
REL_SRCS_PRE := $(REL_SRCS_PRE) FORCE
endif
else
# The etu define was used in the previous compile?
ifneq (,$(filter $(CFLAGS_SYMBOL_ETU),$(REL_CFLAGS_FILE_TEXT)))
REL_SRCS_PRE := $(REL_SRCS_PRE) FORCE
endif
endif
endif

# =================================
# Compile and linke rules (Release)
# =================================

# Compile source files
$(REL_PATH)/%.o: $(REL_SRCS_PRE)
	@mkdir -p $(@D)
	$(CC) -c $(REL_CFLAGS) -o $@ $<

# Link object files
$(REL_ELF): $(REL_OBJS)
	$(LD) $(REL_LDFLAGS) $(REL_OBJS) -o $@
	$(NM) $@ > $@.map
	$(OD) -d $@ > $@.objdump

# ==============================
# Write compiler flag file rules
# ==============================

$(DBG_CFLAGS_FILE): $(DBG_ELF)
	@echo $(DBG_CFLAGS) > $(DBG_CFLAGS_FILE)
	@$(SZ) --format=berkeley $(DBG_ELF)

$(REL_CFLAGS_FILE): $(REL_ELF)
	@echo $(REL_CFLAGS) > $(REL_CFLAGS_FILE)
	@$(SZ) --format=berkeley $(REL_ELF)

# ==============================
# Extract elf load address rules
# ==============================

# Extract elf load address and write to file
$(DBG_ELF_LOAD_FILE): $(DBG_ELF)
	$(eval DBG_ELF_INFO := $(shell $(RE) -l $(DBG_ELF)))
	$(eval DBG_ELF_LOAD_TEXT := $(filter LOAD 0x%,$(DBG_ELF_INFO)))
	$(eval DBG_ELF_LOAD_TEXT := $(subst LOAD$(SPACE),LOAD_,$(DBG_ELF_LOAD_TEXT)))
	$(eval DBG_ELF_LOAD_TEXT := $(patsubst LOAD_%,LOAD,$(DBG_ELF_LOAD_TEXT)))
	$(eval DBG_ELF_LOAD_TEXT := $(subst LOAD$(SPACE),LOAD_,$(DBG_ELF_LOAD_TEXT)))
	$(eval DBG_ELF_LOAD_TEXT := $(filter LOAD_%,$(DBG_ELF_LOAD_TEXT)))
	$(eval DBG_ELF_LOAD_TEXT := $(word 1,$(DBG_ELF_LOAD_TEXT)))
	$(eval DBG_ELF_LOAD_TEXT := $(subst LOAD_,,$(DBG_ELF_LOAD_TEXT)))
	$(info ELF-load-addr: $(DBG_ELF_LOAD_TEXT))
	@echo $(DBG_ELF_LOAD_TEXT) > $(DBG_ELF_LOAD_FILE)

# Extract elf entry address and write to file
$(REL_ELF_LOAD_FILE): $(REL_ELF)
	$(eval REL_ELF_INFO := $(shell $(RE) -l $(REL_ELF)))
	$(eval REL_ELF_LOAD_TEXT := $(filter LOAD 0x%,$(REL_ELF_INFO)))
	$(eval REL_ELF_LOAD_TEXT := $(subst LOAD$(SPACE),LOAD_,$(REL_ELF_LOAD_TEXT)))
	$(eval REL_ELF_LOAD_TEXT := $(patsubst LOAD_%,LOAD,$(REL_ELF_LOAD_TEXT)))
	$(eval REL_ELF_LOAD_TEXT := $(subst LOAD$(SPACE),LOAD_,$(REL_ELF_LOAD_TEXT)))
	$(eval REL_ELF_LOAD_TEXT := $(filter LOAD_%,$(REL_ELF_LOAD_TEXT)))
	$(eval REL_ELF_LOAD_TEXT := $(word 1,$(REL_ELF_LOAD_TEXT)))
	$(eval REL_ELF_LOAD_TEXT := $(subst LOAD_,,$(REL_ELF_LOAD_TEXT)))
	$(info ELF-load-addr: $(REL_ELF_LOAD_TEXT))
	@echo $(REL_ELF_LOAD_TEXT) > $(REL_ELF_LOAD_FILE)

# ===============================
# Extract elf entry address rules
# ===============================

# Extract elf entry address and write to file
$(DBG_ELF_ENTRY_FILE): $(DBG_ELF)
	$(eval DBG_ELF_INFO := $(shell $(RE) -l $(DBG_ELF)))
	$(eval DBG_ELF_ENTRY_TEXT := $(filter Entry 0x%,$(DBG_ELF_INFO)))
	$(eval DBG_ELF_ENTRY_TEXT := $(subst Entry$(SPACE),Entry_,$(DBG_ELF_ENTRY_TEXT)))
	$(eval DBG_ELF_ENTRY_TEXT := $(filter Entry_%,$(DBG_ELF_ENTRY_TEXT)))
	$(eval DBG_ELF_ENTRY_TEXT := $(subst Entry_,,$(DBG_ELF_ENTRY_TEXT)))
	$(info ELF-entry-point: $(DBG_ELF_ENTRY_TEXT))
	@echo $(DBG_ELF_ENTRY_TEXT) > $(DBG_ELF_ENTRY_FILE)

# Extract elf entry address and write to file
$(REL_ELF_ENTRY_FILE): $(REL_ELF)
	$(eval REL_ELF_INFO := $(shell $(RE) -l $(REL_ELF)))
	$(eval REL_ELF_ENTRY_TEXT := $(filter Entry 0x%,$(REL_ELF_INFO)))
	$(eval REL_ELF_ENTRY_TEXT := $(subst Entry$(SPACE),Entry_,$(REL_ELF_ENTRY_TEXT)))
	$(eval REL_ELF_ENTRY_TEXT := $(filter Entry_%,$(REL_ELF_ENTRY_TEXT)))
	$(eval REL_ELF_ENTRY_TEXT := $(subst Entry_,,$(REL_ELF_ENTRY_TEXT)))
	$(info ELF-entry-point: $(REL_ELF_ENTRY_TEXT))
	@echo $(REL_ELF_ENTRY_TEXT) > $(REL_ELF_ENTRY_FILE)

# ========================
# ELF to binary file rules
# ========================
	
# Convert elf to binary
$(DBG_BIN): $(DBG_ELF)
	$(OC) -O binary $(DBG_ELF) $@

# Convert elf to binary
$(REL_BIN): $(REL_ELF)
	$(OC) -O binary $(REL_ELF) $@

# =====================================
# ELF binary to U-Boot image file rules
# =====================================

# Convert binary to U-Boot bootable image
$(DBG_UIMG): $(DBG_BIN) $(DBG_ELF_LOAD_FILE) $(DBG_ELF_ENTRY_FILE)
	$(MK) -A arm -O u-boot -T standalone -C none -a $(DBG_ELF_LOAD_TEXT) -e $(DBG_ELF_ENTRY_TEXT) -n $(BM_PROGRAM_NAME1) -d $(DBG_BIN) $@

# Convert binary to U-Boot bootable image
$(REL_UIMG): $(REL_BIN) $(REL_ELF_LOAD_FILE) $(REL_ELF_ENTRY_FILE)
	$(MK) -A arm -O u-boot -T standalone -C none -a $(REL_ELF_LOAD_TEXT) -e $(REL_ELF_ENTRY_TEXT) -n $(BM_PROGRAM_NAME1) -d $(REL_BIN) $@
