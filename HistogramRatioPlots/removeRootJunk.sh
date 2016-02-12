#!/bin/bash

fileExt=( *.pcm *.so *.d)
echo "Do you want to delete?"
echo "The following extensions will be deleted" ${fileExt[@]}

for i in ${fileExt[@]}
do 
	echo "Deleting the files:  $i"
	rm $i;
done
