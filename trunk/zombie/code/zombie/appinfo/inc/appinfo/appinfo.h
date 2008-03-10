/**
   @file nappinfo.h
   @author Mateu Batle Sastre <mateu.batle@tragnarion.com>
   
   (C) 2005 Tragnarion Studios
*/
#ifndef NAPPINFO_H
#define NAPPINFO_H

// version 00.07.00
#define N_RELEASE_MAJOR             "0"
#define N_RELEASE_MINOR             "8"
#define N_RELEASE_PATCH             "2"
#define N_RELEASE_NUMBER            (atoi(N_RELEASE_MAJOR) * 10000) + (atoi(N_RELEASE_MINOR) * 100) + atoi(N_RELEASE_PATCH)
// release number string
#define N_RELEASE_NUMBER_STR        N_RELEASE_MAJOR "." N_RELEASE_MINOR "." N_RELEASE_PATCH
// compilation date
#define N_RELEASE_BUILD_DATE        __DATE__
// compilation time
#define N_RELEASE_BUILD_TIME        __TIME__
// build id ?
#define N_RELEASE_BUILD_ID          "$BuildId$"
// last version of the working copy
#define N_RELEASE_SVN_REVISION      0
// range of revisions in the working copy
#define N_RELEASE_SVN_RANGE         "$WCRANGE$"
// URL of the working copy
#define N_RELEASE_SVN_HEADURL       "$WCURL$"
// date of the revision
#define N_RELEASE_SVN_DATE          "$WCDATE$"
// 1 if the working copy has mixed revisions
#define N_RELEASE_SVN_MIXED         0
// 1 if the working copy has been locally modified
#define N_RELEASE_SVN_LOCALMOD      0
// string with complete info about the revision
#define N_RELEASE_SVN_REVISION_STR  N_RELEASE_NUMBER_STR " - $WCRANGE$$WCMIXED? Mixed:$$WCMODS? LocalMod:$ - $WCDATE$"

#endif//NAPPINFO_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
