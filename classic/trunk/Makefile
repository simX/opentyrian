# BUILD SETTINGS ###################################
DEBUG := 1

# END SETTINGS #####################################

TARGET := tyrian
OBJS := config.o error.o fonthand.o helptext.o keyboard.o newshape.o nortsong.o nortvars.o opentyr.o pallib.o picload.o pcxload.o pcxmast.o shpmast.o starfade.o tyrian2.o varz.o vga256d.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0 -Wno-unused
else
	DEBUG_FLAGS := -O2 -fomit-frame-pointer -DNDEBUG
endif

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := -ansi -pedantic -Wall -Werror -I$(CURDIR)/src/ $(DEBUG_FLAGS) $(SDL_CFLAGS)
LDFLAGS := $(SDL_LDFLAGS) -lm

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
