#pragma once

#include "Network.h"
#include <iostream>
#include <cstdlib>
#include "math.h"
#include <time.h>

using namespace std;

class Alignment
{
public:

	int *alignment; //array of alignment of two networks
	int *comp; //array of connected components in the resulted alignment
	int maxComp; //maximum connected component of the resulted alignment
	int maxDeg; //max degree of two networks
	bool reverse; //determine which networks is bigger
    float** blast;
	Network network1;
	Network network2;
    
    //constructor
    //finds the smaller network and the maximum degree of the input networks
    //Inputs are two files of networks net1 and net2
	Alignment( Network& net1, Network& net2);
	
    //constructor.does nothing !
    Alignment(void);
    
    //destructor
	~Alignment(void);
    
    //produce a mapping between nodes of two network with respect to input parameter a. 
    //Input parameter a acontrols the factor edgeWeight in assigning the scores to the nodes. a should be between 0 and 1.
    void align(double a);
    //print the alignment(mapping) in a file with input parameter name
	//Input parameter name determines the file that mapping is to be written in.    
	void outputAlignment(string name);
};
