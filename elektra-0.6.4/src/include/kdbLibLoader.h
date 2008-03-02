/* $Id: kdbLibLoader.h 863 2006-08-03 17:54:14Z ylecaillez $ */

#ifndef KDBLIBLOADER_H
#define KDBLIBLOADER_H

/* #include "kdbbackend.h" */

/* Struct which contain export symbols
 *  Format :
 *  --------
 *
 *  filename, NULL
 *  symbol1, &func1,
 *  symbol2, &func2,
 *  filename2, NULL
 *  symbol3, &func3,
 *  symboln, &funcn,
 *  NULL, NULL
 */
typedef struct {
	const char *name;
	void *function;
} kdblib_symbol;


#ifdef ELEKTRA_STATIC

typedef kdblib_symbol* kdbLibHandle;


#else



#ifdef WIN32
/* Windows case, non static */
#include <windows.h>
typedef HMODULE kdbLibHandle;
#else
/* Default case */
#include <ltdl.h>
int kdbLibInit(void);
typedef lt_dlhandle kdbLibHandle;
#endif
#endif

/* Functions */
int kdbLibInit(void);
kdbLibHandle kdbLibLoad(const char *backendName);
void *kdbLibSym(kdbLibHandle handle, const char *symbol);
int kdbLibClose(kdbLibHandle handle);
const char *kdbLibError(void);

#endif /* KDBLIBLOADER_H */
