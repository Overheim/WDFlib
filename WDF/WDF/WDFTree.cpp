//
//  WDFTree.cpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#include "WDFTree.hpp"

WDFTree::WDFTree(float T, float V, float F)
{
	this->T = T;
	this->V = V;
	this->F = F;
}

WDFTree::~WDFTree()
{
	for(WDFMap::iterator mapIter = wdfMap.begin(); mapIter != wdfMap.end(); mapIter++)
		delete (*mapIter).second;
}

float WDFTree::Process(float Vin)
{
	// Check the NULLs
	if(!wdfInput || !wdfRoot)
		return 0.0;
	
	// Set input voltage
	wdfInput->Vs = Vin;
	
	// Wave up & down
	wdfRoot->WaveUp();
	wdfRoot->WaveDown();
	
	// Returns the output voltage
	float Vout = 0.0;
	for(WDFVector::iterator iter = wdfOutputs.begin(); iter != wdfOutputs.end(); iter++)
		Vout += (*iter)->vecPorts[0]->GetVoltage();
	
	return Vout;
}

void WDFTree::AddObject(WDFObject* object)
{
	wdfMap[object->label] = object;
}

WDFObject* WDFTree::FindObject(string id)
{
	return wdfMap[id];
}

float WDFTree::GetSamplingTime()
{
	return T;
}

float WDFTree::GetInputVoltage()
{
	return V;
}

float WDFTree::GetInputFrequency()
{
	return F;
}

void WDFTree::SetInput(WDFObject* input)
{
	wdfInput = dynamic_cast<WDFVoltageSource*>(input);
}

void WDFTree::SetRoot(WDFObject* root)
{
	wdfRoot = root;
}

void WDFTree::SetOutput(WDFObject* output)
{
	wdfOutputs.push_back(output);
}

void WDFTree::Clear()
{
	for(WDFMap::iterator mapIter = wdfMap.begin(); mapIter != wdfMap.end(); mapIter++)
	{
		WDFObject* wdfObj = (*mapIter).second;
		for(vector<WDFPort*>::iterator portIter = wdfObj->vecPorts.begin(); portIter != wdfObj->vecPorts.end(); portIter++)
		{
			(*portIter)->a = 0.0;
			(*portIter)->b = 0.0;
		}
	}
}
