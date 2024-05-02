#include "../transaction/types.h"
/**
 * Formats the transaction type into a string representation.
 *
 * This function takes a transaction type enum value and converts it into a string representation.
 * The resulting string is stored in the `out` buffer.
 *
 * @param txType The transaction type enum value to be formatted.
 * @param out The buffer to store the formatted string.
 * @param out_len The length of the `out` buffer.
 * @return `true` if the transaction type was successfully formatted, `false` otherwise.
 */

bool format_transaction_type(transaction_type_e txType, char *out, size_t out_len);

/**
 * Converts a uint256 value to a decimal string representation.
 *
 * @param value The uint256 value to convert.
 * @param out The output buffer to store the decimal string representation.
 * @param out_len The length of the output buffer.
 * @return Returns true if the conversion is successful, false otherwise.
 */
bool uint256_to_decimal(uint256_t value, char *out, size_t out_len);

/**
 * Converts a uint256 value to a uint64_t value.
 *
 * @param bytes The uint256 value to convert.
 * @return Returns the converted uint64_t value.
 */
uint64_t convertUint256ToUint64(const uint256_t *bytes);

/**
 * Converts an amount represented by a uint256 value to a string representation with a specified
 * number of decimals.
 *
 * @param amount The uint256 value representing the amount.
 * @param decimals The number of decimals to include in the string representation.
 * @param out_buffer The output buffer to store the string representation.
 * @param out_buffer_size The size of the output buffer.
 * @return Returns true if the conversion is successful, false otherwise.
 */
bool ammount_to_string(const uint256_t amount,
                       uint8_t decimals,
                       char *out_buffer,
                       size_t out_buffer_size);

/**
 * Adjusts the number of decimals in a string representation of a value.
 *
 * @param src The source string representation.
 * @param srcLength The length of the source string.
 * @param target The target buffer to store the adjusted string representation.
 * @param targetLength The length of the target buffer.
 * @param decimals The number of decimals to adjust to.
 * @return Returns true if the adjustment is successful, false otherwise.
 */
bool adjustDecimals(const char *src,
                    size_t srcLength,
                    char *target,
                    size_t targetLength,
                    uint8_t decimals);

/**
 * Checks if a buffer contains all zeroes.
 *
 * @param buf The buffer to check.
 * @param n The number of bytes in the buffer.
 * @return Returns 1 if the buffer contains all zeroes, 0 otherwise.
 */
static __attribute__((no_instrument_function)) inline int allzeroes(const void *buf, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    for (size_t i = 0; i < n; ++i) {
        if (p[i]) {
            return 0;
        }
    }
    return 1;
}
