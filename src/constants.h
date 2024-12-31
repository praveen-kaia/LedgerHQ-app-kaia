#pragma once

/**
 * Instruction class of the Kaia application.
 */
#define CLA 0xE0

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Maximum transaction length (bytes).
 */
#define MAX_TRANSACTION_LEN 8190

/**
 * Maximum APDU size (bytes).
 */
#define MAX_APDU_SIZE 255
/**
 * Maximum signature length (bytes).
 */
#define MAX_DER_SIG_LEN 72

/**
 * Exponent used to convert peb to KAIA unit (N KAIA = N * 10^18 kei).
 */
#define EXPONENT_SMALLEST_UNIT 18
