#!/bin/bash



g++ src/cvtrack.cpp \
-Isrc \
-Wall \
-Wextra \
-Wundef \
-Wshadow \
-Wpointer-arith \
-Wcast-align \
-Wstrict-overflow=5 \
-Wwrite-strings \
-Wcast-qual \
-Wswitch-default \
-Wswitch-enum \
-Wconversion \
-Wunreachable-code \
-fsanitize=signed-integer-overflow \
-lSDL2 \
-lGLESv2 \
-lEGL \
-lm \
-I/usr/local/include/opencv4 \
-lopencv_core \
-lopencv_highgui \
-lopencv_features2d \
-lopencv_imgproc \
-lopencv_video \
-obin/cvtrack