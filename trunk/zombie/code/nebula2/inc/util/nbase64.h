#ifndef N_BASE64_H
#define N_BASE64_H

//------------------------------------------------------------------------------
/**
    @class nBase64
    @ingroup Util
    @brief A simple converter between generic data and base64, both ways.
    @author Carles Ros
*/

//------------------------------------------------------------------------------
struct nBase64
{
    /// Get a base64 representation of given data (the result must be freed with n_free)
    static void Base256To64( int size, const char* source, char*& result );
    /// Convert back data represented in base64 (the result must be freed with n_free)
    static void Base64To256( const char* source, int& size, char*& result );
};

#endif
