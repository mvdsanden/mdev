#!/bin/sh

ADAPTERNAME=$1

cat <<EOF
cmake_minimum_required(VERSION 3.7)

include(mvds)

mvds_add_adapter($ADAPTERNAME)
EOF
