#include "kernel/nroot.h"
#include "kernel/ncmdprotocpp.h"

/**
    Macro used to simplify testing
*/
#define NCMDPROTONATIVECPP_DECLARE_TEST(TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM,TestCode) \
    NCMDPROTONATIVECPP_DECLARE(0,,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)                \
    {                                                                                                   \
        n_printf( #MemberName ": ret=(" #TR ") - " #NUMINPARAM "in=" #INPARAM " - " #NUMOUTPARAM "out=" #OUTPARAM " "); \
        TestCode;                                                                                       \
    }

class nTestCmdProtoCPP : public nRoot
{
public:
    nTestCmdProtoCPP();
    virtual ~nTestCmdProtoCPP();

    NCMDPROTOCPP_DECLARE_BEGIN(nTestCmdProtoCPP);

    // 0 parameters nothing returned
    NCMDPROTOCPP_DECLARE_TEST(void,Test0VV, 0, (), 0, (), (0) );

    // 0 parameters with return type
    NCMDPROTOCPP_DECLARE_TEST(int,       Test0IV, 0, (), 0, (), return (0) );
    NCMDPROTOCPP_DECLARE_TEST(bool,      Test0BV, 0, (), 0, (), return (false) );
    NCMDPROTOCPP_DECLARE_TEST(float,     Test0FV, 0, (), 0, (), return (0.0f) );
    NCMDPROTOCPP_DECLARE_TEST(char *,    Test0SV, 0, (), 0, (), return ("test") );
    NCMDPROTOCPP_DECLARE_TEST(void *,    Test0OV, 0, (), 0, (), return (this) );
    NCMDPROTOCPP_DECLARE_TEST(nRoot *,   Test0OV1, 0, (), 0, (), return (this) );

    // 0 parameters with return type, const version
    NCMDPROTOCPP_DECLARE_TEST(const int, Test0CIV, 0, (), 0, (), return (0) );
    NCMDPROTOCPP_DECLARE_TEST(const bool,Test0CBV, 0, (), 0, (), return (false) );
    NCMDPROTOCPP_DECLARE_TEST(const float,  Test0CFV, 0, (), 0, (), return (0.0) );
    NCMDPROTOCPP_DECLARE_TEST(const char *, Test0CSV, 0, (), 0, (), return ("test") );
    NCMDPROTOCPP_DECLARE_TEST(const void *, Test0COV, 0, (), 0, (), return (this) );
    NCMDPROTOCPP_DECLARE_TEST(const nRoot *,Test0COV1, 0, (), 0, (), return (this) );

    // 1 parameter with no return type
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VI,1,(int &), 0, (), (n_printf("%d\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VB,1,(bool &), 0, (), (n_printf("%d\n", (int) inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VF,1,(float &), 0, (), (n_printf("%f\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VS,1,(char * &), 0, (), (n_printf("%s\n", inarg1)) );        
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VO,1,(void * &), 0, (), (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() )) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VO2,1,(nRoot * &), 0, (), (n_printf("0x%x %s\n", inarg1, inarg1->GetName())) );

    // 1 inargeter with no return type, const version
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCI,1,(const int), 0, (), (n_printf("%d\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCB,1,(const bool), 0, (), (n_printf("%d\n", (int) inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCF,1,(const float), 0, (), (n_printf("%f\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCS,1,(const char *), 0, (), (n_printf("%s\n", inarg1)) );        
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCO,1,(const void *), 0, (), (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() )) );
    NCMDPROTOCPP_DECLARE_TEST(void,      Test1VCO2,1,(nRoot *), 0, (), (n_printf("0x%x %s\n", inarg1, inarg1->GetName())) );

    // 1 inargeter with return type
    NCMDPROTOCPP_DECLARE_TEST(int,       Test1II,1,(int), 0, (), return (n_printf("%d\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(bool,      Test1BB,1,(bool), 0, (), return (n_printf("%d\n", (int) inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(float,     Test1FF,1,(float), 0, (), return (n_printf("%f\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(char *,    Test1SS,1,(char *), 0, (), return (n_printf("%s\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(void *,    Test1OO,1,(void *), 0, (), return (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() ), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(nRoot *,   Test1OO2,1,(nRoot *), 0, (), return (n_printf("0x%x %s\n", inarg1, inarg1->GetName()), inarg1) );

    // 1 inargeter with constant return type
    NCMDPROTOCPP_DECLARE_TEST(const int,       Test1CII,1,(int), 0, (), return (n_printf("%d\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const bool,      Test1CBB,1,(bool), 0, (), return (n_printf("%d\n", (int) inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const float,     Test1CFF,1,(float), 0, (), return (n_printf("%f\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const char *,    Test1CSS,1,(char *), 0, (), return (n_printf("%s\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const void *,    Test1COO,1,(void *), 0, (), return (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() ), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const nRoot *,   Test1COO2,1,(nRoot *), 0, (), return (n_printf("0x%x %s\n", inarg1, inarg1->GetName()), inarg1) );

    // 1 inargeter with return type
    NCMDPROTOCPP_DECLARE_TEST(int,       Test1ICI,1,(const int), 0, (), return (n_printf("%d\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(bool,      Test1BCB,1,(const bool), 0, (), return (n_printf("%d\n", (int) inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(float,     Test1FCF,1,(const float), 0, (), return (n_printf("%f\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(char *,    Test1SCS,1,(const char *), 0, (), return (n_printf("%s\n", inarg1), (char *) inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(void *,    Test1OCO,1,(const void *), 0, (), return (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() ), (void *) inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(nRoot *,   Test1OCO2,1,(const nRoot *), 0, (), return (n_printf("0x%x %s\n", inarg1, inarg1->GetName()), (nRoot *) inarg1) );

    // 1 constant inargeter with return type
    NCMDPROTOCPP_DECLARE_TEST(const int,       Test1CICI,1,(int), 0, (), return (n_printf("%d\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const bool,      Test1CBCB,1,(bool), 0, (), return (n_printf("%d\n", (int) inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const float,     Test1CFCF,1,(float), 0, (), return (n_printf("%f\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const char *,    Test1CSCS,1,(char *), 0, (), return (n_printf("%s\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const void *,    Test1COCO,1,(void *), 0, (), return (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() ), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(const nRoot *,   Test1COCO2,1,(nRoot *), 0, (), return (n_printf("0x%x %s\n", inarg1, inarg1->GetName()), inarg1) );

    // several inargeters
    NCMDPROTOCPP_DECLARE_TEST(int,       Test6IIBFSOO, 6,(int,bool,float,char *,void *,nRoot *), 0, (), 
        return (n_printf("%d %d %f %s 0x%x %s 0x%x %s\n", 
        inarg1, 
        (int) inarg2, 
        inarg3, 
        inarg4, 
        inarg5, ((nRoot *) inarg5)->GetName(), 
        inarg6, inarg6->GetName()),  inarg1) 
    );
    NCMDPROTOCPP_DECLARE_TEST(int,       Test6ICICBCFCSCOCO, 6,(const int,const bool,const float,const char *,const void *,const nRoot *), 0, (), 
        return (n_printf("%d %d %f %s 0x%x %s 0x%x %s\n", 
        inarg1, 
        (int) inarg2, 
        inarg3, 
        inarg4, 
        inarg5, ((nRoot *) inarg5)->GetName(), 
        inarg6, inarg6->GetName()),  inarg1) 
    );
    NCMDPROTOCPP_DECLARE_TEST(void,    Test6VIBFSOO, 6,(int,bool,float,char *,void *,nRoot *), 0, (), 
        return (n_printf("%d %d %f %s 0x%x %s 0x%x %s\n", 
        inarg1, 
        (int) inarg2, 
        inarg3, 
        inarg4, 
        inarg5, ((nRoot *) inarg5)->GetName(), 
        inarg6, inarg6->GetName()) ) 
    );
    NCMDPROTOCPP_DECLARE_TEST(void,    Test6VCICBCFCSCOCO, 6,(const int,const bool,const float,const char *,const void *,const nRoot *), 0, (), 
        return (n_printf("%d %d %f %s 0x%x %s 0x%x %s\n", 
        inarg1, 
        (int) inarg2, 
        inarg3, 
        inarg4, 
        inarg5, ((nRoot *) inarg5)->GetName(), 
        inarg6, inarg6->GetName()) ) 
    );

    /// test with new type vector3 and vector3 *
    NCMDPROTOCPP_DECLARE_TEST(vector3, Test0V3V, 0, (), 0, (), return ( vector3(10.0f,20.0f,30.0f) ) );
    NCMDPROTOCPP_DECLARE_TEST(const vector3, Test0CV3V, 0, (), 0, (), return ( vector3(10.0f,20.0f,30.0f) ) );
    NCMDPROTOCPP_DECLARE_TEST(vector3 *, Test0V3PV, 0, (), 0, (), return ( n_new(vector3(10.0f,20.0f,30.0f)) ) );
    NCMDPROTOCPP_DECLARE_TEST(const vector3 *, Test0CV3PV, 0, (), 0, (), return ( n_new(vector3(10.0f,20.0f,30.0f)) ) );

    NCMDPROTOCPP_DECLARE_TEST(void,    Test1VV3,1,(vector3), 0, (), (n_printf("%f %f %f\n", inarg1.x, inarg1.y, inarg1.z)) );
    NCMDPROTOCPP_DECLARE_TEST(void,    Test1VCV3,1,(const vector3), 0, (), (n_printf("%f %f %f\n", inarg1.x, inarg1.y, inarg1.z)) );
    NCMDPROTOCPP_DECLARE_TEST(void,    Test1VV3P,1,(vector3 *), 0, (), (n_printf("%f %f %f\n", inarg1->x, inarg1->y, inarg1->z)) );
    NCMDPROTOCPP_DECLARE_TEST(void,    Test1VCV3P,1,(const vector3 *), 0, (), (n_printf("%f %f %f\n", inarg1->x, inarg1->y, inarg1->z)) );

    /// test output inargeters
    NCMDPROTOCPP_DECLARE_TEST(void,  OutTest1V3, 0, (), 1, (vector3 *), ( outarg1->x = 1.f, outarg1->y = 2.f, outarg1->z = 3.f ) );
    NCMDPROTOCPP_DECLARE_TEST(int,  OutTest1IV3, 0, (), 1, (vector3 *), return ( outarg1->x = 1.f, outarg1->y = 2.f, outarg1->z = 3.f, 25 ) );

    // 1 inargeter with no return type with input references
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VI,1,(int &), 0, (), (n_printf("%d\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VB,1,(bool &), 0, (), (n_printf("%d\n", (int) inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VF,1,(float &), 0, (), (n_printf("%f\n", inarg1)) );
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VS,1,(char * &), 0, (), (n_printf("%s\n", inarg1)) );        
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VO,1,(void * &), 0, (), (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() )) );
    NCMDPROTOCPP_DECLARE_TEST(void,      RefTest1VO2,1,(nRoot * &), 0, (), (n_printf("0x%x %s\n", inarg1, inarg1->GetName())) );

    // 1 inargeter with return type with input references & return references
    NCMDPROTOCPP_DECLARE_TEST(int &,       RefTest1II,1,(int &), 0, (), return (n_printf("%d\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(bool &,      RefTest1BB,1,(bool &), 0, (), return (n_printf("%d\n", (int) inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(float &,     RefTest1FF,1,(float &), 0, (), return (n_printf("%f\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(char * &,    RefTest1SS,1,(char * &), 0, (), return (n_printf("%s\n", inarg1), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(void * &,    RefTest1OO,1,(void * &), 0, (), return (n_printf("0x%x %s\n", inarg1, ((nRoot *) inarg1)->GetName() ), inarg1) );
    NCMDPROTOCPP_DECLARE_TEST(nRoot * &,   RefTest1OO2,1,(nRoot * &), 0, (), return (n_printf("0x%x %s\n", inarg1, inarg1->GetName()), inarg1) );

    NCMDPROTOCPP_DECLARE_TEST(int,  InOutTest1IV3, 1, (vector3 *), 1, (vector3 *), return ( outarg1->x = inarg1->x, outarg1->y = inarg1->y, outarg1->z = inarg1->z, 22 ) );

    NCMDPROTOCPP_DECLARE_END(nTestCmdProtoCPP)
};

