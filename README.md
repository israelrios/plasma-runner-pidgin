# Pidgin Krunner Plugin 

Krunner plugin that provides the list of buddies in Pidgin making it possible to start a conversation from Krunner.

Based on https://github.com/freexploit/plasma-runner-pidgin.


## Installation:


```sh
mkdir build
cd build
cmake ../ \
-DCMAKE_INSTALL_PREFIX=`kf5-config --prefix` \
-DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

Restart Krunner. Then check this plugin is enabled in Krunner settings. Your pidgin buddies should be searchable on Krunner.


## Required Build Dependencies

Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5kio-dev gettext`

OpenSUSE:  
`sudo zypper install cmake extra-cmake-modules libQt5Widgets5 libQt5Core5 ki18n-devel kservice-devel krunner-devel kio-devel gettext-tools`  

Fedora:  
`sudo dnf install cmake extra-cmake-modules kf5-ki18n-devel kf5-kservice-devel kf5-krunner-devel kf5-kio-devel gettext`  
