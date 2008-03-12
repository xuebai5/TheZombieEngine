#ifndef  N_TEX_ANIMATOR_BUILDER_H
#define  N_TEX_ANIMATOR_BUILDER_H
#include "util/nstring.h"
#include "nscene/nanimator.h"
#include "n3dsanimator/nsequencegenerator.h"
//------------------------------------------------------------------------------
/**
    @class nAnimatorBuilder
    @ingroup n3dsMaxAnimators
    @brief builder of nAnimator
*/
class nAnimatorBuilder
{
public:
    virtual const char * GetUniqueString();
    virtual const char * GetClass();
    virtual void         SetToAnimator(nAnimator* anim);
};

//------------------------------------------------------------------------------
/**
    @class nTexAnimatorBuilder
    @ingroup n3dsMaxAnimators
    @brief builder of nTextureAnimator
*/
class nTexAnimatorBuilder : public nAnimatorBuilder
{
public:
    nTexAnimatorBuilder();
    virtual const char * GetUniqueString();
    virtual const char * GetClass();
    virtual void SetToAnimator(nAnimator * anim);

    void SetTexture(nString &text);
    
    //void setNumbers( int first, int last);
   // void SetFirst( int first) ;
   // void SetLast ( int last );
    void SetFrequency( int freq);
    void SetShaderParam( const nString &param );
    const nString& GetShaderParam() const;
    void SetLoopType(nAnimLoopType::Type loopType);
    void SetLoopType(nString loopType);
    void SetSequence(nString sequence); 
    bool InitSequence();
    bool CheckFilesOfAnimator();
    bool CollectTextureAnimator(const nString &dst);

    nString GetNextTexture();
    nString GetTexture();
    nString GetExtension();

    


private:
    bool stringValid;
    nString  stringKey;
    nString  texture;
    nString  extension;
    nString  param;
    nSequenceGenerator sequence;
    int last;
    int first;
    int freq;
    nAnimLoopType::Type loopType;

};

inline
nTexAnimatorBuilder::nTexAnimatorBuilder() :  stringValid(false)
{
}

inline
const char * 
nTexAnimatorBuilder::GetClass()
{
   return "ntextureanimator";
}
inline
nString 
nTexAnimatorBuilder::GetTexture()
{
    return this->texture;
}

inline
nString 
nTexAnimatorBuilder::GetExtension()
{
    return this->extension;
}


#endif