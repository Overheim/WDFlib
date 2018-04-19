#include "WDF.hpp"

#define	DEFAULT_CHILDREN_COUNT_FOR_ADAPTOR		100
#define	DEFAULT_R_TYPE_RP_VALUE					1000

typedef vector<WDFPort*>	vec_wdfportptr;
typedef vector<WDFObject*>	vec_wdfobjptr;

//============================================================
// port
//============================================================
WDFPort::WDFPort(double R, WDFObject* owner)
{
	if(R == 0.0)
	{
		Rp = 1.0;
		Gp = 1.0;
	}
	else
	{
		Rp = R;
		Gp = 1.0 / R;
	}

	a = 0.0;
	b = 0.0;

	this->coupledPort = NULL;
	this->owner = owner;
}

WDFPort::~WDFPort()
{
	
}

double WDFPort::GetVoltage()
{
	return (a + b) / 2.0;
}

double WDFPort::GetCurrent()
{
	return (a - b) / (2.0 * Rp);
}

void WDFPort::SetCoupled(WDFPort* port)
{
	coupledPort = port;
	port->coupledPort = this;
}

//============================================================
// basic wave element
//============================================================
WDFObject::WDFObject(string lbl, WDFType type)
{
	this->label = lbl;
	this->type = type;
	parent = NULL;
}

WDFObject::WDFObject(unsigned int nPorts, unsigned int nChildren, string lbl, WDFType type)
{
	this->label = lbl;
	this->type = type;
	parent = NULL;

	// reserve the size of vector for ports & children
	vecPorts.reserve(nPorts);
	vecChildren.reserve(nChildren);
}

WDFObject::~WDFObject()
{
	
}

void WDFObject::AddChild(WDFObject* child)
{
	child->parent = this;
	vecChildren.push_back(child);
}

WDFPort* WDFObject::GetDecoupledPort()
{
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		if((*iter)->coupledPort == NULL)
			return (*iter);
	
	return NULL;
}

//============================================================
// BASIC CLASS OF LEAVES:
// Leaves have only one port which connected to parent(adaptor),
// and have no child
//============================================================
WDFLeaf::WDFLeaf(double R, string lbl, WDFType type) : WDFObject(1, 0, lbl, type)
{
	vecPorts.push_back(new WDFPort(R, this));
}

WDFLeaf::~WDFLeaf()
{
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		delete *iter;
}

void WDFLeaf::CalculatePortResistance()
{

}

void WDFLeaf::UpdatePortResistance()
{

}

void WDFLeaf::WaveDown()
{
	// get incident wave from the coupled port of the parent
	vecPorts[RFP]->a = vecPorts[RFP]->coupledPort->b;
}

double WDFLeaf::GetVoltage()
{
	return vecPorts[RFP]->GetVoltage();
}

double WDFLeaf::GetCurrent()
{
	return vecPorts[RFP]->GetCurrent();
}

//============================================================
// Resistor
//============================================================
WDFResistor::WDFResistor(double R, string lbl, WDFType type) : WDFLeaf(R, lbl, type)
{

}

WDFResistor::~WDFResistor()
{

}

void WDFResistor::WaveUp()
{
	// b = 0
}

//============================================================
// Capacitor
//============================================================
WDFCapacitor::WDFCapacitor(double C, double T, string lbl, WDFType type) : WDFLeaf(T / (2.0 * C), lbl, type)
{
	this->T = T;
}

WDFCapacitor::~WDFCapacitor()
{

}

void WDFCapacitor::WaveUp()
{
	// b = z^-1 * a
	vecPorts[RFP]->b = vecPorts[RFP]->a;
}

//============================================================
// Inductor
//============================================================
WDFInductor::WDFInductor(double L, double T, string lbl, WDFType type) : WDFLeaf((2.0 * L) / T, lbl, type)
{
	this->T = T;
}

WDFInductor::~WDFInductor()
{

}

void WDFInductor::WaveUp()
{
	// b = z^-1 * (-a)
	vecPorts[RFP]->b = -vecPorts[RFP]->a;
}

//============================================================
// Voltage Source
//============================================================
WDFVoltageSource::WDFVoltageSource(double Vs, double R, string lbl, WDFType type) : WDFLeaf(R, lbl, type)
{
	this->Vs = Vs;
}

WDFVoltageSource::~WDFVoltageSource()
{

}

void WDFVoltageSource::WaveUp()
{
	// b = volatage value
	vecPorts[RFP]->b = Vs;
}

//============================================================
// Open Circuit
//============================================================
WDFOpenCircuit::WDFOpenCircuit(string lbl, WDFType type) : WDFLeaf(1.0, lbl, type)
{
	
}

WDFOpenCircuit::~WDFOpenCircuit()
{
	
}

void WDFOpenCircuit::WaveUp()
{
	// b = z^(-1) * a
	vecPorts[RFP]->b = vecPorts[RFP]->a;
}

