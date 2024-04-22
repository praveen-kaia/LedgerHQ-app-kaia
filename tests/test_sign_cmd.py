import pytest

from application_client.klaytn_transaction import Transaction
from application_client.klaytn_command_sender import KlaytnCommandSender, Errors
from application_client.klaytn_response_unpacker import strip_v_from_signature, unpack_get_public_key_response, unpack_sign_tx_response
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity
from ecdsa import VerifyingKey, SECP256k1
import sha3


def verify_transaction_signature_from_public_key(transaction: bytes, signature: bytes, public_key: bytes):
    try:
        if len(signature) == 65:
            signature = strip_v_from_signature(signature)
        verifying_key = VerifyingKey.from_string(public_key, curve=SECP256k1)    
        return verifying_key.verify(signature, transaction, hashfunc=sha3.keccak_256)
    except Exception as e:
        raise e 
    
# In these tests we check the behavior of the device when asked to sign a transaction



# This function is a parametrized test that will test the signing of a transaction with a raw transaction in hex
def perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex):
    # Use the app interface instead of raw interface
    client = KlaytnCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/8217'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _, _, _ = unpack_get_public_key_response(rapdu.data)

    # Convert the raw transaction hex to bytes
    raw_transaction_bytes = bytes.fromhex(raw_transaction_hex)

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(path=path, transaction=raw_transaction_bytes):
        # Validate the on-screen request by performing the navigation appropriate for this device
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Approve",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                      [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                       NavInsID.USE_CASE_STATUS_DISMISS],
                                                      "Hold to sign",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)

    # The device as yielded the result, parse it and ensure that the signature is correct
    signature = client.get_async_response().data
    result = verify_transaction_signature_from_public_key(raw_transaction_bytes, signature, public_key)
    assert result, "The signature is not valid"


# In this test we send to the device a Cancel transaction to sign and validate it on screen
# The transaction is short and will be sent in one chunk
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_tx_cancel_tx(firmware, backend, navigator, test_name):
    # This is the transaction's data structure and content.
    # The encoding is RLP, and the transaction is as follows:
    # encode(
    #     encode(
    #         type: 0x38,
    #         nonce: 0x19 #25
    #         gasPrice: 0x0ba43b7400, #50000000000
    #         gas: 0x0493e0, #300000
    #         from: 0x6E93a3ACfbaDF457F29fb0E57FA42274004c32EA,
    #     )
    #     chainId: 0x03e9, #1001
    #     0
    #     0
    # )
    raw_transaction_hex = "e8a2e13819850ba43b7400830493e0946e93a3acfbadf457f29fb0e57fa42274004c32ea8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)


# # In this test se send to the device a transaction to sign and validate it on screen
# # The transaction is short and will be sent in one chunk
# # We will ensure that the displayed information is correct by using screenshots comparison
# def test_sign_tx_short_tx(firmware, backend, navigator, test_name):
#     # Use the app interface instead of raw interface
#     client = KlaytnCommandSender(backend)
#     # The path used for this entire test
#     path: str = "m/44'/8217'/0'/0/0"

#     # First we need to get the public key of the device in order to build the transaction
#     rapdu = client.get_public_key(path=path)
#     _, public_key, _, _, _, _ = unpack_get_public_key_response(rapdu.data)

#     # Create the transaction that will be sent to the device for signing
#     transaction = Transaction(
#         nonce=1,
#         to="0xde0b295669a9fd93d5f28d9ec85e40f4cb697bae",
#         value=666,
#         memo="For u EthDev"
#     ).serialize()

#     # Send the sign device instruction.
#     # As it requires on-screen validation, the function is asynchronous.
#     # It will yield the result when the navigation is done
#     with client.sign_tx(path=path, transaction=transaction):
#         # Validate the on-screen request by performing the navigation appropriate for this device
#         if firmware.device.startswith("nano"):
#             navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
#                                                       [NavInsID.BOTH_CLICK],
#                                                       "Approve",
#                                                       ROOT_SCREENSHOT_PATH,
#                                                       test_name)
#         else:
#             navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
#                                                       [NavInsID.USE_CASE_REVIEW_CONFIRM,
#                                                        NavInsID.USE_CASE_STATUS_DISMISS],
#                                                       "Hold to sign",
#                                                       ROOT_SCREENSHOT_PATH,
#                                                       test_name)

