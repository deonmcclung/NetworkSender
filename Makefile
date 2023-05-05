CXXFLAGS=-I.

SENDER_OBJS = Common/Socket.o Sender/main.o
RECEIVER_OBJS = Common/Socket.o Receiver/main.o Receiver/Receiver.o

all: sender receiver

sender: ${SENDER_OBJS}
	${CXX} -o $@ ${SENDER_OBJS}

receiver: ${RECEIVER_OBJS}
	${CXX} -o $@ ${RECEIVER_OBJS}

.PHONY: clean

clean:
	rm -f ${SENDER_OBJS}
	rm -f ${RECEIVER_OBJS}
	rm -f sender receiver
