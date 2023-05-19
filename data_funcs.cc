#include <math.h>
#include <fstream> //CHRIS: 05/16/22
#include <string> //CHRIS: 05/16/22
#include "global_variables.h"
#include "general_funcs.h"
#include "user_inputs.h"
#include "global_constants.h"
#include "data_funcs.h"
#include "io_funcs.h"
#include "cosmo_funcs.h"
#include "rates.h"
//using namespace std; //CHRIS: 05/16/22

using g_constants::yr_to_s;
using user_inputs::N_output;
using user_inputs::t_max;
using user_inputs::frontIF_fHI; //BAYU 08/12/22
using user_inputs::backIF_fHI; //BAYU 08/12/22
using g_constants::pi;
using g_constants::c;
using g_constants::chi_He;
using g_constants::lambda_lya_cm;
using g_constants::h;
using g_constants::lambda_HI_ion_cm;
using g_constants::kpc_to_cm; //BAYU 04/13/23

//Volume-weighted average
double calc_vol_avg(double f[], int n)
{
	double tot = 0;
	for (int i{ 1 }; i < n; i++)  {
		tot += f[i]*(pow(r[i],3) - pow(r[i-1],3))/pow(r[N_r-1],3);
	}
	return tot;
}
//Mass-weighted average
double calc_mass_avg(double f[], double rho[], double m, int n)
{
	double tot    = 0;
	double m_tot  = 0;
	for (int i{ 1 }; i < n; i++)
	{
		m_tot += m*rho[i]*4.*pi/3.*(pow(r[i],3) - pow(r[i-1],3));
		tot   += f[i]*m*rho[i]*4.*pi/3.*(pow(r[i],3) - pow(r[i-1],3));
  }
  return tot/m_tot;
}

//Calculate ionization front position and velocity
double* calc_ifront(double x[], double xprev[], double radius[], int n)
{
	double pos, pos_prev, vel_IF;
	double *ifront = (double*) malloc(sizeof(double) * 2);
	pos       = interpolate(x, radius, 0.5, n);
	pos_prev  = interpolate(xprev, radius, 0.5, n);
	vel_IF       = (pos - pos_prev)/(t_max*yr_to_s*1e6/(N_output - 1));
	*(ifront + 0) = pos;
	*(ifront + 1) = vel_IF;
	return ifront;
}

