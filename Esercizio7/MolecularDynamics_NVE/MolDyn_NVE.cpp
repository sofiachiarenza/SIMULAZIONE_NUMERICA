/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
#include <stdlib.h>     // srand, rand: to generate random number
#include <iostream>     // cin, cout: Standard Input/Output Streams Library
#include <fstream>      // Stream class to both read and write from/to files.
#include <cmath>        // rint, pow
#include <vector>
#include <sstream>
#include <string>
#include "MolDyn_NVE.h"

using namespace std;

int main(){
  Input();             //Inizialization
  //int nconf = 1;
  nblocks = 20 ; //numero di blocchi
  N = (nstep/10)/nblocks ;
  for(int istep=1; istep <= nstep; ++istep){
      Move();           //Move particles with Verlet algorithm
      if(istep%iprint == 0) cout << "Number of time-steps: " << istep << endl;
      if(istep%10 == 0){
          Measure();     //Properties measurement
          //ConfXYZ(nconf);//Write actual configuration in XYZ format //Commented to avoid "filesystem full"!
          //nconf += 1;
      }
  }

  cout << endl << "Calculating block averages..." << endl;


  ofstream outEP , outEK, outET, outG , outP;
  outEP.open( "ave_epot.dat" );
  outEK.open( "ave_ekin.dat" );
  outET.open( "ave_etot.dat" );
  outG.open( "ave_gofr.dat" );
  outP.open( "ave_pres.dat" );
  //outT.open( "ave_temp.dat" );

  ifstream Epot_in , Ekin_in , Etot_in , Gofr_in , Pres_in;
  Epot_in.open( "output_epot.dat" ) ;
  Ekin_in.open( "output_ekin.dat" ) ;
  Etot_in.open( "output_etot.dat" ) ;
  Gofr_in.open( "output_gofr.dat" ) ;
  Pres_in.open( "output_pres.dat" ) ;
  //Temp_in.open( "output_temp.dat" ) ;

  data_blocking( Epot_in , outEP ) ;
  data_blocking( Ekin_in , outEK ) ;
  data_blocking( Etot_in , outET ) ;
  //data_blocking( Temp_in , outT ) ;
  data_blocking_2D( Gofr_in , outG );
  data_blocking( Pres_in , outP );

  outG.close();
  outP.close();
  outEP.close();
  outET.close();
  outEK.close();


  ConfFinal();         //Write final configuration to restart

  cout  << "!!! Remember to ./clean.sh before the next execution !!!" << endl << endl;
  return 0;
}


