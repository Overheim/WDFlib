#ifndef	WDF_HPP
#define	WDF_HPP

#include <vector>
#include <string>
#include "MNA.hpp"
#include "NewtonRaphson.h"

using namespace std;

// port index
enum { RFP=0, LEFT, RIGHT, UP=0, DOWN=1 };

// element type
enum class WDFType
{
	OBJECT,
	LEAF,
	RESISTOR,
	CAPACITOR,
	INDUCTOR,
	VOLTAGE_SOURCE,
	OPEN_CIRCUIT,
	ADAPTOR,
	INVERTER,
	IDEAL_TRANSFORMER,
	GYRATOR,
	DUALIZER,
	SERIES,
	PARALLEL,
	R_TYPE,
	R_TYPE_NL,
	SCATTERING_ADAPTOR,
	SCATTERING_SERIES,
	SCATTERING_PARALLEL,
	ROOT_LEAF,
	R_TYPE_ROOT_LEAF
};

// use this enum element with constructor case of three-port adaptor with two subtrees and one nonlinear element
enum class CircuitModel
{
	SERIES,
	PARALLEL,
	DEFAULT
};

class WDFObject;
class WDFRTypeRootLeaf;

//============================================================
// port variables
//============================================================
class WDFPort
{
public:
	WDFPort(double R, WDFObject* owner=NULL);
	~WDFPort();

	double GetVoltage();		// get the voltage value of the port
	double GetCurrent();		// get the current value of the port
	void SetCoupled(WDFPort* port);

	double Rp, Gp;				// port resistances
	double a, b;				// wave values
	WDFPort* coupledPort;		// coupled port(WDFLeaf <-> WDFAdaptor, WDFAdaptor <-> WDFAdaptor)
	WDFObject* owner;			// WDF element which owns this port
};

//============================================================
// basic wave element
//============================================================
class WDFObject
{
public:
	string label;						// the name of object
	WDFType type;						// the type of object
	vector<WDFPort*> vecPorts;			// the ports of element. Minimum 1 port must be existed, and port #0 means RFP

	WDFObject* parent;					// coupled parent node
	vector<WDFObject*> vecChildren;		// coupled child nodes

	WDFObject(string lbl="WDFObject", WDFType type=WDFType::OBJECT);	// constructor
	WDFObject(unsigned int nPorts, unsigned int nChildren, string lbl="WDFObject", WDFType type=WDFType::OBJECT);	// constructor with the size of ports & children
	virtual ~WDFObject();								// destructor

	virtual void AddChild(WDFObject* child);			// couple the wdf objects(set this object to the parent)
	virtual WDFPort* GetDecoupledPort();				// get WDFPort object not coupled

	virtual void CalculatePortResistance() = 0;			// caculating the resistance of each port
	virtual void UpdatePortResistance() = 0;			// update the resistances after calculating port resistance from the root
	
	virtual void WaveUp() = 0;							// reflected wave to the root
	virtual void WaveDown() = 0;						// incident wave from the root
};

//============================================================
// basic wave element - leaf(base class of resistor, capacitor, voltage source, ...)
//============================================================
class WDFLeaf : public WDFObject
{
public:
	WDFLeaf(double R, string lbl="Leaf", WDFType type=WDFType::LEAF);
	virtual ~WDFLeaf();

	virtual void CalculatePortResistance();				// nothing occurs. only for nonlinear case.
	virtual void UpdatePortResistance();				// nothing occurs. only for nonlinear case.
	
	virtual void WaveUp() = 0;
	virtual void WaveDown();

	virtual double GetVoltage();						// get voltage value of the port
	virtual double GetCurrent();						// get current value of the port
};

//============================================================
// Resistor
//============================================================
class WDFResistor : public WDFLeaf
{
public:
	WDFResistor(double R, string lbl="Resistor", WDFType type=WDFType::RESISTOR);
	virtual ~WDFResistor();

	virtual void WaveUp();
};

//============================================================
// Capacitor
//============================================================
class WDFCapacitor : public WDFLeaf
{
public:
	double T;				// sampling interval

	WDFCapacitor(double C, double T, string lbl="Capacitor", WDFType type=WDFType::CAPACITOR);
	virtual ~WDFCapacitor();
	
	virtual void WaveUp();
};

//============================================================
// Inductor
//============================================================
class WDFInductor : public WDFLeaf
{
public:
	double T;				// sampling interval

	WDFInductor(double L, double T, string lbl="Inductor", WDFType type=WDFType::INDUCTOR);
	virtual ~WDFInductor();
	
	virtual void WaveUp();
};

//============================================================
// Voltage Source
//============================================================
class WDFVoltageSource : public WDFLeaf
{
public:
	double Vs;				// volatage source value(or input value)

	WDFVoltageSource(double Vs, double R, string lbl="VoltageSource", WDFType type=WDFType::VOLTAGE_SOURCE);
	virtual ~WDFVoltageSource();
	
	virtual void WaveUp();
};

