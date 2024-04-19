/**
 * @file deserialize.h
 * @brief Contains functions for deserializing and parsing transactions.
 */

#pragma once

#include "buffer.h"
#include "types.h"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#define RLP_NONE 0

/**
 * @brief Enumeration of legacy RLP transaction fields.
 */
typedef enum {
    LEGACY_RLP_NONE = RLP_NONE,
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

/**
 * @brief Enumeration of cancel RLP transaction fields.
 */
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

/**
 * @brief Enumeration of transaction fee payer types.
 */
typedef enum {
    BASIC = 0,
    FEE_DELEGATED = 1,
    PARTIAL_FEE_DELEGATED = 2
} txFeePayerType_e;

/**
 * @brief Structure representing the parsing context.
 */
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
 * @brief Deserialize raw transaction into a structure.
 *
 * This function deserializes a raw transaction stored in a buffer and populates
 * the transaction structure with the parsed data.
 *
 * @param[in, out] buf Pointer to the buffer with the serialized transaction.
 * @param[out] tx Pointer to the transaction structure.
 * @return PARSING_OK if success, error status otherwise.
 */
parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx);

/**
 * @brief Parse RLP fields.
 *
 * This function parses the RLP fields of a transaction based on the provided
 * parsing context.
 *
 * @param[in, out] parsing_ctx Pointer to the parsing context.
 * @return PARSING_OK if success, error status otherwise.
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
        ((parsing_ctx.tx->txType == LEGACY && parsing_ctx.currentField == LEGACY_RLP_DONE)||                  \
        ((parsing_ctx.tx->txType == CANCEL || parsing_ctx.tx->txType == FEE_DELEGATED_CANCEL ||                 \
            parsing_ctx.tx->txType == PARTIAL_FEE_DELEGATED_CANCEL) &&                                           \
            parsing_ctx.currentField == CANCEL_RLP_DONE))

/**
 * @brief Copy transaction data to the output buffer.
 *
 * This function copies the transaction data from the parser context to the
 * output buffer.
 *
 * @param[in] parser_ctx Pointer to the parser context.
 * @param[out] out Pointer to the output buffer.
 * @param[in] length Length of the data to copy.
 * @return True if the data was copied successfully, false otherwise.
 */
bool copyTxData(parser_context_t *parser_ctx, uint8_t *out, uint32_t length);

/**
 * @brief Read a byte from the transaction buffer.
 *
 * This function reads a byte from the transaction buffer and advances the
 * position in the buffer.
 *
 * @param[in] parser_ctx Pointer to the parser context.
 * @return The byte read from the buffer.
 */
uint8_t readTxByte(parser_context_t *parser_ctx);

/**
 * @brief Decode an RLP encoded field.
 *
 * This function decodes an RLP encoded field based on the provided buffer.
 *
 * @param[in] buffer Buffer containing the RLP encoded field to decode.
 * @param[out] fieldLength Length of the RLP encoded field.
 * @param[out] offset Offset to the beginning of the RLP encoded field from the buffer.
 * @param[out] list True if the field encodes a list, false if it encodes a string.
 * @return True if the RLP header is consistent, false otherwise.
 */
bool rlpDecodeLength(uint8_t *buffer, uint32_t *fieldLength, uint32_t *offset, bool *list);

/**
 * @brief Check if an RLP encoded field can be decoded.
 *
 * This function checks if an RLP encoded field in the buffer can be decoded.
 *
 * @param[in] buffer Buffer containing the RLP encoded field.
 * @param[in] bufferLength Length of the buffer.
 * @param[out] valid True if the RLP encoded field is valid, false otherwise.
 * @return True if the RLP encoded field can be decoded, false otherwise.
 */
bool rlpCanDecode(uint8_t *buffer, uint32_t bufferLength, bool *valid);
