cc = gcc
PAGER = less
.PHONY : all test_client test_server test_mongoose test clean view

all : clean l2_client l2_server mongoose_server test_client test_server test_mongoose

l2_client : l2_client.c csapp.c csapp.h utilities.c utilities.h
	@echo "compiling l2_client"
	$(cc) -ol2_client -pthread l2_client.c csapp.c utilities.c

l2_server: l2_server.c csapp.c csapp.h utilities.c utilities.h
	@echo "compiling l2_server"
	$(cc) -ol2_server -pthread l2_server.c csapp.c utilities.c

mongoose_server: mongoose_server.c mongoose.c mongoose.h utilities.c utilities.h
	@echo "compiling mongoose_server"
	$(cc) -omongoose_server mongoose_server.c mongoose.c utilities.c

test_client: l2_client
	@echo "sending client request for www.cs.drexel.edu/~kschmidt/"
	./l2_client -h www.cs.drexel.edu -f /~kschmidt/ >| client.out
	@echo "test_client completed, results in client.out"

test_server : l2_client l2_server index.html
	@echo "running l2_server on port 40001"
	./l2_server -p 40001 &
	@echo "sending client request for /index.html"
	./l2_client -h 127.0.0.1 -f /index.html -p 40001 >| server.out
	@echo "killing l2_server"
	fuser -k 40001/tcp
	@echo "test_server completed, results in server.out"

test_mongoose : l2_client mongoose_server index.html
	@echo "running mongoose_server on port 40002"
	./mongoose_server -p 40002 &
	@echo "sending client request for /index.html"
	./l2_client -h 127.0.0.1 -f /index.html -p 40002 >| mongoose.out
	@echo "killing mongoose_server"
	fuser -k 40002/tcp
	@echo "test_mongoose completed, results in mongoose.out"

test: test_client test_server test_mongoose

clean :
	-@rm -f ./l2_client
	-@rm -f ./l2_server
	-@rm -f ./mongoose_server
	-@rm -f ./client.out
	-@rm -f ./server.out
	-@rm -f ./mongoose.out

view : l2_client.c l2_server.c mongoose_server.c utilities.c utilities.h
	$(PAGER) l2_client.c l2_server.c mongoose_server.c utilities.c utilities.h
