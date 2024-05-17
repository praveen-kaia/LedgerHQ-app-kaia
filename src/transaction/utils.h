#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#include "types.h"

/**
 * Checks if the given memo is encoded correctly.
 *
 * @param memo The input memo as a pointer to a uint8_t array.
 * @param memo_len The length of the input memo.
 * @return Returns true if the memo is encoded correctly, false otherwise.
 */
bool transaction_utils_check_encoding(const uint8_t *memo, uint64_t memo_len);

/**
 * Formats the given memo into a null-terminated string.
 *
 * @param memo The input memo as a pointer to a uint8_t array.
 * @param memo_len The length of the input memo.
 * @param dst The output buffer to store the formatted memo as a null-terminated string.
 * @param dst_len The length of the output buffer.
 * @return Returns true if the memo is successfully formatted, false otherwise.
 */
bool transaction_utils_format_memo(const uint8_t *memo,
                                   uint64_t memo_len,
                                   char *dst,
                                   uint64_t dst_len);

/**
 * Converts a big-endian byte array to a 32-bit unsigned integer.
 *
 * @param in The input byte array pointer.
 * @param size The size of the input byte buffer.
 * @return The converted 32-bit unsigned integer.
 */

uint32_t u32_from_BE(const uint8_t *in, uint8_t size);
