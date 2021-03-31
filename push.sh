#!/usr/bin/env
branch_name=$(git rev-parse --symbolic-full-name --abbrev-ref HEAD)
#echo $branch_name
git add .
git commit -m "added task"
#git push origin $BRANCH
git push --set-upstream origin $BRANCH
echo pushed
