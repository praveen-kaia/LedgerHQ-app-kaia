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

#ifdef HAVE_NBGL

#include <stdbool.h>  // bool
#include <string.h>   // memset

#include "os.h"
#include "glyphs.h"
#include "os_io_seproxyhal.h"
#include "nbgl_use_case.h"
#include "io.h"
#include "bip32.h"
#include "format.h"
#include "helper/format.h"

#include "display.h"
#include "constants.h"
#include "../globals.h"
#include "../sw.h"
#include "../address.h"
#include "action/validate.h"
#include "../transaction/types.h"
#include "../menu.h"

// Buffer where the transaction amount string is written
static char g_amount[50];
// Buffer where the transaction address string is written
static char g_to[43];
static char g_type[50];
static char g_nonce[30];
static char g_gasPrice[30];
static char g_gasLimit[30];
static char g_feeRatio[30];

static nbgl_layoutTagValue_t pairs[7];
static nbgl_layoutTagValueList_t pairList;
static nbgl_pageInfoLongPress_t infoLongPress;

static void confirm_transaction_rejection(void) {
    // display a status page and go back to main
    validate_transaction(false);
    nbgl_useCaseStatus("Transaction rejected", false, ui_menu_main);
}

static void ask_transaction_rejection_confirmation(void) {
    // display a choice to confirm/cancel rejection
    nbgl_useCaseConfirm("Reject transaction?",
                        NULL,
                        "Yes, Reject",
                        "Go back to transaction",
                        confirm_transaction_rejection);
}

// called when long press button on 3rd page is long-touched or when reject footer is touched
static void review_choice(bool confirm) {
    if (confirm) {
        // display a status page and go back to main
        validate_transaction(true);
        nbgl_useCaseStatus("TRANSACTION\nSIGNED", true, ui_menu_main);
    } else {
        ask_transaction_rejection_confirmation();
    }
}

static void handle_display_legacy() {
    // Setup data to display

    int i = 0;
    pairs[i].item = "Type";
    pairs[i++].value = g_type;

    pairs[i].item = "Amount";
    pairs[i++].value = g_amount;

    pairs[i].item = "To";
    pairs[i++].value = g_to;

    pairs[i].item = "Gas Price";
    pairs[i++].value = g_gasPrice;

    pairs[i].item = "Gas Limit";
    pairs[i++].value = g_gasLimit;

    pairs[i].item = "Nonce";
    pairs[i++].value = g_nonce;

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = i;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_kaia_64px;
    infoLongPress.text = "Sign transaction\nto send KAIA";
    infoLongPress.longPressText = "Hold to sign";
}

static void handle_display_value_transfer() {
    // Setup data to display

    int i = 0;
    pairs[i].item = "Type";
    pairs[i++].value = g_type;

    pairs[i].item = "Amount";
    pairs[i++].value = g_amount;

    pairs[i].item = "To";
    pairs[i++].value = g_to;

    pairs[i].item = "Gas Price";
    pairs[i++].value = g_gasPrice;

    pairs[i].item = "Gas Limit";
    pairs[i++].value = g_gasLimit;

    pairs[i].item = "Nonce";
    pairs[i++].value = g_nonce;

    if (G_context.tx_info.transaction.ratio != 0) {
        pairs[i].item = "Fee Ratio";
        pairs[i++].value = g_feeRatio;
    }

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = i;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_kaia_64px;
    infoLongPress.text = "Sign transaction\nto send KAIA";
    infoLongPress.longPressText = "Hold to sign";
}

static void handle_display_smart_contract_deploy() {
    // Setup data to display
    int i = 0;

    pairs[i].item = "Type";
    pairs[i++].value = g_type;

    pairs[i].item = "Amount";
    pairs[i++].value = g_amount;

    pairs[i].item = "Gas Price";
    pairs[i++].value = g_gasPrice;

    pairs[i].item = "Gas Limit";
    pairs[i++].value = g_gasLimit;

    pairs[i].item = "Nonce";
    pairs[i++].value = g_nonce;

    if (G_context.tx_info.transaction.ratio != 0) {
        pairs[i].item = "Fee Ratio";
        pairs[i++].value = g_feeRatio;
    }

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = i;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_kaia_64px;
    infoLongPress.text = "Sign transaction\nto send KAIA";
    infoLongPress.longPressText = "Hold to sign";
}

