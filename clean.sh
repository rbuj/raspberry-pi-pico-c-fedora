#!/bin/sh -x
find . -name build -exec rm -fr {} \;
find . -name .cortex-debug.registers.state.json  -exec rm -fr {} \;
find . -name .cortex-debug.peripherals.state.json  -exec rm -fr {} \;
