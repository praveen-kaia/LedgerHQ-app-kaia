#include "deserialize.h"

/**
 * Processes the legacy transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxLegacy(parser_context_t *parser_ctx);

/**
 * Processes the value transfer transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxValueTransfer(parser_context_t *parser_ctx);

/**
 * Processes the value transfer with memo transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxValueTransferMemo(parser_context_t *parser_ctx);

/**
 * Processes the smart contract deployment transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxSmartContractDeploy(parser_context_t *parser_ctx);

/**
 * Processes the smart contract execution transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxSmartContractExecution(parser_context_t *parser_ctx);

/**
 * Processes the cancel transaction fields.
 *
 * @param parser_ctx The parser context.
 * @return True if there is an error, false otherwise.
 */
bool processTxCancel(parser_context_t *parser_ctx);
