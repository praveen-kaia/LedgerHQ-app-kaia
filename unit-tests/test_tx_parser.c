#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
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

    // uint8_t output[300];
    // int length = transaction_serialize(&tx, output, sizeof(output));
    // assert_int_equal(length, sizeof(raw_tx));
    // assert_memory_equal(raw_tx, output, sizeof(raw_tx));
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_tx_deserialization)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
