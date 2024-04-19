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

#ifdef HAVE_BAGL

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "ux.h"
#include "glyphs.h"
#include "io.h"
#include "bip32.h"
#include "format.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "../sw.h"
#include "../address.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../helper/format.h"
#include "../menu.h"

static action_validate_cb g_validate_callback;
static char g_type[50];
static char g_nonce[30];
static char g_gasPrice[30];
static char g_gasLimit[30];
static char g_to[43];
static char g_address[43];
static char g_feeRatio[30];
static char g_amount[50];

// Validate/Invalidate public key and go back to home
static void ui_action_validate_pubkey(bool choice) {
    validate_pubkey(choice);
    ui_menu_main();
}

// Validate/Invalidate transaction and go back to home
static void ui_action_validate_transaction(bool choice) {
    validate_transaction(choice);
    ui_menu_main();
}

// Step with icon and text
UX_STEP_NOCB(ux_display_confirm_addr_step, pn, {&C_icon_eye, "Confirm Address"});
// Step with title/text for address
UX_STEP_NOCB(ux_display_address_step,
             bnnn_paging,
             {
                 .title = "Address",
                 .text = g_address,
             });
// Step with approve button
UX_STEP_CB(ux_display_approve_step,
           pb,
           (*g_validate_callback)(true),
           {
               &C_icon_validate_14,
               "Approve",
           });
// Step with reject button
UX_STEP_CB(ux_display_reject_step,
           pb,
           (*g_validate_callback)(false),
           {
               &C_icon_crossmark,
               "Reject",
           });

// FLOW to display address:
// #1 screen: eye icon + "Confirm Address"
// #2 screen: display address
// #3 screen: approve button
// #4 screen: reject button
UX_FLOW(ux_display_pubkey_flow,
        &ux_display_confirm_addr_step,
        &ux_display_address_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_address() {
    if (G_context.req_type != CONFIRM_ADDRESS || G_context.state != STATE_NONE) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }
    memset(g_address, 0, sizeof(g_address));
    uint8_t address[ADDRESS_LEN] = {0};
    if (!address_from_pubkey(G_context.pk_info.raw_public_key, address, sizeof(address))) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    if (format_hex(address, sizeof(address), g_address, sizeof(g_address)) == -1) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    g_validate_callback = &ui_action_validate_pubkey;

    ux_flow_init(0, ux_display_pubkey_flow, NULL);
    return 0;
}

// Step with icon and text
UX_STEP_NOCB(ux_display_review_step,
             pnn,
             {
                 &C_icon_eye,
                 "Review",
                 "Transaction",
             });


// Step with title/text for transaction type
UX_STEP_NOCB(ux_display_type_step,
             bnnn_paging,
             {
                 .title = "Type",
                 .text = g_type,
             });
// Step with title/text for nonce
UX_STEP_NOCB(ux_display_nonce_step,
             bnnn_paging,
             {
                 .title = "Nonce",
                 .text = g_nonce,
             });


// Step with title/text for gas price
UX_STEP_NOCB(ux_display_gas_price_step,
             bnnn_paging,
             {
                 .title = "Gas Price",
                 .text = g_gasPrice,
             });
// Step with title/text for gas limit
UX_STEP_NOCB(ux_display_gas_limit_step,
             bnnn_paging,
             {
                 .title = "Gas Limit",
                 .text = g_gasLimit,
             });
// Step with title/text for destination address
UX_STEP_NOCB(ux_display_to_step,
             bnnn_paging,
             {
                 .title = "To",
                 .text = g_to,
             });

// Step with title/text for Smart Contract
UX_STEP_NOCB(ux_display_smart_contract_step,
             bnnn_paging,
             {
                 .title = "Smart Contract",
                 .text = g_to,
             });
// Step with title/text for fee ratio
UX_STEP_NOCB(ux_display_fee_ratio_step,
             bnnn_paging,
             {
                 .title = "Fee Ratio",
                 .text = g_feeRatio,
             });
