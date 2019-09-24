#!/bin/bash

rm -rf -d lib/sabre
cd ..
sh build.sh
mv sabre/ example/lib/sabre
cd example

sh scripts/build.sh