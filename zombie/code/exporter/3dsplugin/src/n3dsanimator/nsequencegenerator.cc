#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsanimator/nsequencegenerator.h"
void 
nSequenceGenerator::Set(const nString sequence)
{
    this->sequence = sequence;
    this->sequence += " "; //Centinell
}

bool
nSequenceGenerator::Start()
{
    static const state mapNextState[state::sEnd][token::numberTokens]
    = {  //  number,   range ,   invalid , end
        { sNumber,     sInvalid, sInvalid, sInvalid }, // sStart
        { sNumber,     sRange,   sInvalid, sEnd     }, // sNumber
        { sAfterRange, sInvalid, sInvalid, sInvalid }, // sRange
        { sNumber,     sRange,   sInvalid, sEnd     }  // sAfterRange
     };

    state currentState = sStart;
    token currentToken;
    bool continueStateMachine = true;
    bool returnValue = true;

    idxString = 0;
    while ( continueStateMachine )
    {
        switch(currentState)
        {
            case sStart:
                plainSequence = "";
                break;

            case sNumber:
                AppendNumber( plainSequence, currentNumber.val , currentNumber.length );
                plainSequence +=" ";
                break;

            case sRange:
                beforeNumber = currentNumber;
                break;

            case sAfterRange:
                //GrammarNumberAfterRange();
                AppendNumber( plainSequence,  beforeNumber, currentNumber);
                break;

            case sInvalid:
                returnValue = false;
                continueStateMachine = false;
                break;

            case sEnd:
                returnValue = true;
                continueStateMachine = false;
                break;
        }

        if ( continueStateMachine )
        {
            currentToken = this->GetToken(); // this change a currentNumber
            currentState = mapNextState[currentState][currentToken];
        }

    }

    return returnValue;
}

nString
nSequenceGenerator::GetNext() 
{
    nString tmp = plainSequence.GetToken(' ');
    return  tmp;
}

nSequenceGenerator::token
nSequenceGenerator::GetToken()
{
    while ( sequence.Length() > idxString && 
            sequence[idxString] == ' ' ) idxString++;

    if ( sequence.Length() == idxString ) 
    {
        return nSequenceGenerator::token::end;
    }

    if ( isdigit( sequence[ idxString ] ) )
    {
        this->currentNumber.val = atoi( &sequence[ idxString ] );
        this->currentNumber.length= 0;
        do
        {
          idxString++;
          currentNumber.length++;
        } while ( isdigit( sequence[ idxString ] ) ); // The last character is space

        return nSequenceGenerator::token::number;

    } 
    
    if ( sequence[ idxString ]     == '.'  && 
         sequence[ idxString + 1]  == '.' )  // The last character is space
    {
        idxString += 2;
        return nSequenceGenerator::token::range;
    }


    return nSequenceGenerator::token::invalid;

}

void 
nSequenceGenerator::AppendNumber(nString &string, int number, int length)
{
    nString num;
    num.SetInt( number );
    for ( int idx = num.Length() ; idx < length ; idx++)
    {
        string +="0";
    }
    string += num;
}

void 
nSequenceGenerator::AppendNumber(nString &string, seqNumber first, seqNumber last)
{
    int inc;
    int length;

    inc    =  first.val < last.val ? 1 : -1;

    // select the minimun length
    // 01..15 is 2
    //  1..15 is 1
    length =  first.length < last.length ? first.length : last.length; 
    

    // The first number not added
    
    const int end = last.val + inc;

    for ( int index = first.val + inc ; index != end ; index+=inc)
    {
        AppendNumber(string, index, length);
        plainSequence +=" ";
    }

}

nString 
nSequenceGenerator::GetPlainSequence()
{
    return this->plainSequence;
}

nString 
nSequenceGenerator::Get()
{
    return this->sequence;
}