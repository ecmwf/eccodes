#!/bin/bash

# if no command line parameters found
if [ $# -eq 0 ]; then
    file=$(git  status -s | grep "^ M" | sed "s/\s*\s/ /g" | cut -f 3 -d" " | head -1)
    if [ -z "$file" ]; then
        echo "No modified files found."
        exit 1
    fi
else
    file=$1
fi

echo "./src/accessor/$file"

#diff --color=always /home/joobog/git/eccodes_develop/src/accessor/$file $file
diff -u /home/joobog/git/eccodes_develop/src/accessor/$file $file | diff-so-fancy --colors | less -R

git status -s | grep "^M " | wc -l
git status -s | grep "^ M " | wc -l

read -p "Enter y or n: " input
if [ "$input" = "y" ]; then\
    git add $file
else
    echo "Skipping..."
fi
