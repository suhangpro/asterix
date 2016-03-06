#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

typedef struct{
    int peerId;             /* peer id, starting from 0*/	
    string ipAddr;          /* id address */
    int port;               /* port number */
    vector<int> neighbors;  /* peer id of neighbors of a peer */
} peer_t; 

bool cmp_peer(peer_t p1, peer_t p2) {
    return p1.peerId<p2.peerId; 
}

void init_peers(const string, vector<peer_t>&, int*);
void construct_graph(vector<peer_t>&, int); 
void write_peers(const string, const vector<peer_t>&); 

int main(int argc, char* argv[]) {
    if (argc < 2 || argc >3) {
        cerr << "Usage: " << argv[0] << " CONF [PEERS]" << endl;
        return 1;
    }
    string outputPath = "peers"; 
    if (argc==3) {
        outputPath = argv[2];
    }
      
    vector<peer_t> peers; 
    int degree; 

    /* read peer configration file */
    init_peers(string(argv[1]), peers, &degree);
    if (degree>=peers.size()) {
        cerr <<"Error: degree should be smaller than #peers!"<< endl;
        return 1; 
    }
    /* randomly construct graph */ 
    construct_graph(peers, degree);   
    /* sort peers w.r.t. peer id */
    sort(peers.begin(), peers.end(), cmp_peer); 
    for (vector<peer_t>::iterator it=peers.begin(); it!=peers.end();++it) 
        sort(it->neighbors.begin(),it->neighbors.end());
    /* write out peers */
    write_peers(outputPath, peers);

    return 0;
}

void init_peers(const string confFilePath, vector<peer_t>& peers, int* degree) {
    ifstream confFile(confFilePath.c_str());
    if (confFile.is_open()) {
        peer_t currPeer;
        string currIpAddr, nextIpAddr; 
        int nPeers;
        int startPort; 
        int total = 0; 
        
        confFile>>*degree; /* first number is the degree of the graph */
        confFile>>nextIpAddr;
        while(nextIpAddr.size()>0) {
            currIpAddr = nextIpAddr; 
            nextIpAddr = "";	
            confFile>>nPeers>>startPort;
            confFile>>nextIpAddr;
            for (int i=0;i<nPeers;++i) {
                currPeer.peerId = total++; 
                currPeer.ipAddr = currIpAddr;
                currPeer.port = startPort + i;
                peers.push_back(currPeer);
            }
        }
        confFile.close(); 
    }
}

void construct_graph(vector<peer_t>& peers, int degree) {
    int i,j;    
    bool relaxed; 
    for (i=0;i<peers.size();++i) {
        j = i+1; 
        relaxed = false; 
        while(peers[i].neighbors.size()<degree) {
            if (j>=peers.size() || relaxed) {
                relaxed = true;
                j = j%peers.size();
                random_shuffle(peers.begin()+j, peers.begin()+i-1);
            } else {
    	        random_shuffle(peers.begin()+j, peers.end());
            }
            if ((!relaxed && (peers[j].neighbors.size()<degree)) || 
                    (relaxed && (peers[j].neighbors.size()<degree+1) 
                    && (find(peers[j].neighbors.begin(),peers[j].neighbors.end(),peers[i].peerId)==peers[j].neighbors.end()))) {
                peers[j].neighbors.push_back(peers[i].peerId);
                peers[i].neighbors.push_back(peers[j].peerId); 
            }
            ++j; 
        }
    }
}

void write_peers(const string outputPath, const vector<peer_t>& peers) {
    ofstream outputFile(outputPath.c_str());
    if (outputFile.is_open()) {
        outputFile<<peers.size()<<endl;
        for (vector<peer_t>::const_iterator it = peers.begin() ; it != peers.end(); ++it) {
            outputFile<<it->peerId<<" "<<it->ipAddr<<" "<<it->port<<" ";
            outputFile<<it->neighbors.size()<<" ";
            if (it->neighbors.size()>0) {
                outputFile<<it->neighbors[0];
                for (vector<int>::const_iterator it2 = it->neighbors.begin()+1; it2 != it->neighbors.end(); ++it2) 
                    outputFile<<" "<<*it2;
            }
            outputFile<<endl;
        }
        outputFile.close();
    }
}
