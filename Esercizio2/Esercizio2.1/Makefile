CC = g++
CFLAGS = -Wall -O3 --std=c++11


main2.1.exe : main2.1.o random.o lib.h IntegralMC.h IntegralMC.o integranda.h
	$(CC) random.o IntegralMC.o main2.1.o -o main2.1.exe
main2.1.o : main2.1.cpp lib.h integranda.h IntegralMC.h
	$(CC) -c main2.1.cpp -o main2.1.o  $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
IntegralMC.o : IntegralMC.h IntegralMC.cxx
	$(CC) -c IntegralMC.cxx -o IntegralMC.o $(CFLAGS)

clean :
	rm *.o main2.1.exe

cleanall :
	rm data.csv
