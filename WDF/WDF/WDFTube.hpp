//
//  WDFTube.hpp
//  PakarinenTriode
//
//  Created by Won Jae Lee on 2017. 5. 17..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef WDFTube_hpp
#define WDFTube_hpp

#include "WDF.hpp"
#include "NormanKoren.hpp"

// mode of the pentode
typedef enum _PentodeMode
{
	MODE_TRIODE,
	MODE_ULTRA_LINEAR,
	MODE_PENTODE
} PentodeMode;

//============================================================
// TRIODE class using Norman Koren equation
//============================================================
class WDFRTypeNKTriode : public WDFRTypeRootLeaf, public NormanKoren
{
public:
	WDFRTypeNKTriode(TubeModel model, string label="Triode");
	~WDFRTypeNKTriode();
	
	// virtual function from WDFRTypeRootLeaf class
	virtual vec Nonlinear(vec Vc);
	virtual mat DiffNonlinear(vec Vc);
	
protected:
	// Norman Koren Ipk equation
	double EvaluatePlateCurrent(double Vgk, double Vpk);
};

//============================================================
// PENTODE class using Norman Koren equation
//============================================================
class WDFRTypeNKPentode : public WDFRTypeRootLeaf, public NormanKoren
{
public:
	WDFRTypeNKPentode(TubeModel model, PentodeMode mode=MODE_PENTODE, string label="Pentode");
	~WDFRTypeNKPentode();
	
	// set the pentode's mode
	void SetMode(PentodeMode mode);
	
	// virtual function from WDFRTypeRootLeaf class
	virtual vec Nonlinear(vec V);
	virtual mat DiffNonlinear(vec V);
	
protected:
	// Norman Koren equation
	double EvaluatePlateCurrent(double Vpk, double Vg1k, double Vg2k=0.0);
	double EvaluateScreenGridCurrent(double Vg1k, double Vg2k);
	double EvaluateControlGridCurrent(double Vg1k);
	
	double g_cf, g_co;
	PentodeMode mode;	// pentode's mode
};

#endif /* WDFTube_hpp */
