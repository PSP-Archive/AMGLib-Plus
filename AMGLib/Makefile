TARGET_LIB = libAMG.a
OBJS = AMG_Animation.o AMG_VFPU.o AMG_Multimedia.o AMG_Physics.o AMG_User.o AMG_3D.o AMG_Thread.o AMG_Texture.o AMG_Model.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

AMG_LIBS = -llodepng -lpspaudio -lpspmp3 -lbulletpsp -lpsputility -lpspctrl -lpsppower -lpsprtc -lpspgu -lm -lstdc++ -lpspsdk
OSL_LIBS = -losl -lpng -lz -lpsphprm -lpspmpeg -lpspjpeg -lpspmpegbase -lpspaudio -lpspmp3 -lbulletpsp -lpsputility -lpspctrl -lpsppower -lpsprtc -lpspgu -lm -lstdc++ -lpspsdk -lpspaudiocodec

LIBDIR =
LDFLAGS =
LIBS = $(AMG_LIBS)

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
