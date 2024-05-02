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
#include "deserialize.h"
#include "process_txs.h"
#include "process_rlp_fields.h"
#include "utils.h"
#include "types.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#include <stdio.h>  // printf
#define LEDGER_ASSERT(x, y) assert(x)
#define PRINTF printf
#else
#include "ledger_assert.h"
#endif

bool processTxLegacy(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
        case LEGACY_RLP_NONCE:
            error = processNonce(parser_ctx);
            break;
        case LEGACY_RLP_GASPRICE:
            error = processGasprice(parser_ctx);
            break;
        case LEGACY_RLP_STARTGAS:
            error = processGasLimit(parser_ctx);
            break;
        case LEGACY_RLP_TO:
            error = processTo(parser_ctx);
            break;
        case LEGACY_RLP_VALUE:
            error = processValue(parser_ctx);
            break;
        case LEGACY_RLP_DATA:
            error = processData(parser_ctx);
            break;
        case LEGACY_RLP_CHAIN_ID:
            error = processChainID(parser_ctx);
            break;
        case LEGACY_RLP_ZERO1:
        case LEGACY_RLP_ZERO2:
            error = processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;

}

bool processTxValueTransfer(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
         case VALUE_TRANSFER_RLP_CONTENT:
            processContent(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_TYPE:
            processType(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_NONCE:
            processNonce(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_GASPRICE:
            processGasprice(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_GASLIMIT:
            processGasLimit(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_TO:
            processTo(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_VALUE:
            processValue(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_FROM:
            processAndDiscard(parser_ctx);
            // Skip ratio if not partial fee delegated txType
            if (parser_ctx->tx->txType != PARTIAL_FEE_DELEGATED_VALUE_TRANSFER) {
                parser_ctx->currentField++;
            }
            break;
        case VALUE_TRANSFER_RLP_RATIO:
            processRatio(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_CHAIN_ID:
            processChainID(parser_ctx);
            break;
        case VALUE_TRANSFER_RLP_ZERO1:
        case VALUE_TRANSFER_RLP_ZERO2:
            processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;
}

bool processTxValueTransferMemo(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
        case VALUE_TRANSFER_MEMO_RLP_CONTENT:
            processContent(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_TYPE:
            processType(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_NONCE:
            processNonce(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_GASPRICE:
            processGasprice(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_GASLIMIT:
            processGasLimit(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_TO:
            processTo(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_VALUE:
            processValue(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_FROM:
            processAndDiscard(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_DATA:
            processData(parser_ctx);
            // Skip ratio if not partial fee delegated txType
            if (parser_ctx->tx->txType != PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO) {
                parser_ctx->currentField++;
            }
            break;
        case VALUE_TRANSFER_MEMO_RLP_RATIO:
            processRatio(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_CHAIN_ID:
            processChainID(parser_ctx);
            break;
        case VALUE_TRANSFER_MEMO_RLP_ZERO1:
        case VALUE_TRANSFER_MEMO_RLP_ZERO2:
            processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;
}

bool processTxSmartContractDeploy(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
        case SMART_CONTRACT_DEPLOY_RLP_CONTENT:
            processContent(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_TYPE:
            processType(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_NONCE:
            processNonce(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_GASPRICE:
            processGasprice(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_GASLIMIT:
            processGasLimit(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_TO:
            processAndDiscard(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_VALUE:
            processValue(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_FROM:
            processAndDiscard(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_DATA:
            processData(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_HUMAN_READABLE:
            processAndDiscard(parser_ctx);
            // Skip ratio if not partial fee delegated txType
            if (parser_ctx->tx->txType != PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY) {
                parser_ctx->currentField++;
            }
            break;
        case SMART_CONTRACT_DEPLOY_RLP_RATIO:
            processRatio(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_CODE_FORMAT:
            processAndDiscard(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_CHAIN_ID:
            processChainID(parser_ctx);
            break;
        case SMART_CONTRACT_DEPLOY_RLP_ZERO1:
        case SMART_CONTRACT_DEPLOY_RLP_ZERO2:
            processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;
}

bool processTxSmartContractExecution(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
        case SMART_CONTRACT_EXECUTION_RLP_CONTENT:
            processContent(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_TYPE:
            processType(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_NONCE:
            processNonce(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_GASPRICE:
            processGasprice(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_GASLIMIT:
            processGasLimit(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_TO:
            processTo(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_VALUE:
            processValue(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_FROM:
            processAndDiscard(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_DATA:
            processData(parser_ctx);
            // Skip ratio if not partial fee delegated txType
            if (parser_ctx->tx->txType != PARTIAL_FEE_DELEGATED_CANCEL) {
                parser_ctx->currentField++;
            }
            break;
        case SMART_CONTRACT_EXECUTION_RLP_RATIO:
            processRatio(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_CHAIN_ID:
            processChainID(parser_ctx);
            break;
        case SMART_CONTRACT_EXECUTION_RLP_ZERO1:
        case SMART_CONTRACT_EXECUTION_RLP_ZERO2:
            processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;
}

bool processTxCancel(parser_context_t *parser_ctx) {
    bool error = false;
    switch (parser_ctx->currentField) {
        case CANCEL_RLP_CONTENT:
            error = processContent(parser_ctx);
            break;
        case CANCEL_RLP_TYPE:
            error = processType(parser_ctx);
            break;
        case CANCEL_RLP_NONCE:
            error = processNonce(parser_ctx);
            break;
        case CANCEL_RLP_GASPRICE:
            error = processGasprice(parser_ctx);
            break;
        case CANCEL_RLP_GASLIMIT:
            error = processGasLimit(parser_ctx);
            break;
        case CANCEL_RLP_FROM:
            error = processAndDiscard(parser_ctx);
            // Skip ratio if not partial fee delegated txType
            if (parser_ctx->tx->txType != PARTIAL_FEE_DELEGATED_CANCEL) {
                parser_ctx->currentField++;
            }
            break;
        case CANCEL_RLP_RATIO:
            error = processRatio(parser_ctx);
            break;
        case CANCEL_RLP_CHAIN_ID:
            error = processChainID(parser_ctx);
            break;
        case CANCEL_RLP_ZERO1:
        case CANCEL_RLP_ZERO2:
            error = processAndDiscard(parser_ctx);
            break;
        default:
            PRINTF("Invalid RLP decoder parser_ctx\n");
            return true;
    }
    return error;
}