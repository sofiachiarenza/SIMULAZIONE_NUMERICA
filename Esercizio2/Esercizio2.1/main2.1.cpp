#include"random.h"
#include"lib.h"
#include "IntegralMC.h"
#include "integranda.h"

#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>

using namespace std;


int main(){

Random Genny;

Set_Genny_up(Genny);

Integranda myfunction ; //funzione da integrare
Integranda_IS f ; //nuova integranda ~myfunction/cos^2(x)
Integranda_IS_2 g ; //nuova integranda ~myfunction/(1-x^2)
IntegralMC Kerla ( Genny ) ; //classe che calcola il valore dell'integrale

//stampo tutto sul file, ogni riga contiene le 3 stime dell'integrale nel blocco i, con relativo errore
 ofstream out;
 out.open( "data.csv" );

 if(!out){
     cout << "Errore apertura file!\n" ;
     return -1;
 }

 //Integrale col metodo della media
 vector <double> sum_prog ( N , 0.);
 vector <double> sum_prog_2( N , 0.) ;
 vector <double> errors ( N, 0.) ;

 //integrali con l'importance sampling
 vector <double> sum_prog_IS (N , 0.); //evoluzione del valore di I
 vector <double> sum_prog_2_IS (N , 0.) ; //valore quadratico, serve per l'errore
 vector <double> errors_IS (N , 0.) ;

 //integrali con un diverso importance sampling
 vector <double> sum_prog_IS_2 (N , 0.); //evoluzione del valore di I
 vector <double> sum_prog_2_IS_2 (N , 0.) ; //valore quadratico, serve per l'errore
 vector <double> errors_IS_2 (N , 0.) ;

 //calcolo i valori nel primo blocco, per cui non posso mediare
 double s = Kerla.IntegraleAVE( 0. , 1. , myfunction , L );
 sum_prog[0] =  s;
 sum_prog_2[0] = pow( s , 2) ;

 double t = Kerla.IntegraleIS( 0. , 1. , f , L );
 sum_prog_IS[0] =  t;
 sum_prog_2_IS[0] = pow( t , 2) ;

 double u = Kerla.IntegraleIS_2( 0. , 1. , g , L );
 sum_prog_IS_2[0] =  u;
 sum_prog_2_IS_2[0] = pow( u , 2) ;

 //ciclo sui blocchi, faccio tutte e tre le stime dell'integrale
  for(int i = 1 ; i < N; i++){
     s = Kerla.IntegraleAVE( 0. , 1. , myfunction , L );
     t = Kerla.IntegraleIS( 0. , 1. , f , L );
     u = Kerla.IntegraleIS_2( 0. , 1. , g , L);
     sum_prog[i] = sum_prog[i-1] + s ;
     sum_prog_2[i] = sum_prog_2[i-1] + pow(s , 2) ;
     sum_prog_IS[i] = sum_prog_IS[i-1] + t ;
     sum_prog_2_IS[i] = sum_prog_2_IS[i-1] + pow(t , 2) ;
     sum_prog_IS_2[i] = sum_prog_IS_2[i-1] + u;
     sum_prog_2_IS_2[i] = sum_prog_2_IS_2[i-1] + pow( u , 2) ;
 }


  //media cumulativa, nel precedente for ho solo sommato tutte le stime in modo progressivo
  for(int i = 0 ; i<N ; i++){
      sum_prog[i] /= (i+1) ;
      sum_prog_2[i] /= (i+1) ;
      errors[i] = error( sum_prog , sum_prog_2 , i );
      sum_prog_IS[i] /= (i+1) ;
      sum_prog_2_IS[i] /= (i+1) ;
      errors_IS[i] = error( sum_prog_IS , sum_prog_2_IS , i );
      sum_prog_IS_2[i] /= (i+1) ;
      sum_prog_2_IS_2[i] /= (i+1) ;
      errors_IS_2[i] = error( sum_prog_IS_2 , sum_prog_2_IS_2 , i );
  }

  for ( int i = 0 ; i < N ; i++ )
     out << sum_prog[ i ] << "," << errors[ i ] <<"," << sum_prog_IS[ i ] <<"," << errors_IS[i] <<","<< sum_prog_IS_2[ i ] <<"," << errors_IS_2[i] << endl;

  out.close();

 return 0;

}
