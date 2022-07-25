DongshanNezhaSTU Borad DLNA Screen Projection Project
---
## Get DongshanNezhaSTU Tina-SDK 
### Get Tina-SDK V2.0
Open https://d1.docs.aw-ol.com/en/study/study_2getsdk/

### Get DongshanNezhaSTU Patch 

git clone  https://github.com/DongshanPI/Tina-sdk_dongshannezhastu

### Patch Apply And Make

cp Tina-sdk_dongshannezhastu/* -rfvd  tina-d1-h/

Use config_gstreamer Config to .config


## Get Puppnp tprender

### make puppnp

```bash
https://github.com/pupnp/pupnp/releases/download/release-1.14.12/libupnp-1.14.12.tar.bz2

export PATH=$PATH:/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/toolchain/bin
./configure --host=riscv64-unknown-linux-gnu

make 
mkdir tmp
make install DESTDIR=/home/book/Allwinner/libupnp-1.14.12/tmp/
```

### make tprender
vim CMakeLists.txt
```bash
 SET(CROSS_COMPILE 1)
 set(CMAKE_SYSTEM_NAME Linux)
 #CROSS_COMPILE Path
 set(CMAKE_C_COMPILER "/home/book/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/bin/riscv64-unknown-linux-      gnu-gcc")
 #Link LibsPath
link_directories(
                ./libs
                /home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/lib/
)

aux_source_directory(src SOURCE)

include_directories(./)
include_directories(./include)
include_directories(./glib-2.0)
#Include Path
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/)
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/allwinner/)
include_directories(/home/book/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/include/allwinner/include)


```

### How To Use

1. Use  `PhoenixSuit.exe`  Flash Tina-sdk Out dir `tina_d1-h-nezha_uart0.img` to SPI Nand.

2. Use  xr829 wifi connect Network，To enter the d1 system, first configure a wifi information.
Mine is wifi channel name is 100ask password is 100ask.net you replace wifi name and password with yours.
```shell
wifi_connect_ap_test 100ask 100ask.net
```
Pay attention to see ifconfig wlan0 has an ip4 address. If not, use the following command to get it
```shell
udhcpc -i wlan0
```
The first time you run wifi_connect_ap_test, it will automatically get the ip4 address, but the next time you turn it on, it will automatically connect to the wifi but will not automatically get the ip4 address, so check it out

3. The attachment provides tprender_app which contains compiled tprender and upnp libraries
Send tprender_app to D1 with adb
```shell
adb push d:tprender_app /root/
```
enter the directory
```shell
cd /root/tprender_app
```
Copy the library inside to the system path
```shell
cp libs/* /usr/lib/
```
Add tprender execute permission
```shell
chmod +x tprender
```
Enable DLNA client
```shell
./tprender -f "D1"
```
Next, you can use the B station to cast the screen
Operation Tips
There is a TV icon in the upper right corner of the station B when the video is played, click it, and then click the screen casting device.
When station B is updated to the new version, the screen resolution can only be selected
