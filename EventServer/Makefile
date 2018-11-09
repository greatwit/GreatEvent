CXXFLAGS += -std=c++11

CC 	= gcc
CPP = g++

CFLAGS = -Wall -DHAVE_CONFIG_H -g -fPIC #-DHAVE_SYS_TIME_H
CXXFLAGS = -Dlinux -DHAV_FFMPEG
COMPLIE_INCL = -I./net -I../common -I../common/gthread -I./ffmpeg -I./ffmpeg/include
COMPLIE_LIB  = -L./ffmpeg/lib -lavformat -lavcodec -lavdevice -lavfilter -lswresample -lswscale -lavutil
CFLAGS  += -I./net -I../common -I./
CXXFLAGS += $(COMPLIE_INCL)
LDFLAGS += $(COMPLIE_LIB) -lpthread

TARGET = EventServer EventClient FfmpegTest

#--------------------------------------------------------------------

all: $(TARGET)

OBJECTS = net/buffer.o net/epoll.o net/epoll_sub.o net/event.o net/evbuffer.o net/signal.o net/log.o net/net_protocol.o \
		  TaskBase.o TaskVideoRecv.o TaskVideoSend.o TaskFileRecv.o TaskPlayback.o BufferCache.o DataUtils.o IOUtils.o \
		  Session.o EventCall.o NALDecoder.o ActorStation.o TcpClient.o TcpServer.o ffmpeg/FfmpegContext.o \
		  ../common/gthread/gmutex.o ../common/gthread/gthread.o

EventServer=$(OBJECTS) EventServer.o
EventClient=$(OBJECTS) EventClient.o
FfmpegTest = ffmpeg/FfmpegContext.o ffmpeg/FfmpegTest.o ../common/gthread/gmutex.o ../common/gthread/gthread.o
 
EventServer:$(EventServer) 
	 $(CPP) -o EventServer $(EventServer) $(LDFLAGS)

EventClient:$(EventClient) 
	 $(CPP) -o EventClient $(EventClient) $(LDFLAGS)

FfmpegTest:$(FfmpegTest) 
	 $(CPP) -o FfmpegTest $(FfmpegTest) $(LDFLAGS)

.PHONY:clean

clean:
	rm *.o net/*.o ffmpeg/*.o

	