#!/bin/bash

cp -r /cygdrive/c/Users/tmkjm/OneDrive/Documents/robo/source2 /cygdrive/c/cygwin64/home/tmkjm/hrp2/sdk/workspace/
make app=source2
make upload ip=10.0.10.1 from=/home/tmkjm/hrp2/sdk/workspace/app to=test

