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
    cout << "\nUsage: main09.1.exe <seed(int)>\n" << endl;
    return -5;
}

// set seed for reproducibility
int seed = atoi(argv[1]);
arma_rng::set_seed(seed);

//Setting parameters
int dim = 300 ;
int cities = 32 ;
int n_gen = 500 ;

//Generate positions on circle and in square
//positions are also printed on files
mat pos_circle = generate_positions(true , cities);
mat pos_square = generate_positions(false , cities);

Popolazione On_Circle(dim , cities , pos_circle) ;
Popolazione In_Square(dim , cities , pos_square) ;

On_Circle.Order();
In_Square.Order();


//Controllo che la popolazione si crei correttamente e la stampo per controllare anche a occhio
/*On_Circle.Check();
In_Square.Check();
assert( dim == On_Circle.Get_dim() );
assert( dim == In_Square.Get_dim() );
assert( cities == On_Circle.Get_cities() );
assert( cities == In_Square.Get_cities() );

mat config_circ = On_Circle.Get_population() ;
mat config_sq = In_Square.Get_population() ;

print_population( config_circ , true);
print_population( config_sq , false);*/


cout << "Cerco il percorso più breve sulla circonferenza. Farò 500 run." <<endl;

ofstream out_circle;
out_circle.open("cost_circle.csv");

mat config_circ( dim , cities ) = On_Circle.Get_population() ;
//Printing starting point
for (int k = 0; k < cities-1; k++)
    out_circle << config_circle(0,k) << ",";
out_circle << config_circle(0,cities-1) << endl;

for (int i = 0 ; i < n_gen ; i++){
    On_Circle.Evolve();
    config_circ = On_Circle.Get_population() ;
    //Stampo: best_cost , best_cost_averaged
    out_circle << On_Circle.Fitness(config_circ.row(0)) << " , " << average_cost( On_Circle ) << endl ;
}

out_circle.close();

ofstream out;
out.open("best_path_circle.txt");

for (int i = 0 ; i < cities ; i++)
    out << config_circ.at(0 , i) << endl;

out.close();

//(config_circ.row(0)).print("Miglior cammino:");
cout << "Lunghezza del miglior cammino: " << On_Circle.Fitness(config_circ.row(0)) << endl;


cout << "Cerco il percorso più breve nel quadrato. Farò 500 run." <<endl;

ofstream out_square;
out_square.open("cost_square.csv");

mat config_sq( dim , cities );

for (int i = 0 ; i < n_gen ; i++){
    In_Square.Evolve();
    config_sq = In_Square.Get_population() ;
    //Stampo: best_cost , best_cost_averaged
    out_square << In_Square.Fitness(config_sq.row(0)) << " , " << average_cost( In_Square ) << endl ;
}

out_square.close();

ofstream out2;
out2.open("best_path_square.txt");

for (int i = 0 ; i < cities ; i++)
    out2 << config_sq.at(0 , i) << endl;

out2.close();

//(config_sq.row(0)).print("Miglior cammino:");
cout << "Lunghezza del miglior cammino: " << In_Square.Fitness(config_sq.row(0)) << endl;


return 0;
}