//============================================================
// base class of adaptors
//============================================================
WDFAdaptor::WDFAdaptor(unsigned int nPorts, unsigned int nChildren, string lbl, WDFType type) : WDFObject(nPorts, nChildren, lbl, type)
{
	
}

WDFAdaptor::WDFAdaptor(WDFObject* child, string lbl, WDFType type) : WDFObject(2, 1, lbl, type)
{
	// create ports(two-port)
	vecPorts.push_back(new WDFPort(1.0, this));
	vecPorts.push_back(new WDFPort(1.0, this));
	
	// set parent-child relationship
	if(child)
	{
		AddChild(child);
		vecPorts[DOWN]->SetCoupled(child->vecPorts[RFP]);
	}
}

WDFAdaptor::WDFAdaptor(WDFObject* left, WDFObject* right, string lbl, WDFType type) : WDFObject(3, 2, lbl, type)
{
	// create ports(three-port)
	vecPorts.push_back(new WDFPort(1.0, this));	// RFP
	vecPorts.push_back(new WDFPort(1.0, this));	// LEFT
	vecPorts.push_back(new WDFPort(1.0, this));	// RIGHT

	// set parent-child relationship
	if(left)
	{
		AddChild(left);
		vecPorts[LEFT]->SetCoupled(left->vecPorts[RFP]);
	}
	if(right)
	{
		AddChild(right);
		vecPorts[RIGHT]->SetCoupled(right->vecPorts[RFP]);
	}
}

WDFAdaptor::~WDFAdaptor()
{
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		delete *iter;
}

void WDFAdaptor::UpdatePortResistance()
{
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->UpdatePortResistance();
	CalculatePortResistance();
}

void WDFAdaptor::Connect(WDFObject* child)
{
	
}

//============================================================
// Inverter
//============================================================
WDFInverter::WDFInverter(WDFObject* child, string lbl, WDFType type) : WDFAdaptor(child, lbl, type)
{
	CalculatePortResistance();
}

WDFInverter::~WDFInverter()
{

}

void WDFInverter::CalculatePortResistance()
{
	// get port resistance from the child
	vecPorts[DOWN]->Rp = vecChildren[0]->vecPorts[RFP]->Rp;
	vecPorts[DOWN]->Gp = vecChildren[0]->vecPorts[RFP]->Gp;

	// set port resistance of up-port
	vecPorts[UP]->Rp = vecPorts[DOWN]->Rp;
	vecPorts[UP]->Gp = vecPorts[DOWN]->Gp;
}

void WDFInverter::WaveUp()
{
	// get the wave value from the connected child
	vecChildren[0]->WaveUp();
	vecPorts[DOWN]->a = vecPorts[DOWN]->coupledPort->b;
	
	// set the wave value of the up-port
	// b_up = -a_down
	vecPorts[UP]->b = -vecPorts[DOWN]->a;
}

void WDFInverter::WaveDown()
{
	// get the wave value from the parent
	vecPorts[UP]->a = vecPorts[UP]->coupledPort->b;

	// set the wave value to the down-port
	// b_down = -a_up
	vecPorts[DOWN]->b = -vecPorts[UP]->a;
	vecChildren[0]->WaveDown();
}

//============================================================
// Ideal Transformer
//============================================================
WDFIdealTransformer::WDFIdealTransformer(WDFObject* secondary, double N, string lbl, WDFType type) : WDFAdaptor(secondary, lbl, type), N(N==0.0 ? 1.0 : N)
{
	CalculatePortResistance();
}

WDFIdealTransformer::~WDFIdealTransformer()
{

}

void WDFIdealTransformer::CalculatePortResistance()
{
	// UP-port = primary, DOWN-port = secondary
	vecPorts[DOWN]->Rp = vecChildren[0]->vecPorts[RFP]->Rp;
	vecPorts[DOWN]->Gp = vecChildren[0]->vecPorts[RFP]->Gp;

	vecPorts[UP]->Rp = vecPorts[DOWN]->Rp / (N * N);
	vecPorts[UP]->Gp = 1.0 / vecPorts[UP]->Rp;
}

void WDFIdealTransformer::WaveUp()
{
	// get the wave value from the connected child
	vecChildren[0]->WaveUp();
	vecPorts[DOWN]->a = vecPorts[DOWN]->coupledPort->b;

	// set the wave value of the up-port
	// b_up = a_down / N
	vecPorts[UP]->b = vecPorts[DOWN]->a / N;
}

void WDFIdealTransformer::WaveDown()
{
	// get the wave value from the parent
	vecPorts[UP]->a = vecPorts[UP]->coupledPort->b;

	// set the wave value to the down-port
	// b_down = a_up * N
	vecPorts[DOWN]->b = vecPorts[UP]->a * N;
	vecChildren[0]->WaveDown();
}

//============================================================
// Gyrator
//============================================================
WDFGyrator::WDFGyrator(WDFObject* child, double R, string lbl, WDFType type) : WDFAdaptor(child, lbl, type), R(R==0.0 ? 1.0 : R)
{
	CalculatePortResistance();
}

