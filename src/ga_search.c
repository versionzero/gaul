/**********************************************************************
  ga_search.c
 **********************************************************************

  ga_search - Non-evolutionary local search routines.
  Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatible with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:     Routines for local search and optimisation using
		non-evolutionary methods.  These methods are all
		zer-order, that is, they do not require gradients.

  To do:	Add lots of other algorithms.
		- golden search
		- powell
		- brent

 **********************************************************************/

#include "ga_search.h"

/**********************************************************************
  ls_soliswets()
  synopsis:	Solis and Wets local search procedure.
  parameters:
  return:	Lowest fitness found.
  last updated:	11 Jul 2002
 **********************************************************************/


/* FUNCTIONS */
/* ========= */
/*
 Line Minimize attempts to find the best value of alpha
 such that p+ alpha*direction d gives the lowest Err function
 min is the value of the function at p.
 g and k are positions and gradients along this line
*/

boolean LineMinimize(	double (*DerivativeFunc)(),
			int n, double *p, double *d, double *g,
			double *k, double *min, double *alpha, vpointer userdata)
{
    double l1,g1,f1,orig_f1,orig_g1;
    double l2,g2,f2;
    double qa,qb,qc,qd;
    double dl,df;
    double l=0.0,f=0.0,s=0.0;
    double t;
    int i,ls;
    double *x;

#if OPTIMIZERS_DEBUG>0
    printf("DEBUG: line min, step = %f\n",*alpha);
#endif

  x = s_malloc(sizeof(double)*n);

/*
** set the variables that define the search range
** note that l2 is the previous step length
** f1 is the previous function value.
** f2 is the function value at a full step forward.
*/
    l1 = 0.0;
    l2 = *alpha;
    if( l2 < ApproxZero ) l2 = ApproxZero;	/* This eliminates possibility of using v. small or negative step lengths */

    f1 = *min;

    for( i=0; i<n; i++ ) x[i] = p[i]+l2*d[i];
    f2 = (*DerivativeFunc)(n,x,k,userdata);

/*
** f1 anf f2 now bound the search from 0 to alpha
** l1 and l2 bound the step length
** k is the gradient at x, i.e. the full step length
*/

/*
** g1 and g2 are derived from gradients and positions
** and are used below in cubic approx to line function
*/
    g1 = 0.0;
    g2 = 0.0;
    for( i=0; i<n; i++ )
    {   g1 += g[i]*d[i];
        g2 += k[i]*d[i];
    }

    orig_f1=f1;
    orig_g1=g1;

/*
** main loop of finding the best l, i.e. step length
*/
    for( ls=0; ls<LMMAX; ls++ )
    {
        dl = l2-l1;
/*
** escape if the difference between the upper and lower
** bounds in the step length is less than AppoxZero, i.e. v. small
** note that neither l nor f are defined the first time through
** this "error" is saved cos l1=0.0 and l2>=ApproxZero
** this is a very abnormal return condition
*/
        if( dl < ApproxZero )
        {
            printf("RETURN because the search space is too small (%f to %f)\n",l1,l2);
            *alpha = l;
            *min = f;
            s_free(x);
            return( TRUE );
        }

/* find minimum of cubic equation from two points
** and two gradients
*/
        df = f2-f1;

        qa = (-2.0*df + (g1+g2)*dl)/(dl*dl*dl);
        qb = (3.0*df - (g1+g1+g2)*dl)/(dl*dl);
        qc = g1;
        qd = f1;

        t = qb*qb - 3.0*qa*qc;


        if( (fabs(qa)>ApproxZero) && (t>=0.0) )
        {
            l = l1 + (-qb+sqrt(t))/(3.0*qa);
        }
        else
        {
            l = l1 - qc/(2.0*qb);
        }

/*
** ideally l is between l1 and l2
** but often falls below l1 (negative??)
** or greater than l2 (possible extrapolation to a big step)
*/
        if( l < l1 ){
            printf("l < l1, therefore averaging l1 and l2. l=%f step=%f l1=%f l2=%f\n", l, *alpha,l1,l2);
            l = (l1+l2)/2.0;
        }
/*
        if( l > l2 ) printf("extrapolating %f %f\n",l,l2);
        if(( l < l2 )&&(g2<g1)) printf("long range interpolation %f %f\n",l,l2);
*/


        for( i=0; i<n; i++ ){
            x[i] = p[i]+l*d[i];
        }
        f = (*DerivativeFunc)(n,x,k,userdata);


        for(s=0.0, i=0; i<n; i++ ){
            s += k[i]*d[i];
        }
/*
** s is positive if the function is ascending at this point
*/
/*
** normal return
*/
/*
** test determine if the function has decreased "enough" and if the
** gradient is smaller here than that at the original point, i.e.
** we'd hope it is close to the line search minimium
        if( (f<(orig_f1+ALPHA*l*orig_g1)) && (fabs(s/orig_g1)<BETA) || (( l < l2 )&&(g2<g1)) )*/
        if( (f<(orig_f1+ALPHA*l*orig_g1)) && (fabs(s/orig_g1)<BETA) )
        {
            *alpha = l;
            *min = f;
            s_free(x);
            return( TRUE );
        }
        else{

            if( f>=(orig_f1+ALPHA*l*orig_g1) ) printf("failed improvement test %f %f\n",f-orig_f1,ALPHA*l*orig_g1);
            if( fabs(s/orig_g1)>=BETA )   printf("failed gradient test %f %f\n",fabs(s/orig_g1), BETA);
            printf("l1=%f, l=%f, l2=%f, g1=%f, s=%f, g2=%f, f1=%f, f=%f, f2=%f\n",l1,l,l2,g1,s,g2,f1,f,f2);

        }
/*
** only reach here if the above condition was not
** satisfied, i.e. we didnt decend enough.
*/

/*
** if certain conditions hold, adjust the
** boundaries of the search routine
*/
        if( (s>=0.0) || (f>f1)  || (l>l2) )
        {
            l2 = l;
            f2 = f;
            g2 = s;
/*
            printf("upper %f %f %f %f\n",s,f,f1,l);
*/
        } else
        {
            l1 = l;
/*
            printf("lower %f %f %f %f\n",s,f,f1,l);
*/
            f1 = f;
            g1 = s;
        }

/* next pass through this part of the code */
    }

/*
** only get here if the limit on the number of steps
** through has been exceeded. if so, give up, setting f and l
** to either the lower or upper range.
*/
    if( f1 < f )
    {   f = f1;
        l = l1;
    }

    if( f2 < f )
    {   f = f2;
        l = l2;
    }

    t = *min;
    *alpha = l;
    *min = f;

    printf("RETURN due to number of searched being surpassed, new f:%f old f:%f\n",f,t);
    s_free(x);
    return( f < t );
/*
** returns false if we failed to find a
** better result along this line. if this is
** so then we have to reset the search direction in which ever method
** of search direction determination is being used, e.g. conj. grad., quasinewt etc.
*/

}


