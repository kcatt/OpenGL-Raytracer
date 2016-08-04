#########################################################################
# Kane Catt - 4/26/2016                                                 #
# Universal Makefile                                                    #
#                                                                       #
# Can be used with any project. Set up to compile an arbitrary amount   #
# of files in a project that can be in any directory within the         #
# base project directory. Specify the directories that the source       #
# files are contained using the SRC_DIR variable and you can change     #
# the location the object files are placed with the BUILD_DIR variable. #
#                                                                       #
# CS 455 Computer Graphics                                              #
# Spring 2016                                                           #
#                                                                       #
# University of Evansville                                              #
#########################################################################

SRC_DIR    := src
BUILD_DIR  := build
SRC        := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))
OBJ        := $(patsubst src/%.cpp, build/%.o, $(SRC))

BACKUPS    := *~
EXECUTABLE := scene
FLAGS      := -std=c++11 -c -Wall -g
INCLUDES   := $(addprefix -I,$(SRC_DIR))
INCDIR     := /usr/X11R6/include/GL -I/usr/X11R6/include
XLIBS      := -std=c++11 -L/usr/X11R6/lib -lglut -lGLU -lGL -lm -lXmu -lXt -lXext -lSM -lICE -lX11
CC         := clang++

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(INCLUDES) $(FLAGS) $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) -o $@ -I$(INCDIR) $(XLIBS) $^

$(OBJDIR)/%.o: %.cpp %.h
	$(CC) $(FLAGS) $< -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR) $(BACKUPS) $(EXECUTABLE)

$(foreach bdir, $(BUILD_DIR), $(eval $(call make-goal, $(bdir))))
