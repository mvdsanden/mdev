#!/bin/bash

BASE="$(dirname ${BASH_SOURCE[0]})"
source $BASE/config.sh

ROOT_NS=$1
PKGNAME=$2
CLASSNAME=$3

FILENAME_H="${PKGNAME,,}_${CLASSNAME,,}.h"
INCLUDE_GUARD="INCLUDED_${PKGNAME^^}_${CLASSNAME^^}"

repeat(){
    for (( i=1; i<=$1; i++ )); do echo -n "$2"; done
}

$CLANG_FORMAT <<EOF
// $FILENAME_H$(repeat $((67 - ${#FILENAME_H})) " ")-*-c++-*-
#ifndef $INCLUDE_GUARD
#define $INCLUDE_GUARD

namespace $ROOT_NS {
namespace $PKGNAME {

// $(repeat $((7 + ${#CLASSNAME})) "=")
// Class: $CLASSNAME
// $(repeat $((7 + ${#CLASSNAME})) "=")

class $CLASSNAME {
  // ...

  // TYPES

  // DATA

  // PRIVATE MANIPULATORS

public:

  // CREATORS
  $CLASSNAME();
    // ...

  // MANIPULATORS

  // ACCESSORS

};

} // namespace $PKGNAME
} // namespace $ROOT_NS

#endif // $INCLUDE_GUARD

// ----------------------------------------------------------------------------
// Copyright (C) 2018 Marten van de Sanden
// ----------------------------------------------------------------------------
EOF