/* Steepest Descent */
boolean SteepestDescentMinimize(	double (*ObjectiveFunc)(),
					double (*DerivativeFunc)(),
					int n,
					double *p,
					double *px,
					double *gmin,
					int max_iter,
					vpointer userdata)
  {
  double grad,step;
  double val;
  int iter;
  int i;
  double *x,*nx;

  x = s_malloc(sizeof(double)*n);
  nx = s_malloc(sizeof(double)*n);

  step = INITIAL_SD_STEP;
  *gmin = (*DerivativeFunc)(n,p,px,userdata);

  for( iter=0; iter<max_iter; iter++ )
    {
#if OPTIMIZERS_DEBUG>0
    printf("DEBUG: %d: %f [ ",iter,*gmin);
    for(i=0; i<n; i++) printf("%f ", p[i]);
    printf("]\n");
#endif

    for( i=0; i<n; i++ ) x[i]=p[i]-step*px[i];

    val = (*DerivativeFunc)(n,x,nx,userdata);
    if ( val >= *gmin )
      {
      grad = 0.0;
      for( i=0; i<n; i++ ) grad += px[i]*px[i];
      if ( grad < ApproxZero )
        {
#if OPTIMIZERS_DEBUG>0
        printf("DEBUG: SteepestDescentMinimize() Finshed due to small gradient (sum grad squared = %f)\n", grad);
#endif
        s_free(x);
        s_free(nx);
        return(TRUE);
        }

      do
        {
        step *= 0.5;
        if ( step < ApproxZero )
          {
#if OPTIMIZERS_DEBUG>0
          printf("DEBUG: SteepestDescentMinimize() Finshed due to small step size ( step %f)\n", step);
#endif
          s_free(x);
          s_free(nx);
          return(FALSE);
          }

/*
        for( i=0; i<n; i++ ) x[i]=p[i]-step*px[i];
*/
        for( i=0; i<n; i++ ) x[i]=p[i]+step*px[i];
        val = (*ObjectiveFunc)(n,x,userdata);
        } while( val > *gmin );

      for( i=0; i<n; i++ ) p[i] = x[i];
      *gmin = (*DerivativeFunc)(n,p,px,userdata);
      }
    else 
      {
      for( i=0; i<n; i++ )
        {
        px[i] = nx[i];
        p[i] = x[i];
        }
      step *= 1.2;
      *gmin = val;
      }
    }

#if OPTIMIZERS_DEBUG>0
  printf("DEBUG: SteepestDescentMinimize() Finshed because max. iterations performed.\n");
#endif
  s_free(x);
  s_free(nx);

  return(FALSE);
  }


