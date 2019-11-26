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
int randGen(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_h, int &max_w);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout<<"---->CRITICAL ERROR: need one ARG imput file"<<endl;
        return -2;
    }

    vector<Block> blkLib;
    vector<Net> netLib;
    // vector<unsigned int> seqP;
    // vector<unsigned int> seqN;
    vector<unsigned int> seqP{4,2,1,8,5,9,0,6,3,7};
    vector<unsigned int> seqN{9,7,6,3,1,4,2,5,8,0};
    int max_h;
    int max_w;
    int blk_n = 0;
    int fileStat = readFile(argv[1], blkLib, netLib, blk_n);
    if (fileStat != 0)
        return fileStat;
    
    randGen(blkLib, blk_n, seqP, seqN, max_h, max_w);

    // ========call out all blocks========
    cout<<"------Blocks------"<<endl;
    for (vector<Block>::iterator itr=blkLib.begin(); itr!=blkLib.end(); itr++)
    {
        cout<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()<<" ";
        cout<<"x@"<<itr->getLLx()<<" y@"<<itr->getLLy()<<endl;
    }
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

int randGen(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_h, int &max_w)
{
    for (int i=0; i<blk_amt; i++)
    {
        seqP.push_back(i);
        seqN.push_back(i);
    }

    // ========random sequencing========
    // unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    // shuffle(seqP.begin(), seqP.end(), default_random_engine(seed));
    // seed = chrono::system_clock::now().time_since_epoch().count();
    // shuffle(seqN.begin(), seqN.end(), default_random_engine(seed));

    // ========call out sequence values========
    cout<<"P seq: ";
    for (int i=0; i<blk_amt; i++)
    {
        cout<<seqP[i]<<" ";
    }
    cout<<"\nN seq: ";
    for (int i=0; i<blk_amt; i++)
    {
        cout<<seqN[i]<<" ";
    }
    cout<<endl;

    // ========storing relative position========
    bool beforeP = true;
    bool beforeN = true;
    for (int blk_idx=0; blk_idx<blk_amt; blk_idx++)
    {
        beforeP = true;
        // cout<<"["<<blk_idx<<"]:----------------"<<endl;
        for (int idxp=0; idxp<blk_amt; idxp++)
        {
            if (seqP[idxp]==blk_idx)
            {
                // cout<<"<<"<<seqP[idxp]<<"="<<blk_idx<<endl;
                beforeP = false;
                continue;
            }
            beforeN = true;
            for (int idxn=0; idxn<blk_amt; idxn++)
            {
                if (seqN[idxn]==blk_idx)
                {
                    beforeN = false;
                    continue;
                }
                if (seqP[idxp]==seqN[idxn])
                {
                    // cout<<"BLK["<<blk_idx<<"]";
                    blkLib[blk_idx].direction(beforeP, beforeN, &blkLib[seqP[idxp]]);
                    // cout<<" <"<<!beforeP<<","<<!beforeN<<"> ("<<idxp<<","<<idxn<<")"<<endl;
                    // cout<<seqP[idxp]<<"  ";

                    // for (int k=0; k<blk_amt; k++)
                    // {
                    //     cout<<"\n["<<k<<"]"<<"@"<<blkLib[k].getLLx()<<"~"<<blkLib[k].getLLy();
                    // }
                    // cout<<endl;
                    break;
                }
            }
        }
    }

    // for (int k=0; k<blk_amt; k++)
    // {
    //     cout<<"BLK["<<k<<"]:"<<endl;
    //     cout<<"up: ";
    //     for (int i=0; i<blkLib[k].up.size(); i++)
    //     {
    //         cout<<blkLib[k].up[i]->getNum()<<" ";
    //     }
    //     cout<<"\ndown: ";
    //     for (int i=0; i<blkLib[k].down.size(); i++)
    //     {
    //         cout<<blkLib[k].down[i]->getNum()<<" ";
    //     }
    //     cout<<"\nleft: ";
    //     for (int i=0; i<blkLib[k].left.size(); i++)
    //     {
    //         cout<<blkLib[k].left[i]->getNum()<<" ";
    //     }
    //     cout<<"\nright: ";
    //     for (int i=0; i<blkLib[k].right.size(); i++)
    //     {
    //         cout<<blkLib[k].right[i]->getNum()<<" ";
    //     }
    //     cout<<"\n"<<endl;
    // }
    // ========finding critical path========
    Block dummyBottom;
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
    cout<<"____PUSH UP____"<<endl;
    dummyBottom.pushUp();
    // cout<<"____PUSH DN____"<<endl;
    // dummyTop.pushDn();
    cout<<"____PUSH RT____"<<endl;
    dummyLeft.pushRt();
    // cout<<"____PUSH LF____"<<endl;
    // dummyRight.pushLf();
}