#include"random.h"
#include"lib.h"


#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>

using namespace std;

int main(){

Random Genny ;
Set_Genny_up(Genny);

double S0 = 100. ;
double K = 100. ;
double T = 1. ;
double r = 0.1 ;
double sigma = 0.25 ;
//double t = 0 ;

int M = pow(10 , 5) ;
int N = 100;
int L = (M/N) ;

double s = 0. ;

vector <double> sum_prog (N , 0.);
vector <double> sum_prog_2(N , 0.) ;
vector <double> errors (N , 0.) ;
vector <double> sum_prog_put (N , 0.);
vector <double> sum_prog_2_put(N , 0.) ;
vector <double> errors_put (N , 0.) ;

ofstream out;
out.open( "data.csv" );

if(!out){
    cout << "Errore apertura file!\n" ;
    return -1;
}

//CALL
for(int i = 0 ; i < N ; i++ ){
    s = 0. ;
    for (int j = 0 ; j < L ; j++){
        s += call(Genny , S0 , r , sigma , T , K) ;
    }
    s = (s/double(L)) ;

    if (i==0){
        sum_prog[i] = s ;
        sum_prog_2[i] = pow(s , 2);
    }

    else {
        sum_prog[i] = sum_prog[i-1] + s ;
        sum_prog_2[i] = sum_prog_2[i-1] + pow(s , 2) ;
    }
}

//media cumulativa
for(int i = 0 ; i<N ; i++){
    sum_prog[i] /= (i+1) ;
    sum_prog_2[i] /= (i+1) ;
    errors[i] = error( sum_prog , sum_prog_2 , i );
}


for ( int i = 0 ; i < N ; i++ )
	out << sum_prog[ i ] << "," << errors[ i ] << endl;


//PUT

for(int i = 0 ; i < N ; i++ ){
    s = 0. ;
    for (int j = 0 ; j<L ; j++){
        s += put(Genny , S0 , r , sigma , T , K ) ;
    }
    s /= double(L) ;

    if ( i==0 ){
        sum_prog_put[i] = s ;
        sum_prog_2_put[i] = pow(s , 2);
    }
    else{
    sum_prog_put[i] = sum_prog_put[i-1] + s ;
    sum_prog_2_put[i] = sum_prog_2_put[i-1] + pow(s , 2) ;
    }
}

//media cumulativa
for(int i = 0 ; i < N ; i++){
    sum_prog_put[i] /= (i+1) ;
    sum_prog_2_put[i] /= (i+1) ;
    errors_put[i] = error( sum_prog_put , sum_prog_2_put , i );
}

for ( int i = 0 ; i < N ; i++ )
	out << sum_prog_put[ i ] << "," << errors_put[ i ] << endl;



fill( sum_prog.begin() , sum_prog.end() , 0 );
fill( sum_prog_2.begin() , sum_prog_2.end() , 0 );
fill( errors.begin() , errors.end() , 0 );
fill( sum_prog_put.begin() , sum_prog_put.end() , 0 );
fill( sum_prog_2_put.begin() , sum_prog_2_put.end() , 0 );
fill( errors_put.begin() , errors_put.end() , 0 );

double t = 0. ;

for(int i = 0 ; i < N ; i++ ){
    s = 0. ;
    t = 0. ;
    for (int j = 0 ; j < L ; j++){
        s += call_2( Genny , S0 , r , sigma , T , K ) ;
        t += put_2( Genny , S0 , r , sigma , T , K ) ;
    }

    s = (s/double(L)) ;
    t = (t/double(L)) ;

    if (i==0){
        sum_prog[i] = s ;
        sum_prog_2[i] = pow(s , 2);
        sum_prog_put[i] = t ;
        sum_prog_2_put[i] = pow(t , 2);
    }

    else {
        sum_prog[i] = sum_prog[i-1] + s ;
        sum_prog_2[i] = sum_prog_2[i-1] + pow(s , 2) ;
        sum_prog_put[i] = sum_prog_put[i-1] + t ;
        sum_prog_2_put[i] = sum_prog_2_put[i-1] + pow(t , 2) ;
    }
}

//media cumulativa
for(int i = 0 ; i<N ; i++){
    sum_prog[i] /= (i+1) ;
    sum_prog_2[i] /= (i+1) ;
    errors[i] = error( sum_prog , sum_prog_2 , i );
    sum_prog_put[i] /= (i+1) ;
    sum_prog_2_put[i] /= (i+1) ;
    errors_put[i] = error( sum_prog_put , sum_prog_2_put , i );
}


for ( int i = 0 ; i < N ; i++ )
	out << sum_prog[ i ] << "," << errors[ i ] << endl;

for ( int i = 0 ; i < N ; i++ )
    out << sum_prog_put[ i ] << "," << errors_put[ i ] << endl;

out.close() ;

    return 0 ;
}
