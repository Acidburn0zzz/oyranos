/***************************************************************************
                kdb.h  -  Exported methods of the Elektra Library
                             -------------------
    begin                : Mon Dec 29 2003
    copyright            : (C) 2003 by Avi Alkalay
    email                : avi@unix.sh
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/


/* Subversion stuff

$Id: kdb.h 921 2006-09-07 14:24:10Z aviram $

*/

#ifndef KDB_H
#define KDB_H


/*
 * @defgroup general Elektra General definitions
 * @brief Some global definitions when using the Elektra API
 *
 */

/* Windows specific stuff */
#ifdef WIN32
#include <windows.h>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#define ssize_t int
#define strcasecmp stricmp
#define snprintf _snprintf

#else /* WIN32 */
/* The following file doesn't exist on windows so we need to
 * avoid including it */
#include <inttypes.h>

#endif /* WIN32 */

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

/*#define NO_REGEX_H*/ /* Uncomment to disable regex */
/* Makes it possible to compile elektra without regex.
 * For now this has to be defined manually. */
#ifndef NO_REGEX_H
#include <regex.h>
#else
#define regex_t void
#endif

#define RG_KEY_DELIM            '/'


/* When FORMAT_VERSION changes, FORMAT must change too. */
#define RG_KEY_FORMAT_VERSION   2
#define RG_KEY_FORMAT           "RG00" RG_KEY_FORMAT_VERSION


/**
 * Key data types.
 *
 * Key type values grow from the semantically poor to the semantically rich.
 * The gaps between them is for user-defined types.
 *
 * If your application needs value types with more semantics, like @p Color,
 * @p Font, etc, you can still use it. You'll have to define a new type
 * number in the scope of your application, and force the type with
 * keySetType(), or keyNew().
 *
 * The type number is a value between 0 and 255. If your user-defined
 * type >= @p KEY_TYPE_STRING, it will be still treated as a string
 * (in the terms of Unicode handling). If 
 * @p KEY_TYPE_BINARY <= type < @p KEY_TYPE_STRING, Elektra will handle it
 * as a binary value, will not make Unicode handling and will save it
 * hex-encoded.
 *
 * @ingroup key
 * @see keyGetType()
 * @see keySetType() for an example of how to define custom types
 */
enum KeyType {
	KEY_TYPE_UNDEFINED=0,  /*!< Undefined key type */
	KEY_TYPE_DIR=1,        /*!< A directory key */
	KEY_TYPE_LINK=2,       /*!< A symbolink link key.
	                          This gap is for special key meta types,
	                          that can't go into regular files. */
	KEY_TYPE_BINARY=20,    /*!< A binary key.
	                          This gap is for binary data types
	                          that have some semantics that somebody
	                          can invent in the future */
	KEY_TYPE_STRING=40     /*!< A string key */
};






/**
 * Elektra currently supported Key namespaces.
 * @ingroup key
 * @see kdbGetRootKeys(), keyGetNamespace(), keyNameGetNamespace()
 */
enum KeyNamespace {
	KEY_NS_SYSTEM=1,       /*!< The @p system keys */
	KEY_NS_USER=2          /*!< The @p user keys */
};



/*
The key flags bit array. The '.' means whatever:

7654 3210 7654 3210 7654 3210 7654 3210
...1 0... .0.. .1.. ..1. ..0. ...0 1... 0x10042008 Initialized
...1 1... .1.. .1.. ..1. ..1. ...1 1... 0x18442218 Initialized mask
.... .... .... .... .... .... .... ...1 0x00000001 TYPE
.... .... .... .... .... .... .... ..1. 0x00000002 NAME
.... .... .... .... .... .... .... .1.. 0x00000004 VALUE
.... .... .... .... .... .... ..1. .... 0x00000020 DOMAIN
.... .... .... .... .... .... .1.. .... 0x00000040 COMMENT
.... .... .... .... .... .... 1... .... 0x00000060 UID
.... .... .... .... .... ...1 .... .... 0x00000080 GID
.... .... .... .... .... .1.. .... .... 0x00000400 PRM
.... .... .... .... .... 1... .... .... 0x00000800 TIME
.... .... .... .... ...1 .... .... .... 0x00001000 NEEDSYNC
.... .... .... .... .1.. .... .... .... 0x00004000 ACTIVE ***DEPRECATED***
.... .... 1... .... .... .... .... .... 0x00800000 ISSYSTEM
.... ...1 .... .... .... .... .... .... 0x01000000 ISUSER
1... .... .... .... .... .... .... .... 0x80000000 FLAG (general flag)
*/



