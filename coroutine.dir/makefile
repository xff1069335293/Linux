CC = gcc
CFLAG = -g
OBJS = coroutine.o main.o 
BIN = coroutine
$(BIN):$(OBJS)
	$(CC) $(CFLAG) $^ -o $@
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
.PHONY:clean
clean:
	rm -rf *.o $(BIN)