static void handle_display_smart_contract_execution() {
    // Setup data to display
    int i = 0;

    pairs[i].item = "Type";
    pairs[i++].value = g_type;

    pairs[i].item = "Amount";
    pairs[i++].value = g_amount;

    pairs[i].item = "Smart Contract";
    pairs[i++].value = g_to;

    pairs[i].item = "Gas Price";
    pairs[i++].value = g_gasPrice;

    pairs[i].item = "Gas Limit";
    pairs[i++].value = g_gasLimit;

    pairs[i].item = "Nonce";
    pairs[i++].value = g_nonce;

    if (G_context.tx_info.transaction.ratio != 0) {
        pairs[i].item = "Fee Ratio";
        pairs[i++].value = g_feeRatio;
    }

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = i;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_kaia_64px;
    infoLongPress.text = "Sign transaction\nto send KAIA";
    infoLongPress.longPressText = "Hold to sign";
}

static void handle_display_cancel() {
    // Setup data to display
    int i = 0;

    pairs[i].item = "Type";
    pairs[i++].value = g_type;

    pairs[i].item = "Amount";
    pairs[i++].value = g_amount;

    pairs[i].item = "Gas Price";
    pairs[i++].value = g_gasPrice;

    pairs[i].item = "Gas Limit";
    pairs[i++].value = g_gasLimit;

    pairs[i].item = "Nonce";
    pairs[i++].value = g_nonce;

    if (G_context.tx_info.transaction.ratio != 0) {
        pairs[i].item = "Fee Ratio";
        pairs[i++].value = g_feeRatio;
    }

    // Setup list
    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = i;
    pairList.pairs = pairs;

    // Info long press
    infoLongPress.icon = &C_app_kaia_64px;
    infoLongPress.text = "Sign transaction\nto send KAIA";
    infoLongPress.longPressText = "Hold to sign";
}

static void review_continue(void) {
    switch (G_context.tx_info.transaction.txType) {
        case LEGACY:
            handle_display_legacy();
            break;
        case VALUE_TRANSFER:
        case FEE_DELEGATED_VALUE_TRANSFER:
        case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER:
        case VALUE_TRANSFER_MEMO:
        case FEE_DELEGATED_VALUE_TRANSFER_MEMO:
        case PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO:
            handle_display_value_transfer();
            break;
        case SMART_CONTRACT_DEPLOY:
        case FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
        case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY:
            handle_display_smart_contract_deploy();
            break;
        case SMART_CONTRACT_EXECUTION:
        case FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
        case PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION:
            handle_display_smart_contract_execution();
            break;
        case CANCEL:
        case FEE_DELEGATED_CANCEL:
        case PARTIAL_FEE_DELEGATED_CANCEL:
            handle_display_cancel();
            break;
        default:
            PRINTF("Transaction type %d is not supported\n", G_context.tx_info.transaction.txType);
    }

    nbgl_useCaseStaticReview(&pairList, &infoLongPress, "Reject transaction", review_choice);
}

// Public function to start the transaction review
// - Check if the app is in the right state for transaction review
// - Format the amount and address strings in g_amount and g_to buffers
// - Display the first screen of the transaction review
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
    memset(g_feeRatio, 0, sizeof(g_feeRatio));
    memset(g_amount, 0, sizeof(g_amount));

    char type[50] = {0};
    if (!format_transaction_type(G_context.tx_info.transaction.txType, type, sizeof(type))) {
        return io_send_sw(SW_DISPLAY_TYPE_FAIL);
    }
    strncpy(g_type, type, sizeof(g_type));

    char nonce[30] = {0};
    uint64_t nonceValue = convertUint256ToUint64(&G_context.tx_info.transaction.nonce);
    if (!format_u64(nonce, sizeof(nonce), nonceValue)) {
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

    if (format_hex(G_context.tx_info.transaction.to, ADDRESS_LEN, g_to, sizeof(g_to)) == -1) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    char feeRatio[30] = {0};
    if (!format_u64(feeRatio, sizeof(feeRatio), G_context.tx_info.transaction.ratio)) {
        return io_send_sw(SW_DISPLAY_FEERATIO_FAIL);
    }
    strncpy(g_feeRatio, feeRatio, sizeof(g_feeRatio));
    strncat(g_feeRatio, "%%", 1);  // append '%' sign

    char amount[50] = {0};
    if (!amount_to_string(G_context.tx_info.transaction.value,
                          EXPONENT_SMALLEST_UNIT,
                          amount,
                          sizeof(amount))) {
        return io_send_sw(SW_DISPLAY_AMOUNT_FAIL);
    }
    snprintf(g_amount, sizeof(g_amount), "KAIA %.*s", sizeof(amount), amount);

    // Start review
    PRINTF("Displaying transaction review\n");
    nbgl_useCaseReviewStart(&C_app_kaia_64px,
                            "Review transaction\nto send KAIA",
                            NULL,
                            "Reject transaction",
                            review_continue,
                            ask_transaction_rejection_confirmation);
    return DISPLAY_OK;
}

#endif
