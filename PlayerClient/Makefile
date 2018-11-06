C++ = g++

ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CCFLAGS = -fPIC -Wall -Wextra -D$(os) -finline-functions -O3 -fno-strict-aliasing -fvisibility=hidden
#

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

INCL = -I./net
CCFLAGS += $(INCL)


D_OBJ = obj
OBJS = DataBuffer.o NetDataClient.o PlayBackClient.o net/TcpConnect.o
DIR = $(shell pwd)

APP = playerclient

LDFLAGS = -L./ -lstdc++ -lpthread -lm

all: $(APP) $(OBJS) mvobj

%.o: %.cpp %.h udt.h
	$(C++) $(CCFLAGS) $< -c

libplay.so: $(OBJS)
ifneq ($(os), OSX)
	$(C++) -shared -o $@ $^
else
	$(C++) -dynamiclib -o libudt.dylib -lstdc++ -lpthread -lm $^
endif



mvobj:
	mv *.o obj

playerclient: main.o $(OBJS)
	$(C++) $^ -o $@ $(LDFLAGS)

recvfile: recvfile.o
	$(C++) $^ -o $@ $(LDFLAGS)
	
sendvideo: sendvideo.o NALDecoder.o UdtSender.o
	$(C++) $^ -o $@ $(LDFLAGS)

recvvideo: recvvideo.o UdtReceive.o
	$(C++) $^ -o $@ $(LDFLAGS)

clean:
	rm -f obj/*.o net/*.o *.o *.so *.dylib *.a udt

install:
	export LD_LIBRARY_PATH=$(DIR):$$LD_LIBRARY_PATH
