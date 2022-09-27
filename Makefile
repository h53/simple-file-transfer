OBJS = MySocket.o Eventloop.o Frame.o Logger.o TcpServer.o ThreadFlag.o md5.o client.o server.o tools.o dispatcher.o EndianConvert.o common.o

VPATH = command 3rd

CFLAGS = -Wall -g

ft: $(OBJS) main.o
	gcc -o $@ $^ $(CFLAGS) -lpthread
	
test: $(OBJS) test.o
	gcc -o $@ $^ $(CFLAGS) -lpthread

%.o: %.c
	gcc -c $< -o $@

.PHONY: clean
clean:
	-rm *.o test main ft filetransfer log.txt temp.txt