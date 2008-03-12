/**********************************************************************
 *<
	FILE: mtlhdr.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MAXTEXTURE_H
#define __MAXTEXTURE__H

#define USE_STDMTL2_AS_STDMTL
 
#ifdef BLD_MTL
#define MtlExport __declspec( dllexport )
#else
#define MtlExport __declspec( dllimport )
#endif

#pragma warning( push, 3 )
#include "max.h"
#include "imtl.h"
#include "texutil.h"
#include "buildver.h"
#pragma warning( pop )
extern ClassDesc* GetRGBAddDesc();

TCHAR *GetString(int id);

#endif
