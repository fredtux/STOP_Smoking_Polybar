#!/bin/bash
if [[ $UID != 0 ]]; then
    echo "Please run this script with sudo:"
    echo "sudo make install"
    exit 1
fi

echo "Disable service"
systemctl disable stopsmoking
systemctl stop stopsmoking

echo "Delete service"
rm /usr/bin/stopsmoking

echo "Remove configurer"
rm /usr/bin/stopsmoking-config

echo "Remove polybar client"
rm /usr/bin/stopsmoking-polybar

echo "Remove polybar module script"
rm /usr/bin/run_polybar_stopsmoking.sh

echo "Remove service file"
rm /etc/systemd/system/stopsmoking.service

