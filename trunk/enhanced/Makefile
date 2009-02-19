# BUILD SETTINGS ###################################
# Always run 'make clean' after changing this!
DEBUG := 1
USE_PRECOMPILED := 1

BOOST_SUFFIX := -mt
BOOST_ROOT := /usr/include # Directory that contains the boost/ include dir, NOT the dir itself
BOOST_LIBDIR := /usr/lib
# END SETTINGS #####################################

TARGET := tyrian
OBJS := animlib.o backgrnd.o config.o destruct.o editship.o episodes.o Filesystem.o fm_synth.o fmopl.o fonthand.o helptext.o joystick.o jukebox.o keyboard.o lds_play.o loudness.o mainint.o network.o newshape.o nortsong.o nortvars.o opentyr.o palette.o params.o pcxload.o picload.o scroller.o setup.o sndmast.o starlib.o starlib_patterns.o tyrian2.o varz.o vga256d.o video.o video_scale.o BinaryStream.o HighScores.o superpixel.o explosion.o menus.o cheats.o mtrand.o itemscreen.o \
            console/BindManager.o console/Console.o console/CCmd.o console/GameActions.o console/KeyNames.o console/MiscCVars.o \
                console/cvar/CVar.o console/cvar/CVarManager.o console/cvar/CVarFlagsEnumerator.o \
            network/NetCVars.o network/NetManager.o network/Packet.o network/PacketFactory.o \
				network/packets/PacketConnect.o network/packets/PacketNetwork.o network/packets/PacketReliable.o

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
SDL_LDFLAGS := $(shell sdl-config --libs) -lSDL_net -lSDL_mixer

BOOST_LDFLAGS := -I$(BOOST_LIBDIR) -lboost_date_time$(BOOST_SUFFIX) -lboost_filesystem$(BOOST_SUFFIX) -lboost_system$(BOOST_SUFFIX)

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
	@mkdir -p "$(dir $@)"
	$(CXX) -o $@ -MMD -c $(CXXFLAGS) $(INCLUDE_PRECOMPILED) $< 

.PHONY : clean

clean :
	rm -rf obj/* $(PRECOMPILED_H_GCH)
	rm -f $(TARGET)
