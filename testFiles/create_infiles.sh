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


declare -A write_next		# holds which file to write in next, for each country

while IFS= read -r line; do
	record=($line)
	country=${record[3]}
	subdirs=($(ls $inputDir))

	if [[ ! "${subdirs[@]}" =~ "$country" ]]; then		# first time reading this country
		mkdir "$inputDir/$country"						# create country directory
		write_next[$country]=0		

		for(( i = 1; i <= $numFilesPerDirectory; i++ )){		# create all 'numFiles' empty files
			touch "$inputDir/$country/$country-$i.txt"
		}
	fi

	fileIndex=${write_next[$country]}
	((fileIndex++))

	echo ${record[@]} >> "$inputDir/$country/$country-$fileIndex.txt"	# write record in the right file
	
	((fileIndex++))
	write_next[$country]=$fileIndex
	write_next[$country]=$((${write_next[$country]} % $numFilesPerDirectory))	# fix index for next file

done < $inputFile

for directory in $(ls $inputDir); do
	for file in $(ls "$inputDir/$directory"); do
		if [[ ! -s "$inputDir/$directory/$file" ]]; then
			echo Deleting "$inputDir/$directory/$file"...
			rm "$inputDir/$directory/$file"							# remove all empty files
		fi
	done
done