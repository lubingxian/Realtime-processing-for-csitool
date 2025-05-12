# Realtime-processing-for-csitool
A realtime data processing and visualization plugin for the [Linux 802.11n CSI Tool](https://github.com/dhalperi/linux-80211n-csitool).

# Usage
To use this code, you will need all the files from https://github.com/dhalperi/linux-80211n-csitool-supplementary.

1. In matlab:
~~~
run read_bf_socket using Matlab
~~~

2. In netlink:
~~~
gcc log_to_server.c -o log_to_server
sudo ./log_to_server <ip> <port>
~~~
\<ip\> is the ip address of the computer which runs the Matlab script, and \<port\> is 8090 by default. (e.g. if you run the CSI Tool and the Matlab script on the same computer: sudo ./log_to_server 127.0.0.1 8090)

NOTE: If you don't have the iwl_connector.h file in netlink (which means you are using an early verison of the [Linux 802.11n CSI Tool](https://github.com/dhalperi/linux-80211n-csitool)), please use our [old version](https://github.com/lubingxian/Realtime-processing-for-csitool/tree/master/netlink/old%20version) code.

# Screenshot
<img src="pic1.png" width = "60%" />

# Contact
[Bingxian Lu](https://lubingxian.cn)   
Email: bingxian.lu@gmail.com

To cite this work, the best reference is our paper published in IEEE SECON 2016:

Bingxian Lu, Zhicheng Zeng, Lei Wang, Brian Peck, Daji Qiao, and Michael Segal. 
Confining Wi-Fi Coverage: A Crowdsourced Method Using Physical Layer Information. 
IEEE SECON, 2016.

## Stargazers over time

[![Stargazers over time](https://starchart.cc/lubingxian/Realtime-processing-for-csitool.svg)](https://starchart.cc/lubingxian/Realtime-processing-for-csitool)

<img src='//clustrmaps.com/map_v2.png?cl=ffffff&w=a&t=n&d=C_lfSGxZoZOnpCSuuLKo7s6xxPKDte61nwqToqC-sMI' width = "60%" />
