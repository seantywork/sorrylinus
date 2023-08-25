# sorrylinus

===========


I'm sorry Linus, for building the most useless software on earth to handle remotely

the most useful software on earth

**Disclaimer : I am not liable for any misuse of this code**
**Only use it on machines and in environments that you have explicit permissions and authrizations to do so**

## Public access point for controlling sorrylinus socket client

[feebdaed.xyz](https://feebdaed.xyz)

## How to use it

1. 
#### Get Raspberri Pi, which is primarily-intended platform of my choice, with Ubunt 20.04 for Raspberry Pi 

**Please be noted**: this program has never been run on any other platform, so I can't guaruantee it's going to run!

**Exact spec of tested platform**: Noted in /docs/log.md. But I'm sharing that also on here.

uname -a:

```bash
Linux ubuntu-rpi 5.4.0-1069-raspi #79-Ubuntu SMP PREEMPT Thu Aug 18 18:15:22 UTC 2022 aarch64 aarch64 aarch64 GNU/Linux
```

lscpu:

```bash
Architecture:                    aarch64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
CPU(s):                          4
On-line CPU(s) list:             0-3
Thread(s) per core:              1
Core(s) per socket:              4
Socket(s):                       1
Vendor ID:                       ARM
Model:                           4
Model name:                      Cortex-A53
Stepping:                        r0p4
CPU max MHz:                     1400.0000
CPU min MHz:                     600.0000
BogoMIPS:                        38.40
L1d cache:                       128 KiB
L1i cache:                       128 KiB
L2 cache:                        512 KiB
```

lsblk:

```bash
NAME        MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
loop0         7:0    0 57.9M  1 loop /snap/core20/1614
loop1         7:1    0   61M  1 loop /snap/lxd/22761
loop2         7:2    0 40.6M  1 loop /snap/snapd/16299
mmcblk0     179:0    0 14.9G  0 disk
├─mmcblk0p1 179:1    0  256M  0 part /boot/firmware
└─mmcblk0p2 179:2    0 14.6G  0 part /
```

2. 
#### Get the access to your Raspberry Pi machine

it doesn't matter how you get the access, by ssh (which I prefer), by plugging in monitor and keyboard and all.


3.
#### Hit ./run at the root of the clone repository

it's gonna handle preparation for you

You have to type in the SSID of your choice(not the one for your internet connection, but one to which your RPi gets coverted during the preparation)

and also the passphrase for the AP

After the preparation is finished, the machine will be rebooted,

and when you hit ./run again, the server will be up and running on -> 0.0.0.0:8080 (hopefully!)

if anything goes sideway, don't forget that Google is my god and your god too.

(or you could open Issue or PR)

4.
#### Now, AP Name you typed in should be visible to you through WiFi setting

Connect to it. Default Address is 

**192.168.4.1:8080**

5.
#### Further updates will follow...

But I have no idea how quickly it will...


License
-------

Refer to LICENSE file

Thanks To
-----------------
- Http Lib
[Yuji Hirose](https://github.com/yhirose/cpp-httplib)
- Ws Lib
[Peter Thorson](https://github.com/zaphoyd/websocketpp)
- JSON Parser
[Niels Lohmann](https://github.com/nlohmann/json)
