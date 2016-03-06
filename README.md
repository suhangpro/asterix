# asterix
Asterix and the Bazaar

## Usage
* compile everything: 
```bash
make
```
* generate graph: 
```bash
# "p2p.conf" is a configuration file
# graph and peer information will be generate in file "peers"
./generate_graph p2p.conf peers
```
* let it run! 
```bash
# while running, accepts q--quit, i--info, l--list
./asterix.sh peers
```
