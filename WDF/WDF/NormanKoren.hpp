#ifndef NORMAN_KOREN_H
#define NORMAN_KOREN_H


#include <cmath>

//============================================================
// model name list
//============================================================
typedef enum _TubeModel
{
	T_12AX7,
	T_12AT7,
	T_6SL7,
	P_EF86,
	P_6V6GT,
	P_6L6GC,
	P_KT88
} TubeModel;

//============================================================
// Tube class
//============================================================
class NormanKoren
{
public:
	NormanKoren(TubeModel model)
	{
		SetTubeModel(model);
	}
	
	~NormanKoren() {}

	void SetTubeModel(TubeModel model)
	{
		switch(model)
		{
			case T_12AX7:
				mu=100.0;	kx=1.4;		kg1=1060.0;	kg2=0.0;	kp=600.0;	kvb=300.0;
				break;
			case T_12AT7:
				mu=67.49;	kx=1.234;	kg1=419.1;	kg2=0.0;	kp=213.96;	kvb=300.0;
				break;
			case T_6SL7:
				mu=75.89;	kx=1.233;	kg1=1735.2;	kg2=0.0;	kp=1725.27;	kvb=7.0;
				break;
			case P_EF86:
				mu=34.90;	kx=1.35;	kg1=2648.1;	kg2=4627;	kp=222.06;	kvb=4.7;
				break;
			case P_6V6GT:
				mu=10.7;	kx=1.31;	kg1=1672;	kg2=4500;	kp=41.16;	kvb=12.7;
				break;
			case P_6L6GC:
				mu=8.7;		kx=1.35;	kg1=1460;	kg2=4500;	kp=48.0;	kvb=12.0;
				break;
			case P_KT88:
				mu=12.38;	kx=1.246;	kg1=340.4;	kg2=4500;	kp=26.48;	kvb=36.5;
				break;
			default:	// set to 12AX7 as default
//				printf("Error: Wrong tube model selected\n");
				mu=100.0;	kx=1.4;	kg1=1060.0;	kg2=0.0;	kp=600.0;	kvb=300.0;
				break;
		}
	}

protected:
	double signum(double x)
	{
		return x >= 0.0 ? 1.0 : -1.0;
	}
	

	double mu, kx, kg1, kg2, kp, kvb;
};

#endif	//NORMAN_KOREN_H
