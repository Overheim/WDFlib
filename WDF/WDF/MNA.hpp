//
//  MNA.hpp
//  ModifiedNodalAnalysis
//
//  Created by Won Jae Lee on 2017. 4. 6..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef MNA_hpp
#define MNA_hpp

#include "armadillo"

using namespace arma;

//============================================================
// stamp of resistor
//============================================================
class MNA_Stamp_Resistor
{
public:
	MNA_Stamp_Resistor(int i, int j, double G) : i(i), j(j), G(G) {}
	~MNA_Stamp_Resistor() {}
	
	int i,j;		// the number of nodes
	double G;		// conductance
};

//============================================================
// stamp of voltage source
//============================================================
class MNA_Stamp_VoltageSource
{
public:
	MNA_Stamp_VoltageSource(int plus, int minus) : plus(plus), minus(minus) {}
	~MNA_Stamp_VoltageSource() {}
	
	int plus, minus;	// the number of nodes
};

//============================================================
// MNA
//============================================================
class MNA
{
public:
	MNA(mat Y, mat A, mat B, mat D);
	MNA(unsigned int nNodes, unsigned int nVoltageSources);
	~MNA();
	
	void Add(MNA_Stamp_Resistor);
	void Add(MNA_Stamp_VoltageSource);
	
	void Print(int option=0);
	
protected:
	mat X;				// system matrix
	mat Y,A,B,D;		// impedances, voltage sources and nonlinear sources
	unsigned int iVs;	// the index as which the voltage source is added
	
	void SetSystemMatrix();
};

#endif /* MNA_hpp */
