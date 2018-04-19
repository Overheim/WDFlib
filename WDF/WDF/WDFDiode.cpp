//
//  WDFDiode.cpp
//  DiodeClipper
//
//  Created by Won Jae Lee on 2017. 3. 8..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#include <cmath>
#include "WDFDiode.hpp"

//============================================================
// Diode (symmetric)
//============================================================
WDFDiode::WDFDiode(double Is, double Vt, double Ne, string label) : WDFRootLeaf(label)
{
	this->Is = Is;
	this->Vt = Vt;
	this->Ne = Ne;
}

WDFDiode::~WDFDiode()
{
	
}

void WDFDiode::EvaluateReflectedWave()
{
	vecPorts[0]->b = Solve(0);
}

double WDFDiode::Evaluate(double b)
{
	// I = 2*Is*sinh(V/(Ne*Vt))
	WDFPort* port = vecPorts[0];
	return 2.0 * Is * sinh((port->a + b) / (2 * Ne * Vt)) - (port->a - b) / (2.0 * port->Rp);
}

//============================================================
// Diode (asymmetric, R-Type)
//============================================================
WDFRTypeAsymDiode::WDFRTypeAsymDiode(double Is, double Vt, double Ne, bool isInverse, string label) : WDFRTypeRootLeaf(1, label)
{
	this->Is = Is;
	this->Vt = Vt;
	this->Ne = Ne;
	this->isInverse = isInverse;
}

WDFRTypeAsymDiode::~WDFRTypeAsymDiode()
{
	
}

vec WDFRTypeAsymDiode::Nonlinear(vec v_c)
{
	// Aymmetric diode is just a one-port
	double Vc = v_c(0);
	vec i_c(1);
	i_c(0) = isInverse ? -Is * (exp(-Vc / (Ne * Vt)) - 1) : Is * (exp(Vc / (Ne * Vt)) - 1);
	
	return i_c;
}

mat WDFRTypeAsymDiode::DiffNonlinear(vec v_c)
{
	mat J(1,1);
	
	double Vc = v_c(0);
	J(0,0) = isInverse ? Is / (Ne * Vt) * exp(-Vc / (Ne * Vt)) : Is / (Ne * Vt) * exp(Vc / (Ne * Vt));
	
	return J;
}

//============================================================
// Diode (symmetric, R-Type)
//============================================================
WDFRTypeDiode::WDFRTypeDiode(double Is, double Vt, double Ne, string label) : WDFRTypeRootLeaf(1, label)
{
	this->Is = Is;
	this->Vt = Vt;
	this->Ne = Ne;
}

WDFRTypeDiode::~WDFRTypeDiode()
{
	
}

vec WDFRTypeDiode::Nonlinear(vec v_c)
{
	// Symmetric diode is just a one-port
	double Vc = v_c(0);
	vec i_c(1);
	i_c(0) = Is * (exp(Vc / (Ne * Vt)) - exp(-Vc / (Ne * Vt)));
	
	return i_c;
}

mat WDFRTypeDiode::DiffNonlinear(vec v_c)
{
	mat J(1,1);
	
	double Vc = v_c(0);
	J(0,0) = Is / (Ne * Vt) * (exp(Vc / (Ne * Vt)) + exp(-Vc / (Ne * Vt)));
	
	return J;
}
