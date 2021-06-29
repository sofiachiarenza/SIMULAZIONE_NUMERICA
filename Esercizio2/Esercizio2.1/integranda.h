#ifndef __Integranda_h__
#define __Integranda_h__

#include "funzionebase.h"


#include <cmath>
#include <vector>

using namespace std;

class Integranda : public FunzioneBase {

    public:
    Integranda () {};
    ~Integranda() {};

    double Eval( double x ) const override { return M_PI/2. * cos(M_PI*x/2.); } ;

};

class Integranda_IS : public FunzioneBase {
    public:
        Integranda_IS () {};
        ~Integranda_IS() {};

double Eval( double x ) const override { return M_PI/(4. * cos(M_PI*x/2.)); } ;
};

class Integranda_IS_2 : public FunzioneBase {
    public:
        Integranda_IS_2 () {};
        ~Integranda_IS_2() {};

double Eval( double x ) const override { return (M_PI/3.) * cos(M_PI*x/2.)/(1-pow(x,2.)); } ;
};

#endif
