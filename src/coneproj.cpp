
// includes from the plugin
#include <RcppArmadillo.h>
#include <Rcpp.h>


#ifndef BEGIN_RCPP
#define BEGIN_RCPP
#endif

#ifndef END_RCPP
#define END_RCPP
#endif

using namespace Rcpp;


// user includes


// declarations
extern "C" {
SEXP coneACpp( SEXP y, SEXP amat) ;
}

// definition

SEXP coneACpp( SEXP y, SEXP amat ){
BEGIN_RCPP

    Rcpp::NumericVector y_(y);
    Rcpp::NumericMatrix amat_(amat);
    int n = y_.size(), m = amat_.nrow();
    arma::mat namat(amat_.begin(), m, n, false);
    arma::colvec ny(y_.begin(), n, false);
    float sm = 1e-8;
    arma::colvec h(m); h.fill(0);
    arma::colvec obs = arma::linspace(0, m-1, m);
    int check = 0;
    arma::mat amat_in = namat;

    for(int im = 0; im < m; im ++){
        arma::mat nnamat = namat.row(im) * namat.row(im).t();
        amat_in.row(im) = namat.row(im) / sqrt(nnamat(0,0));
    } 

    arma::mat delta = -amat_in;
    arma::colvec b2 = delta * ny;
    arma::colvec theta(n); theta.fill(0);

    if(max(b2) > 2 * sm){
        int i = min(obs.elem(find(b2 == max(b2))));
        h(i) = 1;
    }

    else{check = 1;}

    int nrep = 0;

    while(check == 0 & nrep < (n * n)){
        nrep ++ ;
        arma::colvec indice = arma::linspace(0, delta.n_rows - 1, delta.n_rows);
        indice = indice.elem(find(h == 1));
        arma::mat xmat(indice.n_elem, delta.n_cols); xmat.fill(0);
  
        for(int k = 0; k < indice.n_elem; k ++){
            xmat.row(k) = delta.row(indice(k));
        }

        arma::colvec a = solve(xmat * xmat.t(), xmat * ny);
        arma::colvec avec(m); avec.fill(0);

        if(min(a) < (-sm)){
            avec.elem(find(h == 1)) = a;
            int i = min(obs.elem(find(avec == min(avec))));
            h(i) = 0;
            check = 0;
        }
      
        else{
            check = 1;
            theta = xmat.t() * a;
            b2 = delta * (ny - theta)/n;

            if(max(b2) > 2 * sm){
                int  i = min(obs.elem(find(b2 == max(b2))));
                h(i) = 1;
                check = 0;
            }
        }
    }

    if(nrep > (n * n)){Rcpp::Rcout << "ERROR DID NOT CONVERGE IN CONEPROJ" << std::endl;}

    return wrap(Rcpp::List::create(Rcpp::Named("thetahat") = ny - theta, Named("dim") = n - sum(h), Named("nrep") = nrep));

END_RCPP
}


// declarations
extern "C" {
SEXP coneBCpp( SEXP y, SEXP delta, SEXP vmat) ;
}

// definition

SEXP coneBCpp( SEXP y, SEXP delta, SEXP vmat ){
BEGIN_RCPP

    Rcpp::NumericVector y_(y);
    Rcpp::NumericMatrix delta_(delta);
    Rcpp::NumericMatrix vmat_(vmat);
    int n = y_.size(), m = delta_.nrow(), p = vmat_.ncol();
    arma::colvec ny(y_.begin(), n, false);
    arma::mat ndelta(delta_.begin(), m, n, false);
    arma::mat nvmat(vmat_.begin(), n, p, false);
    float sm = 1e-8;
    arma::colvec h(m+p); h.fill(0);
	
    for(int i = 0; i < p; i ++){
        h(i) = 1;
    }

    arma::colvec obs = arma::linspace(0, m + p - 1, m + p);
    int check = 0;
    arma::colvec scalar(m);
    arma::mat delta_in = ndelta;

    for(int im = 0; im < m; im ++){
        arma::mat nndelta = ndelta.row(im) * ndelta.row(im).t();
        scalar(im) = sqrt(nndelta(0,0));
        delta_in.row(im) = ndelta.row(im) / scalar(im);
    } 

    arma::mat sigma(m + p, n);
    sigma.rows(0, p - 1) = nvmat.t(); sigma.rows(p, m + p-1) = delta_in;
    arma::mat theta = nvmat * solve(nvmat.t() * nvmat, nvmat.t() * ny);
    arma::colvec b2 = sigma * (ny - theta) / n;

    if(max(b2) > 2 * sm){
        int i = min(obs.elem(find(b2 == max(b2))));
        h(i) = 1;
    }

    int nrep = 0;

    if(max(b2) <= 2 * sm){
        check = 1;
        theta.fill(0);
        arma::colvec a = solve(nvmat.t() * nvmat, nvmat.t() * ny);
        arma::colvec avec(m + p); avec.fill(0);
        avec.elem(find(h == 1)) = a;
        return wrap(Rcpp::List::create(Named("yhat") = theta, Named("coefs") = avec, Named("nrep") = nrep, Named("dim") = sum(h)));
    }

    arma:: colvec a; 

    while(check == 0 & nrep < (n * n)){
        nrep ++;
        arma::colvec indice = arma::linspace(0, sigma.n_rows-1, sigma.n_rows); 
        indice = indice.elem(find(h == 1));
        arma::mat xmat(indice.n_elem, sigma.n_cols); xmat.fill(0);

        for(int k = 0; k < indice.n_elem; k ++){
            xmat.row(k) = sigma.row(indice(k));
        }
 
        a = solve(xmat * xmat.t(), xmat * ny);
        arma::colvec a_sub(a.n_elem - p);

        for(int i = p; i <= a.n_elem - 1; i ++){
            a_sub(i-p) = a(i);
        }

        if(min(a_sub) < (- sm)){
            arma::colvec avec(m + p); avec.fill(0);
            avec.elem(find(h == 1)) = a;
            arma::colvec avec_sub(m);

            for(int i = p; i <= p + m - 1; i ++){
                avec_sub(i-p) = avec(i);
            }

            int i = max(obs.elem(find(avec == min(avec_sub))));
            h(i) = 0;
            check = 0;
        }
 
        if(min(a_sub) > (-sm)){
            check = 1;
            theta = xmat.t() * a;
            b2 = sigma * (ny - theta) / n;

            if(max(b2) > 2 * sm){
                int i = min(obs.elem(find(b2 == max(b2))));
                check = 0;
                h(i) = 1;
            }
        }
    }

    arma::colvec avec(m + p); avec.fill(0);
    avec.elem(find(h == 1)) = a;
    arma::colvec avec_orig(m+p); avec_orig.fill(0);
 
    for(int i = 0; i < p; i ++){
        avec_orig(i) = avec(i);
    }
	
    for(int i = p; i < (m+p); i ++){
        avec_orig(i) = avec(i) / scalar(i - p);
    }
 
    if(nrep > (n * n)){Rcpp::Rcout << "ERROR DID NOT CONVERGE IN CONEPROJ" << std::endl;}

    return wrap(Rcpp::List::create(Named("yhat") = theta, Named("coefs") = avec_orig, Named("nrep") = nrep, Named("dim") = sum(h)));

END_RCPP
}


