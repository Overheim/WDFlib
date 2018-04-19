//
//  WDFTriode.cpp
//  DempwolfTriode
//
//  Created by Won Jae Lee on 2017. 4. 27..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#include "WDFTriode.hpp"

WDFRTypeTriode::WDFRTypeTriode(DempwolfTriodeModel model, string label) : WDFRTypeRootLeaf(2, label), DempwolfTriodeParam(model)
{
	
}

WDFRTypeTriode::~WDFRTypeTriode()
{
	
}

vec WDFRTypeTriode::Nonlinear(vec V)
{
	//============================================================
	// input value: V = [Vgk, Vpk]'
	// return value: I = [Igk, Ipk]'
	//============================================================
	vec I(2);
	
	double Ik = G * pow(log(1.0 + exp(C * (V(1) / mu + V(0)))) / C, gamma);
	double Igk = Gg * pow(log(1.0 + exp(Cg * V(0))) / Cg, e) + Ig0;
	
	I(0) = Igk;
	I(1) = Ik - Igk;
	
	return I;
}

mat WDFRTypeTriode::DiffNonlinear(vec V)
{
	mat I(2,2);
	
	const double Vgk = V(0);
	const double Vpk = V(1);
	
	double y_3	= C * (Vpk / mu + Vgk);
	double dy_3	= C / mu;
	double y_2	= 1 + exp(y_3);
	double dy_2	= exp(y_3) * dy_3;
	double y_1	= log(y_2) / C;
	double dy_1	= dy_2 / (C * y_2);
	double dI_k_by_Vpk = gamma * G * pow(y_1, gamma-1) * dy_1;
	
	dy_3	= C;
	y_2		= 1 + exp(y_3);
	dy_2	= exp(y_3) * dy_3;
	y_1		= log(y_2) / C;
	dy_1	= dy_2 / (C * y_2);
	double dI_k_by_Vgk = gamma * G * pow(y_1, gamma-1) * dy_1;
	
	y_3		= Cg * Vgk;
	dy_3	= Cg;
	y_2		= 1.0 + exp(y_3);
	dy_2	= exp(y_3) * dy_3;
	y_1		= log(y_2) / Cg;
	dy_1	= dy_2 / (Cg * y_2);
	double dI_g_by_Vgk = e * Gg * pow(y_1, e-1) * dy_1;
	
	double dI_g_by_Vpk = 0.0;
	
	I(0,0) = dI_g_by_Vgk;					I(0,1) = dI_g_by_Vpk;
	I(1,0) = dI_k_by_Vgk - dI_g_by_Vgk;		I(1,1) = dI_k_by_Vpk - dI_g_by_Vpk;
	
	return I;
}
