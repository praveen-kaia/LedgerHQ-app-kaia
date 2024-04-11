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
    };
    for(;;){
        if(PARSING_IS_DONE(parsing_ctx)){
            return PARSING_OK;
        }
        if ((parsing_ctx.txType == LEGACY && parsing_ctx.currentField == LEGACY_RLP_CHAIN_ID) &&
            (buf->offset == buf->size)) {
            tx->chainID.length = 0;
            return PARSING_OK;
        }
        if (buf->offset == buf->size) {
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
        CX_THROW(0x9999);
    }
    data = *parser_ctx->workBuffer;
    parser_ctx->workBuffer++;
    parser_ctx->commandLength--;
    if (parser_ctx->processingField) {
        parser_ctx->currentFieldPos++;
    }
    return data;
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
