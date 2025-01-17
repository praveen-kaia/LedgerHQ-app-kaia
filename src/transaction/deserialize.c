/*****************************************************************************
 *   Ledger App Kaia.
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
#include <string.h>  // memmove
#include "buffer.h"

#include "constants.h"
#include "deserialize.h"
#include "utils.h"
#include "types.h"
#include "process_txs.h"
#include "process_rlp_fields.h"

#if defined(TEST)
#include "assert.h"
#include <stdio.h>  // printf
#define LEDGER_ASSERT(x, y) assert(x)
#define PRINTF              printf
#else
#include "ledger_assert.h"
#endif

#ifndef PRINTF
#endif
// RLP related

bool rlpDecodeLength(uint8_t *buffer, uint32_t *fieldLength, uint32_t *offset, bool *list) {
    if (*buffer <= 0x7f) {
        *offset = 0;
        *fieldLength = 1;
        *list = false;
    } else if (*buffer <= 0xb7) {
        *offset = 1;
        *fieldLength = *buffer - 0x80;
        *list = false;
    } else if (*buffer <= 0xbf) {
        *offset = 1 + (*buffer - 0xb7);
        *list = false;
        switch (*buffer) {
            case 0xb8:
                *fieldLength = *(buffer + 1);
                break;
            case 0xb9:
                *fieldLength = (*(buffer + 1) << 8) + *(buffer + 2);
                break;
            case 0xba:
                *fieldLength = (*(buffer + 1) << 16) + (*(buffer + 2) << 8) + *(buffer + 3);
                break;
            case 0xbb:
                *fieldLength = (*(buffer + 1) << 24) + (*(buffer + 2) << 16) +
                               (*(buffer + 3) << 8) + *(buffer + 4);
                break;
            default:
                return false;  // arbitrary 32 bits length limitation
        }
    } else if (*buffer <= 0xf7) {
        *offset = 1;
        *fieldLength = *buffer - 0xc0;
        *list = true;
    } else {
        *offset = 1 + (*buffer - 0xf7);
        *list = true;
        switch (*buffer) {
            case 0xf8:
                *fieldLength = *(buffer + 1);
                break;
            case 0xf9:
                *fieldLength = (*(buffer + 1) << 8) + *(buffer + 2);
                break;
            case 0xfa:
                *fieldLength = (*(buffer + 1) << 16) + (*(buffer + 2) << 8) + *(buffer + 3);
                break;
            case 0xfb:
                *fieldLength = (*(buffer + 1) << 24) + (*(buffer + 2) << 16) +
                               (*(buffer + 3) << 8) + *(buffer + 4);
                break;
            default:
                return false;  // arbitrary 32 bits length limitation
        }
    }

    return true;
}

bool rlpCanDecode(uint8_t *buffer, uint32_t bufferLength, bool *valid) {
    if (*buffer <= 0x7f) {
    } else if (*buffer <= 0xb7) {
    } else if (*buffer <= 0xbf) {
        if (bufferLength < (1 + (*buffer - 0xb7))) {
            return false;
        }
        if (*buffer > 0xbb) {
            *valid = false;  // arbitrary 32 bits length limitation
            return true;
        }
    } else if (*buffer <= 0xf7) {
    } else {
        if (bufferLength < (1 + (*buffer - 0xf7))) {
            return false;
        }
        if (*buffer > 0xfb) {
            *valid = false;  // arbitrary 32 bits length limitation
            return true;
        }
    }
    *valid = true;
    return true;
}

parser_status_e parseRLP(parser_context_t *parser_ctx) {
    bool canDecode = false;
    uint32_t offset;
    while (parser_ctx->commandLength != 0) {
        bool valid;
        // Feed the RLP buffer until the length can be decoded
        parser_ctx->rlpBuffer[parser_ctx->rlpBufferPos++] = readTxByte(parser_ctx);
        if (rlpCanDecode(parser_ctx->rlpBuffer, parser_ctx->rlpBufferPos, &valid)) {
            // Can decode now, if valid
            if (!valid) {
                PRINTF("RLP pre-decode error\n");
                return PARSING_ERROR;
            }
            canDecode = true;
            break;
        }
        // Cannot decode yet
        // Sanity check
        if (parser_ctx->rlpBufferPos == sizeof(parser_ctx->rlpBuffer)) {
            PRINTF("RLP pre-decode logic error\n");
            return PARSING_ERROR;
        }
    }
    if (!canDecode) {
        PRINTF("Can't decode\n");
        return PARSING_PROCESSING;
    }
    // Ready to process this field
    if (!rlpDecodeLength(parser_ctx->rlpBuffer,
                         &parser_ctx->currentFieldLength,
                         &offset,
                         &parser_ctx->currentFieldIsList)) {
        PRINTF("RLP decode error\n");
        return PARSING_ERROR;
    }
    if (offset == 0) {
        // Hack for single byte, self encoded
        parser_ctx->workBuffer--;
        parser_ctx->commandLength++;
        parser_ctx->fieldSingleByte = true;
    } else {
        parser_ctx->fieldSingleByte = false;
    }
    parser_ctx->rlpBufferPos = 0;

    if (parser_ctx->outerRLP) {
        parser_ctx->processingOuterRLPField = true;
    } else {
        parser_ctx->currentFieldPos = 0;
        parser_ctx->processingField = true;
    }
    return PARSING_CONTINUE;
}

static void parseNestedRlp(parser_context_t *parser_ctx) {
    parseRLP(parser_ctx);
    parseRLP(parser_ctx);
    parser_ctx->outerRLP = false;
}

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    if (buf->size > MAX_TRANSACTION_LEN) {
        return WRONG_LENGTH_ERROR;
    }
    parser_context_t parser_ctx = {
        .currentField = RLP_NONE + 1,
        .processingField = false,
        .commandLength = buf->size,
        .workBuffer = buf->ptr,
        .outerRLP = true,
        .tx = tx,
    };
    for (;;) {
        if (PARSING_IS_DONE(parser_ctx)) {
            return PARSING_OK;
        }
        // Old style transaction (pre EIP-155). Transactions could just skip `v,r,s` so we
        // needed to cut parsing here. commandLength == 0 could happen in two cases :
        // 1. We are in an old style transaction : just return `PARSING_OK`.
        // 2. We are at the end of an APDU in a multi-apdu process. This would make us return
        // `PARSING_OK` preemptively. Case number 2 should NOT happen as it is up to
        // `ledgerjs` to correctly decrease the size of the APDU (`commandLength`) so that this
        // situation doesn't happen.
        if ((parser_ctx.tx->txType == LEGACY && parser_ctx.currentField == LEGACY_RLP_CHAIN_ID) &&
            (parser_ctx.commandLength == 0)) {
            tx->chainID.length = 0;
            return PARSING_OK;
        }
        if (parser_ctx.commandLength == 0) {
            PRINTF("Command length done\n");
            return PARSING_PROCESSING;
        }
        if (parser_ctx.outerRLP && !parser_ctx.processingOuterRLPField) {
            parseNestedRlp(&parser_ctx);
            // Hack to detect the tx type
            // If the last field parsed was a fieldSingleByte it means the transaction is a Legacy
            // transaction
            if (parser_ctx.fieldSingleByte) {
                parser_ctx.tx->txType = LEGACY;
            } else {
                // Save commandLength before calling parseRLP
                uint32_t rlpStartCommandLength = parser_ctx.commandLength;
                // The byte after the nested rlp is the tx type,
                parseRLP(&parser_ctx);
                parser_ctx.tx->txType = parser_ctx.workBuffer[0];
                // Return to the start of the nested RLP
                parser_ctx.workBuffer -= rlpStartCommandLength - parser_ctx.commandLength;
                parser_ctx.commandLength += rlpStartCommandLength - parser_ctx.commandLength;
                parser_ctx.processingField = false;
            }
            PRINTF("Transaction type: %d\n", parser_ctx.tx->txType);

            continue;
        }
        if (!parser_ctx.processingField) {
            parser_status_e status = parseRLP(&parser_ctx);
            if (status != PARSING_CONTINUE) {
                return status;
            }
        }

        PRINTF("Current field: %d\n", parser_ctx.currentField);
        switch (parser_ctx.tx->txType) {
            bool fault;
            case LEGACY:
                fault = processTxLegacy(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            case VALUE_TRANSFER:
            case FEE_DELEGATED_VALUE_TRANSFER:
            case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER:
                fault = processTxValueTransfer(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            case VALUE_TRANSFER_MEMO:
            case FEE_DELEGATED_VALUE_TRANSFER_MEMO:
            case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO:
                fault = processTxValueTransferMemo(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            case SMART_CONTRACT_DEPLOY:
            case FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
            case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
                fault = processTxSmartContractDeploy(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            case SMART_CONTRACT_EXECUTION:
            case FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
            case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
                fault = processTxSmartContractExecution(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            case CANCEL:
            case FEE_DELEGATED_CANCEL:
            case PARTIAL_FEE_DELEGATED_CANCEL:
                fault = processTxCancel(&parser_ctx);
                if (fault) {
                    return PARSING_ERROR;
                }
                break;
            default:
                PRINTF("Transaction type %d is not supported\n", parser_ctx.tx->txType);
                return PARSING_ERROR;
        }
    }
}

uint8_t readTxByte(parser_context_t *parser_ctx) {
    uint8_t data;
    if (parser_ctx->commandLength < 1) {
        PRINTF("readTxByte Underflow\n");
        return 0;  // This should throw something instead
    }
    data = *parser_ctx->workBuffer;
    parser_ctx->workBuffer++;
    parser_ctx->commandLength--;
    if (parser_ctx->processingField) {
        parser_ctx->currentFieldPos++;
    }
    return data;
}

bool copyTxData(parser_context_t *parser_ctx, uint8_t *out, uint32_t length) {
    if (parser_ctx->commandLength < length) {
        PRINTF("copyTxData Underflow\n");
        return false;
    }
    if (out != NULL) {
        memmove(out, parser_ctx->workBuffer, length);
    }
    parser_ctx->workBuffer += length;
    parser_ctx->commandLength -= length;
    if (parser_ctx->processingField) {
        parser_ctx->currentFieldPos += length;
    }
    return true;
}
