#!/bin/sh

APPNAME=$1

cat <<EOF
cmake_minimum_required(VERSION 3.7)

include(mvds)

mvds_add_application($APPNAME)
EOF
