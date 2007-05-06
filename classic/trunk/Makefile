# BUILD SETTINGS ###################################
DEBUG := 1
# Valid values: WINDOWS, UNIX
PLATFORM := UNIX
PROFILE := 0

# END SETTINGS #####################################

TARGET := tyrian
OBJS := backgrnd.o config.o error.o episodes.o fonthand.o helptext.o joystick.o keyboard.o lvllib.o loudness.o lvlmast.o mainint.o musmast.o network.o newshape.o nortsong.o nortvars.o opentyr.o pallib.o params.o picload.o setup.o pcxload.o pcxmast.o shpmast.o sndmast.o starfade.o tyrian2.o varz.o vga256d.o lds_play.o fm_synth.o adlibemu.o starlib.o jukebox.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0 -Wno-unused
else
	DEBUG_FLAGS := -g3 -O2 -fomit-frame-pointer -DNDEBUG -Wno-unused -fno-strict-aliasing
endif

ifeq ($(PLATFORM), WINDOWS)
	SDL_CFLAGS := -I/mingw/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main
	SDL_LDFLAGS := -L/mingw/lib -lmingw32 -lSDLmain -lSDL -mwindows
else
	SDL_CFLAGS := $(shell sdl-config --cflags)
	SDL_LDFLAGS := $(shell sdl-config --libs)
endif


CFLAGS := -ansi -pedantic -Wall -Wstrict-prototypes -Wold-style-definition -Wmissing-declarations -Werror -I$(CURDIR)/src/ $(DEBUG_FLAGS) $(SDL_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) -lm

ifeq ($(PROFILE), 1)
	CFLAGS += -pg
	LDFLAGS += -pg
endif

####################################################

all : $(TARGET)

OBJS := $(foreach obj, $(OBJS), obj/$(obj))

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o : src/%.c
	$(CC) -o $@ -c $(CFLAGS) $<

.PHONY : clean

clean :
	rm -f obj/*.o
	rm -f $(TARGET)
