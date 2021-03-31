#!/usr/bin/env
BRANCH = $(git symbolic-ref -q HEAD)
echo $BRANCH
git add .
git commit -m "added task"
git push origin $BRANCH