#     # The device as yielded the result, parse it and ensure that the signature is correct
#     response = client.get_async_response().data
#     _, der_sig, _ = unpack_sign_tx_response(response)
#     assert check_signature_validity(public_key, der_sig, transaction)


# # In this test se send to the device a transaction to sign and validate it on screen
# # This test is mostly the same as the previous one but with different values.
# # In particular the long memo will force the transaction to be sent in multiple chunks
# def test_sign_tx_long_tx(firmware, backend, navigator, test_name):
#     # Use the app interface instead of raw interface
#     client = KlaytnCommandSender(backend)
#     path: str = "m/44'/8217'/0'/0/0"

#     rapdu = client.get_public_key(path=path)
#     _, public_key, _, _, _, _ = unpack_get_public_key_response(rapdu.data)

#     transaction = Transaction(
#         nonce=1,
#         to="0xde0b295669a9fd93d5f28d9ec85e40f4cb697bae",
#         value=666,
#         memo=("This is a very long memo. "
#               "It will force the app client to send the serialized transaction to be sent in chunk. "
#               "As the maximum chunk size is 255 bytes we will make this memo greater than 255 characters. "
#               "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam.")
#     ).serialize()

#     with client.sign_tx(path=path, transaction=transaction):
#         if firmware.device.startswith("nano"):
#             navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
#                                                       [NavInsID.BOTH_CLICK],
#                                                       "Approve",
#                                                       ROOT_SCREENSHOT_PATH,
#                                                       test_name)
#         else:
#             navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
#                                                       [NavInsID.USE_CASE_REVIEW_CONFIRM,
#                                                        NavInsID.USE_CASE_STATUS_DISMISS],
#                                                       "Hold to sign",
#                                                       ROOT_SCREENSHOT_PATH,
#                                                       test_name)
#     response = client.get_async_response().data
#     _, der_sig, _ = unpack_sign_tx_response(response)
#     assert check_signature_validity(public_key, der_sig, transaction)


# # Transaction signature refused test
# # The test will ask for a transaction signature that will be refused on screen
# def test_sign_tx_refused(firmware, backend, navigator, test_name):
#     # Use the app interface instead of raw interface
#     client = KlaytnCommandSender(backend)
#     path: str = "m/44'/8217'/0'/0/0"

#     rapdu = client.get_public_key(path=path)
#     _, pub_key, _, _ = unpack_get_public_key_response(rapdu.data)

#     transaction = Transaction(
#         nonce=1,
#         to="0xde0b295669a9fd93d5f28d9ec85e40f4cb697bae",
#         value=666,
#         memo="This transaction will be refused by the user"
#     ).serialize()

#     if firmware.device.startswith("nano"):
#         with pytest.raises(ExceptionRAPDU) as e:
#             with client.sign_tx(path=path, transaction=transaction):
#                 navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
#                                                           [NavInsID.BOTH_CLICK],
#                                                           "Reject",
#                                                           ROOT_SCREENSHOT_PATH,
#                                                           test_name)

#         # Assert that we have received a refusal
#         assert e.value.status == Errors.SW_DENY
#         assert len(e.value.data) == 0
#     else:
#         for i in range(3):
#             instructions = [NavInsID.USE_CASE_REVIEW_TAP] * i
#             instructions += [NavInsID.USE_CASE_REVIEW_REJECT,
#                              NavInsID.USE_CASE_CHOICE_CONFIRM,
#                              NavInsID.USE_CASE_STATUS_DISMISS]
#             with pytest.raises(ExceptionRAPDU) as e:
#                 with client.sign_tx(path=path, transaction=transaction):
#                     navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
#                                                    test_name + f"/part{i}",
#                                                    instructions)
#             # Assert that we have received a refusal
#             assert e.value.status == Errors.SW_DENY
#             assert len(e.value.data) == 0
