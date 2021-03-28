# Instructions for distributed clustering with MPI
To proceed with the installation follow this [guide]{https://mpitutorial.com/tutorials/running-an-mpi-cluster-within-a-lan/}.

On both the machine configure the /etc/hosts file, by indicating the address of both the manager and the worker ids, like in this example:
```
192.168.1.9 worker
192.168.1.12 master
```
As specified in the guide you should clone the repository inside the shared folder among the two.
You can then use the command:
`make distribute`
in order to launch the computation distributed among different machine.
The content of the hostfile should be something like this:
```
localhost slots=2 max-slots=2
worker slots=1 max-slots=1
```
Pay attention to the configuration of the firewall. Use this command on all the machine:
```
sudo ufw disable
sudo iptables -P INPUT ACCEPT
sudo iptables -P OUTPUT ACCEPT
sudo iptables -P FORWARD ACCEPT
sudo iptables -F
```
