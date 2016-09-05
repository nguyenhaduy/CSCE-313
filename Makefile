all: linked_list.o main.cpp
	g++ -g linked_list.o main.cpp -o main
linked_list.o: linked_list.cpp linked_list.h
	g++ -c -g linked_list.cpp
clean:
	rm *.o main
