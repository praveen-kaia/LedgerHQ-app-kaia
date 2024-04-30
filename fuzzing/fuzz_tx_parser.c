#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "transaction/deserialize.h"
#include "transaction/utils.h"
#include "transaction/types.h"
#include "helper/format.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = data, .size = size, .offset = 0};
    transaction_t tx;
    parser_status_e status;
    char type[50] = {0};
    char nonce[30] = {0};
    char gasPrice[30] = {0};
    char gasLimit[30] = {0};
    char to[43] = {0};
    char address[43] = {0};
    char feeRatio[30] = {0};
    char amount[50] = {0};
    memset(&tx, 0, sizeof(tx));

    status = transaction_deserialize(&buf, &tx);

    if (status == PARSING_OK) {
        format_transaction_type(tx.txType, type, sizeof(type));
        printf("type: %s\n", type);

        format_u64(nonce, sizeof(nonce), tx.nonce);
        printf("nonce: %s\n", nonce);

        uint64_t temp = convertUint256ToUint64(&tx.gasprice);
        format_u64(gasPrice, sizeof(gasPrice), temp);
        printf("gasPrice: %s\n", gasPrice);

        temp = convertUint256ToUint64(&tx.startgas);
        format_u64(gasLimit, sizeof(gasLimit), temp);
        printf("gasLimit: %s\n", gasLimit);

        format_hex(tx.to, ADDRESS_LEN, to, sizeof(to));
        printf("destination: %s\n", to);

        format_u64(feeRatio, sizeof(feeRatio), tx.ratio);
        printf("feeRatio: %s\n", feeRatio);

        ammount_to_string(tx.value, 18, amount, sizeof(amount)); // 18 is the num of decimals
        printf("amount: %s\n", amount);
    }

    return 0;
}
