CXX = gcc
FILES = main.o 

program: $(FILES)
	$(CXX) $(FILES) -o osh

main.o : main.c
	$(CXX) -c main.c

.PHONY: clean
clean:
	rm *.o osh

