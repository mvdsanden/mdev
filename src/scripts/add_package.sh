#!/bin/bash

BASE="$(dirname ${BASH_SOURCE[0]})"

GROUPNAME=$1
PACKAGENAME=$2

if [ ! -d "$MVDS_PROJECT_BASE_DIRECTORY/src/groups" ]; then
    mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/groups"
fi

if [ ! -d "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME" ]; then
    $BASE/add_group.sh $GROUPNAME
fi

if [ -d "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME/$PACKAGENAME" ]; then
    echo "Package $PACKAGENAME already exists."
    exit 1
fi

echo $PACKAGENAME >> "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME/group/members"

mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME/$PACKAGENAME"
mkdir "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME/$PACKAGENAME/package"

$BASE/gen_package_cmakelists.sh $GROUPNAME $PACKAGENAME > "$MVDS_PROJECT_BASE_DIRECTORY/src/groups/$GROUPNAME/$PACKAGENAME/CMakeLists.txt"
