#ifndef __lib_h__
#define __lib_h__

#include"popolazione.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <fstream>
#include <armadillo>

using namespace std;
using namespace arma;

mat generate_positions(bool circle , int cities){

    mat pos( cities , 2 );
    double theta;

    if (circle == true){
        ofstream out ;
        out.open("pos_circle.csv");
        for (int i = 0 ; i < cities ; i++) {
            theta = 2*M_PI*randu();
            pos(i , 0) = cos(theta) ;
            pos(i , 1) = sin(theta) ;
            out << pos(i , 0) << " , " << pos(i , 1) << endl;
        }
        return pos ;
    }
    else{
        ofstream out ;
        out.open("pos_square.csv");
        for (int i = 0 ; i < cities ; i++){
            pos(i , 0) = randu()*2 - 1 ; //quadrato di lato 2 centrato nell'origine
            pos(i , 1) = randu()*2 - 1 ;
            out << pos(i , 0) << " , " << pos(i , 1) << endl;
        }
        return pos ;
    }

}

void print_population( mat population , bool cerchio){

    ofstream out ;
    out.open("config.txt",ios::app);

    if( cerchio==true ) {
        string s = "Città disposte su un cerchio. La popolazione è: " ;
        population.print(out , s);
        out << endl << endl;
    }
    else{
        string s = "Città disposte in un quadrato. La popolazione è: " ;
        population.print(out , s);
        out << endl << endl;
    }


}

double average_cost( Popolazione A ){

double cost = 0 ;

mat population = A.Get_population();

for( int i = 0 ; i < int( A.Get_dim() / 2.) ; i++){
    cost += A.Fitness(population.row(i)) ;
}

cost *= 2. / double(A.Get_dim()) ; 

return cost  ;

}


#endif
