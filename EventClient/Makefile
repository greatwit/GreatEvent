PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = EventClient.o TcpSender.o NALDecoder.o spioutils.o ../common/gthread/gmutex.o ../common/gthread/gthread.o 


CFLAGS += -g

GTHREAD_INCL = -I../common/gthread -I../common
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
