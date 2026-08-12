#!/bin/sh

# HELP: Mon application personnelle
# ICON: mon_icon
# GRID: Sokoban

. /opt/muos/script/var/func.sh

APP_BIN="sokoban-arm64"
SETUP_APP "$APP_BIN" ""

# Optionnel :
# SETUP_STAGE_OVERLAY

cd /run/muos/storage/application/sokoban

./$APP_BIN

