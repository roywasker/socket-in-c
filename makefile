CC=gcc
AR=ar
OBJECT_SENDER=sender.o
OBJECT_RECEIVER=receiver.o
OBJECT_LIB=my_mat.o
FLAGS= -Wall -g

all: connections receiver

sender: $(OBJECT_MAIN) libmymat.a
	$(CC) $(FLAGS) -o sender $(OBJECT_SENDER) -lm 

receiver: $(OBJECT_MAIN) libmymat.a
	$(CC) $(FLAGS) -o receiver $(OBJECT_RECEIVER) -lm 

receiver.o: receiver.c 
	$(CC) $(FLAGS) -c receiver.c

sender.o: sender.c 
	$(CC) $(FLAGS) -c sender.c

.PHONY: clean all

clean:
	rm -f *.o *.a sender receiver