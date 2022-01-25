#include "nrutil.h"
void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[], int ma, float **covar, float **alpha, float *chisq,
	    void (*funcs)(float, float [], float *, float [], int), float *alamda)
//  Levenberg-Marquardt method, attempting to reduce the value χ2 of a fit between a set of data points x[1..ndata], y[1..ndata] with individual standard deviations sig[1..ndata], and a nonlinear function dependent on ma coefficients a[1..ma]. The input array ia[1..ma] indicates by nonzero entries those components of a that should be fitted for, and by zero entries those components that should be held fixed at their input values. The program re- turns current best-fit values for the parameters a[1..ma], and χ2 = chisq. The arrays covar[1..ma][1..ma], alpha[1..ma][1..ma] are used as working space during most iterations. Supply a routine funcs(x,a,yfit,dyda,ma) that evaluates the fitting function yfit, and its derivatives dyda[1..ma] with respect to the fitting parameters a at x. On the first call provide an initial guess for the parameters a, and set alamda<0 for initialization (which then sets alamda=.001). If a step succeeds chisq becomes smaller and alamda de- creases by a factor of 10. If a step fails alamda grows by a factor of 10. You must call this routine repeatedly until convergence is achieved. Then, make one final call with alamda=0, so that covar[1..ma][1..ma] returns the covariance matrix, and alpha the curvature matrix. (Parameters held fixed will return zero covariances.)

{
  void covsrt(float **covar, int ma, int ia[], int mfit);
  void gaussj(float **a, int n, float **b, int m);
  void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
	      int ia[], int ma, float **alpha, float beta[], float *chisq,
	      void (*funcs)(float, float [], float *, float [], int)); int j,k,l;
  static int mfit;
  static float ochisq,*atry,*beta,*da,**oneda;
  if (*alamda < 0.0) { 
    atry=vector(1,ma);
    beta=vector(1,ma);
    da=vector(1,ma);
    for (mfit=0,j=1;j<=ma;j++)
      if (ia[j]) mfit++;
    oneda=matrix(1,mfit,1,1);
    *alamda=0.001;
    mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq,funcs);
    ochisq=(*chisq);
    for (j=1;j<=ma;j++) atry[j]=a[j];
  }
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=mfit;k++) covar[j][k]=alpha[j][k];
    covar[j][j]=alpha[j][j]*(1.0+(*alamda));
    oneda[j][1]=beta[j];
  }
  gaussj(covar,mfit,oneda,1);
  for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
  if (*alamda == 0.0) { 
    covsrt(covar,ma,ia,mfit);
    covsrt(alpha,ma,ia,mfit);
    free_matrix(oneda,1,mfit,1,1);
    free_vector(da,1,ma);
    free_vector(beta,1,ma);
    free_vector(atry,1,ma);
    return;
  }
  for (j=0,l=1;l<=ma;l++)
    if (ia[l]) atry[l]=a[l]+da[++j];
  mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq,funcs);
  if (*chisq < ochisq) {
    *alamda *= 0.1;
    ochisq=(*chisq);
    for (j=1;j<=mfit;j++) {
      for (k=1;k<=mfit;k++) alpha[j][k]=covar[j][k];
      beta[j]=da[j];
    }
    for (l=1;l<=ma;l++) a[l]=atry[l];
  } else {
    *alamda *= 10.0;
    *chisq=ochisq;
  }
}

#include "nrutil.h"
void mrqcof(float x[], float y[], float sig[], int ndata, float a[], int ia[], int ma, float **alpha, float beta[], float *chisq, void (*funcs)(float, float [], float *, float [], int))
//Used by mrqmin to evaluate the linearized fitting matrix alpha, and vector beta as in (15.5.8), and calculate χ2.
{
  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  dyda=vector(1,ma);
  for (j=1;j<=ma;j++)
    if (ia[j]) mfit++; 
  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) alpha[j][k]=0.0;
    beta[j]=0.0;
  }
  *chisq=0.0;
  for(i=1;i<=ndata;i++) { 
    (*funcs)(x[i],a,&ymod,dyda,ma);
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
	wt=dyda[l]*sig2i;
	for (j++,k=0,m=1;m<=l;m++)
	  if (ia[m]) alpha[j][++k] += wt*dyda[m];
	beta[j] += dy*wt;
      }
    }
    *chisq += dy*dy*sig2i;
  }
  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
  free_vector(dyda,1,ma);
}

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
void covsrt(float **covar, int ma, int ia[], int mfit)
// Expand in storage the covariance matrix covar, so as to take into account parameters that are being held fixed. (For the latter, return zero covariances.)
{
  int i,j,k;
  float swap;
  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) {
    if (ia[j]) {
      for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j]);
      for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i]);
      k--;
    } 
  }
}


#include <math.h>
#include "nrutil.h"
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
void gaussj(float **a, int n, float **b, int m)
///  Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. a[1..n][1..n] is the input matrix. b[1..n][1..m] is input containing the m right-hand side vectors. On output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of solution vectors.
{
  int *indxc,*indxr,*ipiv;
  int i,icol,irow,j,k,l,ll;
  float big,dum,pivinv,temp;
  
  indxc=ivector(1,n);
  indxr=ivector(1,n);
  ipiv=ivector(1,n);
  for (j=1;j<=n;j++) ipiv[j]=0; 
  for (i=1;i<=n;i++) {
    big=0.0;
    for (j=1;j<=n;j++)
      if (ipiv[j] != 1)
	for (k=1;k<=n;k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a[j][k]) >= big) {
	      big=fabs(a[j][k]);
	      irow=j;
	      icol=k;
	    } 
	  }
        }
    ++(ipiv[icol]);
    //    We now have the pivot element, so we interchange rows, if needed, to put the pivot element on the diagonal. The columns are not physically interchanged, only relabeled: indxc[i], the column of the ith pivot element, is the ith column that is reduced, while indxr[i] is the row in which that pivot element was originally located. If indxr[i] ̸= indxc[i] there is an implied column interchange. With this form of bookkeeping, the solution b’s will end up in the correct order, and the inverse matrix will be scrambled by columns.
    indxr[i]=irow;
    indxc[i]=icol;
    if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix");
    pivinv=1.0/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=1;l<=n;l++) a[icol][l] *= pivinv;
    for (l=1;l<=m;l++) b[icol][l] *= pivinv;
    for (ll=1;ll<=n;ll++)
      if (ll != icol) {
	dum=a[ll][icol];
	a[ll][icol]=0.0;
	for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
	for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
      }
  }
  //This is the end of the main loop over columns of the reduction. It only remains to unscram- ble the solution in view of the column interchanges. We do this by interchanging pairs of columns in the reverse order that the permutation was built up.
  for (l=n;l>=1;l--) {
    if (indxr[l] != indxc[l])
      for (k=1;k<=n;k++)
	SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  }
  free_ivector(ipiv,1,n);
  free_ivector(indxr,1,n);
  free_ivector(indxc,1,n);
}
