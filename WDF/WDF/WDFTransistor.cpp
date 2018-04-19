//
//  WDFTransistor.cpp
//  WDFTreeTest
//
//  Created by Won Jae Lee on 2018. 3. 4..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#include "WDFTransistor.hpp"

WDFRTypeTransistor::WDFRTypeTransistor(TransistorModel model, string label) : WDFRTypeRootLeaf(2, label), TransistorParam(model)
{
	
}

WDFRTypeTransistor::~WDFRTypeTransistor()
{
	
}

vec WDFRTypeTransistor::Nonlinear(vec V)
{
	//============================================================
	// input value: V = [Vbc, Vbe]'
	// return value: I = [Ibc, Ibe]'
	//============================================================
	vec I(2);
	
	const double Vbc = V(0);
	const double Vbe = V(1);
	
	double Ic = Is * (exp(Vbe / Vt) - 1.0) - Is / alphaR * (exp(Vbc / Vt) - 1.0);
	double Ie = Is / alphaF * (exp(Vbe / Vt) - 1.0) - Is * (exp(Vbc / Vt) - 1.0);
	
	I(0) = -Ic;
	I(1) = Ie;
	
	return I;
}

mat WDFRTypeTransistor::DiffNonlinear(vec V)
{
	const double Vbc = V(0);
	const double Vbe = V(1);
	
	double dIc_by_dVbc = -Is / (alphaR * Vt) * exp(Vbc / Vt);
	double dIc_by_dVbe = Is / Vt * exp(Vbe / Vt);
	double dIe_by_dVbc = -Is / Vt * exp(Vbc / Vt);
	double dIe_by_dVbe = Is / (alphaF * Vt) * exp(Vbe / Vt);
	
	mat I(2,2);
	I(0,0) = -dIc_by_dVbc;
	I(0,1) = -dIc_by_dVbe;
	I(1,0) = dIe_by_dVbc;
	I(1,1) = dIe_by_dVbe;
	
	return I;
}
