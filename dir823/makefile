mask:mask.c 
	gcc  $^ -o $@
signal:signal.o
	gcc  $^ -o $@
%.o:%.c
	gcc -E $^ -o $@
.PHONY:clean
clean:
	rm -rf *.o
