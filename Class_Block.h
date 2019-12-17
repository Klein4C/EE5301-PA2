#ifndef BLK_H
#define BLK_H

#include <vector>
using namespace std;

class Block
{
    public:
        unsigned int setNum(unsigned int newNum);
        unsigned int getNum();
        unsigned int setHeight(unsigned int newH);
        unsigned int getHeight();
        unsigned int setWidth(unsigned int newW);
        unsigned int getWidth();
        void setLL(int x, int y);
        int getLLx();
        int getLLy();
        vector<Block*> left;  // [RST]
        vector<Block*> right; // [RST]
        vector<Block*> up; // [RST]
        vector<Block*> down; // [RST]
        void direction(bool beforeP, bool beforeN, Block *targetBlk);
        unsigned int pNum;
        unsigned int nNum;
        vector<int> connNetIdxs;
        // ----for topological/DFS
        int DFSstateUp = -1; // -1:not visited  0:visiting 1:visited [RST]
        Block* DFSpredUp = NULL; // [RST]
        int dUp = -1; // discovery time [RST]
        int fUp = -1; // finish time [RST]
        int DFSstateRt = -1; // -1:not visited  0:visiting 1:visited [RST]
        Block* DFSpredRt = NULL; // [RST]
        int dRt = -1; // discovery time [RST]
        int fRt = -1; // finish time [RST]
        void resetState();
    private:
        unsigned int blk_num;
        unsigned int blk_height=0;
        unsigned int blk_width=0;
        int bottomLeft_x=0; // [RST]
        int bottomLeft_y=0; // [RST]
};

class Net
{
    public:
        unsigned int setNum(unsigned int newNum);
        unsigned int getNum();
        unsigned int setDeg(unsigned int newDeg);
        unsigned int getDeg();
        vector<unsigned int> connBlkIdxs;
        long int refreshHPWL(vector<Block> blkLib);
        void resetHPWL();
        long int getHPWL();
    private:
        unsigned int net_num;
        unsigned int net_degree;
        long int net_hpwl = 0;
        long int hpwlLLx;
        long int hpwlLLy;
        long int hpwlURx;
        long int hpwlURy;

};

// ==========Function Implementation Blk==========
// ----------Block Class----------
unsigned int Block::setNum(unsigned int newNum)
{
    blk_num = newNum;
    return blk_num;
}

unsigned int Block::getNum()
{
    return blk_num;
}

unsigned int Block::setHeight(unsigned int newH)
{
    blk_height = newH;
    return blk_height;
}

unsigned int Block::getHeight()
{
    return blk_height;
}

unsigned int Block::setWidth(unsigned int newW)
{
    blk_width = newW;
    return blk_width;
}

unsigned int Block::getWidth()
{
    return blk_width;
}

void Block::setLL(int x, int y)
{
    bottomLeft_x = x;
    bottomLeft_y = y;
}

int Block::getLLx()
{
    return bottomLeft_x;
}

int Block::getLLy()
{
    return bottomLeft_y;
}

void Block::direction(bool beforeP, bool beforeN, Block *targetBlk)
{
    if (beforeP)
    {
        if (beforeN)
        {
            left.push_back(targetBlk);
            // cout<<blk_num<<" left push: "<<targetBlk->blk_num<<endl;
        }
        else
        {
            up.push_back(targetBlk);
            // cout<<blk_num<<" up push: "<<targetBlk->blk_num<<endl;
        }
    }
    else
    {
        if (beforeN)
        {
            down.push_back(targetBlk);
            // cout<<blk_num<<" down push: "<<targetBlk->blk_num<<endl;
        }
        else
        {
            right.push_back(targetBlk);
            // cout<<blk_num<<" right push: "<<targetBlk->blk_num<<endl;
        }
    }
    
}

void Block::resetState()
{
    left.clear();
    right.clear();
    up.clear();
    down.clear();
    DFSstateUp = -1; // -1:not visited  0:visiting 1:visited
    DFSpredUp = NULL;
    dUp = -1; // discovery time
    fUp = -1; // finish time
    DFSstateRt = -1; // -1:not visited  0:visiting 1:visited
    DFSpredRt = NULL;
    dRt = -1; // discovery time
    fRt = -1; // finish time
    bottomLeft_x=0;
    bottomLeft_y=0;
}


// ----------Net Class----------
unsigned int Net::setNum(unsigned int newNum)
{
    net_num = newNum;
    return net_num;
}

unsigned int Net::getNum()
{
    return net_num;
}

unsigned int Net::setDeg(unsigned int newDeg)
{
    net_degree = newDeg;
    return net_degree;
}

unsigned int Net::getDeg()
{
    return net_degree;
}

long int Net::refreshHPWL(vector<Block> blkLib)
{
    // cout<<endl;
    hpwlLLx = 30000;
    hpwlLLy = 30000;
    hpwlURx = 0;
    hpwlURy = 0;
    for (int i=0; i<connBlkIdxs.size(); i++)
    {
        // cout<<"net-conn-blk["<<blkLib[connBlkIdxs[i]].getNum()<<"]"<<endl;
        long int centerX = blkLib[connBlkIdxs[i]].getLLx()+blkLib[connBlkIdxs[i]].getWidth()/2;
        // cout<<"  Cx:"<<centerX;
        long int centerY = blkLib[connBlkIdxs[i]].getLLy()+blkLib[connBlkIdxs[i]].getHeight()/2;
        // cout<<"  Cy:"<<centerY<<endl;
        hpwlLLx = (centerX<hpwlLLx) ? centerX:hpwlLLx;
        hpwlLLy = (centerY<hpwlLLy) ? centerY:hpwlLLy;
        hpwlURx = (centerX<hpwlURx) ? hpwlURx:centerX;
        hpwlURy = (centerX<hpwlURy) ? hpwlURy:centerY;
        // cout<<"  ->"<<hpwlLLx<<","<<hpwlLLy<<"  "<<hpwlURx<<","<<hpwlURy<<endl;
    }
    net_hpwl = (hpwlURx-hpwlLLx) + (hpwlURy-hpwlLLy);
    // cout<<" --net["<<net_num<<"].HPWL==>"<<net_hpwl<<endl;
    return net_hpwl;
}

void Net::resetHPWL()
{
    net_hpwl = 0;
}

long int Net::getHPWL()
{
    return net_hpwl;
}


#endif