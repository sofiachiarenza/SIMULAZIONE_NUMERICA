#ifndef __popolazione_h__
#define __popolazione_h__

#include "mpi.h"
#include<armadillo>
using namespace arma;

class Popolazione {

public:

  // costruttori
  Popolazione();
  Popolazione( int  , int  , mat );
  // distruttore
  ~Popolazione();

  // metodi
  int Get_dim() const { return m_dimensione ;} ;
  int Get_cities() const { return m_citta ;} ;
  mat Get_population() { return configurazione; } ;
  mat Get_positions() { return positions; }
  void Set_population( mat A ) { configurazione = A ; }
  void Check() ;
  mat Select();
  mat Crossover( mat );
  rowvec Mutate( rowvec ) ;
  double Fitness( rowvec ) ;
  void Order();
  void Evolve();

protected:
    int m_dimensione ;
    int m_citta ;
    mat configurazione ; //popolazione
    mat positions; //posizioni delle città
};

#endif
