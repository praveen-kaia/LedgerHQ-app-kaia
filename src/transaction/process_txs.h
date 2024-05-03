#include "deserialize.h"

bool processTxLegacy(parser_context_t *parser_ctx);
bool processTxValueTransfer(parser_context_t *parser_ctx);
bool processTxCancel(parser_context_t *parser_ctx);
bool processTxValueTransferMemo(parser_context_t *parser_ctx);
bool processTxSmartContractDeploy(parser_context_t *parser_ctx);
bool processTxSmartContractExecution(parser_context_t *parser_ctx);