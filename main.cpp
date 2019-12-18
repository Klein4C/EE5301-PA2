#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
// #include <stdio.h>
#include <vector>
// #include <array>
#include <algorithm> //for shuffle
#include <random>
#include <chrono> // sys time
#include <math.h>
#include <typeinfo>

#include "Class_Block.h"

using namespace std;
#define def_K 0.95
#define def_T0 40000.0
#define def_Tfreez 0.1
#define def_movPerTemp 10
#define def_ALPHA 0.5

int readFile(char* fName, vector<Block> &blkLib, vector<Net> &netLib, int &blk_amt);
int plot(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_w, int &max_h);
void DFSvisit(vector<Block> &blkLib, Block &targetBlk, int &timeStamp, vector<Block*> &Q, bool directUp); // directUp= 1:up  0:right
double getCost(int Area, int WL, double alpha);
bool acceptMove(int dCost, double K, double T);
double coolDown(double prevT);

int main(int argc, char* argv[])
{
    double K = def_K;
    double T0 = def_T0;
    double Tfreez = def_Tfreez;
    double movPerTemp = def_movPerTemp;
    double ALPHA = def_ALPHA;
    double alpha;
    long int acceptNum=0;
    long int rejectNum=0;
    bool enableAccept = false;

    if (argc<3 || argc>7)  
    {
        int argNum = argc;
        cout<<"---->CRITICAL ERROR: 2 or 5 ARGs but get "<<argNum<<endl;
        return -1;
    }
    if (argc == 4)
    {
        ALPHA = stod(argv[3]);
    }
    else if (argc == 5)
    {
        K = stod(argv[3]);
        ALPHA = stod(argv[4]);
    }
    else if (argc == 6)
    {
        K = stod(argv[3]);
        movPerTemp = stoi(argv[4]);
        ALPHA = stod(argv[5]);
    }
    else if (argc == 7)
    {
        K = stod(argv[3]);
        movPerTemp = stoi(argv[4]);
        ALPHA = stod(argv[5]);
        enableAccept = (bool)argv[6];
    }

    if (((string)argv[2]!="-a") && ((string)argv[2]!="-w") && ((string)argv[2]!="-c") && ((string)argv[2]!="-h"))
    {
        cout<<"---->CRITICAL ERROR: unknown mode-option"<<endl;
        return -2;
    }
    switch (argv[2][1])
    {
    case 'a':
        // cout<<"========MODE: A========"<<endl;
        alpha = 1.0;
        break;
    case 'w':
        // cout<<"========MODE: W========"<<endl;
        alpha = 0.0;
        break;
    case 'c':
        // cout<<"========MODE: C========"<<endl;
        alpha = ALPHA;
        break;
    default:
        cout<<">>>FORMAT:\n"
            <<" 1) $> [program] [inFile] [mode]\n"\
            <<" 2) $> [program] [inFile] [mode] [alpha]\n"\
            <<" 3) $> [program] [inFile] [mode] [K] [alpha]\n"\
            <<" 4) $> [program] [inFile] [mode] [K] [MpT] [alpha]\n"\
            <<" 4) $> [program] [inFile] [mode] [K] [MpT] [alpha] [enable]\n"\
            <<endl;
        return 0;
        break;
    }

    vector<Block> blkLib;
    vector<Net> netLib;
    int max_h=0;
    int max_w=0;
    long int Area=0;
    long int WL=0;
    int prevMax_h=0;
    int prevMax_w=0;
    long int prevArea=0;
    long int prevWL=0;
    double cost;
    double prevCost;
    double T = T0;
    int blk_n = 0;
    int fileStat = readFile(argv[1], blkLib, netLib, blk_n);
    if (fileStat != 0)
        return fileStat;
    
    // ================================timer start===============================
    auto startTime = chrono::high_resolution_clock::now();
    // ==========================================================================
    
    // ----------------================randomSequencing================----------
    // --------NOTE: please use the non-initialized vectors instead to implmt randomness
    vector<unsigned int> seqP;
    vector<unsigned int> seqN;
    // vector<unsigned int> seqP{4,2,1,8,5,9,0,6,3,7};
    // vector<unsigned int> seqN{9,7,6,3,1,4,2,5,8,0};
    for (int i=0; i<blk_n; i++)
    {
        seqP.push_back(i);
        seqN.push_back(i);
    }
    // --------NOTE: please uncomment the following code block to really implementing randomness
    unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(seqP.begin(), seqP.end(), default_random_engine(seed));
    seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(seqN.begin(), seqN.end(), default_random_engine(seed));
    // ----------------================calc init rand blk pos================---
    plot(blkLib, blk_n, seqP, seqN, max_w, max_h);

    // ----------------================outputing================----------------
    // ========call out all blocks========
    string inputName = (string)argv[1];
    string outputName = inputName.substr(0, inputName.size()-3) +"_Li_Chengzhe.out" + argv[2][1];
    // cout<<">>>Opening output file: "<<outputName<<endl;
    ofstream outFile(outputName);
    ofstream intermedData(inputName.substr(0, inputName.size()-3)+"-"+argv[2][1]+".csv");
    // --------call out sequence values--------
    // cout<<"Init P seq: ";
    // outFile<<"Init P seq: ";
    // for (int i=0; i<blk_n; i++)
    // {
    //     cout<<seqP[i]<<" ";
    //     outFile<<seqP[i]<<" ";
    // }
    // cout<<"\nInit N seq: ";
    // outFile<<"\nInit N seq: ";
    // for (int i=0; i<blk_n; i++)
    // {
    //     cout<<seqN[i]<<" ";
    //     outFile<<seqN[i]<<" ";
    // }
    // cout<<endl;
    // outFile<<endl;
    // --------call out dimensions and area--------
    // cout<<"Init Chip Dimension(WxH): "<<max_w<<" "<<max_h<<endl;
    // outFile<<"Init Chip Dimension(WxH): "<<max_w<<" "<<max_h<<endl;
    Area = max_w*max_h;
    // cout<<"Init Chip Area: "<<Area<<endl;
    // outFile<<"Init Chip Area: "<<Area<<endl;
    // --------call out blocks with coordinations--------
    // cout<<"------Blocks------"<<endl;
    // outFile<<"------Blocks------"<<endl;
    // for (vector<Block>::iterator itr=blkLib.begin(); itr!=blkLib.end(); itr++)
    // {
    //     cout<<"NO."<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()\
    //         <<" x@"<<itr->getLLx()<<" y@"<<itr->getLLy()<<endl;
    //     outFile<<"NO."<<itr->getNum()<<" "<<itr->getWidth()<<" "<<itr->getHeight()\
    //         <<" x@"<<itr->getLLx()<<" y@"<<itr->getLLy()<<endl;
    // }
    // --------call out nets--------
    WL=0;
    // for (int i=0; i<netLib.size(); i++)
    // {
    //     netLib[i].resetHPWL();
    //     netLib[i].refreshHPWL(blkLib);
    //     WL += netLib[i].getHPWL();
    // }

    // cout<<"\n------Nets------"<<endl;
    // outFile<<"\n------Nets------"<<endl;
    for (vector<Net>::iterator itr=netLib.begin(); itr!=netLib.end(); itr++)
    {
        // cout<<"NO."<<itr->getNum()<<" "<<itr->getDeg();
        // outFile<<"NO."<<itr->getNum()<<" "<<itr->getDeg();
        // for (unsigned int k=0; k<itr->getDeg(); k++)
        // {
        //     // cout<<" "<<itr->connBlkIdxs[k];
        //     outFile<<" "<<itr->connBlkIdxs[k];
        // }
        itr->resetHPWL();
        itr->refreshHPWL(blkLib);
        WL += itr->getHPWL();
        // cout<<" WL:"<<itr->getHPWL();
        // outFile<<" WL:"<<itr->getHPWL();
        // cout<<endl;
        // outFile<<endl;
    }
    // cout<<"--------Init Properties--------\n";
    // cout<<"Init WL:"<<WL<<endl;
    // outFile<<"Init WL:"<<WL<<endl;

    cost = getCost(Area, WL, alpha);
    // cout<<"Init Cost:"<<cost<<endl;
    // outFile<<"Init Cost:"<<cost<<endl;
    

    //----------------================annealing================----------------
    // cin.get();
    // --------store previous properties before annealing--------
    prevMax_h = max_h;
    prevMax_w = max_w;
    prevArea = Area;
    prevWL = WL;
    prevCost = cost;

    long int tCount = 0;
    if (enableAccept)
    {
        intermedData<<"tCount,Temp,"\
            <<"#Op,RjctAcpt,swap1,swap2,"\
            <<"Area,WL,\n";
    }
    while(T>Tfreez)
    {
        tCount++;
        for (int i=0; i<movPerTemp; i++)
        {
            // --------randomly choose 2 blocks and swap--------
            int pnChoice = rand() % 3; // 0:P 1:N 2+:PN
            int blkChoice1 = rand() % blk_n;
            int blkChoice2 = rand() % blk_n;
            int seqChoiceP1 = blkLib[blkChoice1].pNum;
            int seqChoiceP2 = blkLib[blkChoice2].pNum;
            int seqChoiceN1 = blkLib[blkChoice1].nNum;
            int seqChoiceN2 = blkLib[blkChoice2].nNum;

            vector<int> swappedNetIdxs;
            if (pnChoice==0)
            {
                int swapper = seqP[seqChoiceP1];
                seqP[seqChoiceP1] = seqP[seqChoiceP2];
                seqP[seqChoiceP2] = swapper;
            }
            else if (pnChoice==1)
            {
                int swapper = seqN[seqChoiceN1];
                seqN[seqChoiceN1] = seqN[seqChoiceN2];
                seqN[seqChoiceN2] = swapper;
            }
            else
            {
                int swapper = seqP[seqChoiceP1];
                seqP[seqChoiceP1] = seqP[seqChoiceP2];
                seqP[seqChoiceP2] = swapper;
                swapper = seqN[seqChoiceN1];
                seqN[seqChoiceN1] = seqN[seqChoiceN2];
                seqN[seqChoiceN2] = swapper;
            }
            // cout<<"SeqP/N:"<<pnChoice<<" ";
            // for (int i=0; i<blk_n; i++)
            // {
            //     if (i==seqChoiceP1 || i==seqChoiceP2)
            //         cout<<"[";
            //     else
            //         cout<<" ";
            //     cout<<seqP[i];
            //     if (i==seqChoiceP1 || i==seqChoiceP2)
            //         cout<<"]";
            //     else
            //         cout<<" ";
            // }
            // cout<<"---";
            // for (int i=0; i<blk_n; i++)
            // {
            //     if (i==seqChoiceN1 || i==seqChoiceN2)
            //         cout<<"[";
            //     else
            //         cout<<" ";
            //     cout<<seqN[i];
            //     if (i==seqChoiceN1 || i==seqChoiceN2)
            //         cout<<"]";
            //     else
            //         cout<<" ";
            // }
            // cout<<endl;


            swappedNetIdxs.insert(swappedNetIdxs.end(), blkLib[blkChoice1].connNetIdxs.begin(), blkLib[blkChoice1].connNetIdxs.end());
            swappedNetIdxs.insert(swappedNetIdxs.end(), blkLib[blkChoice2].connNetIdxs.begin(), blkLib[blkChoice2].connNetIdxs.end());

            // --------run graph after swapping--------
            // cout<<">>blocks re-initializing"<<endl;
            max_h = 0;
            max_w = 0;
            for (int i=0; i<blk_n; i++)
            {
                blkLib[i].resetState();
            }
            // cout<<">>>Re-Plotting"<<endl;
            plot(blkLib, blk_n, seqP, seqN, max_w, max_h);
            
            // --------generate new properties--------
            Area = max_w * max_h;
            
            for (int i=0; i<swappedNetIdxs.size(); i++)
            {
                netLib[swappedNetIdxs[i]].resetHPWL();
                netLib[swappedNetIdxs[i]].refreshHPWL(blkLib);
            }
            WL=0;
            for (int i=0; i<netLib.size(); i++)
            {
                WL += netLib[i].getHPWL();
            }
            cost = getCost(Area, WL, alpha);
            

            bool accepted = acceptMove(cost-prevCost, K, T);
            if (accepted)
            {
                prevMax_h = max_h;
                prevMax_w = max_w;
                prevArea = Area;
                prevWL = WL;
                prevCost = cost;
                acceptNum++;
                if (enableAccept)
                {
                    cout<<"--NO."<<tCount<<" T="<<T<<" "\
                        <<"Op."<<i<<" swap:"<<blkChoice1<<","<<blkChoice2<<" "\
                        <<"\n  Accepted new Area:"<<Area<<"  WL:"<<WL\
                        <<"  costs: "<<cost<<" - "<<prevCost<<endl;
                    intermedData<<tCount<<","<<T<<","\
                        <<i<<","<<"accept,"<<blkChoice1<<","<<blkChoice2<<","\
                        <<Area<<","<<WL<<",\n";
                }
            }
            else
            {
                rejectNum++;
                if (enableAccept)
                {
                    cout<<"--NO."<<tCount<<" T="<<T<<" "\
                        <<"Op."<<i<<" swap:"<<blkChoice1<<","<<blkChoice2<<" "\
                        <<"rejected new Area:"<<Area<<"  WL:"<<WL<<endl;
                    intermedData<<tCount<<","<<T<<","\
                        <<i<<","<<"reject,"<<blkChoice1<<","<<blkChoice2<<","\
                        <<Area<<","<<WL<<",\n";
                }
            }
            
            // cin.get();
        }
        
        T = coolDown(T);
    }

    // ================================timer stop===============================
    auto endTime = chrono::high_resolution_clock::now();
    // =========================================================================
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
    cout<<">>> Arguement:"<<argv[2]\
        <<"  Width:"<<prevMax_w\
        <<"  Height:"<<prevMax_h\
        <<"  Area:"<<prevArea\
        <<"  HPWL:"<<prevWL\
        <<"  RunTime:"<<duration.count()<<"[ms]\n"\
        <<"  T0:"<<T0\
        <<"  Tfreez:"<<Tfreez\
        <<"  K:"<<K\
        <<"  move/temp:"<<movPerTemp\
        <<"  Alpha:"<<alpha\
        <<endl;
    outFile<<"Arguement:"<<argv[2]\
        <<"  Width:"<<prevMax_w\
        <<"  Height:"<<prevMax_h\
        <<"  Area:"<<prevArea\
        <<"  HPWL:"<<prevWL\
        <<"  RunTime:"<<duration.count()<<"[ms]\n"\
        <<"    T0:"<<T0\
        <<"  Tfreez:"<<Tfreez\
        <<"  K:"<<K\
        <<"  move/temp:"<<movPerTemp\
        <<"  Alpha:"<<alpha\
        <<endl;

    outFile.close();
    intermedData.close();
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
                    newNet.connBlkIdxs.push_back(numFrag);
                    blkLib[numFrag].connNetIdxs.push_back(newNet.getNum());
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

int plot(vector<Block> &blkLib, int &blk_amt, vector<unsigned int> &seqP, vector<unsigned int> &seqN, int &max_w, int &max_h)
{
    // ========storing relative position========
    // cout<<">>>storing relative position"<<endl;
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
    // cout<<">>>finding critical path"<<endl;
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
    // cout<<">>>coord generated"<<endl;


    // ========extracting max dimensions========
    // cout<<">>>extracting max dimensions"<<endl;
    // max_h=0;
    // max_w=0;
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

double getCost(int Area, int WL, double alpha)
{
    double avgCost = alpha*(double)Area + (1.0-alpha)*(double)WL;
    return avgCost;
}

bool acceptMove(int dCost, double K, double T)
{
    if (dCost<0)
    {
        return true;
    }
    else
    {
        double Boltz = exp((-1)*dCost/(K*T));
        double normRand = (double)rand()/(double)RAND_MAX;
        if (normRand<Boltz)
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }
    
}

double coolDown(double prevT)
{
    return 0.98*prevT;
}