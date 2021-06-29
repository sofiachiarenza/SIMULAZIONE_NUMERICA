#include "lib.h"
#include <mpi.h>

#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>
#include<armadillo>

using namespace std;
using namespace arma;

int main(int argc , char* argv[]){

int size , rank;
MPI_Init(&argc, &argv);
MPI_Comm_size( MPI_COMM_WORLD, &size );
MPI_Comm_rank( MPI_COMM_WORLD, &rank );

if (argc != 2) {
    cout << "\nUsage: main102.exe <seed(int)>\n" << endl;
    return -5;
}

// set seed
int seed;
if (rank == 0)
    seed = atoi(argv[1]);
else
    seed = rank;
arma_rng::set_seed(seed);


//Parameters
int n_gen = 1000;       // #evolutions
int dim = 300;          // how many cromos in the population
int n_migr = 25;        // how often to migrate
int n_send = 10;        // best cromos to migrate
int cities = 32;        // length of each cromo

//Generate positions on square and send them to all the nodes
mat pos_square = generate_positions(cities);

// convert from arma::colvec to double[]
double pos_x[cities];
double pos_y[cities];
for (int i = 0; i < cities; i++){
    pos_x[i] = pos_square(i,0);
    pos_y[i] = pos_square(i,1);
}

//send the same positions to all the nodes
if (size > 1) {
    MPI_Bcast( pos_x , cities, MPI_DOUBLE_PRECISION , 0 , MPI_COMM_WORLD);
    MPI_Bcast( pos_y , cities, MPI_DOUBLE_PRECISION , 0 , MPI_COMM_WORLD);
}

//Initialize the node's positions to the ones received
for (int i = 0; i < cities; i++){
    pos_square(i,0) = pos_x[i];
    pos_square(i,1) = pos_y[i];
}

//and print them
ofstream out ;
if( rank == 0 && size > 1 ){
    out.open("pos_square.csv");
    //out << "seed = " << seed << endl;
    for( int i = 0 ; i < cities ; i++ )
        out << pos_x[i] << " , " << pos_y[i] << endl;
}

//creo i vettori e le variabili utili per mandare e ricevere cromosomi
int best_cromos[n_send*cities];
int migrated_cromos[n_send*cities*size];
mat config(dim, cities);
rowvec cromo(cities);
int n_print = 1 ;

//Create the population
Popolazione Pop( dim , cities , pos_square );
Pop.Order();

// print starting point
config = Pop.Get_population();
out.open("cost"+to_string(rank)+".csv");
out << Pop.Fitness(config.row(0)) << ",";
out.close();
out.open("best_cromo"+to_string(rank)+".csv");
for (int k = 0; k < cities-1; k++)
    out << config(0,k) << ",";
out << config(0,cities-1) << endl;
out.close();

for (int i = 0 ; i < n_gen ; i++){

    // show evolution of best cromo length
    if (i%n_print == 0 && i != 0) {
        config = Pop.Get_population();
        out.open("cost"+to_string(rank)+".csv", ios::app);
        out << Pop.Fitness(config.row(0)) << ",";
        out.close();
    }

    Pop.Evolve();

    //Migrating best chromos
    // migrate best cromos every n_migr generations
    if (i%n_migr == 0 && size > 1) {

        Pop.Order();
        config = Pop.Get_population();

        // prepare vector to send
        //Tutti i 10 cromosomi da mandare sono scritti in un unico vettore
        for (int j = 0; j < n_send; j++)
            for (int h = 0; h < cities; h++)
               best_cromos[h+j*cities] = config(j, h);

        // migration
        MPI_Allgather(best_cromos, n_send*cities, MPI_INTEGER, migrated_cromos, n_send*cities, MPI_INTEGER, MPI_COMM_WORLD);

        // Aggiorno la popolazione, ogni nodo riceve 10*size cromosomi dagli altri processi
        // e con questi sostituisce i sui elementi peggiori
        for (int i = 0; i < n_send*size; i++) {
            for (int k = 0; k < cities; k++)
                cromo(k) = migrated_cromos[k+i*cities];
            config.row(dim-i-1) = cromo;
        }
        Pop.Set_population(config);
    }
}

Pop.Order();

// print best cromo length
config = Pop.Get_population();
out.open("cost"+to_string(rank)+".csv", ios::app);
out << Pop.Fitness(config.row(0)) << ",";
out.close();

// print best cromo
out.open("best_cromo"+to_string(rank)+".csv", ios::app);
for (int k = 0; k < cities-1; k++)
    out << config(0,k) << ",";
out << config(0,cities-1) << endl;
out.close();


MPI_Finalize();

return 0;
}
