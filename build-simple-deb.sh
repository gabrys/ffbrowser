#!/bin/bash

cd "`dirname "$0"`"
make && checkinstall -y --maintainer "Piotr Gabryjeluk <piotr@gabryjeluk.pl>" --nodoc --pkggroup web --requires libqtwebkit4 cp ffbrowser /usr/bin

