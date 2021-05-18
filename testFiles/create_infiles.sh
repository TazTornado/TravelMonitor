#!/bin/bash

# Usage: ./create_infiles.sh inputFile inputDir numFilesPerDirectory



#---------------------------------------------------------------------------------------------------#

# check argument 1 - inputFile
if [[ -n $1 ]] && [[ -f $1 ]]; then
	inputFile=$1
	echo inputFile: $inputFile
else
    echo Error in arguments! 
	echo Usage: ./testFile.sh inputFile inputDir numFilesPerDirectory 
	exit 1
fi

# check argument 2 - inputDir
if [[ ! -d "$2" ]]; then
	inputDir=$2
	mkdir -p "$inputDir"
	echo created directory: $inputDir
else
	echo Error! Directory already exists.
	echo Usage: ./testFile.sh inputFile inputDir numFilesPerDirectory 
	exit 1
fi

# check argument 3 - numFilesPerDirectory
if [[ -n $3 ]]; then
	numFilesPerDirectory=$3
else
	echo Error in arguments!
	echo Usage: ./testFile.sh inputFile inputDir numFilesPerDirectory 
	exit 1
fi

for subdir in $(ls $inputDir); do
	echo $subdir
done