#include"random.h"
#include"lib.h"


#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>

using namespace std;

//NON MI INTERESSA SALVARE TUTTA LA TRAIETTORIA COME IN FISICA STATISTICA!!!
int main(){

Random Genny;
Set_Genny_up(Genny);

int M = pow(10 , 5) ; //numero di simulazioni
int L = 100 ; //lunghezza del RW e numero di blocchi

ofstream out;
out.open( "data.csv" );

if(!out){
    cout << "Errore apertura file!\n" ;
    return -1;
}

vector<int> trajectory ;
vector<double> trajectory_3D ;

vector<double> sum_prog (L , 0);
vector<double> sum_prog_2 (L , 0);
vector<double> errors (L , 0);
vector<double> sum_3D (L , 0);
vector<double> sum_2 (L , 0);
vector<double> errors_3D (L , 0);

sum_prog[0] = 0 ;
sum_prog_2[0] = 0 ;
errors[0] = 0 ;
sum_3D[0] = 0 ;
sum_2[0] = 0 ;
errors_3D[0] = 0 ;

out << 0 << "," << 0 << "," << 0 << "," << 0 << endl;

double x = 0;
double y = 0;

for(int i = 1 ; i < L ; i++){

    for(int j = 0 ; j < L ; j++){
        for(int k = 0 ; k<int(M/L) ; k++ ){
        trajectory = make_trajectory( Genny , i );
        trajectory_3D = make_trajectory_3D( Genny , i );
        x += dist_from_origin_square( trajectory ) ;
        y += dist_from_origin_square( trajectory_3D ) ;
    }
    sum_prog[j] = sum_prog[j-1] + x/((M/L))  ;
    sum_prog_2[j] = sum_prog_2[j-1] + pow( x/((M/L)) , 2 ) ;
    sum_3D[j] = sum_3D[j-1] + y/((M/L))  ;
    sum_2[j] = sum_2[j-1] + pow( y/((M/L)) , 2 ) ;
    x = 0. ;
    y = 0. ;
    }

    for(int s = 0 ; s < L; s++){
        sum_prog[s] /= (s+1) ;
        sum_prog_2[s] /= (s+1) ;
        errors[s] = error( sum_prog , sum_prog_2 , s );
        sum_3D[s] /= (s+1) ;
        sum_2[s] /= (s+1) ;
        errors_3D[s] = error( sum_3D , sum_2 , s );
    }

    out << sqrt( sum_prog[99]) << "," << errors[99] << "," << sqrt( sum_3D[99] ) << "," << errors_3D[99] << endl;


    fill( sum_prog.begin() , sum_prog.end() , 0.);
    fill( sum_prog_2.begin() , sum_prog_2.end() , 0.);
    fill( errors.begin() , errors.end() , 0.);
    fill( sum_3D.begin() , sum_3D.end() , 0.);
    fill( sum_2.begin() , sum_2.end() , 0.);
    fill( errors_3D.begin() , errors_3D.end() , 0.);
}


out.close();


return 0;
}
