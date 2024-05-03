/*****************************************************************************
 *   Ledger App Klaytn.
 *   (c) 2024 Blooo SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include "process_rlp_fields.h"
#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#include <stdio.h>  // printf
#define LEDGER_ASSERT(x, y) assert(x)
#define PRINTF              printf
#else
#include "ledger_assert.h"
#endif

bool processContent(parser_context_t *parser_ctx) {
    // Keep the full length for sanity checks, move to the next field
    if (!parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_CONTENT\n");
        return true;
    }
    parser_ctx->dataLength = parser_ctx->currentFieldLength;
    parser_ctx->currentField++;
    parser_ctx->processingField = false;
    return false;
}

bool processAccessList(parser_context_t *parser_ctx) {
    if (!parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_ACCESS_LIST\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processType(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_TYPE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_TYPE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, (uint8_t *) &parser_ctx->tx->txType, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processChainID(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_CHAINID\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_CHAINID\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->chainID.value, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->chainID.length = parser_ctx->currentFieldLength;

        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processNonce(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_NONCE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_NONCE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->nonce.value, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->nonce.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processStartGas(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_STARTGAS\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_STARTGAS %d\n", parser_ctx->currentFieldLength);
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx,
                   parser_ctx->tx->startgas.value + parser_ctx->currentFieldPos,
                   copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->startgas.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

// Alias over `processStartGas()`.
bool processGasLimit(parser_context_t *parser_ctx) {
    return processStartGas(parser_ctx);
}

bool processGasprice(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_GASPRICE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_GASPRICE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx,
                   parser_ctx->tx->gasprice.value + parser_ctx->currentFieldPos,
                   copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->gasprice.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processValue(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_VALUE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_VALUE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->value.value + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->value.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processTo(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_TO\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > ADDRESS_LEN) {
        PRINTF("Invalid length for RLP_TO\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->to + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processData(parser_context_t *parser_ctx) {
    PRINTF("PROCESS DATA\n");
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_DATA\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        // If there is no data, set dataPresent to false.
        if (copySize == 1 && *parser_ctx->workBuffer == 0x00) {
            parser_ctx->tx->dataPresent = false;
        }
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        PRINTF("incrementing field\n");
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processAndDiscard(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for Discarded field\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

bool processRatio(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_RATIO\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_RATIO\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize = MIN(parser_ctx->commandLength,
                                parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, &parser_ctx->tx->ratio + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}