#!/bin/sh

# HELP: Mon application personnelle
# ICON: mon_icon
# GRID: Input

. /opt/muos/script/var/func.sh

APP_BIN="testinput-arm64"
SETUP_APP "$APP_BIN" ""

# Optionnel :
# SETUP_STAGE_OVERLAY

cd /run/muos/storage/application/input

./$APP_BIN

