#!/bin/bash



gcc -c src/sod.c \
-lm \
-Ofast \
-march=native \
-Wall \
-std=c99 \
-obin/sod.o