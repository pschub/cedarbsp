#  Project Name
PROJECT=teensybsp

#  Type of CPU/MCU in target hardware
CPU = cortex-m4
MCU = MK20DX256
TARGETTYPE = arm-none-eabi

OPTIMIZATION = 0
DEBUG = -g

OBJDIR = obj
OUTDIR = lib

#  teensy_loader_cli wants MCU in lowercase.
LOWER_MCU  = $(shell echo $(MCU) | tr A-Z a-z)

#  Grab all c files from current dir
C_FILES = $(wildcard ./common/*.c)
C_FILES += $(wildcard ./drivers/*.c)

#  Generate .o names from c files
OBJ_FILES = $(addprefix $(OBJDIR)/,$(notdir $(C_FILES:.c=.o)))

#  Final hex product of makefile
TARGET= $(OUTDIR)/$(PROJECT).a

#  Location of toolchain
TOOLPATH = $(HOME)/tools/arduino-1.8.2/hardware/tools/arm

#  Location of BSP
BSP_PATH = $(HOME)/progging/arm/bsp

#  List of directories to be searched for include files during compilation
INCDIRS  = -I$(TOOLPATH)/$(TARGETTYPE)/include
INCDIRS += -I./include
INCDIRS += -I./common

# Name and path to the linker script
LSCRIPT = $(BSP_PATH)/common/mk20dx256.ld

#  List the directories to be searched for libraries during linking.
#  Optionally, list archives (libxxx.a) to be included during linking.
LIBDIRS  = -L"$(TOOLPATH)/$(TARGETTYPE)/lib"
LIBS =

#  Compiler options
GCFLAGS = -Wall -fno-common -mcpu=$(CPU) -mthumb -O$(OPTIMIZATION) $(DEBUG)
GCFLAGS += $(INCDIRS)
GCFLAGS += -fdata-sections -ffunction-sections
GCFLAGS += -D__$(MCU)__ -DF_CPU=72000000

# You can uncomment the following line to create an assembly output
# listing of your C files.  If you do this, however, the sed script
# in the compilation below won't work properly.
#GCFLAGS += -c -g -Wa,-a,-ad

#  Assembler options
ASFLAGS = -mcpu=$(CPU)

#  Linker options
LDFLAGS = -mcpu=$(CPU) -mthumb
LDFLAGS += -Wl,-Map=$(OUTDIR)/$(PROJECT).map,-T$(LSCRIPT),--cref
#LDFLAGS += -Wl,-Map=$(OUTDIR)/$(PROJECT).map,-T$(LSCRIPT),--cref,--gc-sections
LDFLAGS += $(LIBDIRS)
LDFLAGS += $(LIBS)


#  Tools paths
#
#  Define an explicit path to the GNU tools used by make.
#  If you are ABSOLUTELY sure that your PATH variable is
#  set properly, you can remove the BINDIR variable.
#
BINDIR = $(TOOLPATH)/bin

CC = $(BINDIR)/arm-none-eabi-gcc
AS = $(BINDIR)/arm-none-eabi-as
AR = $(BINDIR)/arm-none-eabi-gcc-ar
LD = $(BINDIR)/arm-none-eabi-ld
OBJCOPY = $(BINDIR)/arm-none-eabi-objcopy
#SIZE = $(BINDIR)/arm-none-eabi-size
SIZE = $(HOME)/bin/elf_use.py $(LOWER_MCU)
OBJDUMP = $(BINDIR)/arm-none-eabi-objdump
PROGRAM = teensy_loader_cli

#########################################################################

all : $(TARGET)

# Rule to create bin. Not needed for anything?
#$(PROJECT).bin: $(PROJECT).elf
#	$(OBJCOPY) -O binary -j .text -j .data $(PROJECT).elf $(PROJECT).bin

$(TARGET) : $(OBJ_FILES)
	@echo "Making archive of object files..."
	@mkdir -p $(dir $@)
	$(AR) rcs $(TARGET) $(OBJ_FILES)
	@echo

#  Make a hex from elf
%.hex : %.elf
	@echo "Making hex from elf..."
	$(OBJCOPY) -R .stack -O ihex $< $@
	@echo

#  Make an elf from objects
$(TARGET_ELF): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	@echo "Linking objects..."
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $(TARGET_ELF)
	@echo

#  Make object from c file
$(OBJDIR)/%.o : %.c
	@echo Compiling $<, writing to $@...
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@ > $(basename $@).lst
	@echo

#  Make object from bsp c file
$(OBJDIR)/%.o : $(BSP_PATH)/common/%.c
	@echo Compiling $<, writing to $@...
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@ > $(basename $@).lst
	@echo

#  Make object from bsp c file
$(OBJDIR)/%.o : $(BSP_PATH)/drivers/%.c
	@echo Compiling $<, writing to $@...
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@ > $(basename $@).lst
	@echo

#  Make object from assembly
$(OBJDIR)/%.o : %.s
	@echo Assembling $<, writing to $@...
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<  > $(basename $@).lst
	@echo

#  Make some stats
stats: $(TARGET_ELF)
	@echo "Size of executable:"
	@$(SIZE) $(TARGET_ELF)

#  Dump objects
dump: $(TARGET_ELF)
	$(OBJDUMP) -dSl $(TARGET_ELF)

#  Upload to the infernal device
program: $(TARGET)
	@echo "Programming..."
	$(PROGRAM) -v --mcu=$(LOWER_MCU) $(TARGET)

#  Remove all traces
clean:
	@echo "Cleaning up..."
	rm -rf obj
	rm -rf bin


