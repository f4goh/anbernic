#!/bin/sh

# HELP: Mon application personnelle
# ICON: mon_icon
# GRID: Image

. /opt/muos/script/var/func.sh

APP_BIN="testpng-arm64"
SETUP_APP "$APP_BIN" ""

# Optionnel :
# SETUP_STAGE_OVERLAY

cd /run/muos/storage/application/image

./$APP_BIN

