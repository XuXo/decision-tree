#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>

#include <vector>
#include <string>
#include <math.h>
#include <time.h>


//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/ml/ml.hpp>

using namespace std;

static void readDB(string filename){
    
    ifstream file;
    string row;
    
    //char input[500];
    int prevIndex = 0;
    int inc = 0;
    
    vector<vector<string>> data;
    vector<string> rowEntry;
    
    file.open("agaricus-lepiota.data");
    if (!file){
        cerr << "data does not exist" << endl;
        exit(-1);
    }
    
    /* I had some issues with this approach and just don't feel like bothering with it
     bool firstIter = true;
     while (getline(file, row))
     {
     //this is somewhat laborious but it works albeit very fragile
     char* entry = new char[row.length() + 1];
     strcpy(entry, row.c_str());
     //cout<<"entry is"<<entry<<endl;
     
     if (firstIter == true)
     {
     
     string substring1(&entry[0], 1);
     cout<<substring1<<endl<<endl;
     }
     for(char* input = entry; *input != '\0'; ++input)        //test on "abcd,cdf"
     {
     if (*input != ',')
     inc++;
     else {
     string substring(&input[prevIndex], inc);
     //if (firstIter == true)
     //cout<<substring<<endl;
     rowEntry.push_back(substring);
     prevIndex = prevIndex + inc;
     inc = 0;
     }
     
     }
     data.push_back(rowEntry);
     firstIter = false;
     }
     */
    data.clear();
    while(getline(file,row))
    {
        while (row.length() != 0 && row.find(',') != string::npos)
        {
            string substring;
            int comma = row.find_first_of(',');
            substring = row.substr(0, comma);
            rowEntry.push_back(substring);
            row.erase(0, comma+1);
        }
        data.push_back(rowEntry);
        rowEntry.clear();
    }
    cout<<endl<<endl;
    
    cout<<data.size();
    for(int i = 0; i < data.size()-2; i++){
        for(int j = 0; j < data[0].size(); j++)
            cout<<data[i][j];
        cout<<endl;
    }
    
    
    //calculate Entropy(S) by iterating throuhg 1st column
    bool first = true;
    int positive = 0;
    int negative = 0;
    
    string poS = data[0][0];
    for(int i = 0; i < data.size()-2; i++) {
        if( data[i][0] == poS)
            positive++;
        else negative++;
    }
    cout<<"postitive number"<<positive<<endl;
    cout<<"negative number"<<negative<<endl;
    float fracPos = (float) positive/(data.size()-2);
    float fracNeg = (float) negative/(data.size()-2);
    cout<<"fracPos is"<<fracPos<<endl;
    cout<<"fracNeg is"<<fracNeg<<endl;
    float entropyS = -fracPos*(log(fracPos)/log(2.0)) - fracNeg*(log(fracNeg)/log(2.0));
    cout<<"entropy of the system is "<<entropyS<<endl<<endl<<endl;
    
    int numPos, numNeg;
    
    //also need the stats fo entropy (Sv)
    int Spos_pos, Spos_neg, Sneg_neg, Sneg_pos;        //where first index denote outcome and 2nd index for this attribute, ie Spos_pos is where
    
    vector<float>entropies;
    float maxEntropy = 0;
    for(int i = 1; i< data[0].size(); i++) {        //starting at attribute 1
        numPos = 0; numNeg = 0;
        Spos_pos = 0; Spos_neg = 0; Sneg_neg = 0; Sneg_pos = 0;
        string pos = data[0][i];
        for(int j = 0; j< data.size()-2; j++) {
            if (data[j][i] == pos){
                numPos++;
                if (data[j][0] == poS)
                    Spos_pos++;
                else Sneg_pos++;
            }
            else {
                numNeg++;
                if(data[j][0] == poS)
                    Spos_neg++;
                else Sneg_neg++;
            }
        }
        float fracPos = (float) numPos/(data.size()-2);
        float fracNeg = (float) numNeg/(data.size()-2);
        
        cout<<"fracPos is "<<fracPos<<endl;
        cout<<"fracNeg is "<<fracNeg<<endl;
        
        cout<<"numPos is"<<numPos<<endl;
        cout<<"numNeg is"<<numNeg<<endl;
        
        cout<<"spos_pos is"<<Spos_pos<<endl;
        cout<<"spos_neg is"<<Spos_neg<<endl;
        cout<<"sneg_pos is"<<Sneg_pos<<endl;
        cout<<"sneg_neg is"<<Sneg_neg<<endl;
        
        float entropySvPos;
        if ((Spos_pos == 0) || (Sneg_pos == 0)) {
            if ((Spos_pos == 0) && (Sneg_pos == 0))        //had to make special case since it's axiomatically undefined, what is really means is that this set has cardinality 0
                entropySvPos = 0;
            else if (Spos_pos == 0)
                entropySvPos = -1 * (float)Sneg_pos/numPos * log((float)Sneg_pos/numPos)/log(2.0);
            else
                entropySvPos = -1 * (float)Spos_pos/numPos * log((float) Spos_pos/numPos)/log(20);
        }
        else entropySvPos = -((float)Spos_pos/numPos * log((float) Spos_pos/numPos)/log(2.0) - (float)Sneg_pos/numPos * log((float)Sneg_pos/numPos))/log(2.0);
        
        float entropySvNeg;
        if ((Spos_neg == 0) || (Sneg_neg == 0)) {
            if ((Spos_neg == 0) || (Sneg_neg == 0))         //had to make special case since it's axiomatically undefined, what is really means is that this set has cardinality 0
                entropySvNeg = 0;
            else if (Spos_neg == 0)
                entropySvNeg = -1 * (float)Sneg_neg/numNeg * log((float)Sneg_neg/numNeg)/log(2.0);
            else
                entropySvNeg = -1 * (float)Spos_neg/numNeg * log((float) Spos_neg/numNeg)/log(2.0);
        }
        else entropySvNeg = -((float)Spos_neg/numNeg * log((float) Spos_neg/numNeg)/log(2.0) - (float)Sneg_neg/numNeg * log((float)Sneg_neg/numNeg))/log(2.0);
        
        cout<<"entropySvPos is "<<entropySvPos<<endl;
        cout<<"entropySvNeg is "<<entropySvNeg<<endl;
        
        float entropySv = entropyS - fracPos*entropySvPos - fracNeg*entropySvNeg;
        entropies.push_back(entropySv);
        cout<<"entropy for column "<< i <<" is"<<entropySv<<endl<<endl;
        if (entropySv > maxEntropy)
            maxEntropy = entropySv;
    }
    cout<<"max entropy is: "<<maxEntropy;
    
    
    /*
     vector<double> gains;
     so for ex looking at the first column assuming each attribute can only take 2 values
     for(std::vector::iterator it = data.begin(); it != data.end(); ++it){
     vector<string> row = *it;
     firstItr = true;                    //reset to true for every column
     for(std::vector::iterator itt = row.begin(); itt != row.end(); ++itt){
     if (firstItr){
     string attrPos = *itt;      //the first one is designated as the positive
     positive ++;
     firstItr = false;
     }
     
     else {
     if( *itt == attrPos)
     positive++;
     else negative++;
     }
     }
     float fracPos = (float) positive/data.length();
     float fracNeg = (float) negative/data.length();
     }
     */
    
}

int main(int argc, const char *argv[])
{
    cout<<log(16)/log(2)<<endl;
    cout<<exp(3)<<endl;
    string filename = "agaricus-lepiota.data";
    readDB(filename);
    
    //file.close();
    cin.get();
    return 0;
}





//implemented according to Machine Learning by Tom Mitchell

/*
 we define Entropy(S) = -P_+ * log(P_+) - P_- * log(P_-)
 where P_+ is proportion of positive examples in S and P_- is proportion of negative examples in S and log is base 2
 
 so we need to find Gain(S,A) := Entropy(S) - Summation over v in Values(A) of |Sv|/|S| * Entropy(Sv)
 where values (A) is the set of all possible values for attribute A, and Sv is the subset of S for which attribute A has values v (ie., Sv = ) {s in S | A(s) = v}.  So the summation is
 simply the sum of entropies of each subset Sv, weighted by the fraction of examples that belong to Sv. Gain (S, A) intuitively represents the expected reduction in entropy caused by knowing
 the values of attribute A
 
 so let's find Gain(S,A) for each A
 
 
 
 //assuming we know what the two outcomes are by iterating on the first column
 //call it pass or fail
 
 */