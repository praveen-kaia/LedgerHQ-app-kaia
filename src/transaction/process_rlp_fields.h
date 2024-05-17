#pragma once
#include "deserialize.h"

/**
 * @brief Processes the RLP_CONTENT field.
 *
 * This function keeps the full length for sanity checks and moves to the next field.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid for RLP_CONTENT, otherwise false.
 */
bool processContent(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_ACCESS_LIST field.
 *
 * This function checks the type of the field and copies the data if necessary.
 * It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid for RLP_ACCESS_LIST, otherwise false.
 */
bool processAccessList(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_TYPE field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_TYPE, otherwise
 * false.
 */
bool processType(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_CHAINID field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_CHAINID, otherwise
 * false.
 */
bool processChainID(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_NONCE field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_NONCE, otherwise
 * false.
 */
bool processNonce(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_STARTGAS field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_STARTGAS, otherwise
 * false.
 */
bool processStartGas(parser_context_t *parser_ctx);

/**
 * Alias over `processStartGas()`.
 *
 *  * @brief Processes the RLP_STARTGAS field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_STARTGAS, otherwise
 * false.
 */
bool processGasLimit(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_GASPRICE field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_GASPRICE, otherwise
 * false.
 */
bool processGasprice(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_VALUE field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_VALUE, otherwise
 * false.
 */
bool processValue(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_TO field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_TO, otherwise false.
 */
bool processTo(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_DATA field.
 *
 * This function checks the type of the field and copies the data if necessary.
 * It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid for RLP_DATA, otherwise false.
 */
bool processData(parser_context_t *parser_ctx);

/**
 * @brief Processes a discarded field.
 *
 * This function checks the type of the field and discards the data if necessary.
 * It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid for the discarded field, otherwise false.
 */
bool processAndDiscard(parser_context_t *parser_ctx);

/**
 * @brief Processes the RLP_RATIO field.
 *
 * This function checks the type of the field, validates the length, and copies the data if
 * necessary. It also moves to the next field when the current field is fully processed.
 *
 * @param parser_ctx The parser context.
 * @return Returns true if the type is invalid or the length is invalid for RLP_RATIO, otherwise
 * false.
 */
bool processRatio(parser_context_t *parser_ctx);
