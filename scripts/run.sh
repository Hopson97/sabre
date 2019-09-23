#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/sf-net 
else
    ./bin/debug/sf-net 
fi