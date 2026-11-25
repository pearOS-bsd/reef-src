#!/bin/bash
# nd.sh ADDR NAME [context|-s]: diff of an original function and ours, NAME as in the source (host shell)
docker exec rb python3 -B /tk/app/text/nd.py "$@"