/* Steepest Line Descent */
boolean SteepestLineMinimize(	double (*ObjectiveFunc)(),
				double (*DerivativeFunc)(),
				int n,
				double *p,
				double *g,
				double *gmin,
				int max_iter,
				vpointer userdata)
  {
  static double alpha;
  int iter;
  int i;
  double *d,*k;

  d = s_malloc(sizeof(double)*n);
  k = s_malloc(sizeof(double)*n);

  *gmin = (*DerivativeFunc)(n,p,g,userdata);
  for( i=0; i<n; i++ ) d[i] = -g[i];

  alpha = 1.0;
  for( iter=0; iter<max_iter; iter++ )
    {
#if OPTIMIZERS_DEBUG>1
printf("%d: %f\n",iter,*gmin);
#endif
    if( !LineMinimize((*DerivativeFunc), n,p,d,g,k,gmin,&alpha,userdata) )
      {
      for( i=0; i<n; i++ ) p[i] += alpha*d[i];
      s_free(d);
      s_free(k);
      return(TRUE);
      }

    for( i=0; i<n; i++ )
      {
      p[i] += alpha*d[i];
      g[i] = k[i];
      d[i] = -g[i];
      }
   }

  s_free(d);
  s_free(k);

  return(FALSE);
  }


/* Polak-Ribiere Algorithm */
/* NB/ Doesn't actually use the ObjectiveFunc() function! */

boolean ConjugateGradientMinimize(	double (*ObjectiveFunc)(),
				double (*DerivativeFunc)(),
				int n,
				double *p,
				double *g,
				double *gmin,
				int max_iter,
				vpointer userdata)
{
    static double *d,*k;
    static double fp,alpha;
    double dg,pdg;
    double gam,t;
    double kk,gg;
    int restart;
    int i,iter;


    printf("Number of variables %d on entry to conjugate gradient\n",n);

    for(i=0;i<n;i++){
      printf("i: %d, p[i] %f \n",i,p[i]);
    }

    printf("Call A to DerivativeFunc in lineminimize\n");
    fp = (*DerivativeFunc)(n,p,g,userdata);

    printf("GRADIENT after call A to DerivativeFunc\n");

    for(i=0;i<n;i++){
      printf("i: %d, p[i] %f g[i] %f\n",i,p[i],g[i]);
    }
    
    gg = 0.0;
    for( i=0; i<n; i++ )
    {   gg += g[i]*g[i];
        d[i] = -g[i];
    }

    if( gg < ApproxZero ) 
    {   /* Zero gradient! */
        *gmin = fp;
        return( TRUE );
    }

    pdg = -gg;
    restart = n;
    alpha = 1.0/sqrt(gg);

    printf("pdg %f alpha %f restart %d \n",pdg,alpha,restart);
    printf("fp %f \n",fp);

    for( iter=0; iter<max_iter; iter++ )
    {   
        if( !LineMinimize((*ObjectiveFunc), n,p,d,g,k,&fp,&alpha,userdata) )
        {   if( restart == n )
            {   *gmin = fp;
                return( TRUE );
            } else restart = 0;
        }


        printf("fp jag:= %f\n",fp);
	printf("alpha after call to linesearch %f\n",alpha);

        for( i=0; i<n; i++ )
            p[i] += alpha*d[i];

        kk = 0.0;
        for( i=0; i<n; i++ )
            kk += k[i]*k[i];

        if( kk < ApproxZero ) 
        {   /* Zero gradient! */
            if( restart == n )
            {   *gmin = fp;
                return( TRUE );
            } else restart = 0;
        }

        if( restart > 0 )
        {   t = 0.0;
            for( i=0; i<n; i++ )
                t += k[i]*(k[i]-g[i]);
            gam = t/gg;

            for( i=0; i<n; i++ )
                d[i] = -k[i]+gam*d[i];

            dg = 0.0;
            for( i=0; i<n; i++ )
                dg += d[i]*k[i];
            if( restart != n )
                alpha *= pdg/dg;
            pdg = dg;
            restart--;
        } else
        {   /* Beale Restart */
            for( i=0; i<n; i++ )
                d[i] = -k[i];
            restart = n;
        }

        for( i=0; i<n; i++ )
            g[i] = k[i];
        gg = kk;
    }

    *gmin = fp;
    return( FALSE );
}


