all:read write
read:fifo_read.c
	gcc $^ -o $@
write:fifo_write.c
	gcc $^ -o $@
pipe:pipe.c
	gcc $^ -o $@
%.o:%.c
	gcc -c $^ -o $@
.PHONY:clean
clean:
	rm -rf pipe
