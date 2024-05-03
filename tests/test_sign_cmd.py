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

# In these tests test we send to the device a transaction to sign and validate it with the Speculos emulator
# We will ensure that the displayed information is correct by using screenshots comparison

def test_sign_tx_legacy_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "e719850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01808203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_value_transfer_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f84eb847f8450882115c850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a8ca18f07d736b90be550000001946e93a3acfbadf457f29fb0e57fa42274004c32ea8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_fee_delegated_value_transfer_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f83fb838f70919850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_partial_fee_delegated_value_transfer_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f841b83af8380a19850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea1e8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_value_transfer_memo_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f846b83ff83d1019850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_fee_delegated_value_transfer_memo_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f846b83ff83d1119850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_partial_fee_delegated_value_transfer_memo_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f847b840f83e1219850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f1e8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_smart_contract_deploy_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f902efb902e7f902e42819850ba43b7400830493e08001946e93a3acfbadf457f29fb0e57fa42274004c32eab902bc6080604052600560005534801561001557600080fd5b5060405161029c38038061029c8339818101604052810190610037919061007f565b80600081905550506100ac565b600080fd5b6000819050919050565b61005c81610049565b811461006757600080fd5b50565b60008151905061007981610053565b92915050565b60006020828403121561009557610094610044565b5b60006100a38482850161006a565b91505092915050565b6101e1806100bb6000396000f3fe608060405234801561001057600080fd5b50600436106100415760003560e01c80630dbe671f14610046578063d732d95514610064578063e8927fbc1461006e575b600080fd5b61004e610078565b60405161005b91906100d7565b60405180910390f35b61006c61007e565b005b6100766100a3565b005b60005481565b60008054146100a15760016000808282546100999190610121565b925050819055505b565b60016000808282546100b59190610155565b92505081905550565b6000819050919050565b6100d1816100be565b82525050565b60006020820190506100ec60008301846100c8565b92915050565b7f4e487b7100000000000000000000000000000000000000000000000000000000600052601160045260246000fd5b600061012c826100be565b9150610137836100be565b92508282101561014a576101496100f2565b5b828203905092915050565b6000610160826100be565b915061016b836100be565b9250827fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff038211156101a05761019f6100f2565b5b82820190509291505056fea264697066735822122011473ea0fe0dcd65952f4315de5458369b91cb3a2f53790f0906775227a6070c64736f6c634300080f0033000000000000000000000000000000000000000000000000000000000000000180808203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_fee_delegated_smart_contract_deploy_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f2aceb2919850ba43b7400830493e08001946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f80808203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_partial_fee_delegated_smart_contract_deploy_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f3adec2a19850ba43b7400830493e08001946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f801e808203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_smart_contract_execution_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f886b87ff87d3019850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32eab844095ea7b3000000000000000000000000f50782a24afcb26acb85d086cf892bfffb5731b5ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_fee_delegated_smart_contract_execution_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f846b83ff83d3119850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_partial_fee_delegated_smart_contract_execution_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "f847b840f83e3219850ba43b7400830493e0940ee56b604c869e3792c99e35c1c424f88f87dc8a01946e93a3acfbadf457f29fb0e57fa42274004c32ea8568656c6c6f1e8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_cancel_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "e8a2e13819850ba43b7400830493e0946e93a3acfbadf457f29fb0e57fa42274004c32ea8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_fee_delegated_cancel_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "e8a2e13919850ba43b7400830493e0946e93a3acfbadf457f29fb0e57fa42274004c32ea8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)

def test_sign_tx_partial_fee_delegated_cancel_tx(firmware, backend, navigator, test_name):
    raw_transaction_hex = "e9a3e23a19850ba43b7400830493e0946e93a3acfbadf457f29fb0e57fa42274004c32ea1e8203e98080"
    perform_test_sign_tx_with_raw_tx(firmware, backend, navigator, test_name, raw_transaction_hex)
