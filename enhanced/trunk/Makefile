# BUILD SETTINGS ###################################
DEBUG := 1 # Always run 'make clean' after changing this!
USE_PRECOMPILED := 1

BOOST_SUFFIX := -mt
BOOST_ROOT := /usr/include # Directory that contains the boost/ include dir, NOT the dir itself
BOOST_LIBDIR := /usr/lib
# END SETTINGS #####################################

TARGET := tyrian
OBJS := animlib.o backgrnd.o config.o destruct.o editship.o episodes.o error.o fm_synth.o fmopl.o fonthand.o helptext.o joystick.o jukebox.o keyboard.o lds_play.o loudness.o mainint.o network.o newshape.o nortsong.o nortvars.o opentyr.o palette.o params.o pcxload.o picload.o scroller.o setup.o sndmast.o starlib.o tyrian2.o varz.o vga256d.o video.o video_scale.o BinaryStream.o BindManager.o Console.o CVar.o CCmd.o GameActions.o HighScores.o KeyNames.o MiscCVars.o superpixel.o explosion.o menus.o

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS := -g3 -O0 -Werror
else
	DEBUG_FLAGS := -g0 -O2 -fomit-frame-pointer -DNDEBUG
endif

ifeq ($(USE_PRECOMPILED), 1)
	PRECOMPILED_H_GCH := src/precompiled.h.gch
	INCLUDE_PRECOMPILED := -include "precompiled.h"
else
	PRECOMPILED_H_GCH :=
	INCLUDE_PRECOMPILED :=
endif

SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

BOOST_LDFLAGS := -I$(BOOST_LIBDIR) -lboost_filesystem$(BOOST_SUFFIX) -lboost_system$(BOOST_SUFFIX)

CXXFLAGS := --std=c++98 -pedantic -Wall -Wno-unused -I$(CURDIR)/src/ -DPASCAL_TYPES $(DEBUG_FLAGS) $(SDL_CFLAGS) -I$(BOOST_ROOT)
LDFLAGS := $(SDL_LDFLAGS) $(BOOST_LDFLAGS) -lm

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

$(PRECOMPILED_H_GCH) : src/precompiled.h
	$(CXX) -o $@ -c $(CXXFLAGS) $<

obj/%.d : obj/%.o
obj/%.o : src/%.cpp $(PRECOMPILED_H_GCH)
	$(CXX) -o $@ -MMD -c $(CXXFLAGS) $(INCLUDE_PRECOMPILED) $< 
#-H -Winvalid-pch

.PHONY : clean

clean :
	rm -f obj/*.o obj/*.d $(PRECOMPILED_H_GCH)
	rm -f $(TARGET)
