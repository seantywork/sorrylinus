#!/bin/bash

make test-mod-build
sudo make test-mod-link
make build-test
# make test-build