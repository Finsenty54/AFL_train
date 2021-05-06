#!/bin/bash

mkdir in_tmin
cd in_cmin
for i in *; do
  afl-tmin -i "$i" -o "../in_tmin/$i" -m none -- ../first_try -d @@
done
