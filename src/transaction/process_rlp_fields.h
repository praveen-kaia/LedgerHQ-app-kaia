#pragma once
#include "deserialize.h"

bool processContent(parser_context_t *parser_ctx);
bool processAccessList(parser_context_t *parser_ctx);
bool processType(parser_context_t *parser_ctx);
bool processChainID(parser_context_t *parser_ctx);
bool processNonce(parser_context_t *parser_ctx);
bool processStartGas(parser_context_t *parser_ctx);
bool processGasLimit(parser_context_t *parser_ctx);
bool processGasprice(parser_context_t *parser_ctx);
bool processValue(parser_context_t *parser_ctx);
bool processTo(parser_context_t *parser_ctx);
bool processData(parser_context_t *parser_ctx);
bool processAndDiscard(parser_context_t *parser_ctx);
bool processRatio(parser_context_t *parser_ctx);