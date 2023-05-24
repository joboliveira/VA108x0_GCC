#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!   
#
##############################################################################################
#
# Intended use is with Eclipse IDE.  
#
#  Place in root director of project 
#
#  
##############################################################################################
#   Modified for VA108xx-REB1  May 2018    CEM

# Start of default section
#
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m0
 
# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =
 
# List all default directories to look for include files here
DINCDIR =
 
# List the default directory to look for the libraries here
DLIBDIR =
 
# List all default libraries here
DLIBS =
 
#
# End of default section
##############################################################################################
 
##############################################################################################
# Start of user section
#
 
#
# Define project name and Ram/Flash mode here   (Randomly selected name for project output)
 PROJECT        = REB1x_blink

RUN_FROM_FLASH = 1
 
# List all user C define here, like -D_DEBUG=1
UDEFS =
 
# Define ASM defines here
UADEFS =
 
# List C source files here

# List C source files here
COREdothDIR = .\common\hdr
COREdotcDIR = .\common\src
DRIVERdothDIR = .\driver\hdr
DRIVERdotcDIR = .\driver\src


CORELIBDIR = $(LIBSDIR)\CMSIS\Include

LINKER = .\linker
#SRC  = .\src\reb_main.c
SRC  = .\src\main.c
SRC += $(COREdotcDIR)\system_va108xx.c
SRC += $(DRIVERdotcDIR)\driver.common.c
#SRC += .\src\reb_adt75.c
#SRC += .\src\reb_log.c
#SRC += .\src\reb_max11619.c
#SRC += .\src\reb_timer.c
# SRC += .\src\syscall.c
#SRC += .\driver\src\i2c_va108xx.c
#SRC += .\driver\src\i2c_drv_api.c
#SRC += .\driver\src\uart_va108xx.c
#SRC += .\driver\src\uart_drv_api.c
#SRC += .\driver\src\spi_va108xx.c
#SRC += .\driver\src\spi_drv_api.c
#SRC += .\driver\src\gpio_va108xx.c
#SRC += .\driver\src\gpio_drv_api.c
#SRC += .\common\src\hardfault_handler.c
#SRC += .\common\src\irq_va108xx.c
#SRC += .\utilities\src\circular_buffer.c
#SRC += .\utilities\src\redirect.c
#SRC += .\utilities\src\segger_rtt.c
#SRC += .\utilities\src\segger_rtt_printf.c
#SRC += .\utilities\src\segger_rtt_syscalls_gcc.c
#SRC += .\utilities\src\utils.c

# SRC += .\src\


# List ASM source files here

ASRC = .\startup\startup_va108xx.s
 
# List all user directories here


UINCDIR = $(COREdothDIR)  \
           $(DRIVERdothDIR) \
		   .\utilities\hdr \
		   .\inc \
		   .\driver\hdr \
		   .\hdr

# List the user directory to look for the libraries here
ULIBDIR =
 
# List all user libraries here
ULIBS =
 
# Define optimisation level here
OPT = -Os
 
#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
ifeq ($(RUN_FROM_FLASH), 0)

LDSCRIPT = $(LINKER)\va108xx_linker.ld
FULL_PRJ = $(PROJECT)_ram
else

LDSCRIPT = .\linker\va108xx_linker.ld
FULL_PRJ = $(PROJECT)_rom
endif
 
INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
 
ifeq ($(RUN_FROM_FLASH), 0)
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0 -DVECT_TAB_SRAM
else
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=1
endif
 
ADEFS   = $(DADEFS) $(UADEFS)
OBJS  = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)
 
#ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
#LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) --specs=rdimon.specs -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
 
# Generate dependency information
#CPFLAGS += -MD -MP -MF .dep\$(@F).d
 
#
# makefile rules
#

CFLAGS   := -mcpu=cortex-m0 -mlittle-endian -mthumb -mfp16-format=ieee -mno-unaligned-access \
                -ggdb \
                -ffreestanding -fno-math-errno \
                -Os \
                -Wall -pedantic \
                -std=c11 \
                -D__EVAL -D__MICROLIB $(INC_FLAGS)
CPPFLAGS := $(CFLAGS) -MMD -MP 
ASFLAGS  := -mcpu=cortex-m0
LDFLAGS  := --specs=rdimon.specs \
                -Wl,--gc-sections \
                -mcpu=cortex-m0 \
                -nostartfiles \
                -mthumb \
                -Tlinker/va108xx_linker.ld \
                -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch \
                $(LIBDIR)

 
 
all: $(OBJS) $(FULL_PRJ).elf  $(FULL_PRJ).hex $(FULL_PRJ).bin 
ifeq ($(RUN_FROM_FLASH), 0)
	$(TRGT)size $(PROJECT)_ram.elf
else
	$(TRGT)size $(PROJECT)_rom.elf
endif
 
 %o: %c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%o: %s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@
	
%bin: %elf
	$(BIN)  $< $@
	

	
clean:
	del $(OBJS)
	del $(FULL_PRJ).elf
	del $(FULL_PRJ).map
	del $(FULL_PRJ).hex
	del $(FULL_PRJ).bin
#	del $(SRC:.c=.c.bak)
	del $(SRC:.c=.lst)
#   del $(ASRC:.s=.s.bak)
	del $(ASRC:.s=.lst)
	rmdir  .dep /S /Q
    
    
# 
# Include the dependency files, should be the last of the makefile
#

-include $(shell mkdir .dep) $(wildcard .dep/*)

# *** End Of File ***