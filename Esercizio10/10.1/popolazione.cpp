#include"popolazione.h"
#include<cmath>
#include<armadillo>

using namespace std;
using namespace arma;

Popolazione::Popolazione(){
    m_dimensione = 100;
    m_citta = 32 ;
    configurazione.set_size( m_dimensione , m_citta );

    rowvec A = regspace<rowvec>(1 , m_citta) ;

    for(int i = 0 ; i < m_dimensione ; i++){
        A.subvec( 1 , m_citta-1 ) = shuffle( A.subvec( 1 , m_citta-1 ) ) ;
        configurazione.row(i) = A ;
    }

    //Position are set to zeros
    positions = zeros<mat>(m_citta, 2);

}

Popolazione::Popolazione(int dim, int citta, mat pos){
    m_dimensione = dim;
    m_citta = citta ;
    configurazione.set_size( m_dimensione , m_citta );

    rowvec A = regspace<rowvec>(1 , m_citta) ;

    for(int i = 0 ; i < m_dimensione ; i++){
        A.subvec( 1 , m_citta-1 ) = shuffle( A.subvec( 1 , m_citta-1 ) ) ;
        configurazione.row(i) = A ;
    }

    positions = pos ;

    //Check();

}


Popolazione::~Popolazione() {}

void Popolazione::Check(){
    colvec a( m_dimensione , fill::ones );
    bool is_ones = ( size(find( configurazione.col(0) == a )) == size(a) ) ;
    if ( !is_ones ){
        cout << "Errore!!1!!!1!1!1! Parti da città diverse! " << endl;
        return ;
    }

    for(int i = 0 ; i < m_dimensione ; i++){
        bool ok = ( size( unique(configurazione.row(i)) ) == size(regspace<rowvec>(1 , m_citta)) ) ;
        if ( !ok ){
            cout << "Errore!!1!!!1!1!1! Visiti più volte la stessa città! " << endl;
            return ;
        }
    }
}

double Popolazione::Fitness(rowvec cromo){
    double cost = 0. ;
    int index1 , index2 ;

    for (int i = 0 ; i < m_citta - 2 ; i++ ){
        index1 = int(cromo(i)) - 1 ;
        index2 = int(cromo(i+1)) - 1 ;
        cost +=  pow( positions.at(index1 , 0)-positions.at(index2 , 0) , 2. )
                + pow( positions.at(index1 , 1)-positions.at(index2 , 1) , 2. )  ;

    }
    cost += pow( positions.at( index2 , 0)-positions.at(0 , 0) , 2. ) + pow(positions.at( index2 , 1)-positions.at(0 , 1) , 2. ) ;
    //assert(isnormal(cost));
    return cost ;
}

void Popolazione::Order(){
    //cout << "Hey, you're ordering!" << endl;
    uvec indices(m_dimensione) ;

    for ( int i = 0 ; i < m_dimensione ; i++ ){
        indices.at(i) = Fitness( configurazione.row(i) ) ;
    }

    //I cromosomi migliori sono i primi, indices contiene i fitness di tutti i cromosomi
    //Sort_index restituisce un vettore di interi, che sono gli indici degli elementi di indices ordinati
    uvec sorted_indeces = sort_index( indices , "ascend" );

    //sorted_indeces.print("Stampo gli indici riordinati");

    mat copy = configurazione ;
    for (int i = 0 ; i < m_dimensione ; i++ ){
        configurazione.row(i) = copy.row( sorted_indeces.at(i) );
    }

}

mat Popolazione::Select(){
    //cout << "Hey, you're selecting!" << endl;
    //genero i numeri casuali in [0,1)
    double r = randu<double>();
    while( r == 1. ) r = randu<double>();

    double s = randu<double>();
    while( s == 1. ) s = randu<double>();

    /*cout << "r = " << r << endl;
    cout << "s = " << s << endl;*/

    double r1 = int ( m_dimensione * pow( r , 4. ) ) + 1. ;
    double r2 = int ( m_dimensione * pow( s , 4. ) ) + 1. ;

    if( r1==m_dimensione  ) r1-- ;
    if( r2==m_dimensione  ) r2-- ;

    mat parents(2 , m_citta);
    parents.row(0) = configurazione.row(r1);
    parents.row(1) = configurazione.row(r2) ;

    return parents ;

}

