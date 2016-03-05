# asterix
Asterix and the Bazaar

## Usage
* compile: 
```bash
make
```
* generate graph: 
```bash
# test.conf is a configuration file containing information about ip address/port/etc
# see p2p.conf for another example
echo -e "3 \n127.0.0.1 4 1101 \n127.0.0.2 6 92" > test.conf
# graph and peer information will be generate in file "peers"
./generate_peers test.conf peers
```
