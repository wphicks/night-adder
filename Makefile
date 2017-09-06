# Global options
BASEBUILDDIR:=build
TESTDIR=test
SRCDIR=src
INCDIR=include
VIPERINCDIR=../viper-engine ../viper-engine/thread ../viper-engine/containers
VIPERBASEBUILDDIR=../viper-engine/build

CC=clang
CFLAGS=-Wall -std=c99 -pthread
INCFLAGS=-I$(INCDIR) $(addprefix -I, $(VIPERINCDIR))
LDTESTFLAGS=-lgtest -lgtest_main -lpthread -lrt

CXX=clang++
CXXFLAGS=-Wall -std=c++14

# Configurable options
PROFFLAGS=-p
DEBUGFLAGS=-DDEBUG

RELEASE ?= 0
ifeq ($(RELEASE), 0)
    CFLAGS+=-g
    CXXFLAGS+=-g
endif

GCC ?= 0
ifeq ($(GCC), 1)
    CC=gcc
    CXX=g++
    BASEBUILDDIR:=build/gcc
    VIPERBASEBUILDDIR=../viper-engine/build/gcc
endif

OPT ?= 0
CFLAGS+=-O$(OPT)
BUILDDIR:=$(BASEBUILDDIR)/opt$(OPT)
VIPERBUILDDIR:=$(VIPERBASEBUILDDIR)/opt$(OPT)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS+=$(DEBUGFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/debug
    VIPERBUILDDIR:=$(VIPERBASEBUILDDIR)/debug
endif

PROFILE ?= 0
ifeq ($(PROFILE), 1)
    CFLAGS+=$(PROFFLAGS)
    BUILDDIR:=$(BASEBUILDDIR)/profile
    VIPERBUILDDIR:=$(VIPERBASEBUILDDIR)/profile
endif


# Targets
TARGETS=$(TESTS)
TESTS=$(BUILDDIR)/vector_test $(BUILDDIR)/particle_test $(BUILDDIR)/integrator_test

.PHONY: all test memtest objdump clean viper init

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
	rm $(BUILDDIR)/*;\
	$(MAKE) RELEASE=$(RELEASE) DEBUG=$(DEBUG) PROFILE=$(PROFILE) OPT=$(OPT) GCC=$(GCC) -C ../viper-engine clean

viper:
	$(MAKE) RELEASE=$(RELEASE) DEBUG=$(DEBUG) PROFILE=$(PROFILE) OPT=$(OPT) GCC=$(GCC) -C ../viper-engine

init:
	for entry in debug opt0 opt1 opt2 opt3 profile; do\
	    mkdir -p $(BASEBUILDDIR)/$${entry}/asm; \
	    mkdir -p $(BASEBUILDDIR)/gcc/$${entry}/asm; \
	done; \
	$(MAKE) RELEASE=$(RELEASE) DEBUG=$(DEBUG) PROFILE=$(PROFILE) OPT=$(OPT) GCC=$(GCC) -C ../viper-engine init

$(BUILDDIR)/nathread.o: $(SRCDIR)/nathread.linux.c $(INCDIR)/nathread.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/nathread.linux.c -o $(BUILDDIR)/nathread.o

$(BUILDDIR)/vector.o: $(SRCDIR)/vector.c $(INCDIR)/vector.h viper
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/vector.c -o $(BUILDDIR)/vector.o
$(BUILDDIR)/vector_test: $(BUILDDIR)/nathread.o $(BUILDDIR)/vector.o $(TESTDIR)/vector_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(BUILDDIR)/nathread.o $(VIPERBUILDDIR)/vatomic.o $(BUILDDIR)/vector.o $(TESTDIR)/vector_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/vector_test

$(BUILDDIR)/particle.o: $(SRCDIR)/particle.c $(INCDIR)/particle.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/particle.c -o $(BUILDDIR)/particle.o
$(BUILDDIR)/particle_test: $(BUILDDIR)/vector.o $(BUILDDIR)/particle.o $(TESTDIR)/particle_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(VIPERBUILDDIR)/vatomic.o $(BUILDDIR)/vector.o $(BUILDDIR)/particle.o $(TESTDIR)/particle_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/particle_test

$(BUILDDIR)/integrator.o: $(SRCDIR)/integrator.c $(INCDIR)/integrator.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c $(SRCDIR)/integrator.c -o $(BUILDDIR)/integrator.o
$(BUILDDIR)/integrator_test: $(BUILDDIR)/vector.o $(BUILDDIR)/particle.o $(BUILDDIR)/integrator.o $(TESTDIR)/integrator_test.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(VIPERBUILDDIR)/vatomic.o $(BUILDDIR)/vector.o $(BUILDDIR)/particle.o $(BUILDDIR)/integrator.o $(TESTDIR)/integrator_test.cpp $(LDTESTFLAGS) -o $(BUILDDIR)/integrator_test