/**
 * Switches to denote the various Key attributes in methods throughout
 * this library.
 * @ingroup key
 * @see keyNew()
 * @see keyCompare()
 * @see kdbMonitorKey(), kdbMonitorKeys(), the diffMask parameter
 * @see keyGetFlag(), keySetFlag()
 */
enum KeySwitch {
	KEY_SWITCH_TYPE=1,          /*!< Flag for the key type */
	KEY_SWITCH_NAME=1<<1,       /*!< Flag for the key name */
	KEY_SWITCH_VALUE=1<<2,      /*!< Flag for the key data */
	/* do not use 1<<3 see INITMASK */
	/* do not use 1<<4 see INITMASK */
	KEY_SWITCH_OWNER=1<<5,      /*!< Flag for the key user domain */
	KEY_SWITCH_DOMAIN=KEY_SWITCH_OWNER, /*!< An alias */
	KEY_SWITCH_COMMENT=1<<6,    /*!< Flag for the key comment */
	KEY_SWITCH_UID=1<<7,        /*!< Flag for the key UID */
	KEY_SWITCH_GID=1<<8,        /*!< Flag for the key GID */
	/* do not use 1<<9 see INITMASK */
	KEY_SWITCH_MODE=1<<10,      /*!< Flag for the key permissions */
	KEY_SWITCH_TIME=1<<11,      /*!< Flag for the key change time */
	KEY_SWITCH_NEEDSYNC=1<<12,  /*!< Flags that key needs syncronization */
	/* do not use 1<<13 see INITMASK */
	KEY_SWITCH_ACTIVE=1<<14,    /* ****deprecated**** */
	KEY_SWITCH_UMODE=1<<15,     /*!< Flag for key permissions based on umask */
	/* do not use 1<<18 see INITMASK */
	/* do not use 1<<22 see INITMASK */
	KEY_SWITCH_ISSYSTEM=1<<23,  /*!< Flag to denote a @c "system" key */
	KEY_SWITCH_ISUSER=1<<24,    /*!< Flag to denote a @c "user" key */
	/* do not use 1<<27 see INITMASK */
	/* do not use 1<<28 see INITMASK */
	KEY_SWITCH_FLAG=1<<31,      /*!< General purpose flag that has semantics
	                                 only to your app */
	KEY_SWITCH_END=0,           /*!< Used as a parameter terminator to
	                                 keyNew() */
	
	KEY_SWITCH_INITIALIZED=0x10042008,
	KEY_SWITCH_INITMASK=0x18442218
};



/*
 * Deprecated flag names, here for legacy compatibility.
 * Will be removed in the future.
 * Use KeySwitches above instead.
 */
enum KeyFlags {                                       /* _DEPRECATED_ */
	KEY_FLAG_HASTYPE      = KEY_SWITCH_TYPE,          /* _DEPRECATED_ */
	KEY_FLAG_HASKEY       = KEY_SWITCH_NAME,          /* _DEPRECATED_ */
	KEY_FLAG_HASDATA      = KEY_SWITCH_VALUE,         /* _DEPRECATED_ */
	KEY_FLAG_HASDOMAIN    = KEY_SWITCH_DOMAIN,        /* _DEPRECATED_ */
	KEY_FLAG_HASCOMMENT   = KEY_SWITCH_COMMENT,       /* _DEPRECATED_ */
	KEY_FLAG_HASUID       = KEY_SWITCH_UID,           /* _DEPRECATED_ */
	KEY_FLAG_HASGID       = KEY_SWITCH_GID,           /* _DEPRECATED_ */
	KEY_FLAG_HASPRM       = KEY_SWITCH_MODE,          /* _DEPRECATED_ */
	KEY_FLAG_HASTIME      = KEY_SWITCH_TIME,          /* _DEPRECATED_ */
	KEY_FLAG_NEEDSYNC     = KEY_SWITCH_NEEDSYNC,      /* _DEPRECATED_ */
	KEY_FLAG_FLAG         = KEY_SWITCH_FLAG,          /* _DEPRECATED_ */
	KEY_FLAG_INITIALIZED  = KEY_SWITCH_INITIALIZED,   /* _DEPRECATED_ */
	KEY_FLAG_INITMASK     = KEY_SWITCH_INITMASK       /* _DEPRECATED_ */
};                                                    /* _DEPRECATED_ */



