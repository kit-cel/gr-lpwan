


## Features

- LECIM DSSS physical layer of IEEE802.15.4 standard
- Transmitting and receiving in realtime
- Multiple example flowgaphs
- Fragmentation sublayer for splitting frames in smaller 
fragments and retransmission on transmission failure 


## Install instructions for Linux

```
git clone git://github.com/kit-cel/gr-lpwan
cd gr-lpwan
mkdir build
cd build
cmake ..
make
sudo make install
```

After installation it's necessary to generate some hierarchical 
flowgraphs within GRC. These flowgraphs are located in examples/dsss/hier. 


## Platform

- Ubuntu 16.04
- GNU Radio v3.7.10.1
- B210 USRP