WDFGyrator::~WDFGyrator()
{
	
}

void WDFGyrator::CalculatePortResistance()
{
	// UP-port = primary, DOWN-port = secondary
	vecPorts[DOWN]->Rp = vecChildren[0]->vecPorts[RFP]->Rp;
	vecPorts[DOWN]->Gp = vecChildren[0]->vecPorts[RFP]->Gp;
	
	vecPorts[UP]->Rp = (R*R) / vecPorts[DOWN]->Rp;
	vecPorts[UP]->Gp = 1.0 / vecPorts[UP]->Rp;
}

void WDFGyrator::WaveUp()
{
	// get the wave value from the connected child
	vecChildren[0]->WaveUp();
	vecPorts[DOWN]->a = vecPorts[DOWN]->coupledPort->b;
	
	// set the wave value of the up-port
	vecPorts[UP]->b = -vecPorts[DOWN]->a * vecPorts[UP]->Rp / R;
}

void WDFGyrator::WaveDown()
{
	// get the wave value from the parent
	vecPorts[UP]->a = vecPorts[UP]->coupledPort->b;
	
	// set the wave value to the down-port
	vecPorts[DOWN]->b = vecPorts[UP]->a * R / vecPorts[UP]->Rp;
	vecChildren[0]->WaveDown();
}

//============================================================
// Dualizer
//============================================================
WDFDualizer::WDFDualizer(WDFObject* child, bool isInversed, string lbl, WDFType type) : WDFAdaptor(child, lbl, type), isInversed(isInversed)
{
	CalculatePortResistance();
}

WDFDualizer::~WDFDualizer()
{
	
}

void WDFDualizer::CalculatePortResistance()
{
	// UP-port = primary, DOWN-port = secondary
	vecPorts[DOWN]->Rp = vecChildren[0]->vecPorts[RFP]->Rp;
	vecPorts[DOWN]->Gp = vecChildren[0]->vecPorts[RFP]->Gp;
	
	vecPorts[UP]->Rp = vecPorts[DOWN]->Rp;
	vecPorts[UP]->Gp = vecPorts[DOWN]->Gp;
}

void WDFDualizer::WaveUp()
{
	// get the wave value from the connected child
	vecChildren[0]->WaveUp();
	vecPorts[DOWN]->a = vecPorts[DOWN]->coupledPort->b;
	
	// set the wave value of the up-port
	vecPorts[UP]->b = isInversed ? vecPorts[DOWN]->a : -vecPorts[DOWN]->a;
}

void WDFDualizer::WaveDown()
{
	// get the wave value from the parent
	vecPorts[UP]->a = vecPorts[UP]->coupledPort->b;
	
	// set the wave value to the down-port
	vecPorts[DOWN]->b = isInversed ? -vecPorts[UP]->a : vecPorts[UP]->a;
	vecChildren[0]->WaveDown();
}

//============================================================
// Series
//============================================================
WDFSeries::WDFSeries(unsigned int nPorts, string lbl, WDFType type) : WDFAdaptor(nPorts, DEFAULT_CHILDREN_COUNT_FOR_ADAPTOR, lbl, type)
{
	// create n ports, but doesn't connect any child
	// use AddChild function to connect the child
}

WDFSeries::WDFSeries(WDFObject* left, WDFObject* right, string lbl, WDFType type) : WDFAdaptor(left, right, lbl, type)
{
	CalculatePortResistance();
}

WDFSeries::~WDFSeries()
{

}

void WDFSeries::CalculatePortResistance()
{
	// get children's port resistances
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
	{
		WDFPort* port = *iter;
		if(port->coupledPort)
		{
			port->Rp = port->coupledPort->Rp;
			port->Gp = port->coupledPort->Gp;
		}
	}

	// set the RFP's port resistance
	double R = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		R += (*iter)->Rp;
	
	vecPorts[RFP]->Rp = R;
	vecPorts[RFP]->Gp = 1.0 / R;
}

void WDFSeries::WaveUp()
{
	// process wave propagation with children
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveUp();
	
	// get children's reflected wave
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		(*iter)->a = (*iter)->coupledPort->b;

	// calculate RFP
	double totalA = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		totalA -= (*iter)->a;
	
	vecPorts[RFP]->b = totalA;
}

void WDFSeries::WaveDown()
{
	// get incident wave from parent
	vecPorts[RFP]->a = vecPorts[RFP]->coupledPort->b;

	// get total wave values to this adaptor
	double totalA = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		totalA += (*iter)->a;
	
	// set wave to each port
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
	{
		WDFPort* port = (*iter);
		port->b = port->a - port->Rp / vecPorts[RFP]->Rp * totalA;
	}
	
	// process propatation of incident wave
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveDown();
}

