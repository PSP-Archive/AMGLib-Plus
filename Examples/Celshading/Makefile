# ---------------------------------------------
# Makefile of AMGlib for PSPSDK
# By Andresmargar
# ---------------------------------------------

# Defines program name and source files
TARGET = Template
SOURCE_FILES = Source
OBJS = $(SOURCE_FILES)/Main.o
EBOOT_FILES = $(CURDIR)/Eboot

# Compilation options
INCDIR = 
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

# Libraries to link to the project
# Order is important
LIBDIR =
LDFLAGS =
LIBS = -lAMG -lpspjpeg -llodepng -lpspaudio -lpspmp3 -lbulletpsp -lpsputility -lpspctrl -lpsppower -lpsprtc -lpspgu -lm -lstdc++ -lpspsdk

# Options for PBP visualization
# Uncomment if you want to add a banner file
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = CelShading Example
PSP_EBOOT_ICON = $(EBOOT_FILES)/ICON0.PNG
#PSP_EBOOT_PIC1 = $(EBOOT_FILES)/PIC1.PNG
#PSP_EBOOT_SND0 = $(EBOOT_FILES)/SND0.AT3

# Compile all files with the options written
# Also creates a EBOOT.PBP file
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

# Delete *.o files
delete:
	@rm -fr $(CURDIR)/$(SOURCE_FILES)/*.o
	