// Step with title/text for amount
UX_STEP_NOCB(ux_display_amount_step,
             bnnn_paging,
             {
                 .title = "Amount",
                 .text = g_amount,
             });

// FLOW to display transaction information:
// #1 screen: eye icon + "Review Transaction"
// #2 screen: display transaction type
// #3 screen: display nonce
// #4 screen: display gas price
// #5 screen: display gas limit
// #6 screen: display destination/smart contract address (not always present)
// #7 screen: display fee ratio (not always present)
// #8 screen: display amount (not always present)
// #9 screen: approve button
// #10 screen: reject button
UX_FLOW(ux_display_transaction_flow,
        &ux_display_review_step,
        &ux_display_type_step,
        &ux_display_nonce_step,
        &ux_display_gas_price_step,
        &ux_display_gas_limit_step,
        &ux_display_to_step, // or ux_display_smart_contract_step
        &ux_display_fee_ratio_step,
        &ux_display_amount_step,
        &ux_display_approve_step,
        &ux_display_reject_step);

int ui_display_transaction() {
    if (G_context.req_type != CONFIRM_TRANSACTION || G_context.state != STATE_PARSED) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    memset(g_type, 0, sizeof(g_type));
    memset(g_nonce, 0, sizeof(g_nonce));
    memset(g_gasPrice, 0, sizeof(g_gasPrice));
    memset(g_gasLimit, 0, sizeof(g_gasLimit));
    memset(g_to, 0, sizeof(g_to));
    // memset(g_feeRatio, 0, sizeof(g_feeRatio));
    memset(g_amount, 0, sizeof(g_amount));

    char type[50] = {0};
    if (!format_transaction_type(G_context.tx_info.transaction.txType, type, sizeof(type))) {
        return io_send_sw(SW_DISPLAY_TYPE_FAIL);
    }
    strncpy(g_type, type, sizeof(g_type));

    char nonce[30] = {0};
    if(!format_u64(nonce,
                   sizeof(nonce),
                   G_context.tx_info.transaction.nonce)) {
        return io_send_sw(SW_DISPLAY_NONCE_FAIL);
    }
    strncpy(g_nonce, nonce, sizeof(g_nonce));

    char gasPrice[30] = {0};
    uint64_t gasPriceValue = convertUint256ToUint64(&G_context.tx_info.transaction.gasprice);
    if (!format_u64(gasPrice, sizeof(gasPrice), gasPriceValue)) {
        return io_send_sw(SW_DISPLAY_GASPRICE_FAIL);
    }
    strncpy(g_gasPrice, gasPrice, sizeof(g_gasPrice));

    char gasLimit[30] = {0};
    uint64_t gasLimitValue = convertUint256ToUint64(&G_context.tx_info.transaction.startgas);
    if (!format_u64(gasLimit, sizeof(gasLimit), gasLimitValue)) {
        return io_send_sw(SW_DISPLAY_GAS_FAIL);
    }
    strncpy(g_gasLimit, gasLimit, sizeof(g_gasLimit));

    char to[43] = {0};
    if (format_hex(G_context.tx_info.transaction.to, ADDRESS_LEN, to, sizeof(to)) == -1) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
    strncpy(g_to, to, sizeof(g_to));

    char feeRatio[30] = {0};
    if (!format_u64(feeRatio, sizeof(feeRatio), G_context.tx_info.transaction.ratio)) {
        return io_send_sw(SW_DISPLAY_FEERATIO_FAIL);
    }
    strncpy(g_feeRatio, feeRatio, sizeof(g_feeRatio));

    char amount[50] = {0};
    if (!ammount_to_string(G_context.tx_info.transaction.value,
                           EXPONENT_SMALLEST_UNIT,
                           amount,
                           sizeof(amount))) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    snprintf(g_amount, sizeof(g_amount), "KLAY %.*s", sizeof(amount), amount);

    g_validate_callback = &ui_action_validate_transaction;

    ux_flow_init(0, ux_display_transaction_flow, NULL);

    return 0;
}

#endif
