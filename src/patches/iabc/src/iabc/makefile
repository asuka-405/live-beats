TOP := ../..
IABC_PATH = .

CPPFLAGS:= `wx-config --cflags`
CPPFLAGS:= $(CPPFLAGS) -I.. -DGCC 

AR := ar

DEBUG = -g
# Uncomment this if you want to debug
# DEBUG =

LIB_PATH = $(TOP)/lib

OBJ_PATH = $(TOP)/obj

LIB := $(LIB_PATH)/libiabc.a

SRC = abcrules.cpp beamer.cpp event.cpp \
	event_log.cpp mutex.cpp \
	semaphore.cpp string.cpp threads.cpp \
	time_sig.cpp \
	posixmutex.cpp posixsemaphore.cpp posixthreads.cpp \
	dispatch.cpp draw_figure.cpp drawtemp.cpp \
	figure.cpp figure_factory.cpp gm.cpp \
	iabc_media.cpp iabc_preferences.cpp \
        iabc_parse.cpp iabc_media_manager.cpp \
	key_sig.cpp list_box_control.cpp \
	media.cpp message_box.cpp  note_figure.cpp \
	midi_media.cpp \
	music_info.cpp \
	note_info.cpp parse.cpp percentage_meter.cpp \
	pitch.cpp polygon.cpp preprocess.cpp \
	registry.cpp registry_defaults.cpp\
	scan.cpp simple.cpp slurer.cpp \
	line_of_music.cpp text_figure.cpp wd_data.cpp \
	window_media.cpp win_page.cpp winres.cpp \
	wx_dispatch.cpp wx_percentage_meter.cpp \
	wx_pp.cpp wx_winres.cpp \
	wx_frame.cpp \
	wx_splitter.cpp \
	wx_text_buffer.cpp wx_list_box.cpp \
	undo_buffer.cpp

SRCP := $(addprefix $(IABC_PATH)/,$(SRC))
OBJS := $(addprefix $(OBJ_PATH)/,$(SRC))
OBJS := $(OBJS:.cpp=.o)

all: $(LIB) $(OBJS)

$(LIB):$(OBJS)

$(OBJ_PATH)/%.o : $(IABC_PATH)/%.cpp
	$(CC) $(DEBUG) $(CPPFLAGS) -c $< -o $@
	$(AR) -r $(LIB) $@

clean:
	rm -f $(LIB)
	rm -f $(OBJ_PATH)/*.o



