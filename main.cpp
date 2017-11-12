/*
 **************************************************************************************
 *       Filename:  main.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2017-11-09 19:29:09
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#include <stdio.h>
class ABC {
public:
    ABC() {}
    ~ABC() {}
    virtual ABC* operator()() { printf("ABC\n"); return new ABC;}
private:
};

class AAA : public ABC{
public:
    AAA() {}
    ~AAA() {}
    ABC* operator()() { printf("AAA\n"); return new AAA;}

private:
    /* data */
};

int main(int argc, const char *argv[]) {
    ABC* abc = new AAA;
    (*abc)();
    return 0;
}

/********************************** END **********************************************/

