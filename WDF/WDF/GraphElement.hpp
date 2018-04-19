//
//  GraphElement.hpp
//  WDF
//
//  Created by Won Jae Lee on 2018. 1. 24..
//  Copyright © 2018년 Anti Mouse. All rights reserved.
//

#ifndef GraphElement_hpp
#define GraphElement_hpp

#include <string>

using namespace std;

/**
 the number of properties of the electric circuit component
 */
#define NUM_OF_PROPS	8

/**
 the number of options of the electric circuit component
 */
#define NUM_OF_OPTS		8

/**
 A structure for the EDGE, which is the element of the graph
 */
typedef struct _Edge
{
	/**
	 An array that contains the properties of the electric circuit components
	 */
	float	values[NUM_OF_PROPS];
	
	/**
	 An array that contains the options of the electric circuit components
	 */
	bool	options[NUM_OF_OPTS];
	
	/**
	 The id of this edge. It's unique.
	 */
	string	id;
	
	/**
	 A pair of vertices that is adjacent through this edge
	 */
	pair<string, string> vertex_pair;
	
	/**
	 Creates an instance with initial values.
	 */
	_Edge()
	{
		Clear();
	}
	
	/**
	 Copy an edge.
	 
	 @param edge an edge to be copied
	 */
	_Edge(const _Edge &edge)
	{
		id = edge.id;
		
		for(int i=0; i<NUM_OF_PROPS; i++)
			values[i] = edge.values[i];
		
		for(int i=0; i<NUM_OF_OPTS; i++)
			options[i] = edge.options[i];
	}
	
	~_Edge()
	{
		
	}
	
	/**
	 Set all variables to the initial value
	 */
	void Clear()
	{
		id = "";
		vertex_pair.first = "";
		vertex_pair.second = "";
		
		for(int i=0; i<NUM_OF_PROPS; i++)
			values[i] = 0.0;
		
		for(int i=0; i<NUM_OF_OPTS; i++)
			options[i] = false;
	}
} Edge;


//============================================================
// The indices for values
//============================================================
/**
 the number of ports
 */
#define VAL_NUM_OF_PORTS			0

/**
 an input frequency of voltage source
 */
#define VAL_FREQUENCY				(VAL_NUM_OF_PORTS + 1)

/**
 an input voltage of voltage source
 */
#define VAL_VOLTAGE					(VAL_FREQUENCY + 1)

/**
 a resistance
 */
#define VAL_RESISTANCE				(VAL_NUM_OF_PORTS + 1)

/**
 a capacitance
 */
#define VAL_CAPACITANCE				(VAL_NUM_OF_PORTS + 1)

/**
 a inductance
 */
#define VAL_INDUCTANCE				(VAL_NUM_OF_PORTS + 1)

/**
 the model of vacuum tube
 */
#define VAL_VACUUM_TUBE_MODEL		(VAL_NUM_OF_PORTS + 1)

/**
 the model of transistor
 */
#define VAL_TRANSISTOR_MODEL		(VAL_NUM_OF_PORTS + 1)

//============================================================
// The indices for options
//============================================================
/**
 A dummy component
 */
#define OPT_DUMMY			0

/**
 Set as input
 */
#define OPT_INPUT			(OPT_DUMMY+1)

/**
 Set as output
 */
#define OPT_OUTPUT			(OPT_INPUT+1)

/**
 A nonlinear component
 */
#define OPT_NONLINEAR		(OPT_OUTPUT+1)

/**
 A pole: true as positive, false as negative
 */
#define OPT_POLE			(OPT_NONLINEAR+1)

#endif /* GraphElement_hpp */
