//
//  NewtonRaphson.cpp
//  NewtonRaphson
//
//  Created by Won Jae Lee on 2017. 2. 16..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#include <cmath>
#include "NewtonRaphson.h"

//============================================================
NewtonRaphson::NewtonRaphson()
{
	totalIter = 0;
	nSamples = 0;
}

NewtonRaphson::~NewtonRaphson()
{
	
}

double NewtonRaphson::Solve(double guess, int max_iter, double epsilon)
{
	double x = guess;
	double error = 1e6;
	unsigned int iter = 0;
	
	while(fabs(error) / fabs(x) > epsilon)
	{
		guess = Iterate(x);
		error = x - guess;
		x = guess;
		
		if(++iter >= max_iter)
		{
//			cout << "max iteration" << endl;
			break;
		}
	}
	
	totalIter += iter;
	nSamples++;
	
	return x;
}

double NewtonRaphson::GetAvgIter()
{
	return (double)totalIter / (double)nSamples;
}

double NewtonRaphson::Iterate(double x, double dx)
{
	double f = Evaluate(x);
	double xnew = x - dx*f / (Evaluate(x + dx) - f);
	return xnew;
}

//============================================================
// Broyden's method
//============================================================
Broyden::Broyden()
{
	
}

Broyden::~Broyden()
{
	
}

vec Broyden::Solve(vec guess, int max_iter, double epsilon)
{
	vec x = guess;
	vec f = vec(x.size());
	mat A = InitializeJacobian(guess);
	
	int iter = 1;
	double error;
	
	do {
		guess = Iterate(x, A);
		error = norm(guess - x, 1);
		x = guess;
		
		if(iter++ >= max_iter)
		{
//			cout << "<max iteration> " << endl;
			break;
		}
	} while(error > epsilon);
	
	return x;
}

vec Broyden::Iterate(vec x, mat& A)
{
	vec f = Evaluate(x);
	vec x_new = x - inv(A) * f;
	
	// update A
	vec f_new = Evaluate(x_new);
	vec d = x_new - x;
	A = A + (f_new - f - (A * d)) * (d.t() / as_scalar(d.t() * d));
	
	return x_new;
}

mat Broyden::InitializeJacobian(vec guess)
{
	mat A = mat(guess.size(), guess.size());
	const double e = 1e-9;
	
	for(unsigned int c=0; c<A.n_cols; c++)
	{
		vec x = guess;
		x(c) = x(c) + e;
		vec f = (Evaluate(x) - Evaluate(guess)) / e;
		
		for(unsigned int r=0; r<A.n_rows; r++)
			A(r, c) = f(r);
	}
	
	return A;
}

//============================================================
Broyden2::Broyden2()
{
	totalIter = 0;
	nSamples = 0;
}

Broyden2::~Broyden2()
{
	
}

vec Broyden2::Solve(vec guess, int max_iter, double epsilon)
{
	vec x = guess;
	vec f = vec(x.size());
	
	int iter = 1;
	double error;
	
	do {
		guess = Iterate(x);
//		guess.print("guess:");
		error = norm(guess - x, 1);
		x = guess;
		
		if(iter++ >= max_iter)
		{
//			cout << "max iteration" << endl;
			break;
		}
	} while(error > epsilon);
	
	totalIter += iter;
	nSamples++;
	
	return x;
}

double Broyden2::GetAvgIter()
{
	return (double)totalIter / (double)nSamples;
}

vec Broyden2::Iterate(vec x)
{
	vec f = Evaluate(x);
	mat A = GetJacobian(x);
	
	return x - A.i() * f;
}

mat Broyden2::GetJacobian(vec guess)
{
	mat A = mat(guess.size(), guess.size());
	const double e = 1e-9;
	
	for(unsigned int c=0; c<A.n_cols; c++)
	{
		vec x = guess;
		x(c) = x(c) + e;
		vec df = (Evaluate(x) - Evaluate(guess)) / e;
		
		for(unsigned int r=0; r<A.n_rows; r++)
			A(r, c) = df(r);
	}
	
	return A;
}

//============================================================
QuasiNewton::QuasiNewton()
{
	
}

QuasiNewton::~QuasiNewton()
{
	
}

vec QuasiNewton::Solve(vec guess, int max_iter, double epsilon)
{
	vec x = guess;
	vec f = vec(x.size());
	
	int iter = 1;
	double error;
	
	do {
		guess = Iterate(x);
		error = norm(guess - x);
		x = guess;
		
		if(iter++ >= max_iter)
		{
//			cout << "max iteration" << endl;
			break;
		}
	} while(error > epsilon);
	
	return x;
}

vec QuasiNewton::Iterate(vec x)
{
	vec f = Evaluate(x);
	mat A = GetJacobian(x);
	
//	A.print("A:");
	
	return x - A.i() * f;
}
