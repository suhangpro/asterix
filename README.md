# asterix
Asterix and the Bazaar

## Usage
* compile: 
```bash
make
```
* generate graph: 
```bash
# p2p.conf is a configuration file containing information about ip address/port/etc
echo -e "3 \n127.0.0.1 4 1101 \n127.0.0.2 6 92" > p2p.conf
# graph and peer information will be generate in file "peers"
./generate_graph p2p.conf peers
```
