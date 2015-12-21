# Magique
A gadget for SMF.

## Network scheme

There are several types of network devices, each of them has unique 16 bit ID, starting from 1. The id of 0 is reserved for broadcast messages.

- Magique stones; ids from 0x01 to 0xFF ; radio strategy is "broadcast & listen"
- Magique sources; ids from 0x100 to 0x1FF ; radio strategy is "listen & answer"
- Magique masters and special nodes; ids from 0xFF00 to 0xFFFF ; no predefined radio strategy

