#!/bin/sh

APPNAME=$1

cat <<EOF
// $APPNAME.m.cpp                                              -*-c++-*-
#include <iostream>

int main(int argc, char *argv[])
{
  return 0;
}
EOF
