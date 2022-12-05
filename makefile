

all: sender receiver

sender: sender.c
	gcc -o sender sender.c

receiver: receiver.c
	gcc -o receiver receiver.c

clean:
	rm -f *.o sender receiver

runs:
	./sender

runr:
	./receiver

runs-strace:
	strace -f ./sender

runr-strace:
	strace -f ./receiver