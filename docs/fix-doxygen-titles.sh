#!/bin/bash -ex

# Assume doxygen was run first
pushd "`dirname "$0"`"/html

find . -name "*.html" | while read ln
do
  # Make the output SEO friendly by converting the "div" title to the proper "h1"
  sed -i -e 's|<div class="title">\([^<]*\)</div>|<h1>\1</h1>|' "$ln"
done
find . -iname "*.html-e" | xargs rm

popd
