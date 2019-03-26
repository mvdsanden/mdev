#!/bin/bash

BASE="$(dirname ${BASH_SOURCE[0]})"
source $BASE/config.sh

ROOT_NS=$1
PKGNAME=$2
CLASSNAME=$3

FILENAME_H="${PKGNAME,,}_${CLASSNAME,,}.t.cpp"

repeat(){
    for (( i=1; i<=$1; i++ )); do echo -n "$2"; done
}

$CLANG_FORMAT <<EOF
// $FILENAME_H$(repeat $((67 - ${#FILENAME_H})) " ")-*-c++-*-
#include <${PKGNAME,,}_${CLASSNAME,,}.h>

#include <gtest/gtest.h>

using namespace $ROOT_NS;
using namespace $ROOT_NS::$PKGNAME;

TEST(${CLASSNAME}Test, Constructor)
{
  // TEST CONSTRUCTOR
  //
  // Concern:
  //   The objects constructor should work.
  //
  // Test plan:
  //   Construct the object.
  $CLASSNAME obj;
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
// Copyright (C) 2018 Marten van de Sanden
// ----------------------------------------------------------------------------
EOF
