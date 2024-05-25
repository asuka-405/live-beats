TOP := ../..

IABC_PATH := ../iabc
MAIN_PATH := ./

LIB_PATH = $(TOP)/lib

BIN_PATH = $(TOP)/bin

LIB := $(LIB_PATH)/libiabc.a

OBJ_PATH = $(TOP)/obj

DEBUG = -g
CPPFLAGS:= `wx-config --cflags`
CPPFLAGS:= $(CPPFLAGS) $(DEBUG) -I.. -DGCC 
WXLIBS:= `wx-config --libs`

PROGRAM := iabc

SRC:= main.cpp system_dirs.cpp resources.cpp wx24_resources.cpp

OBJS:= $(SRC:.cpp=.o)

all: $(PROGRAM) $(LIB) $(OBJS)

clean:
	rm -f $(BIN_PATH)/$(PROGRAM)
	rm -f *.o

$(PROGRAM): $(LIB) $(OBJS)
	$(CC) $(DEBUG) -L/usr/lib -L/usr/local/lib -L$(LIB_PATH) \
	-L$(OBJ_PATH) $(CPPFLAGS) $(OBJS) \
	-lpthread $(WXLIBS) -liabc -o $(BIN_PATH)/$(PROGRAM)

$(LIB):
	cd $(IABC_PATH); make;cd ${HERE}

$(OBJ_PATH)/%.o : %.cpp
	$(CC) $(DEBUG) $(CPPFLAGS) -c $< -o $@
	$(AR) -r $(LIB) $@


 
