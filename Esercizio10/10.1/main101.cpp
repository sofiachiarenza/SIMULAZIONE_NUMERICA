#include"lib.h"

#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>
#include<armadillo>

using namespace std;
using namespace arma;

int main(int argc, char** argv){

if (argc != 2) {
    cout << "\nUsage: main10.1.exe <seed(int)>\n" << endl;
    return -5;
}

// set seed for reproducibility
int seed = atoi(argv[1]);
arma_rng::set_seed(seed);


int dim = 1 ;
int cities = 32 ;

mat pos_circle = generate_positions(true , cities);
mat pos_square = generate_positions(false , cities);

Popolazione On_Circle(dim , cities , pos_circle) ;
Popolazione In_Square(dim , cities , pos_square) ;


cout << "Looking for the shortest path that connects 32 cities randomly disposed on a circle." << endl;

ofstream out ;
out.open("Circle.txt");

vector <double> temperatures = { 4. , 3.5 , 3. , 2.5 , 2. , 1.5 , 1.25 , 1. , 0.75 , 0.5 , 0.4 , 0.3, 0.2 , 0.1};
int steps = pow(10 , 6) ; //Mosse proposte per ogni temperatura

rowvec best_circle ;
double T ;
double av_cost = 0 ;

//Le mosse proposte sono 6 milioni, non stampo il costo dopo ogni mossa ma una
//media del costo ogni 2000 in modo da non trascurare troppi dati

for (int j = 0 ; j < temperatures.size() ; j++ ){
    T = temperatures[j] ;
    cout << "Sto simulando a temperatura " << T << endl;
    for( int i = 0 ; i < steps ; i++ ){
        best_circle = Move( On_Circle , T ) ; //Move muta il cromosoma e accetta la mossa con prob dipendente da T
        av_cost += On_Circle.Fitness( best_circle );
        if( i%2000 == 0 ) {
            out << av_cost/2000 << endl ;
            av_cost = 0. ;
        }
    }
}

out.close();

ofstream out_path ;
out_path.open("best_path_circle.txt");
for( int i = 0 ; i < cities ; i++ ) out_path << best_circle(i) << endl;

out_path.close();


//SIMULATIONS INSIDE THE SQUARE

cout << "Looking for the shortest path that connects 32 cities randomly disposed inside a square." << endl;


ofstream out_sq ;
out_sq.open("Square.txt");

rowvec best_square ;
av_cost = 0. ;

for (int j = 0 ; j < temperatures.size() ; j++ ){
    T = temperatures[j] ;
    cout << "Sto simulando a temperatura " << T << endl;
    for( int i = 0 ; i < steps ; i++ ){
        best_square = Move( In_Square , T ) ;
        av_cost += In_Square.Fitness( best_square );
        if( i%2000 == 0 ) {
            out_sq << av_cost/2000 << endl ;
            av_cost = 0. ;
        }
    }
}

out_sq.close();

ofstream out_path_sq ;
out_path_sq.open("best_path_square.txt");
for( int i = 0 ; i < cities ; i++ ) out_path_sq << best_square(i) << endl;

out_path_sq.close();


return 0;
}