//Calculate average of ionization front speed //April 21, 2023 we found that xion average is equal to FlexRT method
//double* calc_ifront_avg(double x[], double xprev[], double radius[], int n)
//{
//    	int N_int {60}; //number of ionization fractions in the discrete integral
//    	double xion_list[N_int] = {0};
//	double xion_iter = 0.001; //start at 5% ion fraction
//       	double delta_xion = 0.998/(N_int-1); //step size going from 5% to 95% (90% range)
//    	for (int i=0;i<N_int;i++){
//        	xion_list[i]=xion_iter;
//        	xion_iter+=delta_xion;
//   	}	
//	//{0.05, 0.1 , 0.15, 0.2 , 0.25, 0.3 , 0.35, 0.4 , 0.45, 0.5 , 0.55, 0.6 , 0.65, 0.7 , 0.75, 0.8 , 0.85, 0.9 , 0.95};
//    	//double xion_list[N_int] = {0.25, 0.3 , 0.35, 0.4 , 0.45, 0.5 , 0.55, 0.6 , 0.65, 0.7 , 0.75};
//	double vIF_array[N_int] = {0};
//	double em_array[N_int] = {0};
//        double pos,pos_prev;
//	double *ifront = (double*) malloc(sizeof(double) * 2);
//	for (int i=0;i<N_int;i++) { // find vIF at every x_ion 
//                pos       = interpolate(x, radius, xion_list[i], n);
//                pos_prev  = interpolate(xprev, radius, xion_list[i], n);
//                vIF_array[i] = (pos - pos_prev)/dt;
//		em_array[i]  = interpolate(x, j_lya, xion_list[i], n); 
//		//em_prev = interpolate(j_lya_prev, radius, xion_list[i], n);   
//	}
//	double numerator{0}; //for emissivity average
//	double denominator{0}; //for emissivity average
//	double vIF_ion{0};
//	for (int k=1;k<N_int;k++) { // trapezoidal integral for emissivity average
//                numerator += 0.5*(vIF_array[k-1]*em_array[k-1]+vIF_array[k]*em_array[k])*(xion_list[k]-xion_list[k-1]);
//		denominator += 0.5*(em_array[k-1]+em_array[k])*(xion_list[k]-xion_list[k-1]);
//       		vIF_ion += 0.5*(vIF_array[k-1]+vIF_array[k])*(xion_list[k]-xion_list[k-1]);
//	}	
//	//printf("Em: %.3e Em2: %.3e \t",em_array[5],em_array[6]);
//	//fflush(stdout);
//	*(ifront + 0) = vIF_ion/(xion_list[N_int-1]-xion_list[0]);
//	*(ifront + 1) = numerator/denominator; //averaged IF speed
//	return ifront;
//}	
//        //for (int i{ 0 }; i < N_r; i++) {
//        //        q_eff_lya[i] = coll_ex_rate_H1_acc(temp[i]); // cm3/s
//        //        j_lya[i]=q_eff_lya[i]*nH1[i]*ne[i]*h*c/lambda_HI_ion_cm/4/pi; //erg/cm3/sr
//        //}
//
//	
//	//double pos_array[N_int] = {0};
//	//double vel_IF_avg {0};
//	//double pos,pos_prev;
//	//for (int i=0;i<N_int;i++) { // find vIF at every position. save every position 
//	//	pos       = interpolate(x, radius, xion_list[i], n);
//        //	pos_prev  = interpolate(xprev, radius, xion_list[i], n);
//	//	pos_array[i] = (pos+pos_prev)/2;
//	//	vIF_array[i] = (pos - pos_prev)/dt;
//		//vel_IF_avg+= (pos - pos_prev)/dt; //(t_max*yr_to_s*1e6/(N_output - 1));	
//		//vel_IF_avg+= (pos - pos_prev)/(t_max*yr_to_s*1e6/(N_output - 1));  
//	//}
//	//for (int k=1;k<N_int;k++) { // trapezoidal integral
//	//	vel_IF_avg += 0.5*(vIF_array[k-1]+vIF_array[k])*(pos_array[k]-pos_array[k-1]);
//	//}
//	//return vel_IF_avg/(pos_array[N_int-1]-pos_array[0]); //divide by length (essentially RIF) to get the avg vIF
////}

//Calculate ionization front velocity using the same method of FlexRT
double* calc_ifront_FlexRT()//double x[], double xprev[])
{
	double *ifront = (double*) malloc(sizeof(double) * 2);
	double vel_IF{0};
	double temp_IF{0};
	double denominator{0};
        for (int i=prev_index+1; i < N_r; i++) {
		vel_IF+=dr*(f_H1_prev[i]-f_H1[i])/dt; //pkpc/sec
		temp_IF += 0.5*(temp[i-1]+temp[i])*(f_H1[i]-f_H1[i-1]);
		denominator += f_H1[i]-f_H1[i-1];
	}
	temp_IF/=denominator;
	vel_IF*= kpc_to_cm/denominator; // cm/s
	*(ifront + 0) = vel_IF;
	*(ifront + 1) = temp_IF;
        return ifront;
}



