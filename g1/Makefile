cc = gcc
PAGER = less
.PHONY : all run test clean view

all : g1 run

g1 : database.c database.h utilities.c utilities.h g1.c
	@echo "compiling g1"
	$(cc) -og1 database.c utilities.c g1.c

run : g1
	@echo "running g1"
ifndef DB
	./g1 database.db
else
	./g1 $(DB)
endif

test : g1 testcmds
	@echo "running test commands"
	./g1 test.db < testcmds
	@echo "test database stored at test.db"

clean :
	-@rm -f ./g1
	-@rm -f ./database.db
	-@rm -f ./test.db

view :
	$(PAGER) database.h database.c utilities.h utilities.c g1.c
