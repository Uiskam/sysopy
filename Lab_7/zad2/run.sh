#!/bin/bash
./main & pid=$!
{ sleep 10; kill "$pid"; } &