# libIlluminatIR

IlluminatIR synchronizes lighting devices (and possibly other gadgets) in real-time via a unidirectional infrared serial protocol.

**libilluminatir** is the generic library implementing the protocol used by the transmitter and receiver.

Here are some properties of the packets used by this protocol:

- Only 4 to 19 bytes long
- Can update 1 to 16 channels at once
- 256 channels, each 8 bit wide.
- short key-value pairs can be transmitted as well to provide configuration options and/or trigger commands

For more details have a look at the *illuminatir.h* header file.
