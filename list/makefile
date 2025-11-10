all: buildh list.o main.o link
	
buildh:
	g++ list.h

list.o: list.cpp
	g++ list.cpp -c
	echo list.cpp compiled

main.o: main.cpp
	g++ main.cpp -c
	echo main.cpp compiled

clean:
	rm *.o

link:
	g++ -Wall -Wextra -fsanitize=address list.o main.o -o main
