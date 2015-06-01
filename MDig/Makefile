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

all: tests

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $(basename $@).o

main:  $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

tests: test_convnet


test_convnet:  tests/test_convnet.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@

test_edge:  tests/test_edge.o
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@  res/real_img.jpg


test_segment:  tests/test_segment.o $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)
	./$@
clean:
	find . -iname "*.o" -exec rm '{}' ';'
