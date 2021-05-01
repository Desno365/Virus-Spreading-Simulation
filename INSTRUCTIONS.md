# Instructions for distributed clustering with MPI

To proceed with the installation follow this [guide](https://mpitutorial.com/tutorials/running-an-mpi-cluster-within-a-lan/).

* On both the machines configure the /etc/hosts file by indicating the addresses of both the manager and the worker, like in this example:
```
192.168.1.9 worker
192.168.1.12 master
```

* As specified in the guide you should clone the repository inside the shared folder among the two.

* The content of the hostfile should be something like this:
```
localhost slots=2 max-slots=2
worker slots=1 max-slots=1
```

* Pay attention to the configuration of the firewall. Use these commands on all the machines:
```
sudo ufw disable
sudo iptables -P INPUT ACCEPT
sudo iptables -P OUTPUT ACCEPT
sudo iptables -P FORWARD ACCEPT
sudo iptables -F
```

* You can then use the command `make distribute` in order to launch the computation distributed among the multiple machines.
