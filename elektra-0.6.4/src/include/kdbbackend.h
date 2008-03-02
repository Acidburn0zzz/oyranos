/***************************************************************************
                kdbbackend.h  -  Methods for backend programing
                             -------------------
    begin                : Mon Dec 25 2004
    copyright            : (C) 2004 by Avi Alkalay
    email                : avi@unix.sh
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/


/* Subversion stuff

$Id: kdbbackend.h 876 2006-08-04 07:18:44Z ylecaillez $

*/

#ifndef KDBBACKEND_H
#define KDBBACKEND_H

typedef struct _KDBBackend *    KDBHandle;

#include <pthread.h>
#include <kdb.h>
#include <kdbprivate.h>

#ifdef ELEKTRA_STATIC
        #define KDBEXPORT(module) KDBBackend *libelektra_##module##_LTX_kdbBackendFactory(void)	
#else
        #define KDBEXPORT(module) KDBBackend *kdbBackendFactory(void)
#endif

typedef struct _KDBBackend KDBBackend;





/**
 * Switches to denote the backend methods. Used in calls to kdbBackendExport().
 *
 * @ingroup backend
 */
enum KDBBackendMethod {
	KDB_BE_OPEN=1,               /*!< Next arg is backend for kdbOpen() */
	KDB_BE_CLOSE=1<<1,           /*!< Next arg is backend for kdbClose() */
	KDB_BE_STATKEY=1<<2,         /*!< Next arg is backend for kdbStatKey() */
	KDB_BE_GETKEY=1<<3,          /*!< Next arg is backend for kdbGetKey() */
	KDB_BE_SETKEY=1<<4,          /*!< Next arg is backend for kdbSetKey() */
	KDB_BE_SETKEYS=1<<5,         /*!< Next arg is backend for kdbSetKeys() */
	KDB_BE_RENAME=1<<6,          /*!< Next arg is backend for kdbRename() */
	KDB_BE_REMOVEKEY=1<<7,       /*!< Next arg is backend for kdbRemoveKey() */
	KDB_BE_GETCHILD=1<<8,        /*!< Next arg is backend for kdbGetKeyChildKeys() */
	KDB_BE_MONITORKEY=1<<9,      /*!< Next arg is backend for kdbMonitorKey() */
	KDB_BE_MONITORKEYS=1<<10,    /*!< Next arg is backend for kdbMonitorKeys() */
	KDB_BE_END=0                 /*!< End of arguments */
};


#ifdef __cplusplus
extern "C" {
#endif

KDBBackend *kdbBackendExport(const char *backendName, ...);

typedef KDBBackend *(*KDBBackendFactory)(void);

/* Let the backend be aware of default implementations we provide */
int kdbSetKeys_default(KDBHandle handle, KeySet *ks);
int kdbRename_default(KDBHandle handle, Key *key, const char *newName);
uint32_t kdbMonitorKeys_default(KDBHandle handle, KeySet *interests,
		uint32_t diffMask, unsigned long iterations, unsigned sleep);
uint32_t kdbMonitorKey_default(KDBHandle handle, Key *interest,
		uint32_t diffMask, unsigned long iterations, unsigned sleep);


/* Some handle manipulation methods */
void *kdbhSetBackendData(KDBHandle handle, void *data);
void *kdbhGetBackendData(KDBHandle handle);

pid_t kdbhGetPID(const KDBHandle handle);
pid_t kdbhSetPID(KDBHandle handle,pid_t pid);

pthread_t kdbhGetTID(const KDBHandle handle);
pthread_t kdbhSetTID(KDBHandle handle,pthread_t tid);

uid_t kdbhGetUID(const KDBHandle handle);
uid_t kdbhSetUID(KDBHandle handle,uid_t uid);

gid_t kdbhGetGID(const KDBHandle handle);
gid_t kdbhSetGID(KDBHandle handle,gid_t gid);

mode_t kdbhGetUMask(const KDBHandle handle);
mode_t kdbhSetUMask(KDBHandle handle,mode_t umask);

char *kdbhGetUserName(const KDBHandle handle);
char *kdbhSetUserName(KDBHandle handle,char *userName);

char *kdbhGetBackendName(const KDBHandle handle);
char *kdbhSetBackendName(KDBHandle handle,char *backendName);

#ifdef __cplusplus
}
#endif





#endif /* KDBBACKEND_H */
