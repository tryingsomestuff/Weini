#!/bin/bash

dir=$(readlink -f $(dirname $0)/../)

cd $dir

tagname=$1
git checkout --orphan $tagname
git commit
git tag $tagname
git tag
git push github refs/tags/$tagname
git checkout master

