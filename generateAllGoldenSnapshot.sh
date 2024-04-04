#!/bin/bash

generate_golden_snapshots() {
    if [ $# -ne 1 ]; then
        echo "Usage: $0 [nanos|nanosp|nanox|stax]"
        exit 1
    fi

    PARAM=$(echo "$1" | tr '[:lower:]' '[:upper:]')_SDK
    PARAM_MIN=$(echo "$1" | tr '[:upper:]' '[:lower:]')

    docker exec -it LedgerHQ-app-klaytn-container bash -c 'make -C ./ clean' &&
    git submodule update --init --recursive &&
    docker exec -it LedgerHQ-app-klaytn-container bash -c "export BOLOS_SDK=\$(echo \$$PARAM) && make -C ./ -j DEBUG=1" &&
    docker exec -it -u 0 LedgerHQ-app-klaytn-container bash -c ' [ -f ./tests//requirements.txt ] && pip install -r ./tests//requirements.txt' &&
    docker exec -it LedgerHQ-app-klaytn-container bash -c "pytest ./tests/ --tb=short -v --device $PARAM_MIN --golden_run"
}

generate_golden_snapshots "nanos" > z_output
generate_golden_snapshots "nanosp" >> z_output
generate_golden_snapshots "nanox" >> z_output
generate_golden_snapshots "stax" >> z_output