//============================================================
// Open Circuit
//============================================================
class WDFOpenCircuit : public WDFLeaf
{
public:
	WDFOpenCircuit(string lbl="Open Circuit", WDFType type=WDFType::OPEN_CIRCUIT);
	virtual ~WDFOpenCircuit();
	
	virtual void WaveUp();
};

//============================================================
// wave element - adaptor(base class of ideal transformer, parallel, R-type, ...)
//============================================================
class WDFAdaptor : public WDFObject
{
public:
	WDFAdaptor(unsigned int nPorts, unsigned int nChildren, string lbl="Adaptor", WDFType type=WDFType::ADAPTOR);	// n-port
	WDFAdaptor(WDFObject* child, string lbl="Adaptor", WDFType type=WDFType::ADAPTOR);						// two-port
	WDFAdaptor(WDFObject* left, WDFObject* right, string lbl="Adaptor", WDFType type=WDFType::ADAPTOR);	// three-port
	virtual ~WDFAdaptor();
	
	virtual void UpdatePortResistance();
	virtual void CalculatePortResistance() = 0;			// calculate the resistance of each port
	virtual void Connect(WDFObject* child);

	virtual void WaveUp() = 0;
	virtual void WaveDown() = 0;
};

//============================================================
// Inverter
//============================================================
class WDFInverter : public WDFAdaptor
{
public:
	WDFInverter(WDFObject* child, string lbl="Inverter", WDFType type=WDFType::INVERTER);
	virtual ~WDFInverter();

	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();
};

//============================================================
// Ideal Transformer
//============================================================
class WDFIdealTransformer : public WDFAdaptor
{
public:
	WDFIdealTransformer(WDFObject* secondary, double N, string lbl="Ideal Transformer", WDFType type=WDFType::IDEAL_TRANSFORMER);
	virtual ~WDFIdealTransformer();

	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();

protected:
	double N;	// turns ratio = 1 : N = Ns / Np = V2 / V1
};

//============================================================
// Gyrator
//============================================================
class WDFGyrator : public WDFAdaptor
{
public:
	WDFGyrator(WDFObject* child, double R, string lbl="Gyrator", WDFType type=WDFType::GYRATOR);
	virtual ~WDFGyrator();
	
	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();
	
protected:
	double R;	// V1 = -R * I2, V2 = R * I1
};

//============================================================
// Dualizer: gyrator in case of R = R1 = R2
//============================================================
class WDFDualizer : public WDFAdaptor
{
public:
	WDFDualizer(WDFObject* child, bool isInversed=false, string lbl="Dualizer", WDFType type=WDFType::DUALIZER);
	virtual ~WDFDualizer();
	
	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();
	
protected:
	bool isInversed;
};

//============================================================
// Series
//============================================================
class WDFSeries : public WDFAdaptor
{
public:
	WDFSeries(unsigned int nPorts, string lbl="Series", WDFType type=WDFType::SERIES);
	WDFSeries(WDFObject* left, WDFObject* right, string lbl="Series", WDFType type=WDFType::SERIES);
	virtual ~WDFSeries();

	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();
};

//============================================================
// Parallel
//============================================================
class WDFParallel : public WDFAdaptor
{
public:
	WDFParallel(unsigned int nPorts, string lbl="Parallel", WDFType type=WDFType::PARALLEL);
	WDFParallel(WDFObject* left, WDFObject* right, string lbl="Paralel", WDFType type=WDFType::PARALLEL);
	virtual ~WDFParallel();

	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();
};

//============================================================
// R-Type (the root adaptor)
//============================================================
class WDFRTypeAdaptor : public WDFAdaptor, public MNA
{
public:
	WDFRTypeAdaptor(unsigned int nPorts, unsigned int nChildren, unsigned int nNodes, string lbl="R-Type", WDFType type=WDFType::R_TYPE);
	virtual ~WDFRTypeAdaptor();

	virtual void CalculatePortResistance();
	virtual void WaveUp();
	virtual void WaveDown();

	virtual void Connect(unsigned int i, unsigned int j, WDFObject* child);		// connect the child to this
	virtual void UpdateScatteringMatrix();

protected:
	mat S;		// scattering matrix
	mat a,b;	// wave matrices
	mat ZIt, ZR, I;
};

//============================================================
// R-Type (nonlinear root adaptor)
//============================================================
class WDFRTypeAdaptorNL : public WDFAdaptor, public MNA, public /*Broyden2*/QuasiNewton
{
public:
	/*
	 Create R-Type adaptor using MNA algorithm.
	 nNLs: the number of nonlinear ports
	 nSubTrees: the number of linear elements connected to the root
	 nNodes: the number of nodes inside the root.
	 This adaptor has (nNLs + nSubTrees) ports, (nNLs+nSubTrees+nNodes) nodes.
	 */
	WDFRTypeAdaptorNL(unsigned int nNLs, unsigned int nSubTrees, unsigned int nInternals, string lbl="R-Type NL");
	
