#!/bin/bash

mkdir crash_tmin
cd out/default/crashes
for i in *; do
  afl-tmin -i "$i" -o "../../../crash_tmin/$i" -m none -- ../../../first_try @@
done
