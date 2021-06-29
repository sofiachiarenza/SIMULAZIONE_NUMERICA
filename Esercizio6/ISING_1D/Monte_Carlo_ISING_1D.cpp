/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <iomanip>
#include "Monte_Carlo_ISING_1D.h"

using namespace std;

int main()
{
  Input(); //Inizialization
  for(int iblk=1; iblk <= nblk; ++iblk) //Simulation
  {
    Reset(iblk);   //Reset block averages
    for(int istep=1; istep <= nstep; ++istep)
    {
      Move(metro);
      Measure();
      Accumulate(); //Update block averages
    }
    Averages(iblk);   //Print results for current block
  }
  ConfFinal(); //Write final configuration

  return 0;
}


void Input(void)
{
  ifstream ReadInput,  Readconf;

  cout << "Classic 1D Ising model             " << endl;
  cout << "Monte Carlo simulation             " << endl << endl;
  cout << "Nearest neighbour interaction      " << endl << endl;
  cout << "Boltzmann weight exp(- beta * H ), beta = 1/T " << endl << endl;
  cout << "The program uses k_B=1 and mu_B=1 units " << endl;

//Read seed for random numbers
   int p1, p2;
   ifstream Primes("Primes");
   Primes >> p1 >> p2 ;
   Primes.close();

   string start ;
   cout << "First run? (y/n)" << endl;
   cin >> start ;

   if ( start == "y" ){
       ifstream input("seed.in");
       input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
       rnd.SetRandom(seed,p1,p2);
       input.close();
    }
    else {
        ifstream input( "seed.out" );
        if( !input ){
            cout << "Errore apertura seed.out!" << endl ;
            return ;
        }
        input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
        rnd.SetRandom(seed,p1,p2);
        input.close();
    }

//Read input informations
  ReadInput.open("input.dat");

  ReadInput >> temp ;
  beta = 1.0/temp ;
  cout << "Temperature = " << temp << endl;

  ReadInput >> nspin;
  cout << "Number of spins = " << nspin << endl;

  ReadInput >> J;
  cout << "Exchange interaction = " << J << endl;

  ReadInput >> h;
  cout << "External field = " << h << endl << endl;

  ReadInput >> metro; // if=1 Metropolis else Gibbs

  ReadInput >> nblk;

  ReadInput >> nstep;

  if( metro==1 ) cout << "The program perform Metropolis moves" << endl;
  else cout << "The program perform Gibbs moves" << endl;
  cout << "Number of blocks = " << nblk << endl;
  cout << "Number of steps in one block = " << nstep << endl << endl;
  ReadInput.close();


//Prepare arrays for measurements
  iu = 0; //Energy
  ic = 1; //Heat capacity
  im = 2; //Magnetization
  ix = 3; //Magnetic susceptibility

  n_props = 4; //Number of observables

//initial configuration
if (start == "y"){
    for (int i=0; i<nspin; ++i)
    {
      if(rnd.Rannyu() >= 0.5) s[i] = 1;
      else s[i] = -1;
    }
}
else {
    Readconf.open("config.final") ;
    for (int i=0; i<nspin; ++i){
        Readconf >> s[i] ;
    }
}


//Evaluate energy etc. of the initial configuration
  Measure();

//Print initial values for the potential energy and virial
  cout << "Initial energy = " << walker[iu]/(double)nspin << endl;
}


void Move(int metro){
    int o;
    double p, energy_old, energy_new, sm;
    double energy_up, energy_down;

    for(int i=0; i<nspin; ++i){
        attempted ++;

        //Select randomly a particle (for C++ syntax, 0 <= o <= nspin-1)
        o = (int)(rnd.Rannyu()*nspin);

        //Metropolis
        if(metro==1) {
            sm = s[o] ;
            energy_old = Boltzmann( sm , o );
            sm *= -1 ;
            energy_new = Boltzmann( sm , o );
            p = min( 1. , exp ( - beta * (energy_new - energy_old) ) ) ;
            if ( rnd.Rannyu() < p ) {
                s[o] = sm ;
                accepted ++ ;
            }
        }
        //Gibbs sampling
        else {
            energy_up = Boltzmann( 1 , o );
            energy_down = Boltzmann( -1 , o );
            p = 1./( 1. + exp ( beta*(energy_up-energy_down)) ) ;
            if ( rnd.Rannyu() < p ) s[o] = +1 ;
            else s[o] = -1 ;
            accepted ++ ;

        }
    }
}

