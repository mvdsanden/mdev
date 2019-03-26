#!/bin/bash

BASE="$(dirname ${BASH_SOURCE[0]})"

ADAPTERNAME=$1


if [ ! -d "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters" ]; then
    mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters"
fi

if [ -d "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters/$ADAPTERNAME" ]; then
    echo "Group $ADAPTERNAME already exists."
    exit 1
fi

mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters/$ADAPTERNAME"
mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters/$ADAPTERNAME/adapter"

$BASE/gen_adapter_cmakelists.sh $ADAPTERNAME > "$MVDS_PROJECT_BASE_DIRECTORY/src/adapters/$ADAPTERNAME/CMakeLists.txt"
