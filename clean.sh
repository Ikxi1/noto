#/bin/bash

find . -type f -name '*.o' -delete

rm noto
rm cmake-build-debug/noto

echo "Cleaned project"