/**
 * Some return codes generated by the Elektra library.
 *
 * These are Elektra specific errors only, that the library sets in @c errno.
 * Other errors can be generated by system calls that the API uses.
 * Then @c errno is propagated.
 *
 * The idea is to keep compatibility to POSIX @c errno system, so each library
 * error code maps to some POSIX E* error. Some mappings realy makes no sense,
 * so to detect Elektra errors use the following error names, and to detect
 * system's, use the naming convetions documented in @c errno man page.
 *
 * A very robust program should check @c errno after each API call.
 * @see kdbGetChildKeys() for an example on how to handle errors
 *
 * @ingroup kdb
 */
enum KDBErr {
	KDB_RET_OK=0,                 /*!< No error */


	/* Errors related to invalid/uninitialized objects */
	KDB_RET_NULLKEY=EINVAL,       /*!< Invalid Key object */
	KDB_RET_UNINITIALIZED=EINVAL, /*!< Object not initilized */

	/* Errors related to bad key names or keys not found */
	KDB_RET_NOTFOUND=ENOENT,      /*!< Key was not found */
	KDB_RET_INVALIDKEY=ESRCH, /*!< Key name is not @p 'system/something'
	                                      or @p 'user/something...' */

	/* Errors related to empty internal key properties */
	KDB_RET_NOKEY=ENXIO,        /*!< Key has no name */
	KDB_RET_NODATA=ENODEV,      /*!< Key has no data */
	KDB_RET_NODESC=ENOTDIR,     /*!< Key has no comment/description */
	KDB_RET_NODOMAIN=EDOM,      /*!< Key has no user domain set */
	KDB_RET_NOGROUP=ECHILD,     /*!< Key has no group */
	KDB_RET_NOTIME=ENOTTY,      /*!< Key has no access time set */


	/* Errors related to permissions, no memory or failed internal operations */
	KDB_RET_NOCRED=EACCES,       /*!< No credentials to access resource */
	KDB_RET_TRUNC=ERANGE,        /*!< Buffer was too small */
	KDB_RET_NOMEM=ENOMEM,        /*!< Out of memory */
	KDB_RET_TYPEMISMATCH=EBADF,  /*!< Failed to convert key data due to
	                                   data type */

	/* Errors related to backend access or opening */
	KDB_RET_NOSYS=ENOSYS,        /*!< Backend method not implemented */
	KDB_RET_EBACKEND=EIO         /*!< Error opening backend */
};


/**
 * Options to change the default behavior of some methods.
 *
 * These options should be ORed.
 * @ingroup kdb
 * @see kdbGetChildKeys()
 * @see ksToStream()
 * @see keyToStream()
 */
