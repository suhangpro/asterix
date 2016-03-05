CC=gcc
CXX=g++
RM=rm -f
CFLAGS = -g
INCLUDES = 
LDFLAGS =
LIBS = 

SRCS = generate_graph.c 

OBJS = $(SRCS:.c=.o)

MAIN = generate_graph

.PHONY: clean

all : $(MAIN)
	@echo $(MAIN) compiled

$(MAIN) : $(OBJS) 
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

%.o : %.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean :
	$(RM) *.o *~ $(MAIN)

