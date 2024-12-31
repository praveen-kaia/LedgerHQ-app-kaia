# Kaia Transaction Serialization

## Overview

Kaia uses RLP (Recursive Length Prefix) encoding for transaction serialization, similar to Ethereum. However, Kaia transactions (except for LegacyTransaction) are double encoded, and they contain additional fields when RLP encoded for signature.

## Amount Units

The base unit in Kaia is KAIA. The smallest unit used in a raw transaction is the *kei*: 
1 KAIA = 10^18 kei (1,000,000,000,000,000,000 kei).

## Address Format

Kaia addresses are hexadecimal numbers, derived from the last 20 bytes of the Keccak-256 hash of the public key.

## Structure

### Transaction

A transaction in a blockchain platform is a message sent between nodes that changes the state of the blockchain. For example, when a transaction that sends 10 KAIA from Alice’s account to Bob’s is executed, Alice's balance decreases by 10 KAIA, and Bob's balance increases by 10 KAIA. A transaction is an atomic operation and cannot be interleaved with another transaction. A typical blockchain transaction has components as shown below:

| Component | Description |
| --- | --- |
| `nonce` | A unique identifier for the sender’s transaction. Prevents replay attacks and double-spending. |
| `gasPrice` | The price per unit of gas the sender is willing to pay. |
| `gas` | The maximum amount of gas the transaction is allowed to consume. |
| `to` | The recipient's account address. |
| `value` | The amount of KAIA (in peb) to be transferred. |
| `input` | Data attached to the transaction, often used for contract execution. |
| `v`, `r`, `s` | The components of the cryptographic signature, used to authenticate the sender and ensure transaction integrity. |

### Transaction Types

Kaia supports various transaction types, each with different fields and structures. For a comprehensive list of transaction types, refer to the [Kaia Transaction Types](https://docs.kaia.io/learn/transactions).

### RLP Encoding

RLP (Recursive Length Prefix) encoding is used to serialize Kaia transactions. It is a binary encoding scheme that represents data structures in a compact and efficient manner. For more information, refer to the [RLP specification](https://github.com/ethereum/wiki/wiki/RLP).

### Double Encoding

In Kaia, all transactions except for `LegacyTransaction` are double encoded. This means the RLP encoded transaction is itself RLP encoded again.

### Signature

Deterministic ECDSA ([RFC 6979](https://tools.ietf.org/html/rfc6979)) is used to sign transactions on the [SECP-256k1](https://www.secg.org/sec2-v2.pdf#subsubsection.2.4.1) curve. The message to be signed is created by concatenating and hashing the transaction components.

For **LegacyTransaction**, which uses single encoding, the signing process involves:

1. RLP encode the transaction fields.
2. Hash the RLP encoded transaction using Keccak-256.
3. Sign the hashed message using ECDSA.

Example for a `LegacyTransaction`:

```
SigRLP = encode([nonce, gasPrice, gas, to, value, input, chainid, 0, 0])
SigHash = keccak256(SigRLP)
Signature = sign(SigHash, <private key>)
```

Any other transaction type \(ex: **ValueTransferTransaction**\) uses double encoding, the signing process involves:

1. RLP encode the transaction fields (specific to the transaction type).
2. Hash the RLP encoded transaction using Keccak-256.
3. Sign the hashed message using ECDSA.

Example for a `ValueTransferTransaction`:

```
SigRLP = encode([encode([type, nonce, gasPrice, gas, to, value, from]), chainid, 0, 0])
SigHash = keccak256(SigRLP)
Signature = sign(SigHash, <private key>)
```

This ensures the transaction can be properly processed and verified by the Kaia network.