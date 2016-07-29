#!/bin/bash

clear;

g++ -std=c++11 main.cpp Component.h Component.cpp -o main `pkg-config --libs --cflags opencv`
