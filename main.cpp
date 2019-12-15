#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
// #include <stdio.h>
#include <vector>
// #include <array>
#include <algorithm> //for shuffle
#include <chrono> // sys time

#include "Class_Block.h"

using namespace std;

int readFile(char* fName, vector<Block> &blkLib, vector<Net> &netLib, int &blk_amt);
int randGen(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_w, int &max_h);
void DFSvisit(vector<Block> &blkLib, Block &targetBlk, int &timeStamp, vector<Block*> &Q, bool directUp); // directUp= 1:up  0:right

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout<<"---->CRITICAL ERROR: need one ARG imput file"<<endl;
        return -2;
    }

    vector<Block> blkLib;
    vector<Net> netLib;
    // NOTE: please use the non-initialized vectors instead to implmt randomness
    vector<unsigned int> seqP;
    vector<unsigned int> seqN;
    // vector<unsigned int> seqP{4,2,1,8,5,9,0,6,3,7};
    // vector<unsigned int> seqN{9,7,6,3,1,4,2,5,8,0};
    int max_h=0;
    int max_w=0;
    int blk_n = 0;
    int fileStat = readFile(argv[1], blkLib, netLib, blk_n);
    if (fileStat != 0)
        return fileStat;
    
    randGen(blkLib, blk_n, seqP, seqN, max_w, max_h);

    // ========call out all blocks========
    string outputName = (string)argv[1]+"_Li_Chengzhe.out1";
    cout<<">>>Opening output file: "<<outputName<<endl;
    ofstream outFile(outputName);
    // --------call out sequence values--------
    cout<<"P seq: ";
    outFile<<"P seq: ";
    for (int i=0; i<blk_n; i++)
    {
        cout<<seqP[i]<<" ";
        outFile<<seqP[i]<<" ";
    }
    cout<<"\nN seq: ";
    outFile<<"\nN seq: ";
    for (int i=0; i<blk_n; i++)
    {
        cout<<seqN[i]<<" ";
        outFile<<seqN[i]<<" ";
    }
    cout<<endl;
    outFile<<endl;
    // --------call out dimensions and area--------
    cout<<"Chip Dimension(WxH): "<<max_w<<" "<<max_h<<endl;
    outFile<<"Chip Dimension(WxH): "<<max_w<<" "<<max_h<<endl;
    long int A = max_w*max_h;
    cout<<"Chip Area: "<<A<<endl;
    outFile<<"Chip Area: "<<A<<endl;
    // --------call out blocks with coordinations--------
    cout<<"------Blocks------"<<endl;
    outFile<<"------Blocks------"<<endl;
    for (vector<Block>::iterator itr=blkLib.begin(); itr!=blkLib.end(); itr++)
    {
        cout<<"NO."<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()\
        <<" x@"<<itr->getLLx()<<" y@"<<itr->getLLy()<<endl;
        outFile<<"NO."<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()\
        <<" x@"<<itr->getLLx()<<" y@"<<itr->getLLy()<<endl;
    }
    // --------call out nets--------
    // cout<<"\n------Nets------"<<endl;
    // outFile<<"\n------Nets------"<<endl;
    // for (vector<Net>::iterator itr=netLib.begin(); itr!=netLib.end(); itr++)
    // {
    //     cout<<"NO."<<itr->getNum()<<" "<<itr->getDeg();
    //     outFile<<"NO."<<itr->getNum()<<" "<<itr->getDeg();
    //     for (unsigned int k=0; k<itr->getDeg(); k++)
    //     {
    //         cout<<" "<<itr->connBlks[k];
    //         outFile<<" "<<itr->connBlks[k];
    //     }
    //     cout<<endl;
    //     outFile<<endl;
    // }
    // outFile.close();

    return 0;
}

int readFile(char* fName, vector<Block> &blkLib, vector<Net> &netLib, int &blk_amt)
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
        blk_amt = firstNum;

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

