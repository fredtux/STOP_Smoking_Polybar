# Stop smoking systemd service and polybar integration
**Let's hope we're all gonna quit smoking :) But until then, you can use this nice program to reduce the number of cigarettes. It will tell you when to smoke and pull cigarettes out without you knoing. I'm trying it on me as well, so feel free to share your results**

## How it looks like
Screenshot made using "shades" from: [https://github.com/adi1090x/polybar-themes](https://github.com/adi1090x/polybar-themes)

![Screenshot](https://github.com/fredtux/STOP_Smoking_Polybar/blob/main/Screenshot.png?raw=true)

## How to install it
```bash
make
make install
```

Copy **stopsmoking.ini** to your polybar modules folder and load it in the config.ini file then add it wherever you want.

## How to uninstall it
```bash
make uninstall
```

Also keep in mind the **stopsmoking.ini** polybar module. Everyone has a different configuration so I'm not going to make it automatic.

## Files description
1. stopsmoking.c (the service)
2. stopsmoking-config (the configurer)
3. stopsmoking-polybar (client for the service and middleman to polybar)
4. run_polybar_stopsmoking.sh (script to be executed by polybar)
5. stopsmoking.service (created after make install - service file)