rowvec Popolazione::Mutate(rowvec cromo){

    //cout << "Hey, you're mutating!" << endl;
    int m , r1 , r2 , index , N;

    //Mutazione 1
    //Swap di due geni
    if (randu<double>( ) < 0.5){
        r1 = randi<double>( distr_param( 1 , m_citta - 1 ) ) ;
        r2 = randi<double>( distr_param( 1 , m_citta - 1 ) ) ;
        cromo.swap_cols( r1 , r2 ) ;
        //cout << "Mutazione 1 eseguita!" << endl;
    }

    // mutation 1.1
    //      swap two random adjacent cities
    if (randu() < 0.5){
        r1 = randi<double>( distr_param(1, m_citta - 2) );
        //double f = fitness(cromo);
        cromo.swap_cols(r1, r1+1);
        /*if (fitness(offspring) > f)
            offspring.swap_cols(r1, r1+1);*/ //Funziona meglio ma con una valutazione del costo non è più una mutazione casuale
    }

    //Mutazione 2
    //Shift di una parte di cromosona
    if (randu<double>() < 0.5){
        //cout << "Starting mutazione 2 " << endl;
        //cromo.print("Cromosoma iniziale:");
        index = randi<double>( distr_param( 1 , m_citta - 2 ) ) ;
        N = randi<double>( distr_param( 1 , m_citta - index - 1 ) ) ; //di quante posizioni shiftare
        cromo.subvec( index , m_citta - 1 ) = shift( cromo.subvec( index , m_citta - 1 ) , N );
        //cout << "Mutazione 2 eseguita!" << endl;
        //cromo.print("Cromosoma mutato:");
    }


    //Mutazione 3
    //permutation among m contiguous cities with other m contiguous cities
    if (randu<double>() < 0.5){
        //cout << "Starting mutazione 3" << endl;
        m = randi<double>( distr_param(1, int(( m_citta - 1 )/2.) ) ) ;
        r1 = randi<double>( distr_param( 1, m_citta - 2 )) ;
        if ( ((r1 + m + m_citta)%m_citta) < ((r1 - m + 2*m_citta - 1)%m_citta) )
            r2 = randi<double>( distr_param( (r1 + m + m_citta)%m_citta , (r1 - m + 2*m_citta - 1)%m_citta ));
        else r2 = randi<double>( distr_param( (r1 - m + 2*m_citta - 1)%m_citta  , (r1 + m + m_citta)%m_citta ));


        //To keep 1 fixed i do operations oh the chromosome without its head.
        rowvec h = cromo.tail( m_citta - 1 );

        //cout << "M = " << m << "\t r1 = " << r1 << "\t r2 = " << r2 << endl ;
        //h.print("Chromo:");

        for (int i = 0 ; i < m ; i ++){
            h.swap_cols(( r1 + i )%(m_citta-1) , ( r2 + i )%(m_citta-1));
        }
        //h.print("Chromo after mutation");
        rowvec testa (1 , fill::ones) ;
        cromo = join_rows( testa , h );
        //cout << "Mutazione 3 eseguita!" << endl;
    }


    //Mutazione 4
    //Inversione dell'ordine di m città adiacenti
    if (randu<double>() < 0.5 ){
        //cout << "Starting mutazione 4 " << endl;

        index = randi<double>( distr_param(1 , m_citta - 2 )) ;
        m = randi<double>( distr_param(1 , m_citta - index )) ;
        //cout << "Index = " << index << "\t m = " << m << endl;
        //cromo.print("Cromo:");

        cromo.subvec( index , index + m - 1 ) = reverse(cromo.subvec( index , index + m - 1 )) ;
        //cromo.print("Cromo after mutation:");
        //cout << "Mutazione 4 eseguita!" << endl;
    }

    return cromo ;
}

mat Popolazione::Crossover(mat parents){
    //cout << "Hey, you're crossovering!" << endl;
    //Genitori
    rowvec mother = parents.row(0) ;
    rowvec father = parents.row(1) ;

    //Indice a cui tagliare il cromosoma
    int index = randi<double>( distr_param(1 , m_citta - 1) ) ;

    //Taglio i vettori
    rowvec head_mother = mother.head( index + 1 );
    rowvec tail_mother = mother.tail( m_citta - 1 - index );
    rowvec head_father = father.head( index + 1 );
    rowvec tail_father = father.tail( m_citta - 1 - index );

    //Figlio 1 da mother's head
    rowvec tail1( m_citta - 1 - index , fill::zeros);

    int counter = 0 ;
    rowvec temp;

    for (int i = 0; i < m_citta; i++) {
        temp = intersect(rowvec{father(i)}, tail_mother);
        if (temp.n_cols > 0) {
            tail1(counter) = temp(0);
            counter++ ;
        }
    }


    //Figlio 2 da father's head
    rowvec tail2( m_citta - 1 - index , fill::zeros);

    counter = 0 ;

    for (int i = 0; i < m_citta; i++) {
        temp = intersect(rowvec{mother(i)}, tail_father);
        if (temp.n_cols > 0) {
            tail2(counter) = temp(0);
            counter++;
        }
    }

    rowvec figlio1 = join_rows(head_mother, tail1);
    rowvec figlio2 = join_rows(head_father, tail2);


    //Matrice finale con i due figli
    mat offsprings = join_cols( figlio1 , figlio2 ) ;

    //offsprings.print("offsprings");

    //cout << "Crossover finished!" << endl;

    return offsprings ;
}

void Popolazione::Evolve(){

    //We keep only an elite
    int keep = int(m_dimensione/2.) ;

    //Cromosomi da sostituire all'ultima metà di configurazione
    mat newgen(m_dimensione-keep , m_citta) ;

    Order();

    //sustituiamo gli individui peggiori
    for (int i = 0 ;  i < m_dimensione - keep  ; i +=2 ){
        //cout << "Order() went well!" << endl;

        // select parents preferably the first ones (the best ones)
        mat parents = Select();
        //cout << "Selection went well" << endl;
        //parents.print("Parents:");
        mat offsprings(2 , m_citta);

        if ( randu<double>() < 0.6 )
            offsprings = Crossover(parents) ;
        else
            offsprings = parents ;

        /*cout<< "Printing offsprings: " << endl;
        offsprings.print();*/

        //cout <<"Offsprings generated" << endl;

        offsprings.row(0) = Mutate(offsprings.row(0));
        offsprings.row(1) = Mutate(offsprings.row(1));

        //cout <<"Offsprings mutated" << endl;

        newgen.row(i) = offsprings.row(0) ;
        newgen.row(i+1) = offsprings.row(1) ;

        //cout << "Offsprings appended to newgen" << endl;

    }
    //cout <<"Run eseguita." << endl ;

    configurazione.rows( keep , m_dimensione - 1 ) = newgen.rows( 0 , m_dimensione - keep - 1 ) ;
    Order() ;
    //Check() ;
    //assert(size(m_config.col(0)) == size(colvec(m_dimension)));

    //cout<<"Fitness del primo elemento: " << Fitness(configurazione.row(0)) << endl;
    //cout<<"Fitness dell'ultimo elemento: " << Fitness(configurazione.row(m_dimensione-1)) << endl;


}
