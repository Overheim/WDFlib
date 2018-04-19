//
//  WDFTransistor.hpp
//  WDFTreeTest
//
//  Created by Won Jae Lee on 2018. 3. 4..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef WDFTransistor_hpp
#define WDFTransistor_hpp

#include "WDF.hpp"
#include "WDFTransistorModel.h"

//============================================================
// A BJT transistor class for WDF
// 2 ports(BC & BE) and 2 nonlinear functions(Ibc & Ibe)
//============================================================
class WDFRTypeTransistor : public WDFRTypeRootLeaf, public TransistorParam
{
public:
	WDFRTypeTransistor(TransistorModel model, string label="Transistor");
	~WDFRTypeTransistor();
	
	//============================================================
	// Nonlinear function
	// Input: [Vbc, Vbe]'
	// Output: [Ibc, Ibe]'
	virtual vec Nonlinear(vec V);
	
	//============================================================
	// Nonlinear differential function
	// Input: [Vbc, Vbe]'
	// Output: [d(Ibc)/d(Vbc), d(Ibc)/d(Vbe)]
	//		   [d(Ibe)/d(Vbc), d(Ibe)/d(Vbe)]
	virtual mat DiffNonlinear(vec V);
};

#endif /* WDFTransistor_hpp */