//============================================================
// Parallel
//============================================================
WDFParallel::WDFParallel(unsigned int nPorts, string lbl, WDFType type) : WDFAdaptor(nPorts, DEFAULT_CHILDREN_COUNT_FOR_ADAPTOR, lbl, type)
{
	// create n ports, but doesn't connect any child
	// use AddChild function to connect the child
}
WDFParallel::WDFParallel(WDFObject *left, WDFObject *right, string lbl, WDFType type) : WDFAdaptor(left, right, lbl, type)
{
	CalculatePortResistance();
}

WDFParallel::~WDFParallel()
{

}

void WDFParallel::CalculatePortResistance()
{
	// get children's port resistances
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
	{
		WDFPort* port = *iter;
		if(port->coupledPort)
		{
			port->Rp = port->coupledPort->Rp;
			port->Gp = port->coupledPort->Gp;
		}
	}

	// set the RFP's port resistance
	double G = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		G += (*iter)->Gp;
	
	vecPorts[RFP]->Rp = 1.0 / G;
	vecPorts[RFP]->Gp = G;
}

void WDFParallel::WaveUp()
{
	// process wave propagation with children
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveUp();

	// get children's reflected wave
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		(*iter)->a = (*iter)->coupledPort->b;

	// calculate RFP
	double B = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		B += ((*iter)->Gp / vecPorts[RFP]->Gp) * (*iter)->a;
	
	vecPorts[RFP]->b = B;
}

void WDFParallel::WaveDown()
{
	// get incident wave from parent
	vecPorts[RFP]->a = vecPorts[RFP]->coupledPort->b;

	// calculate reflected wave
	double B = 0.0;
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		B += ((*iter)->Gp / vecPorts[RFP]->Gp) * (*iter)->a;

	// set wave to each port
	for(vec_wdfportptr::iterator iter = vecPorts.begin()+1; iter != vecPorts.end(); iter++)
		(*iter)->b = B + vecPorts[RFP]->a - (*iter)->a;
	
	// process propatation of incident wave
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveDown();
}

//============================================================
// R-Type (the root adaptor)
//============================================================
WDFRTypeAdaptor::WDFRTypeAdaptor(unsigned int nPorts, unsigned int nChildren, unsigned int nNodes, string lbl, WDFType type) : WDFAdaptor(nPorts, nChildren, lbl, type), MNA(nNodes, nPorts)
{
	// [0 I]
	mat ZI = zeros<mat>(nPorts, X.n_rows);
	long i=ZI.n_rows-1, j=ZI.n_cols-1;
	while(i >= 0 && j >= 0)
		ZI(i--,j--) = 1;

	// [0 I]^T
	ZIt = ZI.t();

	// [0 R]
	ZR = zeros<mat>(nPorts, X.n_rows);

	// I
	I = eye<mat>(ZI.n_rows, ZI.n_rows);
	
	// a, b
	a = mat(nPorts, 1);
	b = mat(nPorts, 1);
}

WDFRTypeAdaptor::~WDFRTypeAdaptor()
{

}

void WDFRTypeAdaptor::CalculatePortResistance()
{
	// get children's port resistances
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
	{
		WDFPort* port = *iter;
		if(port->coupledPort)
		{
			port->Rp = port->coupledPort->Rp;
			port->Gp = port->coupledPort->Gp;
		}
	}
}

void WDFRTypeAdaptor::WaveUp()
{
	// process wave propagation with children
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveUp();

	// get children's reflected wave
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		(*iter)->a = (*iter)->coupledPort->b;

	// set incident waves to the up port
	unsigned int nPorts = (unsigned int)vecPorts.size();
	for(unsigned int i=0; i<nPorts; i++)
		a(i,0) = vecPorts[i]->a;

	// calculate reflected waves
	b = S*a;

	// set reflected waves to the down ports
	for(unsigned int i=0; i<nPorts; i++)
		vecPorts[i]->b = b(i,0);
}

void WDFRTypeAdaptor::WaveDown()
{
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveDown();
}

void WDFRTypeAdaptor::Connect(unsigned int iResistor, unsigned int iVoltageSource, WDFObject* child)
{
	// add a port
	vecPorts.push_back(new WDFPort(child->vecPorts[RFP]->Rp, this));
	
	// add a child
	if(child)
	{
		AddChild(child);
		vecPorts.back()->SetCoupled(child->vecPorts[RFP]);
	}
	
	// get the index of port
	unsigned int iExternalNode = (unsigned int)vecPorts.size()-1;
	
	// convert to Thevenin port equivanet
	// 1. add resistor
	Add(MNA_Stamp_Resistor(iResistor, iExternalNode, child->vecPorts[RFP]->Gp));
	// 2. add voltage source: internal node is always connected to minus
	Add(MNA_Stamp_VoltageSource(iExternalNode, iVoltageSource));
}