//Calculate ionization front incident photon flux, velocity, and neutral hydrogen number density
double* calc_ifront_flux_method()
{
        double I_div_nu_pair[2][N_nu]{}; //will interpolate between pairs
        double F_inc_pair[2]{};
        double F_inc{0};
        double fHI_pair[2];
        double vel_IF{0}; //vIF found via flux method
        double *otf_outputs = (double*) malloc(sizeof(double) * 8); //array of outputs
        double three_avg{0}; //average of ne[i]*nH1[i]*q_eff_lya[i]
        double nH_avg{0}; //average over IF
        double C_em{0}; //intensity/emissivity clumping factor
        int i{0}; //iterating over all 1D RT cells
        int numIF{0}; //number of cells within IF
        
	while ((f_H1[i]<frontIF_fHI)&(i+1<N_r)){ // iterating over the IF, if xHI less than front then...
                if (i>prev_index){ //prev_index tracks the location of photoionization equilibrium behind the IF
                        three_avg+=ne[i]*nH1[i]*q_eff_lya[i];
                        nH_avg+=nH[i];
                        numIF+=1;
                        if ((f_H1[i]<backIF_fHI) && (f_H1[i+1]>backIF_fHI)) //find pair where xHI=0.1 (helps find Finc)
                                for (int j=0;j<N_nu;j++)
                                {
                                        I_div_nu_pair[0][j] = I_nu[i][j]/nu[j]; //we track I divided by nu in order to do the integration in the next section
                                        I_div_nu_pair[1][j] = I_nu[i+1][j]/nu[j]; // int (I_nu / h / nu dnu ) = incident Flux ionizing photons
                                        fHI_pair[0] = f_H1[i];
                                        fHI_pair[1] = f_H1[i+1];
                                }
                }
                i+=1;
        }
	nH_avg/=numIF;
        three_avg/=numIF;
        double nH_center = interpolate(f_H1, nH, 0.5, N_r);
        double ne_nH1_center = interpolate(f_H1, ne, 0.5, N_r) * interpolate(f_H1, nH1, 0.5, N_r);
        double T_center =  interpolate(f_H1, temp, 0.5, N_r);
        double q_T_center = coll_ex_rate_H1_acc(T_center);
        double width_IF {interpolate(f_H1, r, 0.75, N_r)-interpolate(f_H1, r, 0.25, N_r)};
	//double width_IF {interpolate(f_H1, r, frontIF_fHI, N_r)-r[prev_index]};//interpolate(f_H1, r, backIF_fHI, N_r)};
        double F_lya = 4*pi*lambda_lya_cm/h/c * trapz_int(j_lya,r,N_r);
        C_em = three_avg/nH_avg/ne_nH1_center*nH_center/q_T_center;
        F_inc_pair[0] = trapz_int(I_div_nu_pair[0],nu,N_nu)*4*pi/h; //I'm not sure whether it's multiplied by pi or 4pi
        F_inc_pair[1] = trapz_int(I_div_nu_pair[1],nu,N_nu)*4*pi/h; //
	F_inc = interpolate(fHI_pair,F_inc_pair,backIF_fHI,2);
	//BAYU MAY 10, 2023. checking new incident flux method
	//double temp_test[N_nu] {0};
	//for (int j=0;j<N_nu;j++){
	//	temp_test[j] =  I_nu[prev_index+1][j]/nu[j];
	//}
	//F_inc = trapz_int(temp_test,nu,N_nu)*4*pi/h;
        vel_IF = (c*F_inc) / (F_inc + c*nH_center*(1+chi_He));
        *(otf_outputs + 0) = F_lya; // photons/s/cm2
        *(otf_outputs + 1) = F_inc; // photons/s/cm2
        *(otf_outputs + 2) = vel_IF; //cm s-1 //flux method
        *(otf_outputs + 3) = T_center; //K
        *(otf_outputs + 4) = width_IF; //cm
        *(otf_outputs + 5) = nH_avg; //g cm-1
        *(otf_outputs + 6) = nH_center; //g cm-1
        *(otf_outputs + 7) = C_em;
        return otf_outputs;
}

void get_j_lya() {
	for (int i{ 0 }; i < N_r; i++) {
                q_eff_lya[i] = coll_ex_rate_H1_acc(temp[i]); // cm3/s
                j_lya[i]=q_eff_lya[i]*nH1[i]*ne[i]*h*c/lambda_HI_ion_cm/4/pi; //erg/cm3/sr
	}
}

