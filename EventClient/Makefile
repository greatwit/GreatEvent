PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

THREAD_PATH  = ../common/gthread
NAREDEC_PATH = ../common/NalBareflow

OBJS = EventClient.o TcpSender.o spioutils.o $(NAREDEC_PATH)/NALDecoder.o $(THREAD_PATH)/gmutex.o $(THREAD_PATH)/gthread.o 


CFLAGS += -g

GTHREAD_INCL = -I$(THREAD_PATH) -I../common
CFLAGS  += $(GTHREAD_INCL)

all:	EventClient

EventClient:	$(OBJS) $^  -lpthread
	$(CXX) -o $@ $^

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr EventClient $(OBJS)