void WDFRTypeAdaptor::UpdateScatteringMatrix()
{
	// update scattering matrix
	// S = I + 2 * [0 R] * X^(-1) * [0 I]^T
	long a=ZR.n_rows-1, b=ZR.n_cols-1;
	while(a >= 0 && b >= 0)
	{
		ZR(a,b) = vecPorts[a]->Rp;
		a--; b--;
	}
	
	S = I + 2 * ZR * inv(X) * ZIt;
//	S.save("S.txt", raw_ascii);
	
//	Y.save("Y.txt", raw_ascii);
//	B.save("B.txt", raw_ascii);
//	C.save("C.txt", raw_ascii);
//	D.save("D.txt", raw_ascii);
	
//	S.load("Smat.txt");
}

//============================================================
// R-Type (nonlinear root adaptor)
//============================================================
WDFRTypeAdaptorNL::WDFRTypeAdaptorNL(unsigned int nNLs, unsigned int nSubTrees, unsigned int nInternals, string lbl) : WDFAdaptor(nNLs+nSubTrees, nSubTrees, lbl, WDFType::R_TYPE_NL), MNA(nNLs+nSubTrees+nInternals, nNLs+nSubTrees), model(CircuitModel::DEFAULT)
{
	this->nNLs = nNLs;
	CreateMatrices(nNLs, nSubTrees);
	bFirstWave = true;
}

WDFRTypeAdaptorNL::WDFRTypeAdaptorNL(WDFObject* left, WDFObject* right, unsigned int nNLs, CircuitModel model, string label) : WDFAdaptor(nNLs+2, 2, label, WDFType::R_TYPE_NL), MNA(0, 0), model(model)
{
	this->nNLs = nNLs;
	CreateMatrices(nNLs, 2);	// the number of subtrees is 2(left & right)
	bFirstWave = true;
	
	// create nonlinear ports
	for(unsigned int i=0; i<nNLs; i++)
		vecPorts.push_back(new WDFPort(DEFAULT_R_TYPE_RP_VALUE, this));
	
	// set parent-child relationship
	// 1. left
	vecPorts.push_back(new WDFPort(left->vecPorts[RFP]->Rp, this));
	if(left)
	{
		AddChild(left);
		vecPorts.back()->SetCoupled(left->vecPorts[RFP]);
	}
	
	// 2. right
	vecPorts.push_back(new WDFPort(right->vecPorts[RFP]->Rp, this));
	if(right)
	{
		AddChild(right);
		vecPorts.back()->SetCoupled(right->vecPorts[RFP]);
	}
}

WDFRTypeAdaptorNL::~WDFRTypeAdaptorNL()
{
	
}

void WDFRTypeAdaptorNL::CalculatePortResistance()
{
	// get the children's port resistances
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
	{
		WDFPort* port = *iter;
		if(port->coupledPort)
		{
			port->Rp = port->coupledPort->Rp;
			port->Gp = port->coupledPort->Gp;
		}
	}
}

void WDFRTypeAdaptorNL::WaveUp()
{
	// 1. Process wave propagation from the children
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveUp();
	
	// 2. Get children's reflected wave
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		(*iter)->a = (*iter)->coupledPort->b;
	
	// 3. Set incident waves to the external-wave vector
	unsigned int nChildren = (unsigned int)vecChildren.size();
	for(unsigned int i=0; i<nChildren; i++)
		a_e(i) = vecPorts[nNLs+i]->a;
	
	//============================================================
	// 4. Execute Newton-Raphson iteration
	// 4-1. Guess initial value
	vec v_c(nNLs), i_c(nNLs);
	if(bFirstWave)
	{
		v_c.fill(0);
		bFirstWave = false;
	}
	else
	{
		v_c = E * a_e + F * i_c_prev;
	}
	
	// 4-2. Execute iteration
	v_c = Solve(v_c);
	
	// 4-3. Get current value(i_c)
	i_c = Nonlinear(v_c);
	//============================================================
//	a_e.print("a:");
//	v_c.print("v:");
//	i_c.print("i:");
	
	// 5. Calculate reflected waves
	b_e = M * a_e + N * i_c;
	
	// 6. Set reflected waves to the down ports
	for(unsigned int i=0; i<nChildren; i++)
		vecPorts[nNLs+i]->b = b_e(i);
	
	// 7. Save current value for next step
	i_c_prev = i_c;
	
	// 8. update the values
	UpdateNonlinearValues(v_c, i_c);
}

void WDFRTypeAdaptorNL::WaveDown()
{
	for(vec_wdfobjptr::iterator iter = vecChildren.begin(); iter != vecChildren.end(); iter++)
		(*iter)->WaveDown();
}

void WDFRTypeAdaptorNL::Connect(unsigned int iResistor, unsigned int iVoltageSource, WDFObject* child)
{
	// add a port
	vecPorts.push_back(new WDFPort(child->vecPorts[RFP]->Rp, this));
	
	// add a child
	if(child)
	{
		AddChild(child);
		vecPorts.back()->SetCoupled(child->vecPorts[RFP]);
	}
	
	// get the index of port
	unsigned int iExternalNode = (unsigned int)vecPorts.size()-1;
	
	// convert to Thevenin port equivanet
	// 1. add resistor
	Add(MNA_Stamp_Resistor(iResistor, iExternalNode, child->vecPorts[RFP]->Gp));
	// 2. add voltage source: internal node is always connected to minus
	Add(MNA_Stamp_VoltageSource(iExternalNode, iVoltageSource));
}

