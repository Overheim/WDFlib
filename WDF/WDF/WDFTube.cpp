//
//  WDFTube.cpp
//  PakarinenTriode
//
//  Created by Won Jae Lee on 2017. 5. 17..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#include "WDFTube.hpp"

//============================================================
// TRIODE class using Norman Koren equation
//============================================================
WDFRTypeNKTriode::WDFRTypeNKTriode(TubeModel model, string label) : WDFRTypeRootLeaf(2, label), NormanKoren(model)
{
	
}

WDFRTypeNKTriode::~WDFRTypeNKTriode()
{
	
}

double WDFRTypeNKTriode::EvaluatePlateCurrent(double Vgk, double Vpk)
{
	double E1 = Vpk / kp * log(1.0 + exp(kp * (1.0/mu + Vgk / sqrt(kvb + Vpk * Vpk))));
	return E1 > 0.0 ? pow(E1, kx) / kg1 * 2.0 : 0.0;	// returns Ipk
}

vec WDFRTypeNKTriode::Nonlinear(vec Vc)
{
	//============================================================
	// input value: V = [Vgk, Vpk]'
	// return value: I = [Igk, Ipk]'
	//============================================================
	vec I(2);
	
	double Ipk = EvaluatePlateCurrent(Vc(0), Vc(1));
	
	I(0) = Vc(0) > 0.0 ? Vc(0) / 2700.0 : Vc(0) / 100.0e9;
	I(1) = Ipk;
	
	return I;
}

mat WDFRTypeNKTriode::DiffNonlinear(vec Vc)
{
	mat I(2,2);
	
	const double Vgk = Vc(0);
	const double Vpk = Vc(1);
	
	double dE5 = 2.0 * Vpk;
	double E5 = Vpk * Vpk + kvb;
	double dE4 = 0.5 * pow(E5, -0.5) * dE5;
	double E4 = sqrt(E5);
	double dE3 = -Vgk * kp * pow(E4, -2) * dE4;
	double E3 = kp / mu + kp * Vgk / E4;
	double dE2 = exp(E3) * dE3;
	double E2 = 1.0 + exp(E3);
	double dE1 = log(E2) / kp + Vpk / kp / E2 * dE2;
	double E1 = Vpk / kp * log(E2);
	
	I(1,1) = E1 > 0.0 ? 2.0 * kx / kg1 * pow(E1, kx-1.0) * dE1 : 0.0;
	
	dE3 = kp / sqrt(kvb + Vpk * Vpk);
	E3 = kp / mu + kp / sqrt(kvb + Vpk * Vpk) * Vgk;
	dE2 = exp(E3) * dE3;
	E2 = 1.0 + exp(E3);
	dE1 = Vpk / kp / E2 * dE2;
	E1 = Vpk / kp * log(E2);
	
	I(1,0) = E1 > 0.0 ? 2.0 * kx / kg1 * pow(E1, kx-1.0) * dE1 : 0.0;
	
	I(0,1) = 0.0;
	I(0,0) = Vgk > 0.0 ? 1.0/2700.0 : 1.0/100.0e9;
	
	return I;
}

//============================================================
// PENTODE class using Norman Koren equation
//============================================================
WDFRTypeNKPentode::WDFRTypeNKPentode(TubeModel model, PentodeMode mode, string label) : WDFRTypeRootLeaf(mode == MODE_TRIODE ? 2 : 3, label), NormanKoren(model)
{
	// set the mode
	SetMode(mode);
	
	// execute pre-calculating of parameters of Ig1-equation
	g_cf = 1e-5;
	g_co = -0.2;
}

WDFRTypeNKPentode::~WDFRTypeNKPentode()
{
	
}

void WDFRTypeNKPentode::SetMode(PentodeMode mode)
{
	this->mode = mode;
}

double WDFRTypeNKPentode::EvaluatePlateCurrent(double Vpk, double Vg1k, double Vg2k)
{
	double Vg2k_new;
	switch(mode)
	{
		case MODE_TRIODE:
			Vg2k_new = Vpk;
			break;
		case MODE_ULTRA_LINEAR:
			Vg2k_new = Vg2k * 0.4 + Vpk * 0.6;
			break;
		case MODE_PENTODE:
		default:
			Vg2k_new = Vg2k;
			break;
	}
	double E1 = Vg2k_new / kp * log(1.0 + exp(kp * (1.0 / mu + Vg1k / Vg2k_new)));
	return E1 >= 0.0 ? pow(E1, kx) / kg1 * 2.0 * atan(Vpk / kvb) : 0.0;
}

double WDFRTypeNKPentode::EvaluateScreenGridCurrent(double Vg1k, double Vg2k)
{
	return exp(kx * log(Vg2k / mu + Vg1k)) / kg2;
}

double WDFRTypeNKPentode::EvaluateControlGridCurrent(double Vg1k)
{
	return Vg1k >= g_co ? g_cf * pow(Vg1k - g_co, 1.5) : 0.;
}

vec WDFRTypeNKPentode::Nonlinear(vec V)
{
	//============================================================
	// input param: V = [Vpk, Vg1k, (Vg2k)]'
	// return param: I = [Ipk, Ig1k, (Ig2k)]'
	//============================================================
	if(mode == MODE_TRIODE)
	{
		vec I(2);
		
		I(0) = EvaluatePlateCurrent(V(0), V(1));
		I(1) = EvaluateControlGridCurrent(V(1));
		
		return I;
	}
	else
	{
		vec I(3);
		
		I(0) = EvaluatePlateCurrent(V(0), V(1), V(2));
		I(1) = EvaluateControlGridCurrent(V(1));
		I(2) = EvaluateScreenGridCurrent(V(1), V(2));
		
		return I;
	}
}