void Input(void){ //Prepare all stuff for the simulation
  ifstream ReadInput,ReadConf,ReadPreConf;
  //double ep, ek, pr, et, vir;
  string equilib , start ;

  cout << "Do I need to equilibrate the system? (y/n)" << endl ;
  cin >> equilib ;
  if (equilib == "y"){
      cout << "Am i starting for the first time? (y/n)" << endl;
      cin >> start ;
}


  cout << "Classic Lennard-Jones fluid        " << endl;
  cout << "Molecular dynamics simulation in NVE ensemble  " << endl << endl;
  cout << "Interatomic potential v(r) = 4 * [(1/r)^12 - (1/r)^6]" << endl << endl;
  cout << "The program uses Lennard-Jones units " << endl;

  seed = 1;    //Set seed for random numbers
  srand(seed); //Initialize random number generator

  ReadInput.open("input.dat"); //Read input

  ReadInput >> temp;
  cout << "Temperature = " << temp << endl ;

  ReadInput >> npart;
  cout << "Number of particles = " << npart << endl;

  ReadInput >> rho;
  cout << "Density of particles = " << rho << endl;
  vol = (double)npart/rho;
  cout << "Volume of the simulation box = " << vol << endl;
  box = pow(vol,1.0/3.0);
  cout << "Edge of the simulation box = " << box << endl;

  ReadInput >> rcut; //cutoff del potenziale
  ReadInput >> delta; //passo di integrazione
  ReadInput >> nstep;
  ReadInput >> iprint;

  cout << "The program integrates Newton equations with the Verlet method " << endl;
  cout << "Time step = " << delta << endl;
  cout << "Number of steps = " << nstep << endl << endl;
  ReadInput.close();


  //Measure of g(r)
  igofr = 4; //index for g(r). Da qui in poi metto i 100 bin in walker
  n_props = n_props + nbins;
  bin_size = (box/2.0)/(double)nbins;


//Read initial configuration
    if ( start == "y"  ){
  cout << "Read initial configuration from file config.0 " << endl << endl;
  ReadConf.open("config.0");
  for (int i=0; i<npart; ++i){
    ReadConf >> x[i] >> y[i] >> z[i];
    x[i] = x[i] * box; //passo in unità di lennard jones
    y[i] = y[i] * box;
    z[i] = z[i] * box;
  }
  ReadConf.close();
}
else {
    cout << "Read initial configuration from file config.final" << endl << endl;
    ReadConf.open("config.final");
    for (int i=0; i<npart; ++i){
      ReadConf >> x[i] >> y[i] >> z[i];
      x[i] = x[i] * box; //passo in unità di lennard jones
      y[i] = y[i] * box;
      z[i] = z[i] * box;
    }
    ReadConf.close();
  }


//Prepare initial velocities
//si potrebbe campionare una distribuzione di MB ad una data T, più raffinato
//condizione iniziale
if ( start == "y" & equilib =="y" ){
   cout << "Prepare random velocities with center of mass velocity equal to zero " << endl << endl;
   double sumv[3] = {0.0, 0.0, 0.0};
   for (int i=0; i<npart; ++i){
     vx[i] = rand()/double(RAND_MAX) - 0.5;
     vy[i] = rand()/double(RAND_MAX) - 0.5;
     vz[i] = rand()/double(RAND_MAX) - 0.5;

     sumv[0] += vx[i];
     sumv[1] += vy[i];
     sumv[2] += vz[i];
   }

   for (int idim=0; idim<3; ++idim) sumv[idim] /= (double)npart;
   double sumv2 = 0.0, fs;
   for (int i=0; i<npart; ++i){
     vx[i] = vx[i] - sumv[0];
     vy[i] = vy[i] - sumv[1];
     vz[i] = vz[i] - sumv[2];

     sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
   }
   sumv2 /= (double)npart;

   fs = sqrt(3 * temp / sumv2);   // fs = velocity scale factor
   for (int i=0; i<npart; ++i){
     vx[i] *= fs;
     vy[i] *= fs;
     vz[i] *= fs;

     xold[i] = Pbc(x[i] - vx[i] * delta);
     yold[i] = Pbc(y[i] - vy[i] * delta);
     zold[i] = Pbc(z[i] - vz[i] * delta);
   }
}
else if (start != "y" & equilib== "y") {
    double sumv[3] = {0.0, 0.0, 0.0};

    ReadPreConf.open("config.almostfinal");

    for (int i=0; i<npart; i++){
        ReadPreConf >> xold[i] >> yold[i] >> zold[i] ;
        xold[i] *= box ;
        yold[i] *= box ;
        zold[i] *= box ;
    }

    ReadPreConf.close();

    for (int i=0; i<npart; i++){
        vx[i] = Pbc(x[i] - xold[i])/(2.0 * delta);
        vy[i] = Pbc(y[i]- yold[i])/(2.0 * delta);
        vz[i] = Pbc(z[i] - zold[i])/(2.0 * delta);

        sumv[0] += vx[i];
        sumv[1] += vy[i];
        sumv[2] += vz[i];
    }

    for (int idim=0; idim<3; ++idim) sumv[idim] /= (double)npart;
    double sumv2 = 0., fs ;
    for (int i=0; i<npart; ++i){
      vx[i] = vx[i] - sumv[0];
      vy[i] = vy[i] - sumv[1];
      vz[i] = vz[i] - sumv[2];

      sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
    }
    sumv2 /= (double)npart;

    fs = sqrt(3 * temp / sumv2);   // fs = velocity scale factor
    for (int i=0; i<npart; ++i){
      vx[i] *= fs;
      vy[i] *= fs;
      vz[i] *= fs;

      xold[i] = Pbc(x[i] - vx[i] * delta);
      yold[i] = Pbc(y[i] - vy[i] * delta);
      zold[i] = Pbc(z[i] - vz[i] * delta);
  }
}

else if ( equilib != "y" ){
    ReadPreConf.open("config.almostfinal");

    for (int i=0; i<npart; i++){
        ReadPreConf >> xold[i] >> yold[i] >> zold[i] ;
        xold[i] *= box ;
        yold[i] *= box ;
        zold[i] *= box ;
    }

    ReadPreConf.close();
}

   return;
}


void Move(void){ //Move particles with Verlet algorithm
  double xnew, ynew, znew, fx[m_part], fy[m_part], fz[m_part];

  for(int i=0; i<npart; ++i){ //Force acting on particle i nelle 3 direzioni
    fx[i] = Force(i,0);
    fy[i] = Force(i,1);
    fz[i] = Force(i,2);
  }

  for(int i=0; i<npart; ++i){ //Verlet integration scheme

    xnew = Pbc( 2.0 * x[i] - xold[i] + fx[i] * pow(delta,2) );
    ynew = Pbc( 2.0 * y[i] - yold[i] + fy[i] * pow(delta,2) );
    znew = Pbc( 2.0 * z[i] - zold[i] + fz[i] * pow(delta,2) );

    //velocità: le usiamo per energia cinetica e temperatura
    vx[i] = Pbc(xnew - xold[i])/(2.0 * delta);
    vy[i] = Pbc(ynew - yold[i])/(2.0 * delta);
    vz[i] = Pbc(znew - zold[i])/(2.0 * delta);


    xold[i] = x[i];
    yold[i] = y[i];
    zold[i] = z[i];

    x[i] = xnew;
    y[i] = ynew;
    z[i] = znew;
  }
  return;
}

