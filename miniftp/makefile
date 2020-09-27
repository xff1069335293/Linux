CC = gcc
CFLAG = -g
OBJS = miniftp.o sysutil.o session.o ftpproto.o privparent.o str.o privsock.o tunable.o parseconf.o hash.o
LIBS = -lcrypt
BIN = miniftp 
$(BIN):$(OBJS)
	$(CC) $(CFLAG) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@ 
.PHONY:clean
clean:
	rm -rf *.o $(BIN)
		
