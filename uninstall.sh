#!/bin/bash
sudo rm /usr/lib/dde-dock/plugins/libnetspeed.so
BRed='\033[1;31m'
BBlue='\033[1;34m'
NC='\033[0m'
 
sudo rm /usr/lib/dde-dock/plugins/libnetspeed.so
echo -e "Remove ${BRed}libnetspeed.so${NC} from ${BBlue}/usr/lib/dde-dock/plugins/${NC}"
sudo rm -r /usr/share/dde-dock-clock
echo -e "Remove ${BRed}/usr/share/dde-dock-netspeed/${NC}"
killall dde-dock