mat WDFRTypeNKPentode::DiffNonlinear(vec V)
{
	//============================================================
	// input param: V = [Vpk, Vg1k, (Vg2k)]'
	// return param: I = [
	// [dIp_by_Vpk, dIp_by_Vg1k, (dIp_by_Vg2k)],
	// [dIg1_by_Vpk, dIg1_by_Vg1k, (dIg1_by_Vg2k)],
	// [(dIg2_by_Vpk), (dIg2_by_Vg1k), (dIg2_by_Vg2k)]
	// ]
	//============================================================	
	if(mode == MODE_TRIODE)
	{
		const double Vpk = V(0);
		const double Vg1k = V(1);
		
		//============================================================
		// Ip
		//============================================================
		// by Vg1k
		double dE3 = kp / Vpk;
		double E3 = kp / mu + kp * Vg1k / Vpk;
		double dE2 = exp(E3) * dE3;
		double E2 = 1.0 + exp(E3);
		double dE1 = Vpk / kp * dE2 / E2;
		double E1 = Vpk / kp * log(E2);
		double dIp_by_Vg1k = (E1 > 0.0) ? 2.0 * atan(Vpk / kvb) * kx / kg1 * pow(E1, kx-1) * dE1 : 0.0;
		
		dE3 = -kp * Vg1k / (Vpk * Vpk);
		E3 = kp * (1.0 / mu + Vg1k / Vpk);
		dE2 = exp(E3) * dE3;
		E2 = 1.0 + exp(E3);
		dE1 = log(E2) / kp + Vpk / kp * dE2 / E2;
		E1 = Vpk / kp * log(E2);
		double dIp_by_Vpk = (E1 > 0.0) ? 2.0 * (pow(E1, kx) / kg1 / (1.0 + (Vpk/kvb * Vpk/kvb)) / kvb + kx / kg1 * pow(E1, kx-1) * dE1 * atan(Vpk / kvb)) : 0.0;
		
		//============================================================
		// Ig1
		//============================================================
		// by Vg1k
		double dIg1_by_Vg1k = (Vg1k > 0.0) ? 1.0/2700.0 : 1.0/100.0e9;
		double dIg1_by_Vpk = 0.0;
		
		//============================================================
		// set Jacobian matrix
		//============================================================
		mat I = mat(2,2);
		
		I(0,0) = dIp_by_Vpk;	I(0,1) = dIp_by_Vg1k;
		I(1,0) = dIg1_by_Vpk;	I(1,1) = dIg1_by_Vg1k;
		
		return I;
	}
	else if(mode == MODE_ULTRA_LINEAR)
	{
		mat I = mat(3,3);
		
		return I;
	}
	else
	{
		const double Vpk = V(0);
		const double Vg1k = V(1);
		const double Vg2k = V(2);
		
		//============================================================
		// Ip
		//============================================================
		// by Vg1k
		double dE3 = kp / Vg2k;
		double E3 = kp / mu + kp * Vg1k / Vg2k;
		double dE2 = exp(E3) * dE3;
		double E2 = 1.0 + exp(E3);
		double dE1 = Vg2k / kp * dE2 / E2;
		double E1 = Vg2k / kp * log(E2);
		double dIp_by_Vg1k = (E1 >= 0.0) ? 2.0 * atan(Vpk / kvb) * kx / kg1 * pow(E1, kx-1) * dE1 : 0.0;
		
		// by Vpk
		double dIp_by_Vpk = (E1 >= 0.0) ? pow(E1, kx) / kg1 * 2.0 / kvb / (1.0 + Vpk/kvb * Vpk/kvb) : 0.0;
		
		// by Vg2k
		dE3 = -kp * Vg1k / (Vg2k * Vg2k);
		dE2 = exp(E3) * dE3;
		dE1 = log(E2) / kp + Vg2k / kp * dE2 / E2;
		double dIp_by_Vg2k = (E1 >= 0.0) ? 2.0 * atan(Vpk / kvb) * kx / kg1 * pow(E1, kx-1) * dE1 : 0.0;
		
		//============================================================
		// Ig1
		//============================================================
		// by Vg1k
		double dIg1_by_Vg1k = (Vg1k >= g_co) ? g_cf * 1.5 * pow(Vg1k - g_co, 0.5) : 0.0;
		double dIg1_by_Vg2k = 0.0;
		double dIg1_by_Vpk = 0.0;
		
		//============================================================
		// Ig2
		//============================================================
		// by Vg1k
		dE2 = 1.0;
		E2 = Vg2k / mu + Vg1k;
		dE1 = kx / E2 * dE2;
		E1 = kx * log(E2);
		double dIg2_by_Vg1k = (E1 >= 0.0) ? dE1 / kg2 * exp(E1) : 0.0;
		
		// by Vg2k
		dE2 = 1.0 / mu;
		dE1 = kx / E2 * dE2;
		double dIg2_by_Vg2k = (E1 >= 0.0) ? dE1 / kg2 * exp(E1) : 0.0;
		
		// by Vpk
		double dIg2_by_Vpk = 0.0;
		
		//============================================================
		// set Jacobian matrix
		//============================================================
		mat dI = mat(3,3);
		
		dI(0,0) = dIp_by_Vpk;	dI(0,1) = dIp_by_Vg1k;		dI(0,2) = dIp_by_Vg2k;
		dI(1,0) = dIg1_by_Vpk;	dI(1,1) = dIg1_by_Vg1k;		dI(1,2) = dIg1_by_Vg2k;
		dI(2,0) = dIg2_by_Vpk;	dI(2,1) = dIg2_by_Vg1k;		dI(2,2) = dIg2_by_Vg2k;
		
		return dI;
	}
}
