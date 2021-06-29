#ifndef __lib_h__
#define __lib_h__

#include"random.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
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


double call( Random &Genny , double S0 , double r , double sigma , double T , double K ){
    double z = Genny.Gauss( 0. , 1. );
    double S = S0 * exp( (r - (pow(sigma,2)/2.))*T + sigma * z * sqrt(T) )  ;
    return exp(-r*T) * max( 0. , S - K ) ;
}

double put( Random &Genny , double S0 , double r , double sigma , double T , double K ){
    double z = Genny.Gauss( 0. , 1. );
    double S = S0 * exp( (r - (pow(sigma,2)/2.))*T + sigma * z * sqrt(T) )  ;
    return exp(-r*T) * max( 0. , K - S ) ;
}


double call_2( Random &Genny , double S0 , double r , double sigma , double T , double K ){
    double S = S0 ;
    for(int i = 0 ; i < 100 ; i++){
        double z = Genny.Gauss( 0. , 1. );
        S *= exp( ( r- pow(sigma , 2)/2. )*0.01 + sigma*z*sqrt(0.01) ) ;
    }
    return exp(-r*T) * max( 0. , S - K ) ;
}

double put_2( Random &Genny , double S0 , double r , double sigma , double T , double K ){
    double S = S0 ;
    for(int i = 0 ; i < 100 ; i++){
        double z = Genny.Gauss( 0. , 1. );
        S *= exp( ( r- pow(sigma , 2)/2. )*0.01 + sigma*z*sqrt(0.01) ) ;
    }
    return exp(-r*T) * max( 0. , K - S ) ;
}


#endif
