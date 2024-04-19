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

#include <stdbool.h> // bool
#include <stddef.h> // size_t
#include <stdint.h> // uint*_t
#include <string.h> // memcpy
#include <stdio.h> // snprintf

#include "format.h"
#include "../transaction/types.h"


bool format_transaction_type(transaction_type_e txType, char *out, size_t out_len) {
    switch (txType) {
        case VALUE_TRANSFER:
            if(snprintf(out, out_len, "VALUE_TRANSFER")){
                return false;
            };
            break;
        case FEE_DELEGATED_VALUE_TRANSFER:
            if(snprintf(out, out_len, "FEE_DELEGATED_VALUE_TRANSFER")){
                return false;
            };
            break;
        case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER:
            if(snprintf(out, out_len, "PARTIAL_FEE_DELEGATED_VALUE_TRANSFER")){
                return false;
            };
            break;
        case VALUE_TRANSFER_MEMO:
            if(snprintf(out, out_len, "VALUE_TRANSFER_MEMO")){
                return false;
            };
            break;
        case FEE_DELEGATED_VALUE_TRANSFER_MEMO:
            if(snprintf(out, out_len, "FEE_DELEGATED_VALUE_TRANSFER_MEMO")){
                return false;
            };
            break;
        case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO:
            if(snprintf(out, out_len, "PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO")){
                return false;
            };
            break;
        case SMART_CONTRACT_DEPLOY:
            if(snprintf(out, out_len, "SMART_CONTRACT_DEPLOY")){
                return false;
            };
            break;
        case FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
            if(snprintf(out, out_len, "FEE_DELEGATED_SMART_CONTRACT_DEPLOY")){
                return false;
            };
            break;
        case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
            if(snprintf(out, out_len, "PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY")){
                return false;
            };
            break;
        case SMART_CONTRACT_EXECUTION:
            if(snprintf(out, out_len, "SMART_CONTRACT_EXECUTION")){
                return false;
            };
            break;
        case FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
            if(snprintf(out, out_len, "FEE_DELEGATED_SMART_CONTRACT_EXECUTION")){
                return false;
            };
            break;
        case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
            if(snprintf(out, out_len, "PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION")){
                return false;
            };
            break;
        case CANCEL:
            if(snprintf(out, out_len, "CANCEL")){
                return false;
            };
            break;
        case FEE_DELEGATED_CANCEL:
            if(snprintf(out, out_len, "FEE_DELEGATED_CANCEL")){
                return false;
            };
            break;
        case PARTIAL_FEE_DELEGATED_CANCEL:
            if(snprintf(out, out_len, "PARTIAL_FEE_DELEGATED_CANCEL")){
                return false;
            };
            break;
        case LEGACY:
            if(snprintf(out, out_len, "LEGACY")){
                return false;
            };
            break;
        default:
            return false;
    }
    return true;
}

bool ammount_to_string(const uint256_t amount,
                                  uint8_t decimals,
                                  char *out_buffer,
                                  size_t out_buffer_size) {
    char tmp_buffer[100] = {0};

    if (uint256_to_decimal(amount, tmp_buffer, sizeof(tmp_buffer)) == false) {
        return false;
    }

    uint8_t amount_len = strnlen(tmp_buffer, sizeof(tmp_buffer));
    uint32_t copySize = out_buffer_size;

    if (adjustDecimals(tmp_buffer,
                       amount_len,
                       out_buffer ,
                       out_buffer_size  - 1,
                       decimals) == false) {
        return false;
    }

    out_buffer[out_buffer_size - 1] = '\0';
    return true;
};

bool adjustDecimals(const char *src,
                                  size_t srcLength,
                                  char *target,
                                  size_t targetLength,
                                  uint8_t decimals) {
    uint32_t startOffset;
    uint32_t lastZeroOffset = 0;
    uint32_t offset = 0;
    if ((srcLength == 1) && (*src == '0')) {
        if (targetLength < 2) {
            return false;
        }
        target[0] = '0';
        target[1] = '\0';
        return true;
    }
    if (srcLength <= decimals) {
        uint32_t delta = decimals - srcLength;
        if (targetLength < srcLength + 1 + 2 + delta) {
            return false;
        }
        target[offset++] = '0';
        target[offset++] = '.';
        for (uint32_t i = 0; i < delta; i++) {
            target[offset++] = '0';
        }
        startOffset = offset;
        for (uint32_t i = 0; i < srcLength; i++) {
            target[offset++] = src[i];
        }
        target[offset] = '\0';
    } else {
        uint32_t sourceOffset = 0;
        uint32_t delta = srcLength - decimals;
        if (targetLength < srcLength + 1 + 1) {
            return false;
        }
        while (offset < delta) {
            target[offset++] = src[sourceOffset++];
        }
        if (decimals != 0) {
            target[offset++] = '.';
        }
        startOffset = offset;
        while (sourceOffset < srcLength) {
            target[offset++] = src[sourceOffset++];
        }
        target[offset] = '\0';
    }
    for (uint32_t i = startOffset; i < offset; i++) {
        if (target[i] == '0') {
            if (lastZeroOffset == 0) {
                lastZeroOffset = i;
            }
        } else {
            lastZeroOffset = 0;
        }
    }
    if (lastZeroOffset != 0) {
        target[lastZeroOffset] = '\0';
        if (target[lastZeroOffset - 1] == '.') {
            target[lastZeroOffset - 1] = '\0';
        }
    }
    return true;
}

bool uint256_to_decimal(const uint256_t value, char *out, size_t out_len) {
    if (value.length > MAX_INT256) {
        // value length is bigger than MAX_INT256 ?!
        return false;
    }

    uint16_t n[16] = {0};
    // Copy and right-align the number
    memcpy((uint8_t *) n + MAX_INT256 - value.length, value.value, value.length);

    // Special case when value is 0
    if (allzeroes(n, MAX_INT256)) {
        if (out_len < 2) {
            // Not enough space to hold "0" and \0.
            return false;
        }
        strlcpy(out, "0", out_len);
        return true;
    }

    uint16_t *p = n;
    for (int i = 0; i < 16; i++) {
        n[i] = __builtin_bswap16(*p++);
    }
    int pos = out_len;
    while (!allzeroes(n, sizeof(n))) {
        if (pos == 0) {
            return false;
        }
        pos -= 1;
        unsigned int carry = 0;
        for (int i = 0; i < 16; i++) {
            int rem = ((carry << 16) | n[i]) % 10;
            n[i] = ((carry << 16) | n[i]) / 10;
            carry = rem;
        }
        out[pos] = '0' + carry;
    }
    memmove(out, out + pos, out_len - pos);
    out[out_len - pos] = 0;
    return true;
}

uint64_t convertUint256ToUint64(const uint256_t* bytes) {
    uint64_t result = 0;
    for (int i = 0; i < bytes->length && i < 8; i++) {
        result <<= 8;  // Shift existing value left by 8 bits
        result |= (uint64_t) bytes->value[i];
    }
    return result;
}