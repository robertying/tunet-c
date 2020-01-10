#!/bin/bash

mkdir -p m4
autoreconf --install || exit 1