	/*
	 Create classical series/parallel adaptor with multiple nonlinear elements.
	 The number of internal nodes in adaptor is zero, so MNA is not used(k-methode only).
	 Each subtree doesn't need to execute the Connect function.
	 */
	WDFRTypeAdaptorNL(WDFObject* left, WDFObject* right, unsigned int nNLs, CircuitModel model, string lbl="R-Type NL");
	
	virtual ~WDFRTypeAdaptorNL();
	
	virtual void CalculatePortResistance();
	
	virtual void WaveUp();
	virtual void WaveDown();
	
	/*
	 Connect the child object to this.
	 iResistor: the number of nodes to which the resistor of Thevenin source is connected inside the root
	 iVoltageSource: the number of nodes to which the voltage source of Thevenin source is connected inside the root
	 */
	virtual void Connect(unsigned int iResistor, unsigned int iVoltageSource, WDFObject* child);
	
	/*
	 Adds the port connected to the nonlinear element with port resistance.
	 iResistor: the index of node to which the resistor of the Thevenin source is connected inside the root
	 iVoltageSource: the index of node to which the voltage source of the Thevenin source is connected inside the root
	 */
	virtual void ConnectNL(unsigned int iResistor, unsigned int iVoltageSource, WDFPort* portOfRootLeaf);
	
	/*
	 Connect the nonlinear elements to this.
	 */
	virtual void ConnectNL(WDFRTypeRootLeaf* rootLeaf);
	
	/*
	 Update scattering(S, S11~S22) and conversion(C, C11~C22) matrix
	 */
	virtual void UpdateMatrices();
	
protected:
	mat S, S11, S12, S21, S22;		// scattering matrices
	mat C, C11, C12, C21, C22;		// conversion matrices
	mat p, E, F, M, N;				// K-method
	vec a_e, b_e;					// wave vectors
	vec i_c_prev;					// previous current values
	mat ZIt, ZR;
	
	CircuitModel model;				// model of the root(parallel, series, ...)
	unsigned int nNLs;				// the number of nonlinear ports
	
	// create matrices(scattering & conversion)
	virtual void CreateMatrices(unsigned int nNLs, unsigned int nSubTrees);
	
	// Newton-Raphson evaluation function (F(x) = 0)
	virtual vec Evaluate(vec v_c);
	
	// Newton-Raphson Jacobian matrix calculation
	virtual mat GetJacobian(vec v_c);
	
	// Nonlinear function (f:v -> i)
	virtual vec Nonlinear(vec v_c);
	
	// the flag value whether the iteration is first process
	bool bFirstWave;
	
	// update the values of nonlinear elements
	void UpdateNonlinearValues(vec Vprev, vec Iprev);
};

//============================================================
// Root Leaf
//============================================================
class WDFRootLeaf : public WDFObject
{
public:
	WDFRootLeaf(string lbl="Root Leaf", WDFType type=WDFType::ROOT_LEAF);
	WDFRootLeaf(WDFAdaptor* adaptor, string lbl="Root Leaf", WDFType type=WDFType::ROOT_LEAF);
	virtual ~WDFRootLeaf();
	
	virtual void Connect(WDFObject* adaptor);			// connect to the tree
	virtual void UpdatePortResistance();				// update port resistances
	virtual void CalculatePortResistance();				// set the port resistances
	
	virtual void WaveUp();								// wave propagation from the children
	virtual void WaveDown();							// wave propagation to the children

	virtual double GetVoltage();						// get voltage value
	virtual double GetCurrent();						// get current value
	
protected:
	virtual void EvaluateReflectedWave() = 0;			// evaluate reflected wave then set to the port
};

//============================================================
// Root Leaf(connected to the R-Type using connectNL function)
// This is a abstract class, so must be inherited.
// Support multiport.
//============================================================
class WDFRTypeRootLeaf : public WDFObject
{
public:
	WDFRTypeRootLeaf(unsigned int nPorts, string lbl="R-Type Root Leaf", WDFType type=WDFType::R_TYPE_ROOT_LEAF);
	virtual ~WDFRTypeRootLeaf();
	
	virtual void UpdatePortResistance();				// update port resistance(not used)
	virtual void CalculatePortResistance();				// set the port resistance(not used)
	
	virtual void WaveUp();
	virtual void WaveDown();
	
	virtual double GetVoltage(unsigned int iPort=RFP);	// get the voltage value of the port
	virtual double GetCurrent(unsigned int iPort=RFP);	// get the current value of the port
	
	virtual void UpdateValues(vec Vprev, vec Iprev);	// update the values for nonlinear equation
	
	virtual vec Nonlinear(vec Vc) = 0;					// calculate Ic fed to the root(Ic = F(Vc))
	virtual mat DiffNonlinear(vec Vc) = 0;				// calculate Jacobian matrix
	
protected:
	vec Vprev, Iprev;		// variables for reactances
};

#endif	//WDF_HPP
