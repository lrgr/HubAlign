#include "Alignment.h"
#include <fstream>
#include <cstdlib>

using namespace std;
//constructor
//finds the smaller network and the maximum degree of the input networks
//Inputs are two files of networks net1 and net2
Alignment::Alignment( Network& net1, Network& net2)
{
    //compare networks to find the biggest one
    if( net1.size > net2.size )
	{
		reverse = true;
		network1 = net2;
		network2 = net1;
	}
	else
	{
        reverse = false;
		network1 = net1;
		network2 = net2;
	}

	//maximum degree of the network
    if(network1.maxDeg > network2.maxDeg)
		maxDeg = network1.maxDeg;
	else
		maxDeg = network2.maxDeg;
    
    ifstream inputFile;
	string token1,token2,line;
    float token3, max=0;
}

//produce a mapping between nodes of two network with respect to input parameter a. 
//Input parameter a acontrols the factor edgeWeight in assigning the scores to the nodes. a should be between 0 and 1.
void Alignment::align(double a, double b)
{
    bool flag;  //check wether or not all the nodes of the smaller network are aligned?
    
    //temporary
    float temp; 
    float a1;   
    float a2; 
    float MINSCORE = -100000;
    int coeff;
    if(network2.numOfEdge>network1.numOfEdge) {
        coeff = network2.numOfEdge/network1.numOfEdge;
    }
    else {
        coeff = network1.numOfEdge/network2.numOfEdge;
    }
 
    int maxNode; // node with max score
    double alpha = a;   //to control factor of edgeweight 
    bool *alignNodes1 = new bool[network1.size]; //aligned nodes of the smaller network
    bool *alignNodes2 = new bool[network2.size]; //aligned nodes of the bigger network
    alignment = new int[network1.size]; //alignment array
    float *nodeScore1 = new float[network1.size]; //scores of nodes of smaller network
    float *nodeScore2 = new float[network2.size]; //scores of nodes of bigger network
    double **alignScore = new double*[network1.size]; //this matrix contains the score of each matching pair
    int *best = new int[network1.size]; //array of best align scores
    
    //initial values
    for(int c1=0; c1<network1.size; c1++)
        alignScore[c1]=new double[network2.size];       
    for(int c1=0; c1<network1.size; c1++)
        alignNodes1[c1]=false;
    for(int c1=0; c1<network2.size; c1++)
        alignNodes2[c1]=false;    
    for(int c1=0; c1<network1.size; c1++)
        alignment[c1]=-1;
    
    
    //initialize nodeScore fro both networks
    for(int c1=0; c1< network1.size; c1++)
        nodeScore1[c1]=network1.nodeWeight[c1];
    for(int c1=0; c1< network2.size; c1++)
        nodeScore2[c1]=network2.nodeWeight[c1];
    
    //finding the nodescore
    for (int c1=0; c1<network1.size; c1++)
        for (int c2=0; c2<network1.size; c2++)
            nodeScore1[c1]+= alpha*network1.edgeWeight[c1][c2]; 
    for (int c1=0; c1<network2.size; c1++)
        for (int c2=0; c2<network2.size; c2++)
            nodeScore2[c1] += alpha*network2.edgeWeight[c1][c2]; 
    
    //find max score 
    //======first network
    float max = -10000;
    for (int c1=0; c1<network1.size; c1++) {
        if (max < nodeScore1[c1]) {
            max = nodeScore1[c1];
        }
    }
    //====== second network
    for (int c1=0; c1<network2.size; c1++) {
        if (max < nodeScore2[c1]) {
            max = nodeScore2[c1];
        }
    }

    //normalize with respect to max
    for (int c1=0; c1<network1.size; c1++) {
        nodeScore1[c1] = nodeScore1[c1]/max; 
    }    

    for (int c1=0; c1<network2.size; c1++) {
        nodeScore2[c1] = nodeScore2[c1]/max;  
    }    
    //END of normalization
    
    //finding the alignscore
    for(int c1=0; c1<network1.size; c1++)
        for(int c2=0; c2<network2.size; c2++){
            alignScore[c1][c2] = (nodeScore1[c1]>nodeScore2[c2])? nodeScore2[c2]:nodeScore1[c1];//topology
            alignScore[c1][c2] = (1-b) * (alignScore[c1][c2]);
            alignScore[c1][c2] += b*blast[c1][c2]; //adding similarity
        } 
    
    flag=true;
    while(flag){ //there is some unaligned nodes in determined iteration
        //find the maximum value of each row of alignscore and save it in array "best"
        for(int c1=0; c1<network1.size; c1++)
            if(!alignNodes1[c1]){
                temp=MINSCORE;
                for(int c2=0; c2<network2.size; c2++)
                    if(temp<alignScore[c1][c2] && !alignNodes2[c2]){
                        best[c1]=c2;
                        temp = alignScore[c1][c2];
                    }
            }
        //doing the alignment
        //find the maximum value of array "best" that means the best score in matrix "alignScore"
        temp=MINSCORE;
        flag=false;
        for(int c1=0; c1<network1.size; c1++)
            if(temp<alignScore[c1][best[c1]] && !alignNodes1[c1] && !alignNodes2[best[c1]]){
                flag=true; //still there is node that is not aligned
                temp = alignScore[c1][best[c1]];
                maxNode = c1;
            }
        if(flag){ //there is some node in first network that are not still aligned 
            
            alignment[maxNode]=best[maxNode]; //align two nodes;
            alignNodes1[maxNode]=true;
            alignNodes2[best[maxNode]]=true;
            //update the align scores
            for(int c1=0; c1 <network1.deg[maxNode]; c1++)
                for(int c2=0; c2<network2.deg[best[maxNode]]; c2++)
                    alignScore[ network1.neighbor[maxNode][c1]][network2.neighbor[best[maxNode]][c2]]=alignScore[ network1.neighbor[maxNode][c1]][network2.neighbor[best[maxNode]][c2]]+(coeff/max); //update the weights
        }
    }//end flag
    
    //memory leak
    delete [] alignNodes1;
    delete [] alignNodes2;
    delete [] nodeScore1;
    delete [] nodeScore2;
    delete [] best;

    for(int j=0; j<network1.size; j++)
    {
        delete [] alignScore[j];
    } 
    delete [] alignScore;
}
     
//print the alignment(mapping) in a file with input parameter name
//Input parameter name determines the file that mapping is to be written in.    
void Alignment::outputAlignment(string name)
{
	string alignFile = name;
    
	alignFile.append(".alignment");
    
    ofstream alignmentFile( alignFile.c_str());
	
    if(reverse){
		for(int i=0; i<network1.size; i++)
			alignmentFile << network2.getName( alignment[ i ] ) << ' ' << network1.getName( i )<< endl;}
	else
		for(int i=0; i<network1.size; i++)
			alignmentFile << network1.getName( i ) << ' ' << network2.getName( alignment[ i ] )<< endl;
}
//constructor
Alignment::Alignment(void)
{
}
//destructor
Alignment::~Alignment(void)
{
}
