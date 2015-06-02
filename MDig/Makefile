CC=gcc
CXX=g++
RM=rm -f

OCV = /nobackup/jingpu/tools/opencv-2.4.11/install
JPCNN = ../DeepBeliefSDK

CPPFLAGS= -O -std=c++11 -g  -I./src -I$(OCV)/include -I$(JPCNN)/src/include
LDFLAG=
LDLIBS= -L$(OCV)/lib -lopencv_core -lopencv_highgui \
	-lopencv_calib3d -lopencv_imgproc -lopencv_features2d \
	-lopencv_video \
	-L$(JPCNN) -ljpcnn

SRCS := $(shell find src -name '*.cpp' -not -name '._*')
OBJS := $(subst .cpp,.o,$(SRCS))

.PHONY: all tests test_convnet test_segment test_app

all: tests

# objects
%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $(basename $@).o

# executables
tests/test_%.out: tests/test_%.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)

# run tests
tests: test_app

test_app:  tests/test_app.out
	./$<

test_convnet:  tests/test_convnet.out
	./$<

test_segment:  tests/test_segment.out
	./$<

test_conv:  tests/test_convnet.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@

test_segment:  tests/test_segment.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@

test_pipeline:  tests/test_pipeline.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@
clean:
	find . -iname "*.o" -exec rm '{}' ';'
	find . -iname "*.out" -exec rm '{}' ';'
