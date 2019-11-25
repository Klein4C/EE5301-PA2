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
    private:
        unsigned int blk_num;
        unsigned int blk_height;
        unsigned int blk_width;
        int bottomLeft_x;
        int bottomLeft_y;
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