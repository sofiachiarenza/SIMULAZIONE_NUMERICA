#ifndef __lib_h__
#define __lib_h__

#include"random.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include<vector>
#include <fstream>

using namespace std;

//definisco una funzione per il calcolo dell'errore

double error ( vector <double> AV , vector <double> AV2 ,  int N){
    if(N==0){
        return 0 ;
    }

    double var = (AV2[N] - pow(AV[N], 2))/N ;
    return pow( var , 0.5 ) ;
};

void Set_Genny_up( Random &Genny ){
    int seed[9];
    int p1, p2;
    ifstream Primes("Primes");
    if (Primes.is_open()){
       Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("seed.in");
    string property;
    if (input.is_open()){
       while ( !input.eof() ){
          input >> property;
          if( property == "RANDOMSEED" ){
             input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
             Genny.SetRandom(seed,p1,p2);
          }
       }
       input.close();
    } else cerr << "PROBLEM: Unable to open seed.in" << endl;
}


template <typename T> void print( T start, T end ) {
	for ( T it = start ; it != end; it++ )
		cout << *it << endl;
}

template <typename T> void print( const vector<T>& V , const char* filename ) {
	ofstream out;
    	out.open( filename );
	for ( int i = 0 ; i < V.size() ; i++ )
		out << V[ i ] << endl;
	out.close();
}

template <typename T> double media( const vector<T>& V ) {
	double a = 0;
	if ( V.size() == 0 )
		return a;
	for ( int i = 0 ; i < V.size() ; i++ )
		a = ( a * i + V[ i ] ) / ( i + 1 );
	return a;
}

template <typename T> double varianza( const vector<T>& V ) {
	double v = 0;
	double m = media( V );
	for ( int i = 0 ; i < V.size() ; i++ )
		v = v + pow( (V[i] - m), 2.0 );
	v = v / V.size() ;
	return v;
}


vector<int> make_trajectory( Random &Genny , int L ){
    vector<int> traj (3 , 0) ; //vettore contenente le posizioni x,y,z del RW

    if(L==0)
        return traj;

    for(int i = 0 ; i < L ; i++){ //L: lunghezza del cammino
        if( Genny.Rannyu() < 0.5 )                      //Passo in avanti o all'indietro equiprobabile
            traj[floor(Genny.Rannyu(0. , 3.))] += 1  ;
        else
            traj[floor(Genny.Rannyu(0. , 3.))] -= 1 ;
    }

return traj ;
}

vector<double> make_trajectory_3D( Random &Genny , int L ){
    vector<double> traj (3 , 0) ; //vettore contenente le posizioni x,y,z del RW

    if(L==0)
        return traj;

    double theta , phi ;

    for( int i = 0 ; i < L ; i++ ){
        theta = Genny.Rannyu(0 , M_PI) ;
        phi = Genny.Rannyu(0, 2*M_PI) ;

        traj[0] += sin(theta)*cos(phi) ;
        traj[1] += sin(theta)*sin(phi) ;
        traj[2] += cos(theta) ;

    }

return traj ;
}

template <typename T> double dist_from_origin_square( vector<T> A ){
    double x = 0. ;
    for(int i = 0 ; i < A.size() ; i++ )
        x += pow(A[i] , 2.) ;

    return x ;
}

#endif
