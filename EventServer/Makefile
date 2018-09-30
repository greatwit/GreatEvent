
#--------------------------------------------------------------------

CC 	= gcc
CPP = g++
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -DHAVE_CONFIG_H -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++ -lpthread -lresolv

LINKER = $(CPP)
LINT = lint -c
RM = /bin/rm -f

LIBEVENT_INCL = -I./net -I../common
#LIBEVENT_INCL = -I$(HOME)/libevent/include -I../common
#LIBEVENT_LIB  = -L$(HOME)/libevent/lib -levent

CFLAGS  += $(LIBEVENT_INCL)
LDFLAGS += $(LIBEVENT_LIB)

#--------------------------------------------------------------------

LIBOBJS = 

TARGET = EventServer

#--------------------------------------------------------------------

all: $(TARGET)


EventServer: net/buffer.o net/epoll.o net/epoll_sub.o net/event.o net/evbuffer.o net/signal.o net/log.o \
			TaskBase.o TaskVideoRecv.o TaskFileRecv.o BufferCache.o sputils.o Ioutils.o Session.o \
			EventCall.o TcpClient.o TcpServer.o EventServer.o
	$(LINKER) $(LDFLAGS) $^ -L. -lpthread -o $@

clean:
	@( $(RM) net/*.o *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $^ -o $@	
