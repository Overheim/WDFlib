//
//  WDFTriode.hpp
//  DempwolfTriode
//
//  Created by Won Jae Lee on 2017. 4. 27..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef WDFTriode_hpp
#define WDFTriode_hpp

#include "WDF.hpp"
#include "DempwolfTriodeModel.h"

//============================================================
// Triode (Dempwolf model)
// 2 ports & 2 nonlinear functions (Ig, Ik)
//============================================================
class WDFRTypeTriode : public WDFRTypeRootLeaf, public DempwolfTriodeParam
{
public:
	WDFRTypeTriode(DempwolfTriodeModel model, string label="R-type triode");
	~WDFRTypeTriode();
	
	//============================================================
	// nonlinear function.
	// input value: V = [Vgk, Vpk]'
	// return value: I = [Ig, Ik]'
	virtual vec Nonlinear(vec V);
	
	//============================================================
	// nonlinear differential function
	// input value: V = [Vgk, Vpk]'
	// return value: I = [Ig', Ik']'
	virtual mat DiffNonlinear(vec V);
};

#endif /* WDFTriode_hpp */
