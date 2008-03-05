// ExceptionHandler.h  Version 1.1
//
// Copyright @ 1998 Bruce Dawson
//
// Author:       Bruce Dawson
//               brucedawson@cygnus-software.com
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// A paper by the original author can be found at:
//     http://www.cygnus-software.com/papers/release_debugging.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	EXCEPTIONHANDLER_H
#define	EXCEPTIONHANDLER_H

// We forward declare PEXCEPTION_POINTERS so that the function
// prototype doesn't needlessly require windows.h.

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

int RecordExceptionInfo(PEXCEPTION_POINTERS data, const char *Message);

#endif
