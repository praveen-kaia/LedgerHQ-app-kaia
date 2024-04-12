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
#include "buffer.h"

#include "deserialize.h"
#include "utils.h"
#include "types.h"

#if defined(TEST) || defined(FUZZ)
#include "assert.h"
#define LEDGER_ASSERT(x, y) assert(x)
#else
#include "ledger_assert.h"
#endif

parser_status_e transaction_deserialize(buffer_t *buf, transaction_t *tx) {
    LEDGER_ASSERT(buf != NULL, "NULL buf");
    LEDGER_ASSERT(tx != NULL, "NULL tx");

    if (buf->size > MAX_TX_LEN) {
        return WRONG_LENGTH_ERROR;
    }
    parser_context_t parsing_ctx = {
        .txType = 0,
        .currentField = RLP_NONE,
        .processingField = false,
        .commandLength = buf->size,
        .workBuffer = buf->ptr,

    };
    for(;;){
        if(PARSING_IS_DONE(parsing_ctx)){
            return PARSING_OK;
        }
        // Old style transaction (pre EIP-155). Transactions could just skip `v,r,s` so we
        // needed to cut parsing here. commandLength == 0 could happen in two cases :
        // 1. We are in an old style transaction : just return `PARSING_OK`.
        // 2. We are at the end of an APDU in a multi-apdu process. This would make us return
        // `PARSING_OK` preemptively. Case number 2 should NOT happen as it is up to
        // `ledgerjs` to correctly decrease the size of the APDU (`commandLength`) so that this
        // situation doesn't happen.
        if ((parsing_ctx.txType == LEGACY && parsing_ctx.currentField == LEGACY_RLP_CHAIN_ID) &&
            (parsing_ctx.commandLength == 0)) {
            tx->chainID.length = 0;
            return PARSING_OK;
        }
        if (parsing_ctx.commandLength == 0) {
            PRINTF("Command length done\n");
            return PARSING_PROCESSING;
        }
        if (!parsing_ctx.processingField) {
            parser_status_e status = parseRLP(&parsing_ctx);
            if (status != PARSING_CONTINUE) {
                return status;
            }
        }
    }

    // // nonce
    // if (!buffer_read_u64(buf, &tx->nonce, BE)) {
    //     return NONCE_PARSING_ERROR;
    // }
    // tx->to = (uint8_t *) (buf->ptr + buf->offset);
    // // TO address
    // if (!buffer_seek_cur(buf, ADDRESS_LEN)) {
    //     return TO_PARSING_ERROR;
    // }
    // // amount value
    // if (!buffer_read_u64(buf, &tx->value, BE)) {
    //     return VALUE_PARSING_ERROR;
    // }
    // // length of memo
    // if (!buffer_read_varint(buf, &tx->memo_len) && tx->memo_len > MAX_MEMO_LEN) {
    //     return MEMO_LENGTH_ERROR;
    // }
    // // memo
    // tx->memo = (uint8_t *) (buf->ptr + buf->offset);
    // if (!buffer_seek_cur(buf, tx->memo_len)) {
    //     return MEMO_PARSING_ERROR;
    // }
    // if (!transaction_utils_check_encoding(tx->memo, tx->memo_len)) {
    //     return MEMO_ENCODING_ERROR;
    // }


    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
uint8_t readTxByte(parser_context_t *parser_ctx) {
    uint8_t data;
    if (parser_ctx->commandLength < 1) {
        PRINTF("readTxByte Underflow\n");
        return 0; // This should throw something instead
    }
    data = *parser_ctx->workBuffer;
    parser_ctx->workBuffer++;
    parser_ctx->commandLength--;
    if (parser_ctx->processingField) {
        parser_ctx->currentFieldPos++;
    }
    return data;
}

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

static parser_status_e parseRLP(parser_context_t *parser_ctx) {
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
    // Ready to process this field
    if (offset == 0) {
        // Hack for single byte, self encoded
        parser_ctx->workBuffer--;
        parser_ctx->commandLength++;
        parser_ctx->fieldSingleByte = true;
    } else {
        parser_ctx->fieldSingleByte = false;
    }
    parser_ctx->currentFieldPos = 0;
    parser_ctx->rlpBufferPos = 0;
    parser_ctx->processingField = true;
    return PARSING_CONTINUE;
}


// Functions to process RLP fields

static bool processContent(parser_context_t *parser_ctx) {
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

static bool processAccessList(parser_context_t *parser_ctx) {
    if (!parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_ACCESS_LIST\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processType(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_TYPE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_TYPE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processChainID(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_CHAINID\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_CHAINID\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->chainID.value, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->chainID.length = parser_ctx->currentFieldLength;
        // chainID = parser_ctx->tx->chainID;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processNonce(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_NONCE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_NONCE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->nonce, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processStartGas(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_STARTGAS\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_STARTGAS %d\n", parser_ctx->currentFieldLength);
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->startgas.value + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->startgas.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

// Alias over `processStartGas()`.
static bool processGasLimit(parser_context_t *parser_ctx) {
    return processStartGas(parser_ctx);
}

static bool processGasprice(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_GASPRICE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_GASPRICE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->gasprice.value + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->gasprice.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processValue(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_VALUE\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_VALUE\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->value.value + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->tx->value.length = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processTo(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_TO\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > ADDRESS_LEN) {
        PRINTF("Invalid length for RLP_TO\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, parser_ctx->tx->to + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        // parser_ctx->tx->destinationLength = parser_ctx->currentFieldLength;
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processData(parser_context_t *parser_ctx) {
    PRINTF("PROCESS DATA\n");
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_DATA\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
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

static bool processAndDiscard(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for Discarded field\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, NULL, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

static bool processRatio(parser_context_t *parser_ctx) {
    if (parser_ctx->currentFieldIsList) {
        PRINTF("Invalid type for RLP_RATIO\n");
        return true;
    }
    if (parser_ctx->currentFieldLength > MAX_INT256) {
        PRINTF("Invalid length for RLP_RATIO\n");
        return true;
    }
    if (parser_ctx->currentFieldPos < parser_ctx->currentFieldLength) {
        uint32_t copySize =
            MIN(parser_ctx->commandLength, parser_ctx->currentFieldLength - parser_ctx->currentFieldPos);
        copyTxData(parser_ctx, &parser_ctx->tx->ratio + parser_ctx->currentFieldPos, copySize);
    }
    if (parser_ctx->currentFieldPos == parser_ctx->currentFieldLength) {
        parser_ctx->currentField++;
        parser_ctx->processingField = false;
    }
    return false;
}