enum KDBOptions {
	KDB_O_RECURSIVE=1,       /*!< Act recursively. */
	KDB_O_DIR=1<<1,          /*!< Include dir keys in result. */
	KDB_O_DIRONLY=1<<2,      /*!< Retrieve only directory keys. */
	KDB_O_NOEMPTY=1<<3,      /* unused. */
	KDB_O_STATONLY=1<<4,     /*!< Only stat key, instead of getting entirelly.*/
	KDB_O_INACTIVE=1<<5,     /*!< Do not ignore inactive keys (that name begins
	                              with .). */
	KDB_O_SORT=1<<6,         /*!< Sort keys. */
	KDB_O_NFOLLOWLINK=1<<7,  /*!< Do not follow symlinks. */

/* XML exporting options for keyToStrem() and ksToStream() */
	KDB_O_CONDENSED=1<<8,    /*!< Compressed XML, not usefull for human eyes.*/
	KDB_O_NUMBERS=1<<9,      /*!< Use numbers intead of user and group names.*/
	KDB_O_XMLHEADERS=1<<10,  /*!< Show also the XML header of the document. */
	KDB_O_FULLNAME=1<<11,    /*!< Export @p user keys using full name
	                              (e.g. user:username/some/key). */
	KDB_O_FULLUGID=1<<12,    /*!< Don't supress obvious key UID,
	                              GID, and user domain. Affects
	                              only @p user keys. */
	KDB_O_HIER=1<<13,        /*!< Export to the new hierarchical XML
	                              representation using key basename.
	                              See ksToStream(). */

/* Options used by ksLookupRE() methods */
	KDB_O_NOCASE=1<<15,      /*!< Ignore case in ksLookup*() methods */
	KDB_O_NOSPANPARENT=1<<16,/*!< Don't continue search if end of current
	                              folder reached, in ksLookupRE() */
	KDB_O_ALL=1<<17,         /*!< Restart search, in ksLookup*() from
	                              start -> cursor when cursor -> end
	                              failed */

/* Obsolete/renamed options */
	KDB_O_NOVALUE  = KDB_O_DIRONLY,
	KDB_O_LOOP     = KDB_O_ALL

};


/* Key Name Anatomy

Key::key is the key name. It is a unique identifier for a kdb key.
An exmaple of a complex key name is:

	user:some.user/My Environment/SOME.VAR

From this example:
  Root name = "user:some.user"
      Owner = "some.user"
  Base name = "SOME.VAR"
Parent name = "user:some.user/My Environment"

*/





typedef struct _Key       Key;
typedef struct _KeySet    KeySet;

#ifndef KDBBACKEND_H
/* If we are not in a backend implementation context... */
typedef void *            KDBHandle;
#endif

/**
 * Object returned by kdbGetInfo() containing some informations about the
 * Elektra library and backend being used.
 *
 * You should not allocate or deallocate memory for each of these members.
 * This is a library responsability.
 *
 * @see kdbGetInfo(), kdbFreeInfo(), kdbInfoToString()
 * @see commandInfo() of the 'kdb info' command to see it in action
 * @ingroup kdb
 */
typedef struct _KDBInfo {
	/* all members are pointers because we'll point only to pre-allocated
	   or static strings. We won't allocate nothing for each member. */
	char *version;             /*!< Version of the library*/
	char *backendName;         /*!< Name of backend being or
	                                that will be used */
	uint8_t backendIsOpen;     /*!< 1 if backend was already open
	                                with kdbOpen(), 0 otherwise */
} KDBInfo;




