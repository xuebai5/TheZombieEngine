#include "util/nstring.h"
//------------------------------------------------------------------------------
/**
    examples of sequence\n
    1 2 3  generate a 1 2 3 \n
    1..10  generate a 1 2 3 4 5 6 7 8 9 10 \n
    01..10 generate a 01 02 03 04 05 06 07 08 09 10 \n
    4..1   generate a 4 3 2 1 \n
    1..4 5 7 6..1 generate 1 2 3 4 5 7 6 5 4 3 2 1 \n
    1..3..9..4..8..1 generate a 1 2 3 4 5 6 7 8 9 8 7 6 5 4 5 6 7 8 7 6 5 4 3 2 1 \n
    @class nSequenceGenerator
    @ingroup n3dsMaxAnimators
    @brief parser of sequence\n
*/
class nSequenceGenerator 
{
public:
    /// Set the sequence ( with syntax)
    void Set(const nString sequence);
    /// Get the current sequence
    nString Get();
    ///  Analize sequence, calculate a plain sequence
    bool Start();
    /// return the plain sequence
    nString GetPlainSequence();
    /// return the next number in the plain sequence
    nString GetNext();
private:
    enum token
    {
        number  = 0,
        range   = 1,
        invalid = 2,
        end     = 3,
        numberTokens = 4,
    };
    enum state
    {
        sStart  = 0,
        sNumber = 1,
        sRange  = 2,
        sAfterRange = 3,
        sEnd = 4,
        sInvalid = 5
    };

      
    typedef struct 
    {
        int val;
        int length;
    } seqNumber;

    nString sequence;
    nString plainSequence;
    int idxString;
    seqNumber currentNumber;
    seqNumber beforeNumber;

    token GetToken();
    void AppendNumber(nString &string, int number, int length);
    void AppendNumber(nString &string, seqNumber first, seqNumber last); 
    
};