###############################################################################
# CS/CNS 171 Fall 2015
#
# This is a template Makefile for OpenGL programs. Edit it however you find
# convenient.
# 
# The current version of this file should compile OpenGL programs just fine on
# Debian-based Linux operating systems.
#
# If you run Mac OS or other distributions of Linux, then you may have to
# fool around with the 'INCLUDE' and 'LIBDIR' lines below before the Makefile
# can compile the OpenGL parts successfully.
###############################################################################
CC = g++
FLAGS = -g -o

INCLUDE = -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES1 = src/*.cpp
SOURCES2 = src_texture/*.cpp

LIBS = -lGLEW -lGL -lGLU -lglut -lm -lpng

EXENAME1 = opengl_renderer
EXENAME2 = opengl_texture_renderer

opengl_renderer: $(SOURCES1)
	$(CC) $(FLAGS) $(EXENAME1) $(INCLUDE) $(LIBDIR) $(SOURCES1) $(LIBS)

opengl_texture_renderer: $(SOURCES2)
	$(CC) $(FLAGS) $(EXENAME2) $(INCLUDE) $(LIBDIR) $(SOURCES2) $(LIBS)

all: opengl_renderer opengl_texture_renderer

clean:
	rm -rf *.o *.dSYM $(EXENAME1) $(EXENAME2)

.PHONY: all clean
