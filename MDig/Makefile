CC=gcc
CXX=g++
RM=rm -f

OCV = /nobackup/jingpu/tools/opencv-2.4.11/install


CPPFLAGS= -O -std=c++11 -g  -I./src -I$(OCV)/include
LDFLAG=
LDLIBS= -L$(OCV)/lib -lopencv_core -lopencv_highgui \
	-lopencv_calib3d -lopencv_imgproc -lopencv_features2d \
	-lopencv_video \
	-L../DeepBeliefSDK -ljpcnn

SRCS := $(shell find src -name '*.cpp' -not -name '._*')
OBJS := $(subst .cpp,.o,$(SRCS))

all: main

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $(basename $@).o

main:  $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

run: main
	./main


clean:
	find . -iname "*.o" -exec rm '{}' ';'
