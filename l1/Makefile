cc = gcc
PAGER = less
.PHONY : all p1 p2 p3 p4 p5 clean view

all : p1 p2 p3 p4 p5

p%.o : p%.c
	$(cc) -o $@ $<

p1 : p1.o
	@echo "running p1"
	./p1.o

p2 : p2.o
	@echo "running p2"
	./p2.o

p3 : p3.o
	@echo "running p3"
	./p3.o

p4 : p4.o
	@echo "running p4"
	./p4.o

p5 : p5.o
	@echo "running p5"
ifndef N
	./p5.o
else
	./p5.o $(N)
endif

clean :
	-@rm ./p*.o

view :
	$(PAGER) p1.c p2.c p3.c p4.c p5.c
