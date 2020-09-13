#!/bin/sh
cat <<EOF
.TH diffimg 1 "January 10, 2014" "version $1" "USER COMMANDS"
.SH NAME
diffimg - Image difference viewer
.SH SYNOPSIS
.B diffimg [options] img1 img2
.I option option
.B ["
.I --help --reset-config
.B ..."] 
.SH DESCRIPTION
DiffImg is a simple image comparison tool which take two images with the same size as input. Some statitics are computed and the positions where pixel differ are displayed as a color mask.
.SH OPTIONS
DiffImg have some options but uses also argument as input files.  As a Qt application it supports Qt options; please see:
http://doc.trolltech.com/4.2/qapplication.html#QApplication
.TP 5
--help
The usage of the command
.TP
-reset-config
clear the saved preference parameters 
.SH AUTHOR
xbee (xbee (at) xbee.net)
EOF