# Old version
If you are using an early verison of the Linux 802.11n CSI Tool (or the log_to_server doestn't work), please try this old version.

# Usage
Replace the original log_to_file.c with our version.

Modify the ip address and port number in serv_port_ip.conf.
~~~
make
sudo ./log_to_file serv_port_ip.conf
~~~
