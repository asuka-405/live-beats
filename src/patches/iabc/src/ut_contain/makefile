IABC_PATH = ../iabc

CPPFLAGS = -I.. -DGCC -g

PROGRAM := containut

SRC =  $(IABC_PATH)/string.cpp \
	$(IABC_PATH)/mutex.cpp \
	$(IABC_PATH)/posixmutex.cpp \
	./containut.cpp

OBJS = $(SRC:.cpp=.o)

all: $(OBJS) $(PROGRAM) 

clean:
	rm -f $(PROGRAM)
	rm -f $(IABC_PATH)/*.o

containut:
	$(CC) $(CPPFLAGS) -L/usr/lib -lstdc++ -lpthread $(OBJS) -o containut

%.o:%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

 
