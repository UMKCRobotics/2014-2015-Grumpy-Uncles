#!/bin/bash

RUNLEVEL=$(echo "$(runlevel)" | cut -d ' ' -f 2)
if [[ $RUNLEVEL -eq 4 ]]; then
	echo "self-startup requested."
	gadget
fi

