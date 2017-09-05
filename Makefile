# Global options
BASEBUILDDIR:=build
TESTDIR=test
SRCDIR=src
INCDIR=include

CC=clang
CFLAGS=-Wall -std=c99 -pthread -g
INCFLAGS=-Iinclude
LDTESTFLAGS=-lgtest -lgtest_main -lpthread -lrt

CXX=clang++
CXXFLAGS=-Wall -std=c++14 -g

# Configurable options
PROFFLAGS=-p
DEBUGFLAGS=-DDEBUG

GCC ?= 0
ifeq ($(GCC), 1)
    CC=gcc
    CXX=g++
    BASEBUILDDIR:=build/gcc
endif

OPT ?= 0
CFLAGS+=-O$(OPT)
BUILDDIR:=$(BASEBUILDDIR)/opt$(OPT)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS+=$(DEBUGFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/debug
endif

PROFILE ?= 0
ifeq ($(PROFILE), 1)
    CFLAGS+=$(PROFFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/profile
endif


# Targets
TARGETS=$(TESTS)
TESTS=$(BUILDDIR)/geometry_test $(BUILDDIR)/particle_test $(BUILDDIR)/integrator_test

.PHONY: all test memtest objdump clean

all : $(TARGETS)

test: $(TESTS)
	for test_file in $(TESTS) ; do\
	    $$test_file  || exit 1; \
	done

memtest: $(TESTS)
	for test_file in $(TESTS) ; do\
	    valgrind --tool=memcheck $$test_file  || exit 1; \
	done

objdump: $(TARGETS)
	for obj_file in $(BUILDDIR)/*.o; do\
	    objdump -S $${obj_file} > $(BUILDDIR)/asm/$$(basename $$obj_file).asm; \
	done

clean:
	rm $(BUILDDIR)/*

$(BUILDDIR)/geometry.o: $(SRCDIR)/geometry.c $(INCDIR)/geometry.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/geometry.c -o $(BUILDDIR)/geometry.o
$(BUILDDIR)/geometry_test: $(BUILDDIR)/geometry.o $(TESTDIR)/geometry_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/geometry.o $(TESTDIR)/geometry_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/geometry_test

$(BUILDDIR)/particle.o: $(SRCDIR)/particle.c $(INCDIR)/particle.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/particle.c -o $(BUILDDIR)/particle.o
$(BUILDDIR)/particle_test: $(BUILDDIR)/geometry.o $(BUILDDIR)/particle.o $(TESTDIR)/particle_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/geometry.o $(BUILDDIR)/particle.o $(TESTDIR)/particle_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/particle_test

$(BUILDDIR)/integrator.o: $(SRCDIR)/integrator.c $(INCDIR)/integrator.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/integrator.c -o $(BUILDDIR)/integrator.o
$(BUILDDIR)/integrator_test: $(BUILDDIR)/geometry.o $(BUILDDIR)/particle.o $(BUILDDIR)/integrator.o $(TESTDIR)/integrator_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/geometry.o $(BUILDDIR)/particle.o $(BUILDDIR)/integrator.o $(TESTDIR)/integrator_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/integrator_test
