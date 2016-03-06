CC=gcc
CXX=g++
RM=rm -f
CFLAGS = -g -std=c++11
INCLUDES = 
LDFLAGS =
LIBS = -lpthread 

SRCS1 = generate_graph.cpp 
SRCS2 = buyer.cpp seller.cpp peer.cpp run_peer.cpp 

OBJS1 = $(SRCS1:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)

MAIN1 = generate_graph
MAIN2 = run_peer

.PHONY: clean

all : $(MAIN1) $(MAIN2)
	@echo $(MAIN1), $(MAIN2) compiled

$(MAIN1) : $(OBJS1) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN1) $(OBJS1) $(LFLAGS) $(LIBS)

$(MAIN2) : $(OBJS2) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN2) $(OBJS2) $(LFLAGS) $(LIBS)

%.o : %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean :
	$(RM) *.o *~ $(MAIN)

