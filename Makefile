TARGET = test_driver
OPT = -O0
CSTANDARD = -std=gnu99
DDK_INCLUDE_PATH = /ucrt64/include/ddk

CC = gcc
LD = ld
RM = rm -f
STRIP = strip
OBJDUMP = objdump -x

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -municode
CFLAGS += $(CTANDARD)
CFLAGS += $(OPT)

# Do not link startup, compiler support
# or C standard libraries.
CFLAGS += -nostartfiles
CFLAGS += -nostdlib
CFLAGS += -nodefaultlibs

CFLAGS += -fPIC
CFLAGS += -shared

# Include path to ntddk.h or wdm.h.
CFLAGS += -I$(DDK_INCLUDE_PATH)

LDFLAGS  = --exclude-all-symbols
LDFLAGS += --gc-sections
LDFLAGS += --dynamicbase
LDFLAGS += --nxcompat

LDFLAGS += -subsystem=native
LDFLAGS += -file-alignment=0x200
LDFLAGS += -section-alignment=0x1000
LDFLAGS += -image-base=0x140000000
LDFLAGS += --stack=0x100000

LDFLAGS +=-entry=DriverEntry

SRC = $(TARGET).c

OBJ = $(SRC:%.c=%.o)

LIBS  = -lntoskrnl
LIBS += -lhal

all: sys strip dump

dump: strip
	$(OBJDUMP) $(TARGET).sys

strip: sys
	$(STRIP) $(TARGET).sys

sys: $(TARGET).sys

.SECONDARY: $(TARGET).sys
.PRECIOUS: $(OBJ)

$(TARGET).sys: $(OBJ) 
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET).sys
	$(RM) $(SRC:%.c=%.o)

.PHONY: all sys clean strip dump

