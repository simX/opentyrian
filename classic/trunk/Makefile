# BUILD SETTINGS ###################################
DEBUG := 1
# Valid values: WINDOWS, UNIX
PLATFORM := UNIX
PROFILE := 0

# END SETTINGS #####################################

TARGET := tyrian
OBJS := backgrnd.o config.o error.o episodes.o fonthand.o helptext.o joystick.o keyboard.o lvllib.o loudness.o lvlmast.o mainint.o musmast.o network.o newshape.o nortsong.o nortvars.o opentyr.o pallib.o params.o picload.o setup.o pcxload.o pcxmast.o shpmast.o sndmast.o starfade.o tyrian2.o varz.o vga256d.o lds_play.o fm_synth.o adlibemu.o starlib.o jukebox.o animlib.o destruct.o shplib2.o shplib.o picload2.o editship.o scroller.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0
else
	DEBUG_FLAGS := -g -O2 -fomit-frame-pointer -DNDEBUG -fno-strict-aliasing
endif

ifeq ($(PLATFORM), WINDOWS)
	SDL_CFLAGS := -I/mingw/include/SDL -D_GNU_SOURCE=1 -Dmain=SDL_main
	SDL_LDFLAGS := -L/mingw/lib -lmingw32 -lSDLmain -lSDL -mwindows
else
	SDL_CFLAGS := $(shell sdl-config --cflags)
	SDL_LDFLAGS := $(shell sdl-config --libs)
endif


CFLAGS := -ansi -pedantic -Wall -Wstrict-prototypes -Wold-style-definition -Wmissing-declarations -Wno-unused -Werror -I$(CURDIR)/src/ $(DEBUG_FLAGS) $(SDL_CFLAGS)
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

ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif

obj/%.d : src/%.c
	@echo 'Generating dependencies for file $<'
	@set -e; $(CC) -MM -MT obj/$*.o $(CFLAGS) $< \
	    | sed 's/obj\/\($*\)\.o[ :]*/obj\/\1.o obj\/$*\.d : /g' > $@; \
	    [ -s $@ ] || rm -f $@

obj/%.o : src/%.c
	$(CC) -o $@ -c $(CFLAGS) $<

.PHONY : clean

clean :
	rm -f obj/*.o obj/*.d
	rm -f $(TARGET)
