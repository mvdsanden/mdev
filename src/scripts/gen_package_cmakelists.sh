#!/bin/sh

GROUPNAME=$1
PACKAGENAME=$2

cat <<EOF
cmake_minimum_required(VERSION 3.7)

include(mvds)

mvds_add_package($PACKAGENAME)
EOF
