#!/bin/bash

BASE="$(dirname ${BASH_SOURCE[0]})"
source $BASE/config.sh

ROOT_NS=$1
PKGNAME=$2
CLASSNAME=$3

FILENAME_CPP="${PKGNAME,,}_${CLASSNAME,,}.cpp"

repeat(){
    for (( i=1; i<=$1; i++ )); do echo -n "$2"; done
}

$CLANG_FORMAT <<EOF
// $FILENAME_CPP$(repeat $((67 - ${#FILENAME_CPP})) " ")-*-c++-*-
#include <${PKGNAME,,}_${CLASSNAME,,}.h>

namespace $ROOT_NS {
namespace $PKGNAME {

// $(repeat $((7 + ${#CLASSNAME})) "-")
// Class: $CLASSNAME
// $(repeat $((7 + ${#CLASSNAME})) "-")

// CREATORS
$CLASSNAME::$CLASSNAME()
{
}

// MANIPULATORS

// ACCESSORS

} // namespace $PKGNAME
} // namespace $ROOT_NS

// ----------------------------------------------------------------------------
// Copyright (C) 2018 Marten van de Sanden
// ----------------------------------------------------------------------------
EOF
