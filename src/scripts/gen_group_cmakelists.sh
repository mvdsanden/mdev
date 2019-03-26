#!/bin/sh

GROUPNAME=$1

cat <<EOF
cmake_minimum_required(VERSION 3.7)

include(mvds)

mvds_add_group($GROUPNAME)
EOF
