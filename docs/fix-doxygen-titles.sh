#!/bin/bash -ex

# Assume doxygen was run first
current_directory=$(dirname "$0")

# Run this in a temp directory because we can't write to the html directory
mkdir "$current_directory"/modified-doxygen-output

find "$current_directory/html" -name "*.html" | while read ln
do
  tempfile=$(mktemp)

  # Make the output SEO friendly by converting the "div" title to the proper "h1"
  sed -e 's|<div class="title">\([^<]*\)</div>|<h1>\1</h1>|' "$ln" > "$tempfile"
  filename=$(basename "$ln")

  # Move the temp file to the modified-doxygen-output where we will upload it from
  mv "$tempfile" "docs/modified-doxygen-output/$filename"
done
