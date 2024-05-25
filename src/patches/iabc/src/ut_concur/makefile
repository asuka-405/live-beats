IABC_PATH = ../iabc

CPPFLAGS = -I.. `wx-config --cflags` -DGCC -g

PROGRAM := ut_concur

SRC =   ../ut_concur/ut_concur.cpp \
	event.cpp \
	event_log.cpp mutex.cpp \
	semaphore.cpp string.cpp threads.cpp \
	posixmutex.cpp posixsemaphore.cpp posixthreads.cpp \
	scan.cpp simple.cpp \
	wd_data.cpp \

SRCP := $(addprefix $(IABC_PATH)/,$(SRC))
OBJS := $(SRCP:.cpp=.o)

all: $(OBJS) $(PROGRAM) 

clean:
	rm -f $(PROGRAM)
	rm -f $(OBJS)
	rm -f *.o
	rm -f ut_concur

$(PROGRAM):	
	$(CC) -L/usr/lib -L/usr/local/lib -lpthread -lstdc++ $(CPPFLAGS) $(OBJS) -o $(PROGRAM)

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

 
