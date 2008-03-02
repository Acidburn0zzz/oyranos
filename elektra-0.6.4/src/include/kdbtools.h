/***************************************************************************
                kdbtools.h  -  Elektra High Level methods
                             -------------------
    begin                : Sat Jan 22 2005
    copyright            : (C) 2005 by Avi Alkalay
    email                : avi@unix.sh
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/


/* Subversion stuff

$Id: kdbtools.h 610 2006-02-16 03:05:24Z aviram $

*/

#ifndef KDBTOOLS_H
#define KDBTOOLS_H


#include "kdb.h"






#ifdef __cplusplus
extern "C" {
#endif


int ksFromXMLfile(KeySet *ks,const char *filename);
int ksFromXML(KeySet *ks,int fd);


#ifdef __cplusplus
}
#endif

#endif /* KDB_H */
