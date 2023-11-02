#!/bin/bash -ex

# Output this to a temp directory because we can't write to the html directory
# Assume doxygen was run first
pushd "`dirname "$0"`"
cp -R html modified-doxygen-output

find ./modified-doxygen-output -name "*.html" | while read ln
do
  # Make the output SEO friendly by converting the "div" title to the proper "h1"
  sed -i -e 's|<div class="title">\([^<]*\)</div>|<h1>\1</h1>|' "$ln"
done
find ./modified-doxygen-output -iname "*.html-e" -exec rm '{}' \;

popd
