CFLAGS = -Wall -O3 --std=c++11

main101.exe : main101.o popolazione.o lib.h popolazione.h
	g++ popolazione.o main101.o  -o main101.exe  $(CFLAGS) -larmadillo
main101.o : main101.cpp
	g++ -c main101.cpp -o main101.o $(CFLAGS)
popolazione.o : popolazione.cpp popolazione.h
	g++ -c popolazione.cpp -o popolazione.o $(CFLAGS)
clean :
	rm *.o main101.exe Circle.txt Square.txt
