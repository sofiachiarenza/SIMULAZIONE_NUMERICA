CFLAGS = -Wall -O3 --std=c++11

main102.exe : main102.o popolazione.o lib.h popolazione.h
	mpicxx popolazione.o main102.o  -o main102.exe  $(CFLAGS) -larmadillo
main102.o : main102.cpp
	mpicxx -c main102.cpp -o main102.o $(CFLAGS)
popolazione.o : popolazione.cpp popolazione.h
	mpicxx -c popolazione.cpp -o popolazione.o $(CFLAGS)
clean :
	rm *.o main102.exe
