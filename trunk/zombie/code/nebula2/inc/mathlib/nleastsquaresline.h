#ifndef N_LEASTSQUARESLINE_H
#define N_LEASTSQUARESLINE_H
//------------------------------------------------------------------------------
/**
    @class nLeastSquaresLine
    @ingroup NebulaMathDataTypes
    @brief Calculate line parameters using the least squares method. 
    It provides a and b for the line equation y = a * x + b
    
    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
template <typename nFloatType>
class nLeastSquaresLine
{
public:
    nLeastSquaresLine();
    ~nLeastSquaresLine();
    
    void AddPoint(nFloatType x, nFloatType y);
    
    nFloatType CalculateA();
    nFloatType CalculateB();
    
private:
    nFloatType x, y;
    nFloatType xx, xy, yy;
    int n;
};

//------------------------------------------------------------------------------
/**
*/
template <typename nFloatType>
nLeastSquaresLine<nFloatType>::nLeastSquaresLine() :
    x(0), y(0),
    xx(0), xy(0), yy(0),
    n(0)
{
}

//------------------------------------------------------------------------------
/**
*/
template <typename nFloatType>
nLeastSquaresLine<nFloatType>::~nLeastSquaresLine()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
template <typename nFloatType>
void
nLeastSquaresLine<nFloatType>::AddPoint(nFloatType x, nFloatType y)
{
    this->x += x;
    this->y += y;
    this->xx += x * x;
    this->xy += x * y;
    this->yy += y * y;
    this->n++;
}

//------------------------------------------------------------------------------
/**
*/
template <typename nFloatType>
nFloatType
nLeastSquaresLine<nFloatType>::CalculateA()
{
    nFloatType a;    
    a = (n * this->xy - this->x * this->y) / (n * this->xx - this->x * this->x);
    return a;
}

//------------------------------------------------------------------------------
/**
*/
template <typename nFloatType>
nFloatType
nLeastSquaresLine<nFloatType>::CalculateB()
{
    nFloatType b;    
    b = (this->y * this->xx - this->x * this->xy) / (n * this->xx - this->x * this->x);
    return b;
}

//------------------------------------------------------------------------------
#endif // N_LEASTSQUARESLINE_H
