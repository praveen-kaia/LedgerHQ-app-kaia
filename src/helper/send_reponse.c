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

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove
#include <stdio.h>  // PRINTF

#include "buffer.h"
#include "cx.h"

#include "send_response.h"
#include "../constants.h"
#include "../globals.h"
#include "../address.h"
#include "../sw.h"

#include "../transaction/utils.h"


static const char HEXDIGITS[] = "0123456789abcdef";

void getEthAddressStringFromBinary(uint8_t *address, char *out, cx_sha3_t *sha3Context) {
    // save some precious stack space
    union locals_union {
        uint8_t hashChecksum[HASH_LENGTH];
        uint8_t tmp[51];
    } locals_union;

    uint8_t i;
    uint32_t offset = 0;

    for (i = 0; i < 20; i++) {
        uint8_t digit = address[i];
        locals_union.tmp[offset + 2 * i] = HEXDIGITS[(digit >> 4) & 0x0f];
        locals_union.tmp[offset + 2 * i + 1] = HEXDIGITS[digit & 0x0f];
    }

    CX_THROW(cx_keccak_init_no_throw(sha3Context, 256));

    CX_THROW(cx_hash_no_throw((cx_hash_t *) sha3Context,
                              CX_LAST,
                              locals_union.tmp,
                              offset + 40,
                              locals_union.hashChecksum,
                              32));

    for (i = 0; i < 40; i++) {
        uint8_t digit = address[i / 2];
        if ((i % 2) == 0) {
            digit = (digit >> 4) & 0x0f;
        } else {
            digit = digit & 0x0f;
        }
        if (digit < 10) {
            out[i] = HEXDIGITS[digit];
        } else {
            int v = (locals_union.hashChecksum[i / 2] >> (4 * (1 - i % 2))) & 0x0f;
            if (v >= 8) {
                out[i] = HEXDIGITS[digit] - 'a' + 'A';
            } else {
                out[i] = HEXDIGITS[digit];
            }
        }
    }
    out[40] = '\0';
}

void getEthAddressStringFromKey(uint8_t *publicKey,
                                char *out,
                                cx_sha3_t *sha3Context) {
    uint8_t hashAddress[HASH_LENGTH];

    CX_THROW(cx_keccak_init_no_throw(sha3Context, 256));

    CX_THROW(cx_hash_no_throw((cx_hash_t *) sha3Context,
                              CX_LAST,
                              publicKey + 1,
                              64,
                              hashAddress,
                              32));

    getEthAddressStringFromBinary(hashAddress + 12, out, sha3Context);
}

int helper_send_response_pubkey() {
    uint8_t resp[1 + PUBKEY_LEN + 1 + ADDRESS_IN_ASCII_HEX_LEN + 1 + CHAINCODE_LEN] = {0};
    size_t offset = 0;

    resp[offset++] = PUBKEY_LEN;
    memmove(resp + offset, G_context.pk_info.raw_public_key, PUBKEY_LEN);
    offset += PUBKEY_LEN;

    resp[offset++] = ADDRESS_IN_ASCII_HEX_LEN;
    cx_sha3_t sha3;
    getEthAddressStringFromKey(G_context.pk_info.raw_public_key, (char *) resp + offset, &sha3);    
    offset += ADDRESS_IN_ASCII_HEX_LEN;

    resp[offset++] = CHAINCODE_LEN;
    memmove(resp + offset, G_context.pk_info.chain_code, CHAINCODE_LEN);
    offset += CHAINCODE_LEN;

    return io_send_response_pointer(resp, offset, SW_OK);
}


void format_signature_out(const uint8_t *signature, uint8_t *out) {
    uint8_t offset = 1;
    uint8_t xoffset = 4;  // point to r value
    // copy r
    uint8_t xlength = signature[xoffset - 1];
    if (xlength == 33) {
        xlength = 32;
        xoffset++;
    }
    memmove(out + offset + 32 - xlength, signature + xoffset, xlength);
    offset += 32;
    xoffset += xlength + 2;  // move over rvalue and TagLEn
    // copy s value
    xlength = signature[xoffset - 1];
    if (xlength == 33) {
        xlength = 32;
        xoffset++;
    }
    memmove(out + offset + 32 - xlength, signature + xoffset, xlength);
}

int helper_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    uint32_t v_out = u32_from_BE(G_context.tx_info.transaction.chainID.value, MIN(4, G_context.tx_info.transaction.chainID.length));
    resp[offset++] = (v_out * 2) + 35 + G_context.tx_info.v;
    
    format_signature_out(G_context.tx_info.signature, resp);
    PRINTF("Signature out: %.*H\n", 64, resp + 1);

    return io_send_response_pointer(resp, 65, SW_OK);
}
