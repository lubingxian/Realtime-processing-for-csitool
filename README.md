# Realtime-processing-for-csitool
A realtime data processing and visualization plugin for the [Linux 802.11n CSI Tool](https://github.com/dhalperi/linux-80211n-csitool).

# Usage
To use this code, you will need all the files from https://github.com/dhalperi/linux-80211n-csitool-supplementary.

1. In netlink:
~~~
gcc log_to_server.c -o log_to_server
sudo ./log_to_server <ip> <port>
~~~
Tips: If you are using an early verison of the [Linux 802.11n CSI Tool](https://github.com/dhalperi/linux-80211n-csitool) (or the log_to_server doesn't work), please try our old version code.

2. In matlab:
run read_bf_socket.m

Tips for Matlab version > R2014a: 
We use EraseMode to increase the rendering speed. However, starting in R2014b, the EraseMode property has been removed from all graphics objects. https://mathworks.com/help/matlab/graphics_transition/how-do-i-replace-the-erasemode-property.html

# Screenshot
<img src="pic1.png" width = "60%" />

# Contact
Email: bingxian.lu@gmail.com