int randGen(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_w, int &max_h)
{
    for (int i=0; i<blk_amt; i++)
    {
        seqP.push_back(i);
        seqN.push_back(i);
    }

    // ========random sequencing========
    // NOTE: please uncomment the following code block to really implementing randomness
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(seqP.begin(), seqP.end(), default_random_engine(seed));
    seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(seqN.begin(), seqN.end(), default_random_engine(seed));



    // ========storing relative position========
    cout<<">>>storing relative position"<<endl;
    for (int idx=0; idx<blk_amt; idx++) // O(n)
    {
        blkLib[seqP[idx]].pNum = idx;
        blkLib[seqN[idx]].nNum = idx;
    }
    for (int i=0; i<blk_amt; i++) // O(n^2)
    {
        bool beforeP = true;
        bool beforeN = true;
        // cout<<"For BLK["<<i<<"]("<<blkLib[i].pNum<<","<<blkLib[i].nNum<<"):"<<endl;
        for (int j=0; j<blk_amt; j++)
        {
            if (i!=j)
            {
                beforeP = (blkLib[j].pNum<blkLib[i].pNum);
                beforeN = (blkLib[j].nNum<blkLib[i].nNum);
                // cout<<"        ["<<j<<"]("<<blkLib[j].pNum<<","<<blkLib[j].nNum<<"): "<<beforeP<<"-"<<beforeN<<"  ";
                blkLib[i].direction(beforeP, beforeN, &blkLib[j]);
            }
        }
    }


    // ========finding critical path========
    cout<<">>>finding critical path"<<endl;
    vector<Block*> topoQUp;
    vector<Block*> topoQRt;
    int timeStamp;
    for  (vector<Block>::iterator itr=blkLib.begin(); itr!=blkLib.end(); itr++)
    {
        itr->DFSstateUp = -1;
        itr->DFSpredUp = NULL;
        itr->DFSstateRt = -1;
        itr->DFSpredRt = NULL;
    }
    // cout<<">>>creating dummies"<<endl;
    Block dummyBottom; // dummies have default w and h as 0
    Block dummyTop;
    Block dummyLeft;
    Block dummyRight;
    for (int i=0; i<blk_amt; i++)
    {
        if (blkLib[i].down.size()==0)
        {
            dummyBottom.up.push_back(&blkLib[i]);
        }
        if (blkLib[i].up.size()==0)
        {
            dummyTop.down.push_back(&blkLib[i]);
        }
        if (blkLib[i].left.size()==0)
        {
            dummyLeft.right.push_back(&blkLib[i]);
        }
        if (blkLib[i].right.size()==0)
        {
            dummyRight.left.push_back(&blkLib[i]);
        }
    }

    dummyBottom.DFSstateUp = -1;
    dummyBottom.DFSpredUp = NULL;
    dummyBottom.dUp = -1;
    dummyBottom.fUp = -1;
    dummyBottom.setWidth(0);
    dummyBottom.setHeight(0);
    dummyBottom.setLL(0, 0);
    dummyLeft.DFSstateRt = -1;
    dummyLeft.DFSpredRt = NULL;
    dummyLeft.dRt = -1;
    dummyLeft.fRt = -1;
    dummyLeft.setWidth(0);
    dummyLeft.setHeight(0);
    dummyLeft.setLL(0, 0);

    // cout<<">>>starting DFS"<<endl;
    timeStamp = 0;
    DFSvisit(blkLib, dummyBottom, timeStamp, topoQUp, true);

    timeStamp = 0;
    DFSvisit(blkLib, dummyLeft, timeStamp, topoQRt, false); 

    for (int i=1; i<topoQUp.size(); i++) // [0] will be dummy, assume least 1 cell
    {
        // topoQUp.size() equal to topoQRt.size() = #of cells = blk_amt
        for (int j=0; j<topoQRt[i]->left.size(); j++)
        {
            int newX = topoQRt[i]->left[j]->getLLx()\
                        +topoQRt[i]->left[j]->getWidth();
            if (newX>topoQRt[i]->getLLx())
            {
                topoQRt[i]->setLL(newX, topoQRt[i]->getLLy());
                // cout<<"  x refreshed to: "<<newX<<endl;
            }
            else
            {
                // cout<<"  x remained tobe: "<<topoQRt[i]->getLLx()<<endl;
            }
        }

        for (int j=0; j<topoQUp[i]->down.size(); j++)
        {
            int newY = topoQUp[i]->down[j]->getLLy()\
                        +topoQUp[i]->down[j]->getHeight();
            if (newY>topoQUp[i]->getLLy())
            {
                topoQUp[i]->setLL(topoQUp[i]->getLLx(), newY);
                // cout<<"  y refreshed to: "<<newY<<endl;
            }
            else
            {
                // cout<<"  y remained tobe: "<<topoQUp[i]->getLLy()<<endl;
            }
        }
    }
    cout<<">>>coord generated"<<endl;


    // ========extracting max dimensions========
    cout<<">>>extracting max dimensions"<<endl;
    for (int i=0; i<blk_amt; i++)
    {
        if (blkLib[i].getLLx()+blkLib[i].getWidth() > max_w)
        {
            max_w = blkLib[i].getLLx()+blkLib[i].getWidth();
            // cout<<"new W: "<<max_w<<" from "<<i<<endl;
        }
        if (blkLib[i].getLLy()+blkLib[i].getHeight() > max_h)
        {
            max_h = blkLib[i].getLLy()+blkLib[i].getHeight();
            // cout<<"new H: "<<max_h<<" from "<<i<<endl;
        }
    }
}

void DFSvisit(vector<Block> &blkLib, Block &targetBlk, int &timeStamp, vector<Block*> &Q, bool directUp) // directUp= 1:up  0:right
{
    timeStamp++;
    if (directUp)
    {
        targetBlk.dUp = timeStamp;
        targetBlk.DFSstateUp = 0;
        for (int i=0; i<targetBlk.up.size(); i++)
        {
            if (targetBlk.up[i]->DFSstateUp==-1)
            {
                targetBlk.up[i]->DFSpredUp=&targetBlk;
                DFSvisit(blkLib, *targetBlk.up[i], timeStamp, Q, directUp);
            }
        }
        targetBlk.DFSstateUp = 1;
        timeStamp++;
        targetBlk.fUp = timeStamp;
        Q.insert(Q.begin(), &targetBlk);
    }
    else
    {
        targetBlk.dRt = timeStamp;
        targetBlk.DFSstateRt = 0;
        for (int i=0; i<targetBlk.right.size(); i++)
        {
            if (targetBlk.right[i]->DFSstateRt==-1)
            {
                targetBlk.right[i]->DFSpredRt=&targetBlk;
                DFSvisit(blkLib, *targetBlk.right[i], timeStamp, Q, directUp);
            }
        }
        targetBlk.DFSstateRt = 1;
        timeStamp++;
        targetBlk.fRt = timeStamp;
        Q.insert(Q.begin(), &targetBlk);
    }
    
}