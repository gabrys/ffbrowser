#!/bin/bash

file="$1"

# prepare tmp place
mkdir tmp-ipk tmp-control tmp-data

# uncompress deb
cd tmp-ipk
ar -x ../"$file"
cd ..

# modify control (armel -> armv4t, add dependencies)
cd tmp-control
tar -xzf ../tmp-ipk/control.tar.gz
sed -i 's/armel/armv4t/' control
sed -i 's/libqtwebkit4/libqtwebkit4, qt4-plugin-imageformat-jpeg, qt4-plugin-imageformat-gif/' control
tar -czf ../tmp-ipk/control.tar.gz .
cd ..

# modify data (add icon & shortcut)
cd tmp-data
tar -xzf ../tmp-ipk/data.tar.gz
mkdir -p usr/share/icons/shr/86x86/apps/ usr/share/applications
cp ../ffbrowser.png usr/share/icons/shr/86x86/apps/
cp ../ffbrowser.desktop usr/share/applications
tar -czf ../tmp-ipk/data.tar.gz .
cd ..

# compress back to ipk
cd tmp-ipk
ipk="`basename "$file" | sed 's/armel.deb/armv4t.ipk/'`"
ar rcs ../"$ipk" debian-binary control.tar.gz data.tar.gz
cd ..

# clean up
rm -rf tmp-ipk tmp-control tmp-data
