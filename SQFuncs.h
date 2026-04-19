//
// SQFuncs: These are the tools to register custom functions within the Squirrel VM.
//
//	Written for Liberty Unleashed by the Liberty Unleashed Team.
//

#pragma once
#ifndef _SQFUNCS_H
#define _SQFUNCS_H

#include "SQMain.h"
#include "squirrel.h"

#define _SQUIRRELDEF(x) SQInteger x(HSQUIRRELVM v)

#ifdef __cplusplus
extern "C" {
#endif
	SQInteger				RegisterSquirrelFuncwebnet				(HSQUIRRELVM v, SQFUNCTION f, const SQChar* fname, unsigned char uiParams, const SQChar* szParams);
	void					RegisterFuncswebnet						(HSQUIRRELVM v);
#ifdef __cplusplus
}
#endif

#endif
