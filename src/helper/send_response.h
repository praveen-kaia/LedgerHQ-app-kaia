#pragma once

#include "os.h"
#include "macros.h"

/**
 * Length of public key.
 */
#define PUBKEY_LEN (MEMBER_SIZE(pubkey_ctx_t, raw_public_key))
/**
 * Length of chain code.
 */
#define CHAINCODE_LEN (MEMBER_SIZE(pubkey_ctx_t, chain_code))

/**
 * @brief The length of an address in ASCII hexadecimal format.
 *
 * This constant defines the length of an address in ASCII hexadecimal format.
 * The address is represented as a string of characters, where each character
 * represents a hexadecimal digit. The length of the address is fixed at 40 characters.
 */
#define ADDRESS_IN_ASCII_HEX_LEN 40

/**
 * @brief The length of a hash value.
 *
 * This constant defines the length of a hash value in bytes. A hash value is
 * typically used to uniquely identify data. The length of the hash value is fixed
 * at 32 bytes.
 */
#define HASH_LENGTH 32

/**
 * Helper to send APDU response with public key and chain code.
 *
 * response = PUBKEY_LEN (1) ||
 *            G_context.pk_info.public_key (PUBKEY_LEN) ||
 *            CHAINCODE_LEN (1) ||
 *            G_context.pk_info.chain_code (CHAINCODE_LEN)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_pubkey(void);

/**
 * Helper to send APDU response with signature and v (parity of
 * y-coordinate of R).
 *
 * response = G_context.tx_info.signature_len (1) ||
 *            G_context.tx_info.signature (G_context.tx_info.signature_len) ||
 *            G_context.tx_info.v (1)
 *
 * @return zero or positive integer if success, -1 otherwise.
 *
 */
int helper_send_response_sig(void);

/**
 * Converts a binary Ethereum address to a string representation.
 *
 * @param address The binary Ethereum address.
 * @param out The output string where the address will be stored.
 * @param sha3Context The SHA3 context used for hashing.
 */
void getEthAddressStringFromBinary(uint8_t *address, char *out, cx_sha3_t *sha3Context);

/**
 * Converts a public key to a string representation of the corresponding Ethereum address.
 *
 * @param publicKey The public key.
 * @param out The output string where the address will be stored.
 * @param sha3Context The SHA3 context used for hashing.
 */
void getEthAddressStringFromKey(uint8_t *publicKey, char *out, cx_sha3_t *sha3Context);