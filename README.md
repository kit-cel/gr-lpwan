
This is a GNU Radio out-of-tree module and contains an implementation of the new LECIM DSSS PHY layer of IEEE802.15.4.

## Features

- LECIM DSSS physical layer of IEEE802.15.4 standard
- Transmitting and receiving in realtime
- Multiple example flowgraphs
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


## Test Platform

- Ubuntu 16.04
- GNU Radio v3.7.10.1
- B210 USRP