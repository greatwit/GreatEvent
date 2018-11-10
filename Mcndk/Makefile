CXXFLAGS += -std=c++11
 
CC 	= gcc
CPP = g++

FFMPEG_PATH  = ../common/ffmpeg
THREAD_PATH  = ../common/gthread

COMPLIE_INCL = -I./include -I../common -I$(FFMPEG_PATH) -I$(FFMPEG_PATH)/include -I$(THREAD_PATH)
COMPLIE_LIB  = -L$(FFMPEG_PATH)/lib -lavformat -lavcodec -lavdevice -lavfilter -lswresample -lswscale -lavutil -lpthread
CXXFLAGS += $(COMPLIE_INCL)
LDFLAGS += $(COMPLIE_LIB)

OBJECTS = main.o FileDeCodecJni.o $(FFMPEG_PATH)/FfmpegContext.o $(THREAD_PATH)/gthreadpool.o
EXE=FfmpegTest
 
 
$(EXE):$(OBJECTS)
	$(CPP) -o $(EXE) $(OBJECTS) $(LDFLAGS) 

.PHONY:clean
clean:
	rm $(EXE) $(OBJECTS)
