//
//  WDFTree.hpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef WDFTree_hpp
#define WDFTree_hpp

#include "WDF.hpp"
#include <map>

/**
 A type for storing the WDF objects and finding the object using id
 */
typedef map<string, WDFObject*>	WDFMap;

/**
 A type for the storing the WDF objects as array
 */
typedef vector<WDFObject*>		WDFVector;

/**
 A class for building a tree of WDF objects. It takes an (audio) sample as input, process filteration, then creates an output (audio) sample.
 */
class WDFTree
{
public:
	/**
	 Create an WDFTree instance
	 
	 @param T sampling period
	 @param V voltage of the sample
	 @param F frequency of the sample
	 */
	WDFTree(float T, float V, float F);
	~WDFTree();
	
	/**
	 A process function of the tree
	 
	 @param Vin input voltage
	 @return output voltage
	 */
	float Process(float Vin);
	
	/**
	 Add an WDF object to the tree with option
	 
	 @param object an WDF object to be added
	 */
	void AddObject(WDFObject* object);
	
	/**
	 Find an WDF object with id
	 
	 @param id an id to search
	 @return an WDF object with which the id is same
	 */
	WDFObject* FindObject(string id);
	
	/**
	 Get the sampling time
	 
	 @return the sampling time
	 */
	float GetSamplingTime();
	
	/**
	 Get the voltage of the input source(gain)
	 
	 @return the input voltage of the input source(gain)
	 */
	float GetInputVoltage();
	
	/**
	 Get the frequency of the input source
	 
	 @return the frequency of the input source
	 */
	float GetInputFrequency();
	
	/**
	 Set an WDF object to the input
	 
	 @param input an WDF object to be set as input
	 */
	void SetInput(WDFObject* input);
	
	/**
	 Set an WDF object to the root
	 
	 @param root an WDF object to be set as root
	 */
	void SetRoot(WDFObject* root);
	
	/**
	 Set an WDF object to the output
	 
	 @param output an WDF object to be set as output
	 */
	void SetOutput(WDFObject* output);
	
	/**
	 Clear all wave values in the tree elements
	 */
	void Clear();
	
protected:
	/**
	 the sampling period
	 */
	float T;
	
	/**
	 the input voltage
	 */
	float V;
	
	/**
	 the input frequency
	 */
	float F;
	
	/**
	 a map for the WDF objects - [id : object]
	 */
	WDFMap wdfMap;
	
	/**
	 an WDF object which is set as root
	 */
	WDFObject* wdfRoot;
	
	/**
	 an WDF object which is set as input
	 */
	WDFVoltageSource* wdfInput;
	
	/**
	 a vector of WDF objects which is set as output
	 */
	WDFVector wdfOutputs;
};

#endif /* WDFTree_hpp */
