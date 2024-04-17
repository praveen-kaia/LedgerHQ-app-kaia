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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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

uint64_t convertUint256ToUint64(const uint256_t* bytes) {
    uint64_t result = 0;
    for (int i = 0; i < bytes->length && i < 8; i++) {
        result <<= 8;  // Shift existing value left by 8 bits
        result |= (uint64_t) bytes->value[i];
    }
    return result;
}