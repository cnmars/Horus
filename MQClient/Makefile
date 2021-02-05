CPP=/usr/bin/x86_64-w64-mingw32-g++
SOURCES=*.cc
INCLUDES=-Ipaho/include -Lpaho/lib
LINK=-lwininet -lws2_32 -lurlmon -lrpcrt4

all:
	$(CPP) $(SOURCES) $(INCLUDES) $(LINK)
