#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=$2
Name=DiffImg
Comment=Image comparison tool
Type=Application
GenericName=image comparison tool
TryExec=$1/bin/diffimg
Exec=$1/bin/diffimg
Categories=Utility;Application;
Icon=$1/share/pixmaps/diffimg.png
EOF
