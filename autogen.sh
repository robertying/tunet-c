#!/bin/bash

mkdir -p m4
autoreconf --install --force || exit 1
