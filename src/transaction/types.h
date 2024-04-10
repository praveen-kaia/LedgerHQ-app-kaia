#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define MAX_TX_LEN   510
#define ADDRESS_LEN  20
#define INT256_LEN  32
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)

typedef enum {
    PARSING_OK = 1,
    NONCE_PARSING_ERROR = -1,
    TO_PARSING_ERROR = -2,
    VALUE_PARSING_ERROR = -3,
    MEMO_LENGTH_ERROR = -4,
    MEMO_PARSING_ERROR = -5,
    MEMO_ENCODING_ERROR = -6,
    WRONG_LENGTH_ERROR = -7
} parser_status_e;

typedef struct uint256_t {
    uint8_t value[INT256_LEN];
    uint8_t length;
} uint256_t;

typedef struct {
    uint256_t gasprice;
    uint256_t startgas; /// also known as `gaslimit`
    uint256_t value;    /// amount value
    uint256_t chainID;
    uint64_t nonce;     /// nonce (8 bytes)
    uint8_t *to;        /// pointer to address (20 bytes)
    uint8_t ratio;      /// ratio for partial fee delegated tx
    bool dataPresent;   /// flag for data presence
} transaction_t;
