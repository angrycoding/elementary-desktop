#!/bin/bash
cd "$(dirname "$0")"
THEME=$(gsettings get org.gnome.desktop.interface icon-theme | tr -d "'")
THEME="/usr/share/icons/${THEME}"
OSNAME="$(cat /etc/os-release)"
devilspie2 & npm start $THEME $OSNAME