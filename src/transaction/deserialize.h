#pragma once

#include "buffer.h"

#include "types.h"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define RLP_NONE 0
typedef enum {
    LEGACY_RLP_NONE = RLP_NONE,
    LEGACY_RLP_CONTENT,
    LEGACY_RLP_TYPE,
    LEGACY_RLP_NONCE,
    LEGACY_RLP_GASPRICE,
    LEGACY_RLP_STARTGAS,
    LEGACY_RLP_TO,
    LEGACY_RLP_VALUE,
    LEGACY_RLP_DATA,
    LEGACY_RLP_CHAIN_ID,
    LEGACY_RLP_ZERO1,
    LEGACY_RLP_ZERO2,
    LEGACY_RLP_DONE
} rlpLegacyTxField_e;

typedef enum {
    CANCEL_RLP_NONE = RLP_NONE,
    CANCEL_RLP_CONTENT,
    CANCEL_RLP_TYPE,
    CANCEL_RLP_NONCE,
    CANCEL_RLP_GASPRICE,
    CANCEL_RLP_GASLIMIT,
    CANCEL_RLP_FROM,
    CANCEL_RLP_RATIO,
    CANCEL_RLP_CHAIN_ID,
    CANCEL_RLP_ZERO1,
    CANCEL_RLP_ZERO2,
    CANCEL_RLP_DONE
} rlpCancelTxField_e;

typedef struct {
    uint8_t txType;  /// transaction type
    uint8_t currentField;  /// current field being parsed
    uint32_t currentFieldPos; /// position in the current field
    uint32_t currentFieldLength; /// length of the current field
    bool currentFieldIsList; /// flag to indicate if the current field is a list
    uint8_t processingField;  /// flag to indicate if a field is being processed
    bool fieldSingleByte;  /// flag to indicate if the field is a single byte
    bool outerRLP; /// flag to indicate if the outer RLP is being processed
    bool processingOuterRLPField; /// flag to indicate if a field in the outer RLP is being processed
    uint8_t commandLength;  /// length of the command
    uint32_t dataLength;  /// length of the data
    uint8_t rlpBuffer[5];  /// buffer to store RLP data
    uint32_t rlpBufferPos;  /// position in the RLP buffer
    const uint8_t *workBuffer; /// pointer to the buffer being parsed
    transaction_t *tx;  /// pointer to the transaction structure
    txFeePayerType_e feePayerType; /// if the transaction is basic, fee delegated or partial fee delegated
} parser_context_t;

/**
 * Deserialize raw transaction in structure.
 *
 * @param[in, out] buf
 *   Pointer to buffer with serialized transaction.
 * @param[out]     tx
 *   Pointer to transaction structure.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx);

/**
 * Parse RLP fields.
 *
 * @param[in, out] parsing_ctx
 *   Pointer to parsing context.
 *
 * @return PARSING_OK if success, error status otherwise.
 *
 */
static parser_status_e parseRLP(parser_context_t *parsing_ctx);

/**
 * @def PARSING_IS_DONE(parsing_ctx)
 * @brief Macro to check if parsing is done.
 *
 * This macro checks if parsing is complete by evaluating the current parsing context and transaction type.
 * It returns true if parsing is done, and false otherwise.
 *
 * @param parsing_ctx The parsing context.
 * @return True if parsing is done, false otherwise.
 */
#define PARSING_IS_DONE(parsing_ctx)                                                                     \
        ((parsing_ctx.txType == LEGACY && parsing_ctx.currentField == LEGACY_RLP_DONE)||                  \
        ((parsing_ctx.txType == CANCEL || parsing_ctx.txType == FEE_DELEGATED_CANCEL ||                 \
            parsing_ctx.txType == PARTIAL_FEE_DELEGATED_CANCEL) &&                                           \
            parsing_ctx.currentField == CANCEL_RLP_DONE))


uint8_t readTxByte(parser_context_t *parser_ctx);

/**
 * @brief Decode an RLP encoded field - see
 * https://github.com/ethereum/wiki/wiki/RLP
 * @param [in] buffer buffer containing the RLP encoded field to decode
 * @param [out] fieldLength length of the RLP encoded field
 * @param [out] offset offset to the beginning of the RLP encoded field from the
 * buffer
 * @param [out] list true if the field encodes a list, false if it encodes a
 * string
 * @return true if the RLP header is consistent
 */
bool rlpDecodeLength(uint8_t *buffer, uint32_t *fieldLength, uint32_t *offset, bool *list);

bool rlpCanDecode(uint8_t *buffer, uint32_t bufferLength, bool *valid);
