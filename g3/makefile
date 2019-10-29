cc = gcc
PAGER = less
.PHONY : clean build run test view
all : clean build run test view

build : g3.c game.c game.h utilities.c utilities.h
	@echo "compiling programs..."
	$(cc) -og3 g3.c game.c utilities.c
	@echo "done compiling"

run : build
	@echo "running g3 with GAMES=${GAMES} and DIMENSION=${DIMENSION}"
	./g3 "${GAMES}" "${DIMENSION}"
	@echo "done run"

test : build
	@echo "running g3 with GAMES=4 and DIMENSION=8"
	./g3 4 8 >| test1.out
	@echo "running g3 with GAMES=8 and DIMENSION=4"
	./g3 8 4 >| test2.out
	@echo "running g3 with GAMES=10 and DIMENSION=10"
	./g3 10 10 >| test3.out
	@echo "running g3 with GAMES=1 and DIMENSION=16"
	./g3 1 16 >| test4.out
	@echo "running g3 with GAMES=2 and DIMENSION=20"
	./g3 2 20 >| test5.out
	@echo "running g3 with GAMES=0 and DIMENSION=5"
	./g3 0 5 >| test6.out
	@echo "running g3 with GAMES=2 and DIMENSION=0"
	./g3 2 0 >| test7.out
	@echo "running g3 with GAMES=3 and DIMENSION=2"
	./g3 3 2 >| test8.out
	@echo "running g3 with GAMES=foo and DIMENSION=bar"
	./g3 foo bar >| test9.out
	@echo "running g3 with GAMES= and DIMENSION= "
	./g3 >| test10.out
	@echo "done tests"


clean :
	-@rm -f ./g3
	-@rm -f ./test1.out
	-@rm -f ./test2.out
	-@rm -f ./test3.out
	-@rm -f ./test4.out

view : g3.c game.c game.h utilities.c utilities.h
	$(PAGER) g3.c game.c game.h utilities.c utilities.h
