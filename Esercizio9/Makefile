CFLAGS = -Wall -O3 --std=c++11

main9.exe : main9.o popolazione.o lib.h popolazione.h
	g++ popolazione.o main9.o  -o main9.exe  $(CFLAGS) -larmadillo
main9.o : main9.cpp
	g++ -c main9.cpp -o main9.o $(CFLAGS)
popolazione.o : popolazione.cpp popolazione.h
	g++ -c popolazione.cpp -o popolazione.o $(CFLAGS)
clean :
	rm *.o main9.exe 
