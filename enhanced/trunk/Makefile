# BUILD SETTINGS ###################################
DEBUG := 0
# Valid values: WINDOWS, UNIX, GP2X
PLATFORM := UNIX

# If building for the GP2X
GP2X_CHAINPREFIX := /opt/open2x/gcc-4.1.1-glibc-2.3.6
GP2X_CHAIN := $(GP2X_CHAINPREFIX)/bin/arm-open2x-linux-

# END SETTINGS #####################################

TARGET := tyrian
OBJS := animlib.o backgrnd.o config.o destruct.o editship.o episodes.o error.o fm_synth.o fmopl.o fonthand.o helptext.o joystick.o jukebox.o keyboard.o lds_play.o loudness.o mainint.o network.o newshape.o nortsong.o nortvars.o opentyr.o palette.o params.o pcxload.o picload.o scroller.o setup.o sndmast.o starlib.o tyrian2.o varz.o vga256d.o video.o video_scale.o BinaryStream.o BindManager.o Console.o CVar.o CCmd.o GameActions.o HighScores.o KeyNames.o MiscCVars.o superpixel.o explosion.o menus.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0 -Werror
else
	DEBUG_FLAGS := -g -O2 -fomit-frame-pointer -DNDEBUG
endif

ifeq ($(PLATFORM), WINDOWS)
	SDL_CFLAGS := -I/mingw/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main
	SDL_LDFLAGS := -L/mingw/lib -lmingw32 -lSDLmain -lSDL -mwindows -lboost-filesystem
endif
ifeq ($(PLATFORM), GP2X)
	SDL_CFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --cflags` -I$(GP2X_CHAINPREFIX)/include -DTARGET_GP2X -mcpu=arm920t -mtune=arm920t -msoft-float -ffast-math -funroll-loops
	SDL_LDFLAGS := `$(GP2X_CHAINPREFIX)/bin/sdl-config --libs` -static -L$(GP2X_CHAINPREFIX)/lib
	CXX := $(GP2X_CHAIN)g++
	STRIP := $(GP2X_CHAIN)strip
endif
ifeq ($(PLATFORM), UNIX)
	SDL_CFLAGS := $(shell sdl-config --cflags)
	SDL_LDFLAGS := $(shell sdl-config --libs) -lboost_filesystem
endif


CXXFLAGS := --std=c++98 -pedantic -Wall -Wno-unused -I$(CURDIR)/src/ -DPASCAL_TYPES $(DEBUG_FLAGS) $(SDL_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) -lm

SVN_REV := $(shell svnversion src -n)
ifneq ($(SVN_REV), )
	ifeq ($(SVN_REV), exported)
		SVN_REV := unknown
	endif
	
	CXXFLAGS += -DSVN_REV=\"$(SVN_REV)\"
endif

####################################################

all : $(TARGET)

OBJS := $(foreach obj, $(OBJS), obj/$(obj))

$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

obj/%.d : obj/%.o
obj/%.o : src/%.cpp
	$(CXX) -o $@ -MMD -c $(CXXFLAGS) $<

.PHONY : clean

clean :
	rm -f obj/*.o obj/*.d
	rm -f $(TARGET)