// declarations
extern "C" {
SEXP qprogCpp( SEXP q, SEXP c, SEXP amat, SEXP b) ;
}

// definition

SEXP qprogCpp( SEXP q, SEXP c, SEXP amat, SEXP b ){
BEGIN_RCPP

    Rcpp::NumericVector c_(c);
    Rcpp::NumericMatrix q_(q);
    Rcpp::NumericMatrix amat_(amat);
    Rcpp::NumericVector nb(b);
    int n = c_.size(), m = amat_.nrow();
    arma::colvec nc(c_.begin(), n, false);
    arma::mat namat(amat_.begin(), m, n, false);
    arma::mat nq(q_.begin(), n, n, false);
    bool constr = is_true(any( nb != 0 ));
    arma::colvec theta0(n);
    arma::colvec nnc(n);

    if(constr){
        arma::colvec b_(nb.begin(), m, false);
        theta0 = solve(namat, b_);
        nnc = nc - nq * theta0;
    } 

    else{nnc = nc; }

    arma::mat preu = chol(nq);
    arma::mat u = trimatu(preu); 
    arma::colvec z = inv(u).t() * nnc;
    arma::mat atil = namat * inv(u);

    float sm = 1e-8;
    arma::colvec h(m); h.fill(0);
    arma::colvec obs = arma::linspace(0, m-1, m);
    int check = 0;

    for(int im = 0; im < m; im ++){
        arma::mat atilnorm = atil.row(im) * atil.row(im).t();
        atil.row(im) = atil.row(im) / sqrt(atilnorm(0,0));
    } 

    arma::mat delta = -atil;
    arma::colvec b2 = delta * z;
    arma::colvec phi(n); phi.fill(0);

    if(max(b2) > 2 * sm){
        int i = min(obs.elem(find(b2 == max(b2))));
        h(i) = 1;
    }

    else{check = 1;}

    int nrep = 0;

    while(check == 0 & nrep < (n * n)){
        nrep ++ ;
        arma::colvec indice = arma::linspace(0, delta.n_rows - 1, delta.n_rows);
        indice = indice.elem(find(h == 1));
        arma::mat xmat(indice.n_elem, delta.n_cols); xmat.fill(0);
  
        for(int k = 0; k < indice.n_elem; k ++){
        xmat.row(k) = delta.row(indice(k));
        }

        arma:: colvec a = solve(xmat * xmat.t(), xmat * z);
        arma:: colvec avec(m); avec.fill(0);

        if(min(a) < (-sm)){
            avec.elem(find(h == 1)) = a;
            int i = min(obs.elem(find(avec == min(avec))));
            h(i) = 0;
            check = 0;
        }
      
        else{
            check = 1;
            phi = xmat.t() * a;
            b2 = delta * (z - phi)/n;
    
            if(max(b2) > 2 * sm){
                int  i = min(obs.elem(find(b2 == max(b2))));
                h(i) = 1;
                check = 0;
            }
        }
    }

    arma::colvec thetahat = solve(u, z - phi);

    if(constr){
        thetahat = thetahat + theta0;
    }

    if(nrep > (n * n)){Rcpp::Rcout << "ERROR DID NOT CONVERGE IN CONEPROJ" << std::endl;}

    return wrap(Rcpp::List::create(Rcpp::Named("thetahat") = thetahat, Named("dim") = n - sum(h), Named("nrep") = nrep));

END_RCPP
}

