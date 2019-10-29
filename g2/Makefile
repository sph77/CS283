cc = gcc
PAGER = less
.PHONY : compile_server compile_client compile_genkey compile_keycrack clean view

compile_server : server.c csapp.c csapp.h utilities.c utilities.h rsa.c rsa.h
	@echo "compiling server"
	$(cc) -oserver -pthread server.c rsa.c utilities.c csapp.c -lm

server : compile_server
	@echo "running server"
	./server -p "${PORT}" -e "${E} ${C}" -d "${D} ${DC}"

compile_client : client.c csapp.c csapp.h utilities.c utilities.h rsa.c rsa.h
	@echo "compiling client"
	$(cc) -oclient -pthread client.c rsa.c utilities.c csapp.c -lm

client : compile_client
	@echo "running client"
	./client -s "${SERVER}" -p "${PORT}" -e "${E} ${C}" -d "${D} ${DC}"

compile_genkey : genkey.c rsa.c rsa.h
	@echo "compiling genkey"
	$(cc) -ogenkey genkey.c rsa.c -lm

genkey : compile_genkey
	@echo "running genkey"
	./genkey "${MPRIME}" "${NPRIME}"

compile_keycrack : keycrack.c rsa.c rsa.h
	@echo "compiling keycrack"
	$(cc) -okeycrack keycrack.c rsa.c -lm

keycrack : compile_keycrack
	@echo "running keycrack"
	./keycrack "${E}" "${C}"

clean :
	-@rm -f ./server
	-@rm -f ./client
	-@rm -f ./genkey
	-@rm -f ./keycrack

view : rsa.h rsa.c utilities.h utilities.c genkey.c keycrack.c client.c server.c
	$(PAGER) rsa.h rsa.c utilities.h utilities.c genkey.c keycrack.c client.c server.c
