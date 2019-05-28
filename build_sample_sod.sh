#!/bin/bash



gcc bin/sod.o src/sample_sod.c \
-Isrc \
-Ofast \
-march=native \
-Wall \
-lm \
-lSDL2 \
-lSDL2 \
-lGLESv2 \
-lEGL \
-obin/sample_sod
