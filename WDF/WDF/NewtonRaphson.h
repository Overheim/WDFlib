//
//  NewtonRaphson.h
//  NewtonRaphson
//
//  Created by Won Jae Lee on 2017. 2. 16..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef NewtonRaphson_h
#define NewtonRaphson_h

#include "armadillo"

using namespace arma;

//============================================================
// Newton-Raphson iteration
//============================================================
class NewtonRaphson
{
public:
	NewtonRaphson();
	virtual ~NewtonRaphson();
	
	virtual double Solve(double guess, int max_iter=100, double epsilon=1e-9);
	double GetAvgIter();
	
protected:
	virtual double Iterate(double x, double dx=1e-6);
	virtual double Evaluate(double x) = 0;
	
	unsigned int totalIter;
	unsigned int nSamples;
};

//============================================================
// Broyden's method
//============================================================
class Broyden
{
public:
	Broyden();
	virtual ~Broyden();
	
	vec Solve(vec guess, int max_iter=100, double epsilon=1e-9);
	
protected:
	vec Iterate(vec x, mat& A);
	mat InitializeJacobian(vec guess);
	virtual vec Evaluate(vec x) = 0;
};

//============================================================
class Broyden2
{
public:
	Broyden2();
	virtual ~Broyden2();
	
	vec Solve(vec guess, int max_iter=100, double epsilon=1e-9);
	double GetAvgIter();
	
protected:
	vec Iterate(vec x);
	mat GetJacobian(vec guess);
	virtual vec Evaluate(vec x) = 0;
	
	unsigned int totalIter;
	unsigned int nSamples;
};

//============================================================
class QuasiNewton
{
public:
	QuasiNewton();
	virtual ~QuasiNewton();
	
	vec Solve(vec guess, int max_iter=100, double epsilon=1e-9);
	
protected:
	vec Iterate(vec x);
	virtual mat GetJacobian(vec guess) = 0;
	virtual vec Evaluate(vec x) = 0;
};

#endif /* NewtonRaphson_h */
