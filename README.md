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
make
sudo make install
```

Restart Krunner. Then check this plugin is enabled in Krunner settings. Your pidgin buddies should be searchable on Krunner.
