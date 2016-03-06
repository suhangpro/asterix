# asterix
Asterix and the Bazaar

## Usage
* compile everything: 
```bash
make
```
* generate graph: 
```bash
# p2p.conf is a configuration file containing information about ip address/port/etc
# This example provides 2 addresses (127.0.0.1 & 127.0.0.2), the numbers of peers 
# on them (4 & 6), the starting point of available ports (1101 & 92), and the degree 
# of the requested graph (3). 
echo -e "3 \n127.0.0.1 4 1101 \n127.0.0.2 6 92" > p2p.conf
# graph and peer information will be generate in file "peers"
./generate_graph p2p.conf peers
```
* let it run! 
```bash
# while running, accepts q--quit, i--info
./asterix.sh peers
```