void WDFRTypeAdaptorNL::ConnectNL(unsigned int iResistor, unsigned int iVoltageSource, WDFPort* portOfRootLeaf)
{
	// get the index of external node
	unsigned int iExternalNode = (unsigned int)vecPorts.size();
	
	// create nonlinear ports
	vecPorts.push_back(new WDFPort(DEFAULT_R_TYPE_RP_VALUE, this));
	
	// couple internal ports with the root leaf
	WDFPort *decoupled = NULL;
	if((decoupled = GetDecoupledPort()) != NULL && portOfRootLeaf != NULL)
	{
		decoupled->SetCoupled(portOfRootLeaf);
	}
	
	//============================================================
	// convert to Thevenin port equivanet
	//============================================================
	// 1. add resistor
	Add(MNA_Stamp_Resistor(iResistor, iExternalNode, vecPorts.back()->Gp));
	
	// 2. add voltage source: iExternal -> (+), iVoltage -> (-)
	Add(MNA_Stamp_VoltageSource(iExternalNode, iVoltageSource));
}

void WDFRTypeAdaptorNL::ConnectNL(WDFRTypeRootLeaf* rootLeaf)
{
	// couple internal ports with the root leaf
	WDFPort *decoupled1 = NULL, *decoupled2 = NULL;
	
	while((decoupled1 = GetDecoupledPort()) != NULL && (decoupled2 = rootLeaf->GetDecoupledPort()) != NULL)
	{
		decoupled1->SetCoupled(decoupled2);
		decoupled1->Rp = decoupled2->Rp;
		decoupled1->Gp = decoupled2->Gp;
	}
}

void WDFRTypeAdaptorNL::UpdateMatrices()
{
	//============================================================
	// Update CONVERSION matrices
	//============================================================
	// Ri: internal port resistance matrix(nonlinear)
	// Re: external port resistance matrix(subtrees)
	// C = [[C11, C12], [C21, C22]] = [[-Ri, I], [-2Ri, I]]
	for(unsigned int i=0; i<nNLs; i++)
	{
		C11(i,i) = -vecPorts[i]->Rp;
		C21(i,i) = -2.0 * vecPorts[i]->Rp;
	}
	for(unsigned int r=0; r<C.n_rows; r++)
	{
		for(unsigned int c=0; c<C.n_cols; c++)
		{
			if(r < nNLs && c < nNLs)		C(r,c) = C11(r, c);
			else if(r < nNLs && c >= nNLs)	C(r,c) = C12(r, c-nNLs);
			else if(r >= nNLs && c < nNLs)	C(r,c) = C21(r-nNLs, c);
			else if(r >= nNLs && c >= nNLs)	C(r,c) = C22(r-nNLs, c-nNLs);
		}
	}
//	C.save("C.txt", raw_ascii);
	
	//============================================================
	// Update SCATTERING matrices
	//============================================================
	// create [0 R]
	// R = diag(Ri, Re)
	long i=ZR.n_rows-1, j=ZR.n_cols-1;
	while(i >= 0 && j >= 0)
	{
		ZR(i,j) = vecPorts[i]->Rp;
		i--; j--;
	}
	
	if(model == CircuitModel::DEFAULT)		// MNA is enable
	{
		// S = I + 2 * [0 R] * X^(-1) * [0 I]^T
		mat I = eye<mat>(ZR.n_rows, ZIt.n_cols);
		S = I + 2 * ZR * X.i() * ZIt;
//		S.load("Smat.txt");
	}
	else
	{
		S = mat(S11.n_rows + S21.n_rows, S11.n_cols + S12.n_cols);
		// reflection coefficient of each port
		const unsigned int size = (unsigned int)vecPorts.size();
		double *refs = new double[size];
		
		if(model == CircuitModel::PARALLEL)	// parallel
		{
			// set reflection coefficients
			double Gp_total = 0.0;
			for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
				Gp_total += (*iter)->Gp;
			
			for(unsigned int i=0; i<size; i++)
				refs[i] = 2.0 * vecPorts[i]->Gp / Gp_total;
			
			// set scattering matrix
			for(unsigned int r=0; r<S.n_rows; r++)
				for(unsigned int c=0; c<S.n_cols; c++)
					S(r,c) = (r == c) ? (refs[c]-1.0) : refs[c];
		}
		else	// series
		{
			// set reflection coefficients
			double Rp_total = 0.0;
			for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
				Rp_total += (*iter)->Rp;
			
			for(unsigned int i=0; i<size; i++)
				refs[i] = 2.0 * vecPorts[i]->Rp / Rp_total;
			
			// set scattering matrix
			for(unsigned int r=0; r<S.n_rows; r++)
				for(unsigned int c=0; c<S.n_cols; c++)
					S(r,c) = (r == c) ? (1.0-refs[r]) : -refs[r];
		}
		delete[] refs;
	}
	
	// partition S into S11 ~ S22
	for(unsigned int r=0; r<S.n_rows; r++)
	{
		for(unsigned int c=0; c<S.n_cols; c++)
		{
			if(r < nNLs && c < nNLs)
				S11(r,c) = S(r,c);
			else if(r < nNLs && c >= nNLs)
				S12(r,c-nNLs) = S(r,c);
			else if(r >= nNLs && c < nNLs)
				S21(r-nNLs, c) = S(r,c);
			else
				S22(r-nNLs, c-nNLs) = S(r,c);
		}
	}
//	S.save("S.txt", raw_ascii);
	
	//============================================================
	// create H, E, F, M, N
	mat I = eye<mat>(C22.n_rows, S11.n_cols);
//	S11.print("S11:");
	mat H = inv(I - C22 * S11);
	E = C12 * (I + S11 * H * C22) * S12;
	F = C12 * S11 * H *	C21 + C11;
	M = S21 * H * C22 * S12 + S22;
	N = S21 * H * C21;
	
//	E.save("E.txt", raw_ascii);
//	F.save("F.txt", raw_ascii);
//	M.save("M.txt", raw_ascii);
//	N.save("N.txt", raw_ascii);
}

