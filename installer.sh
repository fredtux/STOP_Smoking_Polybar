#!/bin/bash
if [[ $UID != 0 ]]; then
    echo "Please run this script with sudo:"
    echo "sudo make install"
    exit 1
fi

echo "Create service file"
echo "[Unit]
Description=Stop Smoking Service
After=network.target

[Service]
Type=forking
ExecStart=/usr/bin/stopsmoking
StandardOutput=append:/var/log/stopsmoking1.log
StandardError=append:/var/log/stopsmoking2.log
StandardInput=null
User=${USER}

[Install]
WantedBy=multi-user.target"> stopsmoking.service

echo "Copy service"
cp stopsmoking /usr/bin/stopsmoking
chown root:root /usr/bin/stopsmoking
chmod +x /usr/bin/stopsmoking

echo "Copy configurer"
cp stopsmoking-config /usr/bin/stopsmoking-config
chown root:root /usr/bin/stopsmoking-config
chmod +x /usr/bin/stopsmoking-config

echo "Copy polybar client"
cp stopsmoking-polybar /usr/bin/stopsmoking-polybar
chown root:root /usr/bin/stopsmoking-polybar
chmod +x /usr/bin/stopsmoking-polybar

echo "Copy polybar module script"
cp run_polybar_stopsmoking.sh /usr/bin/run_polybar_stopsmoking.sh
chown root:root /usr/bin/run_polybar_stopsmoking.sh
chmod +x /usr/bin/run_polybar_stopsmoking.sh

echo "Runing initial config"
/usr/bin/stopsmoking-config -c 20 -s 8 -f 23 -t 20

echo "Copy service file"
cp stopsmoking.service /etc/systemd/system/stopsmoking.service
chown root:root /usr/bin/stopsmoking.service

echo "Enable service"
systemctl enable stopsmoking