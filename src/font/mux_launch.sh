#!/bin/sh

# HELP: Mon application personnelle
# ICON: mon_icon
# GRID: font

. /opt/muos/script/var/func.sh

APP_BIN="testfont-arm64"
SETUP_APP "$APP_BIN" ""

# Optionnel :
# SETUP_STAGE_OVERLAY

cd /run/muos/storage/application/font

./$APP_BIN