void WDFRTypeAdaptorNL::CreateMatrices(unsigned int nNLs, unsigned int nSubTrees)
{
	//============================================================
	// Set the size of conversion matrices
	// Ri = nNLs x nNLs
	// Re = nSubTrees x nSubTrees
	//============================================================
	C11 = zeros<mat>(nNLs, nNLs);	// -Ri
	C12 = eye<mat>(nNLs, nNLs);		// I
	C21 = zeros<mat>(nNLs, nNLs);	// -2Ri
	C22 = eye<mat>(nNLs, nNLs);		// I
	C = mat(nNLs*2, nNLs*2);
	
	//============================================================
	// Set the size of scattering matrices
	// R = diag(Ri, Re)
	//============================================================
	// [0 I]
	unsigned int nPorts = nNLs + nSubTrees;
	mat ZI = zeros<mat>(nPorts, X.n_rows);
	long i=ZI.n_rows-1, j=ZI.n_cols-1;
	while(i >= 0 && j >= 0)
		ZI(i--,j--) = 1;
	
	// [0 I]^T
	ZIt = ZI.t();
	
	// [0 R]
	ZR = zeros<mat>(nPorts, X.n_rows);
	
	// S11, S12, S21, S22
	S11 = mat(nNLs, nNLs);
	S12 = mat(nNLs, nSubTrees);
	S21 = mat(nSubTrees, nNLs);
	S22 = mat(nSubTrees, nSubTrees);
	
	//============================================================
	// a, b(for the external)
	//============================================================
	a_e = vec(nSubTrees);
	b_e = vec(nSubTrees);
	
	//============================================================
	// previous voltage values
	//============================================================
	i_c_prev = vec(nNLs);
	i_c_prev.fill(0);
}

vec WDFRTypeAdaptorNL::Evaluate(vec v_c)
{
	return E * a_e + F * Nonlinear(v_c) - v_c;
}

mat WDFRTypeAdaptorNL::GetJacobian(vec v_c)
{
	mat J(nNLs, nNLs), dI(nNLs, nNLs);
	J.fill(0.0);
	dI.fill(0.0);
	
	unsigned int i=0, j=0, r=0, c=0;
	do {
		// get each nonlinear elements
		WDFRTypeRootLeaf* leaf = (WDFRTypeRootLeaf*)vecPorts[i]->coupledPort->owner;
		
		// cut v_c
		vec temp_v_c(leaf->vecPorts.size());
		for(unsigned int a=0; a<temp_v_c.size(); a++)
		{
			temp_v_c(a) = v_c(j);
			j++;
		}
		
		// get Jacobian matrix
		mat temp_dI = leaf->DiffNonlinear(temp_v_c);
		
		// copy to the differential current matrix
		for(unsigned int a=0; a<temp_dI.n_rows; a++)
		{
			c = i;
			for(unsigned int b=0; b<temp_dI.n_cols; b++)
			{
				dI(r,c) = temp_dI(a,b);
				c++;
			}
			r++;
		}
		
		// update the indices
		i += leaf->vecPorts.size();
	} while(i < nNLs);
	
	// calculate the whole matrix
	J = F * dI - eye<mat>(F.n_rows, dI.n_cols);
	
	return J;
}

