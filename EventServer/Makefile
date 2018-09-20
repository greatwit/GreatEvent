
#--------------------------------------------------------------------

CC = g++
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++ -lpthread -lresolv

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

LIBEVENT_INCL = -I$(HOME)/libevent/include -I../common
LIBEVENT_LIB  = -L$(HOME)/libevent/lib -levent

CFLAGS  += $(LIBEVENT_INCL)
LDFLAGS += $(LIBEVENT_LIB)

#--------------------------------------------------------------------

LIBOBJS = 

TARGET = EventServer

#--------------------------------------------------------------------

all: $(TARGET)


EventServer: spbuffer.o sputils.o spioutils.o spsession.o speventcb.o spserver.o EventServer.o
	$(LINKER) $(LDFLAGS) $^ -L. -levent -lpthread -o $@

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

