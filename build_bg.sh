#!/bin/bash



gcc src/bg.c \
-Isrc \
-Ofast \
-march=native \
-Wall \
-lm \
-lSDL2 \
-lGLESv2 \
-lEGL \
-obin/bg
