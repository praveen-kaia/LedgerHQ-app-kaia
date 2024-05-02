#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/deserialize.h"
#include "transaction/process_txs.h"
#include "transaction/process_rlp_fields.h"
#include "types.h"

static void test_tx_deserialization(void **state) {
    (void) state;

    transaction_t tx;
    // clang-format off
    uint8_t raw_tx[] = {
        0xe7, 0x19, 0x85, 0x0b, 0xa4, 0x3b, 0x74, 0x00,
        0x83, 0x04, 0x93, 0xe0, 0x94, 0x0e, 0xe5, 0x6b,
        0x60, 0x4c, 0x86, 0x9e, 0x37, 0x92, 0xc9, 0x9e,
        0x35, 0xc1, 0xc4, 0x24, 0xf8, 0x8f, 0x87, 0xdc,
        0x8a, 0x01, 0x80, 0x82, 0x03, 0xe9, 0x80, 0x80
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);

    assert_int_equal(status, PARSING_OK);

}
static void test_error_tx_deserialization(void **state) {
    (void) state;

    transaction_t tx;
    // clang-format off
    uint8_t raw_tx[] = {
        0xd4, 0x7d, 0x6d, 0x1f, 0x78, 0x97, 0x0b, 0x9c,
        0xb8, 0x89, 0xb8, 0x3f, 0xc7, 0xc7, 0x07, 0xa0,
        0x0f, 0x59, 0x87, 0xf1, 0x4e, 0x06, 0x05, 0x8d,
        0xeb, 0xc4, 0xcb, 0x84, 0xf2, 0xa4, 0x0d, 0xa8,
        0x7d, 0x08, 0x57, 0x89, 0xbd, 0xc9, 0xb5, 0x10
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);

    assert_int_not_equal(status, PARSING_OK);

}

int main() {
    const struct CMUnitTest tests[] = { 
        cmocka_unit_test(test_tx_deserialization),
        cmocka_unit_test(test_error_tx_deserialization)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
