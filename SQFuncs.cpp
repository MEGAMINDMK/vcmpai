#include <stdlib.h>
#include <stdio.h>
#include "SQFuncs.h"
#include <cstdio>
#include <fstream>

extern PluginFuncs* VCMP;
extern HSQAPI sq;

// ================= TEST FUNCTION =================
_SQUIRRELDEF(SQ_hello)
{
	printf("WEBNET plugin loaded!\n");
	return 0;
}

// ================= AI FUNCTION (FROM MAIN) =================
extern SQInteger fn_getChatGPTResponse(HSQUIRRELVM v);

// ================= REGISTER WRAPPER =================
SQInteger RegisterSquirrelFuncwebnet(
    HSQUIRRELVM v,
    SQFUNCTION f,
    const SQChar* fname,
    unsigned char ucParams,
    const SQChar* szParams)
{
    char szNewParams[32];

    sq->pushroottable(v);
    sq->pushstring(v, fname, -1);
    sq->newclosure(v, f, 0);

    if (ucParams > 0 && szParams)
    {
        ucParams++; // root table compensation
        sprintf(szNewParams, "t%s", szParams);
        sq->setparamscheck(v, ucParams, szNewParams);
    }

    sq->setnativeclosurename(v, -1, fname);
    sq->newslot(v, -3, SQFalse);
    sq->pop(v, 1);

    return 0;
}

// ================= REGISTER FUNCTIONS =================
void RegisterFuncswebnet(HSQUIRRELVM v)
{
    // test function
    RegisterSquirrelFuncwebnet(v, SQ_hello, "Add", 0, nullptr);

    // 🔥 AI FUNCTION
    RegisterSquirrelFuncwebnet(
        v,
        fn_getChatGPTResponse,
        "getChatGPTResponse",
        3,
        "sis"   // string, int, string
    );
}