#ifdef __cplusplus
extern "C" {
#endif


/**************************************

KeyDB methods

***************************************/

int kdbOpen(KDBHandle *handle);
int kdbOpenDefault(KDBHandle *handle);
int kdbOpenBackend(KDBHandle *handle, char *backendName);
int kdbClose(KDBHandle *handle);

int kdbGetValue(KDBHandle handle, const char *keyname, char *returned,
	size_t maxSize);
int kdbGetKeyByParent(KDBHandle handle, const char *parentName,
	const char *baseName, Key *returned);
int kdbGetKeyByParentKey(KDBHandle handle, const Key *parent,
	const char *baseName, Key *returned);
int kdbGetValueByParent(KDBHandle handle, const char *parentName,
	const char *baseName, char *returned, size_t maxSize);

int kdbSetValue(KDBHandle handle, const char *keyname, const char *value);
int kdbSetValueByParent(KDBHandle handle, const char *parentName,
	const char *baseName, const char *value);

int kdbRename(KDBHandle handle, Key *key, const char *newName);
int kdbRemove(KDBHandle handle, const char *keyName);
int kdbRemoveKey(KDBHandle handle, const Key *key);
int kdbLink(KDBHandle handle, const char *oldPath, const char *newKeyName);

int kdbGetKeyByParent(KDBHandle handle, const char *parentName,
	const char *baseName, Key *returned);
int kdbGetKeyByParentKey(KDBHandle handle, const Key *parent,
	const char *basename, Key *returned);
int kdbGetValueByParent(KDBHandle handle, const char *parentName,
	const char *baseName, char *returned, size_t maxSize);


int kdbStatKey(KDBHandle handle, Key *key);
int kdbGetKey(KDBHandle handle, Key *key);
int kdbSetKey(KDBHandle handle, Key *key);

ssize_t kdbGetKeyChildKeys(KDBHandle handle, const Key *parentName,
	KeySet *returned, unsigned long options);
ssize_t kdbGetChildKeys(KDBHandle handle, const char *parentName,
	KeySet *returned, unsigned long options);
ssize_t kdbGetRootKeys(KDBHandle handle, KeySet *returned);

int kdbSetKeys(KDBHandle handle, KeySet *ks);

uint32_t kdbMonitorKey(KDBHandle handle, Key *interest, uint32_t diffMask,
	unsigned long iterations, unsigned usleep);
uint32_t kdbMonitorKeys(KDBHandle handle, KeySet *interests, uint32_t diffMask,
	unsigned long iterations, unsigned sleep);

KDBInfo *kdbGetInfo(KDBHandle handle);
void kdbFreeInfo(KDBInfo *info);
int kdbInfoToString(KDBInfo *info,char *string,size_t maxSize);


/**************************************

Error manipulation methods

***************************************/


char *kdbStrError(int errnum);
int kdbPrintError(const char * msg);




/**************************************

Key methods

***************************************/

int keyInit(Key *key);
int keyClose(Key *key);

Key *keyNew(const char *keyName, ...);
int keyDel(Key *key);
#define keyFree(x) keyDel(x)

int keyIsInitialized(const Key *key);
int keyNeedsSync(const Key *key);
int keyDup(const Key *source,Key *dest);

/* int keySerialize(const Key *key,void *buffer, size_t maxSize);
int keyUnserialize(Key *key,const void *buffer); 
size_t keyGetSerializedSize(const Key *key); */

uint8_t keyGetType(const Key *key);
uint8_t keySetType(Key *key,uint8_t type);
uint8_t keySetDir(Key *key,mode_t customUmask);

int keySetFlag(Key *key);
int keyClearFlag(Key *key);
int keyGetFlag(const Key *key);

ssize_t keyGetRecordSize(const Key *key);
ssize_t keyGetNameSize(const Key *key);
ssize_t keyGetFullNameSize(const Key *key);

ssize_t keyGetName(const Key *key, char *returnedName, size_t maxSize);
char *keyStealName(const Key *key);
ssize_t keySetName(Key *key, const char *newName);

ssize_t keyGetFullName(const Key *key, char *returnedName, size_t maxSize);
ssize_t keyGetRootName(const Key *key, char *returned, size_t maxSize);
ssize_t keyGetFullRootName(const Key *key, char *returned, size_t maxSize);

ssize_t keyGetBaseName(const Key *key, char *returned, size_t maxSize);
char *keyStealBaseName(const Key *key);
ssize_t keyNameGetBaseNameSize(const char *keyName);
ssize_t keyGetBaseNameSize(const Key *key);
ssize_t keyAddBaseName(Key *key,const char *baseName);
ssize_t keySetBaseName(Key *key,const char *baseName);

char *keyNameGetOneLevel(const char *keyName, size_t *size);
ssize_t keyGetParentName(const Key *key, char *returned, size_t maxSize);
ssize_t keyGetParentNameSize(const Key *key);

ssize_t keyNameGetRootNameSize(const char *keyName);
ssize_t keyGetRootNameSize(const Key *key);

ssize_t keyNameGetFullRootNameSize(const char *keyName);
ssize_t keyGetFullRootNameSize(const Key *key);

ssize_t keyGetCommentSize(const Key *key);
ssize_t keyGetComment(const Key *key, char *returnedDesc, size_t maxSize);
char *keyStealComment(const Key *key);
ssize_t keySetComment(Key *key, const char *newDesc);

uid_t keyGetUID(const Key *key);
int keySetUID(Key *key, uid_t uid);

gid_t keyGetGID(const Key *key);
int keySetGID(Key *key, gid_t gid);

mode_t keyGetAccess(const Key *key);
int keySetAccess(Key *key, mode_t mode);
int keySetUAccess(Key *key, mode_t umask);

ssize_t keyGetOwnerSize(const Key *key);
ssize_t keyGetOwner(const Key *key, char *returned, size_t maxSize);
char *keyStealOwner(const Key *key);
ssize_t keySetOwner(Key *key, const char *userDomain);


ssize_t keyGetValueSize(const Key *key);
ssize_t keyGetDataSize(const Key *key);

ssize_t keyGetString(const Key *key, char *returnedString, size_t maxSize);
ssize_t keySetString(Key *key, const char *newString);
void *keyStealValue(const Key *key);

ssize_t keyGetBinary(const Key *key, void *returnedBinary, size_t maxSize);
ssize_t keySetBinary(Key *key, const void *newBinary, size_t dataSize);

ssize_t keySetRaw(Key *key, const void *newBinary, size_t dataSize);

ssize_t keyGetLink(const Key *key, char *returnedTarget, size_t maxSize);
ssize_t keySetLink(Key *key, const char *target);

time_t keyGetMTime(const Key *key);
time_t keyGetATime(const Key *key);
time_t keyGetCTime(const Key *key);

int keyIsSystem(const Key *key);
int keyNameIsSystem(const char *keyName);

int keyIsUser(const Key *key);
int keyNameIsUser(const char *keyName);

int keyGetNamespace(const Key *key);
int keyNameGetNamespace(const char *keyName);

int keyIsDir(const Key *key);
int keyIsLink(const Key *key);
int keyIsBin(const Key *key);
int keyIsString(const Key *key);

Key *keyNext(Key *key);

uint32_t keyCompare(const Key *key1, const Key *key2);

ssize_t keyToStream(const Key *key, FILE* stream, unsigned long options);
ssize_t keyToStreamBasename(const Key *key, FILE* stream,
	const char *parent, const size_t parentSize, unsigned long options);

/*
void *keySerialize(Key *key);
Key *keyUnserialize(void *serialized);
*/

/**************************************

KeySet methods

***************************************/

KeySet *ksNew();
int ksDel(KeySet *ks);
#define ksFree(x) ksDel(x)

int ksInit(KeySet *ks);
int ksClose(KeySet *ks);
ssize_t ksGetSize(KeySet *ks);

ssize_t ksInsert(KeySet *ks, Key *toInsert);
ssize_t ksAppend(KeySet *ks, Key *toAppend);
Key *ksPop(KeySet *ks);
Key *ksPopLast(KeySet *ks);

ssize_t ksInsertKeys(KeySet *ks, KeySet *toInsert);
ssize_t ksAppendKeys(KeySet *ks, KeySet *toAppend);

ssize_t ksToStream(const KeySet *ks, FILE* stream, unsigned long options);
ssize_t ksGetCommonParentName(const KeySet *ks,char *returnedCommonParent,
	const size_t maxSize);
int ksCompare(KeySet *ks1, KeySet *ks2, KeySet *removed);
void ksSort(KeySet *ks);

Key *ksLookupByName(KeySet *ks, const char *name,unsigned long options);
Key *ksLookupByValue(KeySet *ks, const char *value,unsigned long options);
Key *ksLookupByBinaryValue(KeySet *ks, void *value, size_t size,
	unsigned long options);
uint32_t ksLookupRE(KeySet *ks, uint32_t where,
	const regex_t *regexp, unsigned long options);
	
int ksRewind(KeySet *ks);
Key *ksNext(KeySet *ks);
Key *ksCurrent(const KeySet *ks);

Key *ksHead(KeySet *ks);
Key *ksTail(KeySet *ks);





/***************************************

Helpers

***************************************/


size_t strblen(const char *s);

#ifdef __cplusplus
}
#endif


#endif /* KDB_H */
