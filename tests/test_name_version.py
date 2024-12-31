from application_client.kaia_command_sender import KaiaCommandSender
from application_client.kaia_response_unpacker import unpack_get_app_and_version_response


# Test a specific APDU asking BOLOS (and not the app) the name and version of the current app
def test_get_app_and_version(backend, backend_name):
    # Use the app interface instead of raw interface
    client = KaiaCommandSender(backend)
    # Send the special instruction to BOLOS
    response = client.get_app_and_version()
    # Use an helper to parse the response, assert the values
    app_name, version = unpack_get_app_and_version_response(response.data)

    assert app_name == "Kaia"
    assert version == "1.1.0"
