#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define MAX_TX_LEN   510
#define ADDRESS_LEN  20
#define MAX_INT256  32
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)

typedef enum {
    PARSING_OK = 2,
    PARSING_PROCESSING = 1,
    PARSING_CONTINUE = 0,
    PARSING_ERROR = -1,
    NONCE_PARSING_ERROR = -2,
    TO_PARSING_ERROR = -3,
    VALUE_PARSING_ERROR = -4,
    MEMO_LENGTH_ERROR = -5,
    MEMO_PARSING_ERROR = -6,
    MEMO_ENCODING_ERROR = -7,
    WRONG_LENGTH_ERROR = -8
} parser_status_e;

typedef struct uint256_t {
    uint8_t value[MAX_INT256];
    uint8_t length;
} uint256_t;

typedef enum {
    EIP2930 = 0x01,
    EIP1559 = 0x02,

    VALUE_TRANSFER = 0x08,
    FEE_DELEGATED_VALUE_TRANSFER = 0x09,
    PARTIAL_FEE_DELEGATED_VALUE_TRANSFER = 0x0A,

    VALUE_TRANSFER_MEMO = 0x10,
    FEE_DELEGATED_VALUE_TRANSFER_MEMO = 0x11,
    PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO = 0x12,

    SMART_CONTRACT_DEPLOY = 0x28,
    FEE_DELEGATED_SMART_CONTRACT_DEPLOY = 0x29,
    PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY = 0x2A,

    SMART_CONTRACT_EXECUTION = 0x30,
    FEE_DELEGATED_SMART_CONTRACT_EXECUTION = 0x31,
    PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION = 0x32,

    CANCEL = 0x38,
    FEE_DELEGATED_CANCEL = 0x39,
    PARTIAL_FEE_DELEGATED_CANCEL = 0x3A,

    LEGACY = 0xc0,
} transaction_type_e;

typedef struct {
    transaction_type_e txType;      /// transaction type
    uint8_t nonce;                 /// nonce (8 bytes)
    uint256_t gasprice;
    uint256_t startgas;             /// also known as `gaslimit`
    uint8_t *to;                    /// pointer to address (20 bytes)
    uint8_t ratio;                  /// ratio for partial fee delegated tx
    uint256_t value;                /// amount value
    uint256_t chainID;
    bool dataPresent;               /// flag for data presence
} transaction_t;

