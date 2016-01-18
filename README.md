# Magique
A gadget for SMF.

## Network scheme

There are several types of network devices, each of them has unique 16 bit ID, starting from 1. The id of 0 is reserved for broadcast messages.

- Magique stones; ids from 0x01 to 0xFF ; radio strategy is "broadcast & listen"
- Magique sources; ids from 0x100 to 0x1FF ; radio strategy is "listen & answer"
- Magique masters and special nodes; ids from 0xFF00 to 0xFFFF ; no predefined radio strategy

## Hardware trivia

- LiFePO4 battery has 500mA
- Power consumption is:
  - In IDLE 0.040mA @ 3.0V, 0.120mA @ 3.3V
  - Display powered up: 10mA
  - CPU working: 1mA
  - NRF rx/tx: <15mA
  - That is 5000 hours (208 days) doing nothing,
  - 50 hours (2 days) displaying stuff and
  - 20 hours constantly transmitting and displaying
