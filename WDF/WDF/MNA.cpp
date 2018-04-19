//
//  MNA.cpp
//  ModifiedNodalAnalysis
//
//  Created by Won Jae Lee on 2017. 4. 6..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#include "MNA.hpp"

//============================================================
// MNA
//============================================================
MNA::MNA(mat Y, mat A, mat B, mat D) : Y(Y), A(A), B(B), D(D), iVs(0)
{
	SetSystemMatrix();
}

MNA::MNA(unsigned int nNodes, unsigned int nVoltageSources) : iVs(0)
{
	X = zeros<mat>(nNodes+nVoltageSources, nNodes+nVoltageSources);
	Y = zeros<mat>(nNodes, nNodes);
	A = zeros<mat>(nNodes, nVoltageSources);
	B = zeros<mat>(nVoltageSources, nNodes);
	D = zeros<mat>(nVoltageSources, nVoltageSources);
}

MNA::~MNA()
{
	
}

void MNA::Add(MNA_Stamp_Resistor resistor)
{
	int i = resistor.i;
	int j = resistor.j;
	double G = resistor.G;
	
	if(i >= 0)				Y(i,i) = Y(i,i) + G;
	if(j >= 0)				Y(j,j) = Y(j,j) + G;
	if(i >= 0 && j >= 0)	Y(i,j) = Y(i,j) - G;
	if(i >= 0 && j >= 0)	Y(j,i) = Y(j,i) - G;
	
	SetSystemMatrix();
}

void MNA::Add(MNA_Stamp_VoltageSource vs)
{
	if(vs.plus >= 0)
	{
		A(vs.plus, iVs) = 1;
		B(iVs, vs.plus) = 1;
	}
	
	if(vs.minus >= 0)
	{
		A(vs.minus, iVs) = -1;
		B(iVs, vs.minus) = -1;
	}
	
	SetSystemMatrix();
	iVs++;
}

void MNA::Print(int option)
{
	switch(option)
	{
		case 1:
			Y.print("Y:");
			break;
		case 2:
			A.print("A:");
			break;
		case 3:
			B.print("B:");
			break;
		case 4:
			D.print("D:");
			break;
		default:
			X.print("X:");
			break;
	}
}

void MNA::SetSystemMatrix()
{
	// X = [ [Y, A], [B, D] ]
	uword nNodes = Y.n_cols;
	uword nVS = B.n_rows;
	uword size = nNodes + nVS;
	
	for(unsigned int row=0; row<size; row++)
	{
		for(unsigned int col=0; col<size; col++)
		{
			if(col < nNodes)
			{
				if(row < nNodes)
					X(row, col) = Y(row, col);
				else
					X(row, col) = B(row-nNodes, col);
			}
			else
			{
				if(row < nNodes)
					X(row, col) = A(row, col-nNodes);
				else
					X(row, col) = D(row-nNodes, col-nNodes);
			}
		}
	}
}
