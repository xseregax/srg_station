#-----------------------------------------------------------------------
# 
#

TARGET := srg_station

MCU := atmega16a
MCUDUDE := atmega16
F_CPU := 16000000

AVRDUDE_PORT := /dev/ttyS0
AVRDUDE_BAUD := 57600
AVRDUDE_PROGR := avr109

MODULES := ./src ./src/pt ./src/common

#-----------------------------------------------------------------------
# config of makefile
#

CFLAGS := -Os -std=gnu99
#CFLAGS += -Wall
CFLAGS += -Werror -Wextra
CFLAGS += -DF_CPU=$(F_CPU)UL
CFLAGS += -funsigned-bitfields -fshort-enums -fshort-wchar
CFLAGS += -frename-registers
CFLAGS += -funsigned-char -fpack-struct -finline-functions

# Copy common part of flags into C++ flags variable
CXXFLAGS = $(CFLAGS)
CXXFLAGS += -fno-exceptions -fno-rtti
CXXFLAGS += -fno-threadsafe-statics

# add standard flag to CFLAGS _after_ copying CFLAGS to CXXFLAGS
CFLAGS += $(CSTD)
CXXFLAGS += $(CXXSTD)

# linker flags
#---------------- Library Options ----------------
LDFLAGS := -Wl,-u,vfprintf -lprintf_flt -lm

CFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections
#LDFLAGS += -Wl,--print-gc-sections
LDFLAGS += -Wl,--relax

AVRDUDE := avrdude -b $(AVRDUDE_BAUD) -c $(AVRDUDE_PROGR) -P $(AVRDUDE_PORT) -p $(MCUDUDE) -v -V
AVRDUDE_2 := avrdude -b 115200 -c ponyser -P $(AVRDUDE_PORT) -p $(MCUDUDE) -v

# Set default toolchain prefix if it does not defind in environment or project makefile
TOOL ?= avr-

# Set tool names
CC	:= $(TOOL)gcc
AS	:= $(TOOL)gcc -x assembler-with-cpp
BIN	:= $(TOOL)objcopy
OBJCOPY := $(TOOL)objcopy
OBJDUMP := $(TOOL)objdump
SIZE	:= $(TOOL)size

ELFSIZE := $(SIZE) --target=elf32-avr


OBJDIR := ./obj
LSTDIR := ./lst
EXEDIR := ./exe

SRCDIRS := $(MODULES)
INCDIRS := $(SRCDIRS)

