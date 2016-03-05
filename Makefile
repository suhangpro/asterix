CC=gcc
CXX=g++
RM=rm -f
CFLAGS = -g
INCLUDES = 
LDFLAGS =
LIBS = 

SRCS = generate_peers.c 

OBJS = $(SRCS:.c=.o)

MAIN = generate_peers

.PHONY: clean

all : $(MAIN)
	@echo $(MAIN) compiled

$(MAIN) : $(OBJS) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

%.o : %.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean :
	$(RM) *.o *~ $(MAIN)

