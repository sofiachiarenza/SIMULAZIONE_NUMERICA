#ifndef __IntegralMC_h__
#define __IntegralMC_h__

#include "random.h"
#include "funzionebase.h"

#include<vector>
#include<cstdlib>

using namespace std;

class IntegralMC {

	public :

	IntegralMC( Random &Genny ) { m_myrand = &Genny; };
	~IntegralMC() {};

	double IntegraleHoM( double , double , double , FunzioneBase* , int );
	double IntegraleHoM( double , double , double , FunzioneBase& , int );
	double IntegraleAVE( double , double , FunzioneBase* , int );
	double IntegraleAVE( double , double , FunzioneBase& , int );
	//Importance sampling: non usano la distribuzione uniforme per estrarre i punti!
	double IntegraleIS( double , double , FunzioneBase& , int );
	double IntegraleIS( double , double , FunzioneBase* , int );
	double IntegraleIS_2( double , double , FunzioneBase& , int );
	double IntegraleIS_2( double , double , FunzioneBase* , int );

    //Per più dimensioni
	//double Media ( const FunzioneScalareBase & , const vector<double>& inf, const vector<double>& sup, unsigned int punti );

	private :

	Random* m_myrand;

};

#endif

//preso da esercizio 10.3 di TNDS
