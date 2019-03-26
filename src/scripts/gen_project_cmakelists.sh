#!/bin/bash

PROJECTNAME=$1

cat <<EOF
cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH "\${CMAKE_CURRENT_LIST_DIR}/modules")

include(mvds)

mvds_build()
EOF
