#ifndef RESULTPAIR_H
#define RESULTPAIR_H

/**
 * \brief ResultPair.h ...::: Result pair for 2 channel value :::...
*
* ..
*
* author: tonis @ 2013
*
*/
class ResultPair
{
public:
    ResultPair(double x, double y);
    ResultPair();
    double x;
    double y;
    virtual ~ResultPair();
protected:
private:
};

#endif // RESULTPAIR_H
