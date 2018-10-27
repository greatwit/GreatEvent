CXXFLAGS += -std=c++11
 
CC 	= gcc
CPP = g++

COMPLIE_INCL = -I./include
COMPLIE_LIB  = -L./lib -lavformat -lavcodec -lavdevice -lavfilter -lswresample -lswscale -lavutil
CXXFLAGS += $(COMPLIE_INCL)
LDFLAGS += $(COMPLIE_LIB)

OBJECTS = FfmpegTest.o FfmpegContext.o
EXE=FfmpegTest
 
 
$(EXE):$(OBJECTS)
	$(CPP) -o $(EXE) $(OBJECTS) $(LDFLAGS)

.PHONY:clean
clean:
	rm $(EXE) $(OBJECTS)
