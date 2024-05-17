/**
 * @file types.h
 * @brief Defines the data types and constants used in transaction processing.
 */

#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#define ADDRESS_LEN  20
#define MAX_INT256   32
#define MAX_MEMO_LEN 465  // 510 - ADDRESS_LEN - 2*SIZE(U64) - SIZE(MAX_VARINT)

/**
 * @brief Enumeration of parser status codes.
 */
typedef enum {
    PARSING_OK = 2,                                 // Parsing completed successfully.
    PARSING_PROCESSING = 1,                         // Parsing is still in progress.
    PARSING_CONTINUE = 0,                           // Parsing needs to continue.
    PARSING_ERROR = -1,                             // Parsing encountered an error.
    NONCE_PARSING_ERROR = -2,                       // Error parsing the nonce.
    TO_PARSING_ERROR = -3,                          // Error parsing the recipient address.
    VALUE_PARSING_ERROR = -4,                       // Error parsing the transaction value.
    MEMO_LENGTH_ERROR = -5,                         // Error with the length of the memo.
    MEMO_PARSING_ERROR = -6,                         // Error parsing the memo.
    MEMO_ENCODING_ERROR = -7,                       // Error encoding the memo.
    WRONG_LENGTH_ERROR = -8                          // Error with the length of the transaction.
} parser_status_e;

/**
 * @brief Structure representing a 256-bit unsigned integer.
 */
typedef struct uint256_t {
    uint8_t value[MAX_INT256];  // The value of the 256-bit integer.
    uint8_t length;             // The length of the integer in bytes.
} uint256_t;

/**
 * @brief Enumeration of transaction types.
 */
typedef enum {
    EIP2930 = 0x01,                                        // EIP-2930 transaction type.
    EIP1559 = 0x02,                                        // EIP-1559 transaction type.
    LEGACY = 0xc0,                                         // Legacy transaction type.
    VALUE_TRANSFER = 0x08,                                 // Value transfer transaction type.
    FEE_DELEGATED_VALUE_TRANSFER = 0x09,                   // Fee-delegated value transfer transaction type.
    PARTIAL_FEE_DELEGATED_VALUE_TRANSFER = 0x0A,           // Partial fee-delegated value transfer transaction type.
    VALUE_TRANSFER_MEMO = 0x10,                            // Value transfer with memo transaction type.
    FEE_DELEGATED_VALUE_TRANSFER_MEMO = 0x11,              // Fee-delegated value transfer with memo transaction type.
    PARTIAL_FEE_DELEGATED_VALUE_TRANSFER_MEMO = 0x12,      // Partial fee-delegated value transfer with memo transaction type.
    SMART_CONTRACT_DEPLOY = 0x28,                          // Smart contract deployment transaction type.
    FEE_DELEGATED_SMART_CONTRACT_DEPLOY = 0x29,            // Fee-delegated smart contract deployment transaction type.
    PARTIAL_FEE_DELEGATED_SMART_CONTRACT_DEPLOY = 0x2A,    // Partial fee-delegated smart contract deployment transaction type.
    SMART_CONTRACT_EXECUTION = 0x30,                       // Smart contract execution transaction type.
    FEE_DELEGATED_SMART_CONTRACT_EXECUTION = 0x31,         // Fee-delegated smart contract execution transaction type.
    PARTIAL_FEE_DELEGATED_SMART_CONTRACT_EXECUTION = 0x32, // Partial fee-delegated smart contract execution transaction type.
    CANCEL = 0x38,                                         // Cancel transaction type.
    FEE_DELEGATED_CANCEL = 0x39,                           // Fee-delegated cancel transaction type.
    PARTIAL_FEE_DELEGATED_CANCEL = 0x3A                    // Partial fee-delegated cancel transaction type.
} transaction_type_e;

/**
 * @brief Structure representing a transaction.
 */
typedef struct {
    transaction_type_e txType;       // The type of the transaction.
    uint256_t nonce;                 // The nonce of the transaction.
    uint256_t gasprice;              // The gas price of the transaction.
    uint256_t startgas;              // The start gas (gas limit) of the transaction.
    uint8_t to[ADDRESS_LEN];         // The recipient address of the transaction.
    uint8_t ratio;                   // The ratio for partial fee-delegated transactions.
    uint256_t value;                 // The value (amount) of the transaction.
    uint256_t chainID;               // The chain ID of the transaction.
    bool dataPresent;                // Flag indicating whether data is present in the transaction.
} transaction_t;
