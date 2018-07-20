/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection */

#ifndef _Included_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
#define _Included_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    getNextTableHeaderInternal
 * Signature: (J[Ljava/lang/String;[I[Ljava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_getNextTableHeaderInternal
  (JNIEnv *, jobject, jlong, jobjectArray, jintArray, jobjectArray, jobjectArray);

/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    initializePointersInternal
 * Signature: (JJLnl/cwi/monetdb/embedded/jdbc/EmbeddedDataBlockResponse;)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_initializePointersInternal
  (JNIEnv *, jobject, jlong, jlong, jobject);

/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    sendQueryInternal
 * Signature: (JLjava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_sendQueryInternal
  (JNIEnv *, jobject, jlong, jstring, jboolean);

/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    sendAutocommitCommandInternal
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_sendAutocommitCommandInternal
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    sendReleaseCommandInternal
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_sendReleaseCommandInternal
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection
 * Method:    sendCloseCommandInternal
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_nl_cwi_monetdb_embedded_jdbc_JDBCEmbeddedConnection_sendCloseCommandInternal
  (JNIEnv *, jobject, jlong, jint);

#ifdef __cplusplus
}
#endif
#endif
