//
//  DempwolfTriodeModel.h
//  DempwolfTriode
//
//  Created by Won Jae Lee on 2017. 3. 8..
//  Copyright © 2017년 Won Jae Lee. All rights reserved.
//

#ifndef DempwolfTriodeModel_h
#define DempwolfTriodeModel_h

//============================================================
// model names of the triodes
//============================================================
typedef enum DEMPWOLF_TRIODE_MODEL {
	RSD_1,
	RSD_2,
	EHX_1,
	DEMPWOLF_TRIODE_MODEL_MAX
} DempwolfTriodeModel;

//============================================================
// class of the triode
//============================================================
class DempwolfTriodeParam
{
public:
	DempwolfTriodeParam(DempwolfTriodeModel model) : model(model)
	{
		SetModel(model);
	}
	virtual ~DempwolfTriodeParam()
	{
		
	}
	
	void SetModel(DempwolfTriodeModel model)
	{
		switch(model)
		{
			case RSD_1:
				G = 2.242e-3;
				mu = 103.2;
				gamma = 1.26;
				C = 3.4;
				Gg = 6.177e-4;
				e = 1.314;
				Cg = 9.901;
				Ig0 = 8.025e-8;
				break;
			case RSD_2:
				G = 2.173e-3;
				mu = 100.2;
				gamma = 1.28;
				C = 3.19;
				Gg = 5.911e-4;
				e = 1.358;
				Cg = 11.76;
				Ig0 = 4.527e-8;
				break;
			case EHX_1:
				G = 1.371e-3;
				mu = 86.9;
				gamma = 1.349;
				C = 4.56;
				Gg = 3.263e-4;
				e = 1.156;
				Cg = 11.99;
				Ig0 = 3.917e-8;
				break;
			default:	// RSD-1
				model = RSD_1;
				G = 2.242e-3;
				mu = 103.2;
				gamma = 1.26;
				C = 3.4;
				Gg = 6.177e-4;
				e = 1.314;
				Cg = 9.901;
				Ig0 = 8.025e-8;
//				printf("Default model(RSD-1) is chosen\n");
				break;
		}
	}
	
protected:
	// model variable
	DempwolfTriodeModel model;
	
	// parameters for evaluating currents
	double G, mu, gamma, C, Gg, e, Cg, Ig0;
};

#endif /* DempwolfTriodeModel_h */