vec WDFRTypeAdaptorNL::Nonlinear(vec v_c)
{
	vec i_c(nNLs);
	
	unsigned int i=0, j=0;
	do
	{
		WDFRTypeRootLeaf* leaf = (WDFRTypeRootLeaf*)vecPorts[i]->coupledPort->owner;
		
		// cut v_c
		vec temp_v_c(leaf->vecPorts.size());
		for(unsigned int a=0; a<temp_v_c.size(); a++)
			temp_v_c(a) = v_c(j++);
		
		// execute nonlinear function
		vec temp_i_c = leaf->Nonlinear(temp_v_c);
		
		// copy to current vector
		for(unsigned int a=0; a<temp_i_c.size(); a++)
			i_c(i++) = temp_i_c(a);
		
	} while(i < nNLs);
	
	return i_c;
}

void WDFRTypeAdaptorNL::UpdateNonlinearValues(vec Vprev, vec Iprev)
{
	unsigned int i=0;
	do
	{
		WDFRTypeRootLeaf* leaf = (WDFRTypeRootLeaf*)vecPorts[i]->coupledPort->owner;
		
		// cut Vprev & Iprev to the current port
		vec Vtemp(leaf->vecPorts.size());
		vec Itemp(leaf->vecPorts.size());
		for(unsigned int a=0; a<Vtemp.size(); a++)
		{
			Vtemp(a) = Vprev(i);
			Itemp(a) = Iprev(i);
			i++;
		}
		
		// set the current values
		leaf->UpdateValues(Vtemp, Itemp);
	} while(i < nNLs);
}

//============================================================
// Root Leaf
//============================================================
WDFRootLeaf::WDFRootLeaf(string lbl, WDFType type) : WDFObject(1, 1, lbl, type)
{
	// root leaf has one port
	vecPorts.push_back(new WDFPort(1.0, this));
}

WDFRootLeaf::WDFRootLeaf(WDFAdaptor* adaptor, string lbl, WDFType type) : WDFObject(1, 1, lbl, type)
{
	// root leaf has one port
	vecPorts.push_back(new WDFPort(1.0, this));

	// connect to the tree
	Connect(adaptor);
}

WDFRootLeaf::~WDFRootLeaf()
{
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		delete *iter;
}

void WDFRootLeaf::Connect(WDFObject* adaptor)
{
	// root leaf has one child, no parent
	if(adaptor)
	{
		AddChild(adaptor);
		vecPorts[0]->SetCoupled(adaptor->vecPorts[RFP]);
	}
	
	// set port resistance
	CalculatePortResistance();
}

void WDFRootLeaf::CalculatePortResistance()
{
	// get port resistance
	WDFPort* port = vecPorts[0];
	if(port->coupledPort)
	{
		port->Rp = port->coupledPort->Rp;
		port->Gp = port->coupledPort->Gp;
	}
}

void WDFRootLeaf::UpdatePortResistance()
{
	vecChildren[0]->UpdatePortResistance();
	CalculatePortResistance();
}

void WDFRootLeaf::WaveUp()
{
	// process wave propagation to the root
	vecChildren[0]->WaveUp();
	
	// get incident wave
	vecPorts[0]->a = vecPorts[0]->coupledPort->b;
	
	// evaluate reflected wave
	EvaluateReflectedWave();
}

void WDFRootLeaf::WaveDown()
{
	vecChildren[0]->WaveDown();
}

double WDFRootLeaf::GetVoltage()
{
	return vecPorts[0]->GetVoltage();
}

double WDFRootLeaf::GetCurrent()
{
	return vecPorts[0]->GetCurrent();
}

//============================================================
// Root Leaf(connected to the R-Type using connectNL function)
// This class can create multiports, but not children
//============================================================
WDFRTypeRootLeaf::WDFRTypeRootLeaf(unsigned int nPorts, string lbl, WDFType type) : WDFObject(nPorts, 0, lbl, type)
{
	// create ports
	for(unsigned int i=0; i<nPorts; i++)
		vecPorts.push_back(new WDFPort(DEFAULT_R_TYPE_RP_VALUE, this));
	
	// create matrices for previous voltage and current values
	Vprev = zeros<vec>(nPorts);
	Iprev = zeros<vec>(nPorts);
}

WDFRTypeRootLeaf::~WDFRTypeRootLeaf()
{
	for(vec_wdfportptr::iterator iter = vecPorts.begin(); iter != vecPorts.end(); iter++)
		delete *iter;
}

void WDFRTypeRootLeaf::UpdatePortResistance()
{
	
}

void WDFRTypeRootLeaf::CalculatePortResistance()
{
	
}

void WDFRTypeRootLeaf::WaveUp()
{
	
}

void WDFRTypeRootLeaf::WaveDown()
{
	
}

double WDFRTypeRootLeaf::GetVoltage(unsigned int iPort)
{
	return vecPorts[iPort]->GetVoltage();
}

double WDFRTypeRootLeaf::GetCurrent(unsigned int iPort)
{
	return vecPorts[iPort]->GetCurrent();
}

void WDFRTypeRootLeaf::UpdateValues(vec Vprev, vec Iprev)
{
	this->Vprev = Vprev;
	this->Iprev = Iprev;
}
