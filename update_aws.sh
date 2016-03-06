#!/bin/bash
make
scp -i group4.pem `cat aws-files` group4@ec2-54-147-232-101.compute-1.amazonaws.com:~/
echo Update ec2-54-147-232-101.compute-1.amazonaws.com successfully. 
scp -i group4.pem `cat aws-files` group4@ec2-54-159-217-74.compute-1.amazonaws.com:~/
echo Update ec2-54-159-217-74.compute-1.amazonaws.com successfully. 

