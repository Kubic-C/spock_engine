#!/bin/bash

# Use with msys or a linux console/terminal

echo "Updating up spock_engine"
git pull
git submodule init
git submodule update
git submodule update --init --recursive