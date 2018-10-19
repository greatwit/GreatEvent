C++ = g++

ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CCFLAGS = -fPIC -Wall -Wextra -D$(os) -finline-functions -O3 -fno-strict-aliasing -fvisibility=hidden

ifeq ($(arch), IA32)
   CCFLAGS += -DIA32
endif

ifeq ($(arch), POWERPC)
   CCFLAGS += -mcpu=powerpc
endif

ifeq ($(arch), SPARC)
   CCFLAGS += -DSPARC
endif

ifeq ($(arch), IA64)
   CCFLAGS += -DIA64
endif

ifeq ($(arch), AMD64)
   CCFLAGS += -DAMD64
endif

INCL = -I./common
CCFLAGS += $(INCL)


D_OBJ = obj
OBJS = api.o buffer.o cache.o ccc.o channel.o common.o core.o epoll.o list.o md5.o packet.o queue.o window.o
DIR = $(shell pwd)

APP = sendfile recvfile sendvideo recvvideo

LDFLAGS = -L./ -ludt -lstdc++ -lpthread -lm

all: libudt.so libudt.a udt $(APP) mvobj

%.o: %.cpp %.h udt.h
	$(C++) $(CCFLAGS) $< -c

libudt.so: $(OBJS)
ifneq ($(os), OSX)
	$(C++) -shared -o $@ $^
else
	$(C++) -dynamiclib -o libudt.dylib -lstdc++ -lpthread -lm $^
endif

libudt.a: $(OBJS)
	ar -rcs $@ $^

udt:
	cp udt.h udt

mvobj:
	mv *.o obj

sendfile: sendfile.o
	$(C++) $^ -o $@ $(LDFLAGS)

recvfile: recvfile.o
	$(C++) $^ -o $@ $(LDFLAGS)
	
sendvideo: sendvideo.o NALDecoder.o UdtSender.o
	$(C++) $^ -o $@ $(LDFLAGS)

recvvideo: recvvideo.o UdtReceive.o
	$(C++) $^ -o $@ $(LDFLAGS)

clean:
	rm -f obj/*.o *.o *.so *.dylib *.a udt

install:
	export LD_LIBRARY_PATH=$(DIR):$$LD_LIBRARY_PATH
