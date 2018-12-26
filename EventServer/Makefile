CXXFLAGS += -std=c++11

CC 	= gcc
CPP = g++
CFLAGS 		= -Wall -DHAVE_CONFIG_H -g -fPIC #-DHAVE_SYS_TIME_H
CXXFLAGS 	+= -Dlinux -DHAV_FFMPEG

FFMPEG_PATH  = ../common/ffmpeg
THREAD_PATH  = ../common/gthread
NAREDEC_PATH = ../common/NalBareflow

COMPLIE_INCL = -I./net -I../common -I$(THREAD_PATH) -I$(FFMPEG_PATH) -I$(FFMPEG_PATH)/include
COMPLIE_LIB  = -L$(FFMPEG_PATH)/lib -lavformat -lavcodec -lavdevice -lavfilter -lswresample -lswscale -lavutil
CFLAGS   += -I./ -I./net -I../common
CXXFLAGS += $(COMPLIE_INCL)
LDFLAGS  += $(COMPLIE_LIB) -lpthread

TARGET = EventServer EventClient FfmpegTest

#--------------------------------------------------------------------

all: $(TARGET)

OBJECTS = net/buffer.o net/epoll.o net/epoll_sub.o net/event.o net/evbuffer.o net/signal.o net/log.o net/net_protocol.o \
		  TaskBase.o TaskVideoRecv.o TaskVideoSend.o TaskFileSend.o TaskPlayback.o BufferCache.o DataUtils.o IOUtils.o \
		  Session.o EventCall.o  ActorStation.o \
		  $(FFMPEG_PATH)/FfmpegContext.o $(THREAD_PATH)/gthreadpool.o $(NAREDEC_PATH)/NALDecoder.o

EventServer=$(OBJECTS) EventServer.o PlayerServer.o
EventClient=$(OBJECTS) EventClient.o TcpClient.o
FfmpegTest = $(FFMPEG_PATH)/FfmpegContext.o $(FFMPEG_PATH)/FfmpegTest.o $(THREAD_PATH)/gthreadpool.o
 
EventServer:$(EventServer) 
	 $(CPP) -o EventServer $(EventServer) $(LDFLAGS)

EventClient:$(EventClient) 
	 $(CPP) -o EventClient $(EventClient) $(LDFLAGS)

FfmpegTest:$(FfmpegTest) 
	 $(CPP) -o FfmpegTest $(FfmpegTest) $(LDFLAGS)

.PHONY:clean

clean:
	rm *.o net/*.o $(FFMPEG_PATH)/*.o $(NAREDEC_PATH)/*.o $(THREAD_PATH)/*.o

	