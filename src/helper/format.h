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

