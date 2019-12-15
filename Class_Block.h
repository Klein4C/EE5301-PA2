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
        vector<Block*> left;
        vector<Block*> right;
        vector<Block*> up;
        vector<Block*> down;
        void direction(bool beforeP, bool beforeN, Block *targetBlk);
        unsigned int pNum;
        unsigned int nNum;
        // ----for topological/DFS
        int DFSstateUp = -1; // -1:not visited  0:visiting 1:visited
        Block* DFSpredUp = NULL;
        int dUp = -1; // discovery time
        int fUp = -1; // finish time
        int DFSstateRt = -1; // -1:not visited  0:visiting 1:visited
        Block* DFSpredRt = NULL;
        int dRt = -1; // discovery time
        int fRt = -1; // finish time
    private:
        unsigned int blk_num;
        unsigned int blk_height=0;
        unsigned int blk_width=0;
        int bottomLeft_x=0;
        int bottomLeft_y=0;
};

class Net
{
    public:
        unsigned int setNum(unsigned int newNum);
        unsigned int getNum();
        unsigned int setDeg(unsigned int newDeg);
        unsigned int getDeg();
        vector<unsigned int> connBlks;
    private:
        unsigned int net_num;
        unsigned int net_degree;
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

#endif