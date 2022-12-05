
all: sender receiver

sender: Sender.c
	gcc -o sender Sender.c

receiver: Receiver.c
	gcc -o receiver Receiver.c

clean:
	rm -f *.o sender receiver

runs:
	./sender

runr:
	./receiver