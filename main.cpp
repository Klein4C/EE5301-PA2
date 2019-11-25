#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
// #include <stdio.h>
#include <vector>

#include "Class_Block.h"

using namespace std;

int readFile(char* fName, vector<Block> &blkLib, vector<Net> &netLib);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout<<"---->CRITICAL ERROR: need one ARG imput file"<<endl;
        return -2;
    }

    vector<Block> blkLib;
    vector<Net> netLib;
    int fileStat = readFile(argv[1], blkLib, netLib);
    if (fileStat != 0)
        return fileStat;
    
    // cout<<"------Blocks------"<<endl;
    // for (vector<Block>::iterator itr=blkLib.begin(); itr!=blkLib.end(); itr++)
    // {
    //     cout<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()<<endl;
    // }
    // cout<<"\n------Nets------"<<endl;
    // for (vector<Net>::iterator itr=netLib.begin(); itr!=netLib.end(); itr++)
    // {
    //     cout<<itr->getNum()<<" "<<itr->getDeg();
    //     for (unsigned int k=0; k<itr->getDeg(); k++)
    //     {
    //         cout<<" "<<itr->connBlks[k];
    //     }
    //     cout<<endl;
    // }

    return 0;
}

int readFile(char* fName, vector<Block> &blkLib, vector<Net> &netLib)
{
    char lineBuf[1024];
    ifstream ifs(fName);
    if (ifs.is_open() == 0)
    {
		cout << "----->Error opening file: " << fName << endl;
		return -3;
	}

    unsigned int blkAmt; // total block amount
    unsigned int netAmt; // total net amount

    while(ifs.good()) // read all blocks-------------------
    {
        ifs.getline(lineBuf, 1023); // read first line:blk amount
        string lineStr(lineBuf); // convert to C++ string
		if (lineStr.empty())	// is empty line?
			continue;
        istringstream iss(lineStr);
        unsigned int firstNum;  // blk amount number
        iss >> firstNum;
        blkAmt = firstNum; // get blk amount number

        for (unsigned int i=0; i<blkAmt; i++)
        {
            ifs.getline(lineBuf, 1023);
            string lineStr(lineBuf);
            istringstream iss(lineStr);
            Block newBlk;
            unsigned int numFrag; // blk number or height or width <u_int>
            iss >> numFrag;
            if (i != numFrag)
            {
                cout<<"----->FORMAT ERROR: blk-idx in file not in consistent order:i-"<<i<<"; numFrag-"<<numFrag<<endl;
                return -4;
            }
            newBlk.setNum(numFrag);
            iss >> numFrag;
            newBlk.setWidth(numFrag);
            iss >> numFrag;
            newBlk.setHeight(numFrag);

            blkLib.push_back(newBlk);
        }
        break;
    }

    bool netsFound = false;
    while(ifs.good()) // detect "Nets"-----------------------------
    {
        ifs.getline(lineBuf, 1023);
        string lineStr(lineBuf);
		if (lineStr.empty())
			continue;
        istringstream iss(lineStr);
        string firstWord;  // read to check "Nets"
        iss >> firstWord;
        if (firstWord.find("Nets") != string::npos)
        {
            netsFound = true;
            break;
        }
    }
    if (netsFound)
    {
        while (ifs.good())
        {
            ifs.getline(lineBuf, 1023); // read first line:net amount
            string lineStr(lineBuf); // convert to C++ string
            if (lineStr.empty())	// is empty line?
                continue;
            istringstream iss(lineStr);
            unsigned int firstNum;  // net amount number
            iss >> firstNum;
            netAmt = firstNum;

            for (unsigned int i=0; i<netAmt; i++)
            {
                ifs.getline(lineBuf, 1023);
                string lineStr(lineBuf);
                istringstream iss(lineStr);
                Net newNet;
                newNet.setNum(i);
                unsigned int numFrag; // net degree or conn_blkNum <u_int>
                iss >> numFrag; // get net degree
                newNet.setDeg(numFrag);
                for (unsigned int j=0; j<newNet.getDeg(); j++)
                {
                    iss >> numFrag;
                    newNet.connBlks.push_back(numFrag);
                }
                
                netLib.push_back(newNet);
            }
        }
    }
    else
    {
        cout<<"---->FATAL ERROR: Nets NOT Found"<<endl;
        return -5;
    }

    return 0;
}

