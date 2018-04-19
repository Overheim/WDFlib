//
//  WDFDiode.hpp
//  DiodeClipper
//
//  Created by Won Jae Lee on 2017. 3. 8..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef WDFDiode_hpp
#define WDFDiode_hpp

#include "WDF.hpp"

//============================================================
// Diode (symmetric)
//============================================================
class WDFDiode : public WDFRootLeaf, public NewtonRaphson
{
public:
	WDFDiode(double Is, double Vt, double Ne, string label="Diode");
	~WDFDiode();
	
	virtual void EvaluateReflectedWave();
	
protected:
	// diode parameters
	double Is, Vt, Ne;
	
	// Newton-Raphson iteration
	virtual double Evaluate(double x);
};

//============================================================
// Diode (asymmetric, R-Type)
//============================================================
class WDFRTypeAsymDiode : public WDFRTypeRootLeaf
{
public:
	WDFRTypeAsymDiode(double Is, double Vt, double Ne, bool isInverse, string label="R-Type Asymmetric Diode");
	~WDFRTypeAsymDiode();
	
	virtual vec Nonlinear(vec v_c);
	virtual mat DiffNonlinear(vec v_c);
	
protected:
	// diode parameters
	double Is, Vt, Ne;
	
	bool isInverse;	// inverse, or not
};

//============================================================
// Diode (symmetric, R-Type)
//============================================================
class WDFRTypeDiode : public WDFRTypeRootLeaf
{
public:
	WDFRTypeDiode(double Is, double Vt, double Ne, string label="R-Type Diode");
	~WDFRTypeDiode();
	
	virtual vec Nonlinear(vec v_c);
	virtual mat DiffNonlinear(vec v_c);
	
protected:
	// diode parameters
	double Is, Vt, Ne;
};

#endif /* WDFDiode_hpp */
