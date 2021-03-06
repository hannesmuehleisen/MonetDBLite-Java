/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2008-2019 MonetDB B.V.
 */

#include "nl_cwi_monetdb_embedded_env_MonetDBEmbeddedDatabase.h"

#include "monetdb_config.h"
#include "monetdb_embedded.h"
#include "javaids.h"
#include "converters.h"
#include "gdk_posix.h"
#include "mal_exception.h"
#include "mal_linker.h"

JNIEXPORT jobject JNICALL Java_nl_cwi_monetdb_embedded_env_MonetDBEmbeddedDatabase_startDatabaseInternal
	(JNIEnv *env, jclass monetDBEmbeddedDatabase, jstring dbDirectory, jboolean silentFlag, jboolean sequentialFlag) {
	const char* dbdir_string_tmp = NULL, *loadPath_tmp = NULL;
	char *err = NULL;
	jclass exceptionCls, loaderCls = NULL;
	jfieldID pathID;
	jstring loadPath = NULL;
	jobject result = NULL;
	int i = 0, foundExc = 0;

	if(dbDirectory) {
		if(!(dbdir_string_tmp = (*env)->GetStringUTFChars(env, dbDirectory, NULL))) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		if(!strncmp(dbdir_string_tmp, ":memory:", 8)) { //activate in-memory mode
			(*env)->ReleaseStringUTFChars(env, dbDirectory, dbdir_string_tmp);
			dbdir_string_tmp = NULL;
		}
	}
	if(!monetdb_is_initialized()) {
		//initialize the java ID fields for faster Java data loading later on
		if(!initializeIDS(env)) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		//because of the dlopen stuff, this step has to be done before the monetdb_startup call
		if(!(loaderCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBJavaLiteLoader"))) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		if(!(pathID = (*env)->GetStaticFieldID(env, loaderCls, "loadedLibraryFullPath", "Ljava/lang/String;"))) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		if(!(loadPath = (jstring) (*env)->GetStaticObjectField(env, loaderCls, pathID))) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		if(!(loadPath_tmp = (*env)->GetStringUTFChars(env, loadPath, NULL))) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
			goto endofinit;
		}
		if((err = initLinker(loadPath_tmp)) != MAL_SUCCEED) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			while(err[i] && !foundExc) {
				if(err[i] == '!')
					foundExc = 1;
				i++;
			}
			(*env)->ThrowNew(env, exceptionCls, err + (foundExc ? i : 0));
			freeException(err);
			goto endofinit;
		}
		GDK_set_bin_import_swap(true);
		if ((err = monetdb_startup((char*) dbdir_string_tmp, (bool) silentFlag, (bool) sequentialFlag)) != MAL_SUCCEED) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			while(err[i] && !foundExc) {
				if(err[i] == '!')
					foundExc = 1;
				i++;
			}
			(*env)->ThrowNew(env, exceptionCls, err + (foundExc ? i : 0));
			freeException(err);
			goto endofinit;
		}
		result = (*env)->NewObject(env, monetDBEmbeddedDatabase, getMonetDBEmbeddedDatabaseConstructorID(), dbDirectory,
								   silentFlag, sequentialFlag);
		if(!result) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
		}
	} else {
		exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
		(*env)->ThrowNew(env, exceptionCls, "Only one MonetDB Embedded database is allowed per process");
	}
endofinit:
	if(dbdir_string_tmp)
		(*env)->ReleaseStringUTFChars(env, dbDirectory, dbdir_string_tmp);
	if(loadPath_tmp)
		(*env)->ReleaseStringUTFChars(env, loadPath, loadPath_tmp);
	if(loaderCls)
		(*env)->DeleteLocalRef(env, loaderCls);
	if(loadPath)
		(*env)->DeleteLocalRef(env, loadPath);
	return result;
}

JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_env_MonetDBEmbeddedDatabase_stopDatabaseInternal
	(JNIEnv *env, jobject database) {
	jclass exceptionCls;
	char *err = NULL;
	(void) env;
	(void) database;

	if(monetdb_is_initialized()) {
		if((err = monetdb_shutdown()) != MAL_SUCCEED) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, err);
			freeException(err);
		}
		//release the java ID fields
		releaseIDS(env);
	} else {
		exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
		(*env)->ThrowNew(env, exceptionCls, "The MonetDB Embedded database is not running");
	}
}

JNIEXPORT jobject JNICALL Java_nl_cwi_monetdb_embedded_env_MonetDBEmbeddedDatabase_createConnectionInternal
	(JNIEnv *env, jobject database) {
	jclass exceptionCls;
	monetdb_connection conn = NULL;
	jobject result = NULL;
	char *err = NULL;
	int i = 0, foundExc = 0;
	(void) database;

	if((err = monetdb_connect(&conn)) != MAL_SUCCEED) {
		exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
		while(err[i] && !foundExc) {
			if(err[i] == '!')
				foundExc = 1;
			i++;
		}
		(*env)->ThrowNew(env, exceptionCls, err + (foundExc ? i : 0));
		freeException(err);
	} else {
		result = (*env)->NewObject(env, getMonetDBEmbeddedConnectionClassID(),
								   getMonetDBEmbeddedConnectionConstructorID(), (jlong) conn);
		if(!result) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
		}
	}
	return result;
}

JNIEXPORT jobject JNICALL Java_nl_cwi_monetdb_embedded_env_MonetDBEmbeddedDatabase_createJDBCEmbeddedConnectionInternal
	(JNIEnv *env, jobject database) {
	jclass exceptionCls;
	monetdb_connection conn = NULL;
	jobject result = NULL;
	char *err = NULL;
	int i = 0, foundExc = 0;
	(void) database;

	if((err = monetdb_connect(&conn)) != MAL_SUCCEED) {
		exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
		while(err[i] && !foundExc) {
			if(err[i] == '!')
				foundExc = 1;
			i++;
		}
		(*env)->ThrowNew(env, exceptionCls, err + (foundExc ? i : 0));
		freeException(err);
	} else {
		result = (*env)->NewObject(env, getJDBCEmbeddedConnectionClassID(),
								   getJDBCDBEmbeddedConnectionConstructorID(), (jlong) conn);
		if(!result) {
			exceptionCls = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
			(*env)->ThrowNew(env, exceptionCls, MAL_MALLOC_FAIL);
		}
	}
	return result;
}