double Boltzmann(int sm, int ip) {
  double ene = -J * sm * ( s[Pbc(ip-1)] + s[Pbc(ip+1)] ) - h * sm;
  return ene;
}

void Measure(){
  double u = 0.0, m = 0.0 ;

//cycle over spins
  for (int i=0; i<nspin; ++i)
  {
     u += -J * s[i] * s[Pbc(i+1)] - 0.5 * h * (s[i] + s[Pbc(i+1)]);
     m += s[i] ;
  }
  walker[iu] = u ;
  walker[im] = m ;
  walker[ix] = m*m ;
  walker[ic] = u*u ;
}

//Reset block averages
void Reset(int iblk) {

   if(iblk == 1){
       for(int i=0; i<n_props; ++i){
           glob_av[i] = 0;
           glob_av2[i] = 0;
       }
   }

   for(int i=0; i<n_props; ++i){
     blk_av[i] = 0;
   }

   blk_norm = 0;
   attempted = 0;
   accepted = 0;
}

//Update block averages
void Accumulate(void) {

   for(int i=0; i<n_props; ++i){
     blk_av[i] = blk_av[i] + walker[i];
   }

   blk_norm = blk_norm + 1.0;
}

//Print results for current block
void Averages(int iblk) {

   ofstream Ene, Heat, Mag, Chi;

    cout << "Block number " << iblk << endl;
    cout << "Acceptance rate " << accepted/attempted << endl << endl;


    //Energy
    stima_u = blk_av[iu]/blk_norm/(double)nspin;
    glob_av[iu]  += stima_u;
    glob_av2[iu] += stima_u*stima_u;
    err_u=Error(glob_av[iu],glob_av2[iu],iblk);

    if( iblk == nblk - 1 ){
        Ene.open("outputs/output.ene.txt",ios::app);
        Ene << temp <<  "," << stima_u << "," << glob_av[iu]/(double)iblk << "," << err_u << endl;
        Ene.close();
    }


    //Heat Capacity
    stima_c = (blk_av[ic]/blk_norm - pow( blk_av[iu]/blk_norm , 2 ))*beta*beta/(double)nspin ;
    glob_av[ic] += stima_c ;
    glob_av2[ic] += stima_c*stima_c;
    err_c = Error( glob_av[ic],glob_av2[ic],iblk) ;

    if( iblk == nblk - 1 ){
        Heat.open("outputs/output.heat.txt",ios::app);
        Heat << temp <<  "," << stima_c << "," << glob_av[ic]/(double)iblk << "," << err_c << endl;
        Heat.close();
    }

    //Susceptibility
    stima_x = beta*blk_av[ix]/blk_norm/(double)nspin ;
    glob_av[ix] += stima_x ;
    glob_av2[ix] += stima_x*stima_x;
    err_x=Error(glob_av[ix],glob_av2[ix],iblk);

    if( iblk == nblk - 1 ){
        Chi.open("outputs/output.x.txt",ios::app);
        Chi << temp <<  "," << stima_x << "," << glob_av[ix]/(double)iblk << "," << err_x << endl;
        Chi.close();
    }


    //Magnetization
    stima_m = blk_av[im]/blk_norm/(double)nspin ;
    glob_av[im] += stima_m ;
    glob_av2[im] += stima_m*stima_m;
    err_m=Error(glob_av[im],glob_av2[im],iblk);

    if ( iblk == nblk - 1 ){
        Mag.open( "outputs/output.mag.txt",ios::app );
        Mag << temp <<  "," << stima_m << "," << glob_av[im]/(double)iblk << "," << err_m << endl;
        Mag.close();
    }

    cout << "----------------------------" << endl << endl;
}



void ConfFinal(void)
{
  ofstream WriteConf;

  cout << "Print final configuration to file config.final " << endl << endl;
  WriteConf.open("config.final");
  for (int i=0; i<nspin; ++i)
  {
    WriteConf << s[i] << endl;
  }
  WriteConf.close();

  rnd.SaveSeed();
}

int Pbc(int i)  //Algorithm for periodic boundary conditions
{
    if(i >= nspin) i = i - nspin;
    else if(i < 0) i = i + nspin;
    return i;
}

double Error(double sum, double sum2, int iblk)
{
    if(iblk==1) return 0.0;
    else return sqrt((sum2/(double)iblk - pow(sum/(double)iblk,2))/(double)(iblk-1));
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