CSRC	:= $(wildcard $(addsuffix /*.c,$(SRCDIRS)))
CPPSRC	:= $(wildcard $(addsuffix /*.cpp,$(SRCDIRS)))
ASRC    := $(wildcard $(addsuffix /*.S,$(SRCDIRS)))
BAK     := $(wildcard $(addsuffix /*.*~*,$(SRCDIRS)))
OBJ	:= $(addprefix $(OBJDIR)/,$(notdir $(CSRC:.c=.o) $(CPPSRC:.cpp=.o) $(ASRC:.S=.o) ))
LST	:= $(addprefix $(LSTDIR)/,$(notdir $(CSRC:.c=.lst) $(CPSRC:.cpp=.o) $(ASRC:.S=.lst) ))

TARGETEXT := hex eep

TARGETELF := $(EXEDIR)/$(TARGET).elf
TARGETPGM := $(addprefix $(EXEDIR)/$(TARGET).,$(TARGETEXT))
TARGETCOF := $(EXEDIR)/$(TARGET).cof

GENDEPFLAGS = -Wp,-MM,-MP,-MT,$@,-MF,$(@:.o=.d)
CFLAGS  += -mmcu=$(MCU)
ASFLAGS += -mmcu=$(MCU)
LDFLAGS += -mmcu=$(MCU) -Wl,-Map=$(LSTDIR)/$(TARGET).map,--cref

# search path for common sources and headers
vpath
vpath %.c $(SRCDIRS)
vpath %.cpp $(SRCDIRS)
vpath %.h $(SRCDIRS)
vpath %.S $(SRCDIRS)




# do not print any commands
.SILENT :


#-----------------------------------------------------------------------
# targets of makefile
#
.PHONY : all debug printok depends

# default target - binaries for AVR programming
all:	begin $(TARGETPGM) printok

begin:
	echo ======== Beginning of project  \"$(TARGET)\"  processing. MCU=$(MCU); $(CC) --version ;\
	mkdir -p $(OBJDIR) $(EXEDIR) $(LSTDIR)

#	which mkdir.exe ; which avr-gcc.exe ;\

printok:  $(TARGETELF)
	echo ======== All OK,  project size: ; $(ELFSIZE) $<

#-----------------------------------------------------------------------
# compilation rules
#
$(OBJDIR)/%.o : %.c
	echo ====  Compiling $< ; \
	$(CC) -E $(CFLAGS)  $(addprefix -I,$(INCDIRS)) $(GENDEPFLAGS) $< ;\
	$(CC) -c $(CFLAGS)  $(addprefix -I,$(INCDIRS)) \
		-Wa,-ahlmsd=$(LSTDIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(OBJDIR)/%.o : %.cpp
	echo ====  Compiling $< ; \
	$(CC) -E $(CXXFLAGS)  $(addprefix -I,$(INCDIRS)) $(GENDEPFLAGS) $< ;\
	$(CC) -c $(CXXFLAGS)  $(addprefix -I,$(INCDIRS)) \
		-Wa,-ahlmsd=$(LSTDIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(OBJDIR)/%.o : %.S
	echo ====  Compiling $< ; \
	$(AS) -E $(ASFLAGS) $(addprefix -I,$(INCDIRS)) $(GENDEPFLAGS) $< ; \
	$(AS) -c $(ASFLAGS) $(addprefix -I,$(INCDIRS)) \
		-Wa,-alm=$(LSTDIR)/$(notdir $(<:.S=.lst)) $< -o $@

%.s : %.c
	echo ====  Generate assembly source from $< ; \
	$(CC) -S $(CFLAGS) $(addprefix -I,$(INCDIRS)) $< -o $(<:.c=.s)

%.s : %.cpp
	echo ====  Generate assembly source from $< ; \
	$(CC) -S $(CXXFLAGS) $(addprefix -I,$(INCDIRS)) $< -o $(<:.cpp=.s)

%.E : %.c # preprocess only
	echo ====  Generate PREPROCESSED file from $< ; \
	$(CC) -E $(CFLAGS) $(addprefix -I,$(INCDIRS)) $< -o $(<:.c=.E)

%.E : %.cpp # preprocess only
	echo ====  Generate PREPROCESSED file from $< ; \
	$(CC) -E $(CXXFLAGS) $(addprefix -I,$(INCDIRS)) $< -o $(<:.cpp=.E)

#-----------------------------------------------------------------------
# elf post-processing rules
#

# hex-files for programmer
%.hex: %.elf
	echo ====  extract $(notdir $@) ; \
	$(BIN) -O ihex -R .eeprom $< $@

%.eep: %.elf
	echo ====  extract $(notdir $@) ; \
	$(BIN) -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 $< $@

#	$(OBJCOPY) --debugging \
#	--change-section-address .data-0x800000 \
#	--change-section-address .bss-0x800000 \
#	--change-section-address .noinit-0x800000 \
#	--change-section-address .eeprom-0x810000  \
#	-O coff-avr $< $@


# "dizassemble" elf file
dump: $(TARGETELF)
	echo ====  disassemble $(notdir $<) ; \
	$(OBJDUMP) -d -S -C $< >$(LSTDIR)/$(TARGET).dump

#-----------------------------------------------------------------------
# link: instructions to create elf output file from object files
#
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
$(TARGETELF) : $(OBJ)
	echo ====  Link to $(notdir $@) ;\
	$(CC) $^ $(LIB) $(LDFLAGS) -o $@

#-----------------------------------------------------------------------
#  dependencies

# all obj files depend on makefile because of makefile contain
#  some compiler's switches (optimisation, debug options, etc..)
$(OBJ) : makefile

# dependencies inclusion
# if make target list not contain this targets - include dependencies
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
 ifeq (,$(findstring cleanall,$(MAKECMDGOALS)))
  ifeq (,$(findstring directories,$(MAKECMDGOALS)))
-include $(shell mkdir $(OBJDIR) 2>/dev/null) $(wildcard $(OBJDIR)/*.d)
  endif
 endif
endif

#-----------------------------------------------------------------------
#  create temporary and ouptut directories
#
.PHONY : directories
directories:
	echo ====  Create working directories ;\
	mkdir -p $(OBJDIR) $(LSTDIR) $(EXEDIR)

.PHONY : dummy
dummy:


#-----------------------------------------------------------------------
#  flash with avrdude
#
.PHONY : flash fuses_512 fuses_1024 bootloader

flash : $(TARGETPGM)
	$(AVRDUDE) -U flash:w:$^:i

fuses_512:
	$(AVRDUDE_2) -U lfuse:w:0x2f:m -U hfuse:w:0x8a:m

fuses_1024:
	$(AVRDUDE_2) -U lfuse:w:0x2f:m -U hfuse:w:0x88:m

bootloader:
	$(AVRDUDE_2) -U flash:w:./bootloader/main.hex:i


#-----------------------------------------------------------------------
# clean project directories from working & backup ('~' suffix) files
#
.PHONY : clean cleanall
clean:
	echo ====  Erasing working directories and indent/editor backup files; \
	rm -f $(TARGETCOF) $(EXEDIR)/$(TARGET).obj $(EXEDIR)/$(TARGET).sym ; \
	rm -f $(TARGETELF) ; \
	rm -f $(LSTDIR)/* ; \
	rm -f $(OBJDIR)/* ; \
	rm -f $(CSRC:.c=.s) ; \
	rm -f $(BAK)

cleanall: clean
	echo ====  Remove target binaries and working directories ; \
	rm -f $(TARGETPGM) ; \
	test -e $(EXEDIR) && rmdir $(EXEDIR) ; \
	test -e $(LSTDIR) && rmdir $(LSTDIR) ; \
	test -e $(OBJDIR) && rmdir $(OBJDIR)
