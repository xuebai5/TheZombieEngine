
#pragma warning( push, 3 )

#include <maxscrpt/maxscrpt.h>
#include <maxscrpt/strings.h>
#include <maxscrpt/Parser.h>
#include <maxscrpt/numbers.h>

#pragma warning( pop )

//#include "export2/nmax.h"
#include "n3dsutils/nmaxscriptcall.h"

//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++.

    @param script script string which to be evaluate.
    @param res, pointer to result of script
*/
bool nMaxScriptCall(const char* script, Value*& res)
{
    TCHAR* cmds = (TCHAR*)script;

    static bool initialized = false;
    if (false == initialized)
    {
        init_MAXScript();
        initialized = true;
    }

    init_thread_locals();

    push_alloc_frame();

    two_typed_value_locals(StringStream* ss, Value* result);

    vl.ss = new StringStream(cmds);

    //bool ret = false;
    try 
    {
        //vl.result = LocalExecuteScript(vl.ss,&res);
        //res = true;
        CharStream* source = vl.ss;

        init_thread_locals();
        push_alloc_frame();
        three_typed_value_locals(Parser* parser, Value* code, Value* result);
        CharStream* out = thread_local(current_stdout);
        vl.parser = new Parser (out);

        try 
        {
            source->flush_whitespace();

            while (!source->at_eos()) 
            {
                vl.code   = vl.parser->compile(source);

                // evaluate the given script.
                vl.result = vl.code->eval()->get_heap_ptr();

                source->flush_whitespace();
            }
            source->close();
        } 
        catch (...) 
        {
            return false;
        }

        res = vl.result;
        if (vl.result == NULL)
            vl.result = &ok;

        pop_alloc_frame();
        //return_value(vl.result);
    }
    catch(...)
    {
        return false;
    }

    thread_local(current_result) = vl.result;
    thread_local(current_locals_frame) = vl.link;
    pop_alloc_frame();

    return true;
}

//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++.

    @param script script string which to be evaluate.
*/

bool nMaxScriptCall(const char* script)
{
    Value* tmpValue;
    return nMaxScriptCall(script, tmpValue);
}

//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++, return string

    @param script script string which to be evaluate.
    @param val out char pointer
*/
bool nMaxScriptGetValue(const char*script, char* &val)
{
    Value* value;
    bool result = nMaxScriptCall(script, value);

    if ( result && is_string(value))
    {
        val = value->to_string();
        return true;
    } else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++, return int

    @param script script string which to be evaluate.
    @param val out integer
*/
bool nMaxScriptGetValue(const char*script, int   &val)
{
    Value* value;
    bool result = nMaxScriptCall(script, value);

    if ( result && is_int(value))
    {
        val = value->to_int();
        return true;
    } else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++, return float

    @param script script string which to be evaluate.
    @param val out integer
*/
bool nMaxScriptGetValue(const char*script, float   &val)
{
    Value* value;
    bool result = nMaxScriptCall(script, value);

    if ( result && is_float(value))
    {
        val = value->to_float();
        return true;
    } else
    {
        return false;
    }
}


//-----------------------------------------------------------------------------
/**
    Call max script and evaluate it in C++, return boolean

    @param script script string which to be evaluate.
    @param val out boolena
*/
bool nMaxScriptGetValue(const char*script, bool &val)
{
    Value* value;
    bool result = nMaxScriptCall(script, value);

    if ( result && is_bool(value))
    {
        val = TRUE == value->to_bool();
        return true;
    } else
    {
        return false;
    }
}