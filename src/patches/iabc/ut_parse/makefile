IABC_PATH = ../src/iabc

CPPFLAGS = -I../src `wx-config --cflags` -DGCC -g

PROGRAM := ut_parse

SRC =   ../../ut_parse/ut_parse.cpp \
	event_log.cpp \
	abcrules.cpp iabc_parse.cpp iabc_preferences.cpp \
	media.cpp music_info.cpp mutex.cpp \
	note_info.cpp parse.cpp percentage_meter.cpp \
        pitch.cpp preprocess.cpp scan.cpp simple.cpp \
	string.cpp posixmutex.cpp


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

 
