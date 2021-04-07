#!/bin/bash
BGreen='\033[1;32m'
BBlue='\033[1;34m'
NC='\033[0m'
ts_list=(`ls translations/*.ts`)
for ts in "${ts_list[@]}"
do
    printf "\nProcess ${ts}\n"
    lrelease "${ts}"
done
strip libnetspeed.so
sudo cp libnetspeed.so /usr/lib/dde-dock/plugins/libnetspeed.so
echo -e "Copy ${BGreen}libnetspeed.so${NC} to ${BBlue}/usr/lib/dde-dock/plugins/${NC}"
sudo mkdir -p /usr/share/dde-dock-netspeed/translations
sudo cp translations/*.qm /usr/share/dde-dock-netspeed/translations/
echo -e "Copy ${BGreen}*.qm${NC} to ${BBlue}/usr/share/dde-dock-netspeed/translations/${NC}"
killall dde-dock
