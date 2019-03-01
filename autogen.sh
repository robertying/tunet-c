#!/bin/bash

mkdir -p m4
cp cJSON/cJSON.c lib/
cp cJSON/cJSON.h lib/
cp sds/sds.c lib/
cp sds/sds.h lib/
cp sds/sdsalloc.h lib/
cp utf8proc/utf8proc.c lib/
cp utf8proc/utf8proc.h lib/
cp utf8proc/utf8proc_data.c lib/

autoreconf --install || exit 1
