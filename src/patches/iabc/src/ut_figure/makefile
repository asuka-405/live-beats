TOP := ../..

IABC_PATH := $(TOP)/src/iabc
MAIN_PATH := ./

LIB_PATH = $(TOP)/lib

BIN_PATH = $(TOP)/bin

OBJ_PATH = $(TOP)/obj

DEBUG = -g
CPPFLAGS:= `wx-config --cflags`
CPPFLAGS:= $(CPPFLAGS) -I.. -DGCC 
WXLIBS:= `wx-config --libs`
LIB:= libfigure.a
PROGRAM := ut_figure

SRC =   ../ut_figure/doc.cpp \
	event.cpp \
	event_log.cpp mutex.cpp \
	semaphore.cpp string.cpp threads.cpp \
	posixmutex.cpp posixsemaphore.cpp posixthreads.cpp \
	dispatch.cpp draw_figure.cpp drawtemp.cpp \
	figure.cpp figure_factory.cpp gm.cpp \
	media.cpp message_box.cpp  \
	music_info.cpp \
	note_info.cpp \
	pitch.cpp polygon.cpp \
	scan.cpp simple.cpp \
	text_figure.cpp wd_data.cpp \
	winres.cpp \
	wx_dispatch.cpp \
	wx_pp.cpp wx_winres.cpp

SRCP := $(addprefix $(IABC_PATH)/,$(SRC))
OBJS := $(SRCP:.cpp=.o)

all: $(PROGRAM)

$(LIB): $(OBJS)

clean:
	rm -f $(BIN_PATH)/$(PROGRAM)
	rm -f $(OBJS)
	rm -f $(LIB)

$(PROGRAM): $(OBJS) $(LIB)
	$(CC) $(DEBUG) -L. -L/usr/lib -L/usr/local/lib -L$(LIB_PATH) \
	$(CPPFLAGS) \
	-lpthread $(WXLIBS) $(OBJS) -o $(PROGRAM)

%.o : %.cpp
	$(CC) $(DEBUG) $(CPPFLAGS) -c $< -o $@
	$(AR) -r $(LIB) $@


 
