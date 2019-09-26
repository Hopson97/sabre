#!/bin/bash

rm -rf -d lib/sabre
cd ..
ls
sh build.sh
mv sabre/ example/lib
cd example



sh scripts/build.sh $1