cc = gcc
PAGER = less
.PHONY : compile run time clean view
all : compile run time clean view

compile : stadium_NOMUTEX.c stadium_MUTEXINSIDEFOR.c stadium_MUTEXOUTSIDEFOR.c
	@echo "compiling programs..."
	$(cc) -pthread -ostadium_nomutex stadium_NOMUTEX.c
	$(cc) -pthread -ostadium_mutexinsidefor stadium_MUTEXINSIDEFOR.c
	$(cc) -pthread -ostadium_mutexoutsidefor stadium_MUTEXOUTSIDEFOR.c
	@echo "done compiling"

run : compile
	@echo "running no mutex"
	./stadium_nomutex
	@echo "running mutex inside for"
	./stadium_mutexinsidefor
	@echo "running mutex outside for"
	./stadium_mutexoutsidefor

time : compile
	@echo "timing no mutex"
	time ./stadium_nomutex
	@echo "timing mutex inside for"
	time ./stadium_mutexinsidefor
	@echo "timing mutex outside for"
	time ./stadium_mutexoutsidefor

clean :
	-@rm -f ./stadium_nomutex
	-@rm -f ./stadium_mutexinsidefor
	-@rm -f ./stadium_mutexoutsidefor

view : stadium_nomutex.c stadium_mutexinsidefor.c stadium_mutexoutsidefor.c
	$(PAGER) stadium_nomutex.c stadium_mutexinsidefor.c stadium_mutexoutsidefor.c
