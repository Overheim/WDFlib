//
//  WDFTransistorModel.h
//  WDFTreeTest
//
//  Created by Won Jae Lee on 2018. 3. 6..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef WDFTransistorModel_h
#define WDFTransistorModel_h


//============================================================
// Model name of BJT transistors
//============================================================
typedef enum TRANSISTOR_MODEL {
	BJT_2N2222,
	BJT_2N5089
} TransistorModel;

//============================================================
// A class of BJT transistor
//============================================================
class TransistorParam
{
public:
	TransistorParam(TransistorModel model) : model(model)
	{
		SetModel(model);
	}
	
	virtual ~TransistorParam()
	{
		
	}
	
	void SetModel(TransistorModel model)
	{
		switch(model)
		{
			case BJT_2N2222:
				Is = 1.0e-14;
				Vt = 25.85e-3;
				betaF = 200.0;
				betaR = 3.0;
				alphaF = betaF / (betaF + 1.0);
				alphaR = betaR / (betaR + 1.0);
				break;
			case BJT_2N5089:
				Is = 5.911e-15;
				Vt = 25.85e-3;
				alphaF = 0.9993;
				alphaR = 0.5579;
				betaF = alphaF / (1.0 - alphaF);
				betaR = alphaR / (1.0 - alphaR);
				break;
			default:
				printf("2N222 is set as default\n");
				Is = 1.0e-14;
				Vt = 25.85e-3;
				betaF = 200.0;
				betaR = 3.0;
				alphaF = betaF / (betaF + 1.0);
				alphaR = betaR / (betaR + 1.0);
				break;
		}
	}
	
protected:
	// The value of BJT model
	TransistorModel model;
	
	// The parameters for evaluating currents
	double Is, Vt, betaF, betaR, alphaF, alphaR;
};

#endif /* WDFTransistorModel_h */
