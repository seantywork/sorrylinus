#ifndef SOLI_HEADER_TEST
#define SOLI_HEADER_TEST

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#define MAX_TEST_UNAME_LEN 1024 *  5

void soli_test_uname(char* result);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif