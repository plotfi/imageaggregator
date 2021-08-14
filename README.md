# imageaggregator
imageaggregator is a daemon I wrote in undergrad that listens for incoming images and stitches then together using Magick++

build.sh to build; Currently nothing builds.

IGs should connect to port 31338
Clients should connect to port 5678
Defaults to connecting on port 9012 to the AC.
src/imageaggregator.cpp has #defines of stuff I should really be passing as a run time param
