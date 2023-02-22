#!/bin/bash
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
sudo cp stopsmoking /usr/bin/stopsmoking
sudo chown root:root /usr/bin/stopsmoking
sudo chmod +x /usr/bin/stopsmoking

echo "Copy configurer"
sudo cp stopsmoking-config /usr/bin/stopsmoking-config
sudo chown root:root /usr/bin/stopsmoking-config
sudo chmod +x /usr/bin/stopsmoking-config

echo "Copy polybar client"
pkill stopsmoking-pol
sudo cp stopsmoking-polybar /usr/bin/stopsmoking-polybar
sudo chown root:root /usr/bin/stopsmoking-polybar
sudo chmod +x /usr/bin/stopsmoking-polybar

echo "Copy polybar module script"
sudo cp run_polybar_stopsmoking.sh /usr/bin/run_polybar_stopsmoking.sh
sudo chown root:root /usr/bin/run_polybar_stopsmoking.sh
sudo chmod +x /usr/bin/run_polybar_stopsmoking.sh

echo "Runing initial config"
/usr/bin/stopsmoking-config -c 20 -s 8 -f 23 -t 20

echo "Copy service file"
sudo cp stopsmoking.service /etc/systemd/system/stopsmoking.service
sudo chown root:root /etc/systemd/system/stopsmoking.service

echo "Enable service"
sudo systemctl enable stopsmoking

echo "Start service"
sudo systemctl restart stopsmoking