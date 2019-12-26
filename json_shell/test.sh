#!/bin/sh

source ./jshn.sh

json_load "$(cat ./1.json)"
json_get_var name1 name
echo "name1="$name1