double Force(int ip, int idir){ //Compute forces as -Grad_ip V(r)
  double f=0.0;
  double dvec[3], dr;

  for (int i=0; i<npart; ++i){
    if(i != ip){
      dvec[0] = Pbc( x[ip] - x[i] );  // distance ip-i in pbc - minimum image convention
      dvec[1] = Pbc( y[ip] - y[i] );
      dvec[2] = Pbc( z[ip] - z[i] );

      dr = dvec[0]*dvec[0] + dvec[1]*dvec[1] + dvec[2]*dvec[2];
      dr = sqrt(dr);

      if(dr < rcut){
        f += dvec[idir] * (48.0/pow(dr,14) - 24.0/pow(dr,8)); // -Grad_ip V(r)
      }
    }
  }

  return f;
}

void Measure(){ //Properties measurement
  double v, t, vij , w , wij;
  double dx, dy, dz, dr, dv;
  ofstream Gofr , Pres , Epot , Ekin , Etot ;


  Epot.open( "output_epot.dat",ios::app );
  Ekin.open( "output_ekin.dat",ios::app );
  //Temp.open( "output_temp.dat",ios::app );
  Etot.open( "output_etot.dat",ios::app );
  Gofr.open( "output_gofr.dat",ios::app );
  Pres.open( "output_pres.dat",ios::app );



  v = 0.0; //reset observables
  t = 0.0;
  w = 0.0;

//reset the hystogram of g(r)
for (int k=0; k < nbins ; ++k) walker[k]=0.0;


//cycle over pairs of particles
  for (int i=0; i<npart-1; ++i){
    for (int j=i+1; j<npart; ++j){

     dx = Pbc( xold[i] - xold[j] ); // here I use old configurations [old = r(t)]
     dy = Pbc( yold[i] - yold[j] ); // to be compatible with EKin which uses v(t)
     dz = Pbc( zold[i] - zold[j] ); // => EPot should be computed with r(t)

     dr = dx*dx + dy*dy + dz*dz;
     dr = sqrt(dr);

     int bin = int( dr / bin_size );
     if ( bin < 100 ) walker[ bin ] += 2.0 ;

     if(dr < rcut){
         vij = 4.0/pow(dr,12) - 4.0/pow(dr,6);
         wij = 1.0/pow(dr,12) - 0.5/pow(dr,6);
//Potential energy and virial
       v += vij;
       w += wij;
    }
    }
  }

  //pressure
  w = 48.0 * w / 3.0;

  double stima_pres = rho * temp + (w / vol) ;
  Pres << stima_pres << endl ;

//Kinetic energy
  for (int i=0; i<npart; ++i) t += 0.5 * (vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i]);


 // g(r)
 for(int i=0; i < nbins ; i++){
       dv = 4.*M_PI*( pow(bin_size*(double)(i+1.), 3. ) - pow( bin_size*(double)(i), 3.))/3.;
       walker[i] /= rho * dv * (double)npart;
   }

    stima_pot = v/(double)npart; //Potential energy per particle
    stima_kin = t/(double)npart; //Kinetic energy per particle
    //stima_temp = (2.0 / 3.0) * t/(double)npart; //Temperature
    stima_etot = (t+v)/(double)npart; //Total energy per particle

    Epot << stima_pot  << endl;
    Ekin << stima_kin  << endl;
    //Temp << stima_temp << endl;
    Etot << stima_etot << endl;

    for (int i = 0 ; i < nbins ; i++ ){
        Gofr << walker[ i ] ;
        if ( i != nbins-1 ) Gofr << ",";
        else Gofr << endl;
    }

    Epot.close();
    Ekin.close();
    //Temp.close();
    Etot.close();
    Gofr.close();
    Pres.close();

    return;
}


void ConfFinal(void){ //Write final configuration
  ofstream WriteConf;
  ofstream WritePreConf;


  cout << endl << "Print final configuration to file config.final " << endl;
  WriteConf.open("config.final");


  for (int i=0; i<npart; ++i){
      WriteConf << x[i]/box << "   " <<  y[i]/box << "   " << z[i]/box << endl;
  }
  WriteConf.close();


  cout <<"Print the almost final configuration to config.almostfinal " << endl << endl;
  WritePreConf.open("config.almostfinal");

  for (int i=0; i<npart; ++i){
    WritePreConf << xold[i]/box << "   " <<  yold[i]/box << "   " << zold[i]/box << endl;
  }
  WritePreConf.close();


  return;
}

