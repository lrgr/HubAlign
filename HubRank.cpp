// NetAlign.cpp : Defines the entry point for the console application.

#include "Alignment.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

exception er;

int main(int argc, char* argv[])
{
    double aa = 0.1; //a is alpha that controlls the factor of edgeweights
    int tt = 6; //controlls the step of making the skeleton

    char* name1; //name of the first network
    char* name2; //name of second network
    char* blastFile;
    try 
    {
        if(argc < 3) {
            cout << "There should be two files as input!" <<endl;
            return -1;
        }
        else //input arguments
        {
            int i = 1; //counter for input parameters
            name1 = argv[ i++ ]; //first network
            name2 = argv[ i++ ]; //second network
            
			while (i<argc) //check all the input parameters
            {
                if ( ( strlen(argv[i]) == 2 ) && ( argv[i][0]=='-' ) && ( i + 1 < argc) ) //wether or not the parameter has started with '-' and has a value
                {
                    i++; //to read the value of input parameter
                    if( argv[i-1][1]=='a' )
                    {
                        aa = atof(argv[i]);
                        if( aa <0 || aa > 1) //the value of a should be between zero and one
                        {
                            cout << "Error : value of a must be between 0 and 1" << endl;
                            return -1;
                        }
                    }
                    else if( argv[i-1][1]=='t')
                    {
                        tt = atoi(argv[i]);
                        if( tt > 100)
                        {
                            cout << "Error : value of t must be between 0 and 100" << endl;
                            return -1;
                        }
                    }
                    i++;// to reach the next input parameter if there is
                }
                else
                {
                    cout << "Error in argument : " << argv[i] << endl; //avaz karde am
                    return -1;
                }
            }
		} //end else

        //construct the networks
        Network network1(name1); 
        Network network2(name2);
        bool reverse = false; //it means size of first input network is bigger than second one
        
        if(network1.size > network2.size)
            reverse = true;
        //making the skeletons of the networks 
        network1.makeSkeleton(tt);
        network2.makeSkeleton(tt);
        

        //align two networks with each other
        Alignment alignment( network1, network2);
        alignment.align(aa);

        //calculation of evaluating measures
        int CCCV = 0, CCCE = 0; //LCCS based on vertex and edge
        float EC = 0, NC = 0, IC = 0;
		CCCV += alignment.CCCV;
		CCCE += alignment.CCCE;
		EC += alignment.EC;
		NC += alignment.NC;
		IC += alignment.IC;        
        
		//making the name for output file
        stringstream strm;
        strm << "(" << name1 << "-" << name2;
		strm << ")" << "-a" << aa;
        strm << ")" << "-t" << tt;
        
        //no random alignment
     //   if(nn==1) 
       // {
         //   alignment.outputEvaluation(strm.str());
           // alignment.outputAlignment(strm.str());
      //  }
    }
	catch(exception &e)
	{
		cout << "Error in arguments or input files!" << endl;
		e.what();
		return -1;
	}
    return 0;
}

