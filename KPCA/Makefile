CC=g++
CFLAGS= -Wall -pedantic -Ofast -std=c++11 
LDFLAGS= -I Diploma/eigen

SRC= kpca.cpp main.cpp kernel_functions.cpp
MAIN= kpca

$(MAIN):
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(MAIN)

.PHONY:
	clean install

ifeq ($(OS),Windows_NT)
    install:
		copy $(MAIN).exe C:\Users\Public
else
	install:
		cp $(MAIN) /tmp/bin
endif
	

ifeq ($(OS),Windows_NT)
    clean:
		del $(MAIN).exe *.o *~
else
	clean:
		$(RM) $(MAIN) *.o *~
endif