void ConfXYZ(int nconf){ //Write configuration in .xyz format
  ofstream WriteXYZ;

  WriteXYZ.open("frames/config_" + to_string(nconf) + ".xyz");
  WriteXYZ << npart << endl;
  WriteXYZ << "This is only a comment!" << endl;
  for (int i=0; i<npart; ++i){
    WriteXYZ << "LJ  " << Pbc(x[i]) << "   " <<  Pbc(y[i]) << "   " << Pbc(z[i]) << endl;
  }
  WriteXYZ.close();
}

double Pbc(double r){  //Algorithm for periodic boundary conditions with side L=box
    return r - box * rint(r/box);
}

double error ( vector <double> AV , vector <double> AV2 ,  int N){
    if(N==0){
        return 0 ;
    }
    double var = abs(AV2[N] - pow(AV[N], 2))/N ;
    return pow( var , 0.5 ) ;
};

void data_blocking( ifstream & in , ofstream & out ){
    vector <double> sum_prog ( nblocks , 0. );
    vector <double> sum_prog_2( nblocks , 0. ) ;
    vector <double> errors ( nblocks, 0. ) ;

    double input = 0. ;
    double s = 0. ;

    for( int i = 0 ; i < nblocks ; i ++){
        for( int j = 0 ; j < N ; j++ ){
            in >> input ;
            s += input ;
        }
        s /= double(N) ;

        if (i==0){
          sum_prog[i] = s ;
          sum_prog_2[i] = pow(s , 2);
      }

      else {
          sum_prog[i] = sum_prog[i-1] + s ;
          sum_prog_2[i] = sum_prog_2[i-1] + pow(s , 2) ;
      }
      s = 0. ;
  }

  for(int i = 0 ; i < nblocks ; i++){
      sum_prog[i] /= (i+1) ;
      sum_prog_2[i] /= (i+1) ;
      errors[i] = error( sum_prog , sum_prog_2 , i );
  }


  for ( int i = 0 ; i < nblocks ; i++ )
  	out << sum_prog[ i ] << " , " << errors[ i ] << endl;
}


void data_blocking_2D( ifstream & in , ofstream & out ){
    //Data blocking for g(r)
    vector < vector<double> > sum_prog ( nbins , vector<double>( nblocks , 0. )  ) ;
    vector < vector<double> > sum_prog_2( nbins , vector<double>( nblocks , 0. )  ) ;
    vector < vector<double> > errors ( nbins , vector<double>( nblocks , 0. )  ) ;

    vector <double> input;
    string aux ;
    vector <double> s ( nbins , 0. ) ;


    for( int i = 0 ; i < nblocks ; i ++) {
        for( int j = 0 ; j < N ; j++ ){
            input.clear(); //pulisco il vettore che contiene la riga
            getline( in , aux ) ;

            //Converto la stringa contenente g(r) in un vettore di double
            stringstream ss( aux );
            while( ss.good() ){
                string substr ;
                getline( ss , substr , ',');
                input.push_back( stod( substr ) );
            }

            for ( int k = 0 ; k < nbins ; k++ ) s[k] += input[k] ;
        }

        for ( int k = 0 ; k < nbins ; k++ ) s[k] /= double(N) ;


        if (i==0){
          for (int k = 0 ; k < nbins ; k++){
              sum_prog[k][i] = s[k] ;
              sum_prog_2[k][i] = s[k]*s[k];
          }
      }

      else {
          for (int k = 0 ; k < nbins ; k++){
              sum_prog[k][i] = sum_prog[k][i-1] + s[k] ;
              sum_prog_2[k][i] = sum_prog_2[k][i-1] + (s[k]*s[k]) ;
          }
      }

      fill( s.begin() , s.end() , 0. ) ;
  }


  for(int i = 0 ; i < nblocks ; i++){
      for (int k = 0 ; k < nbins ; k++){
          sum_prog[k][i] /= double(i+1) ;
          sum_prog_2[k][i] /= double(i+1) ;
      }
  }

  for (int k = 0 ; k < nbins ; k++ ){
      for ( int i = 0 ; i < nblocks ; i++){
          if (i==0) errors[k][i] = 0. ;
          else {
              errors[k][i] = error( sum_prog[k] , sum_prog_2[k] , i );
          }
      }
  }

  for ( int k = 0 ; k < nbins ; k++ )
    out << sum_prog[ k ][ nblocks - 1  ] << " , " << errors[ k ][ nblocks - 1  ] << endl;

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
