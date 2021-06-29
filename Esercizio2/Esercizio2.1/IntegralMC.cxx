#include "IntegralMC.h"


double IntegralMC::IntegraleHoM( double xmin , double xmax , double fmax , FunzioneBase* f , int npunti ) {

	double x = 0;
	double y = 0;
	double nhit = 0;	//punti accettati

	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Rannyu( xmin , xmax );
		y = m_myrand->Rannyu( 0. , fmax );
		if ( y < f->Eval( x ) )
			nhit++;
	}

	return ( fmax * (xmax-xmin) *((double)nhit/(double)npunti) );
}


double IntegralMC::IntegraleHoM( double xmin , double xmax , double fmax , FunzioneBase& f , int npunti ) {

	double x = 0;
	double y = 0;
	double nhit = 0;	//punti accettati

	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Rannyu( xmin , xmax );
		y = m_myrand->Rannyu( 0. , fmax );
		if ( y < f.Eval( x ) )
			nhit++;
	}

	return ( fmax * (xmax-xmin) *((double)nhit/(double)npunti) );

}

double IntegralMC::IntegraleAVE( double xmin , double xmax , FunzioneBase* f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Rannyu( xmin , xmax );
		sum = sum + f->Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}


double IntegralMC::IntegraleAVE( double xmin , double xmax , FunzioneBase& f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Rannyu( xmin , xmax );
		sum = sum + f.Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}

double IntegralMC::IntegraleIS( double xmin , double xmax , FunzioneBase& f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Imp_Sampling( xmin , xmax , 1.); //1 è il massimo della distribuzione
		sum = sum + f.Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}

double IntegralMC::IntegraleIS( double xmin , double xmax , FunzioneBase* f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Imp_Sampling( xmin , xmax , 1.);
		sum = sum + f->Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}

double IntegralMC::IntegraleIS_2( double xmin , double xmax , FunzioneBase& f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Imp_Sampling_2( xmin , xmax , 1.5);
		sum = sum + f.Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}

double IntegralMC::IntegraleIS_2( double xmin , double xmax , FunzioneBase* f , int npunti ) {

	double sum = 0;
	double x = 0;
	for ( int i = 0 ; i < npunti ; i++ ) {
		x = m_myrand->Imp_Sampling_2( xmin , xmax , 1.5);
		sum = sum + f->Eval( x );
	}

	return ( sum * (xmax-xmin) /(double)npunti );
}


/*double IntegralMC::Media ( const FunzioneScalareBase &f , const vector<double>& inf, const vector<double>& sup, unsigned int punti ){

	double sum = 0;
	vector<double> x = { 0. , 0. } ;
	int k = 0;
	for ( int i = 0 ; i < punti ; i++ ) {
		x[0] = m_myrand->Rannyu( inf[0] , sup[0] ); //estraggo le x tra xmin e xmax
		x[1] = m_myrand->Rannyu( inf[1] , sup[1] ); //estraggo le y tra ymin e ymax

		sum = sum + f.Eval( x );
		k++;
	}

	return ( sum * (sup[0]-inf[0]) * ( sup[1]-inf[1] ) /(double)k );

}*/
