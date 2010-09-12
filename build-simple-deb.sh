#!/bin/bash

cd "`dirname "$0"`"
make && checkinstall -y --nodoc --pkggroup web --requires libqtwebkit4 cp ffbrowser /usr/bin

