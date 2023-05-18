# CRC-IDECC

This project consist in a Introduction, Detection, and Error Correction on Content using CRC algorithm.

## Analysis of Capacity

Given the following polynomial that we have chosen to use in our CRC algorithm:

$G(x) = x^{7} + x^{5} + x^{4} + x^{3} + x^{1} + x^{0}$

(in binary: `0b10111001`)

We can determine that the hamming distance of this polynomial is 4, which means that we can detect up to 3 errors and correct up to 1 error.

## Overall Architecture

The established architecture is capable of handling multiple clients by connecting to a proxy, which is in turn connected to a server, using the TCP protocol with multi-threading support.

To send a message, the client inserts a character and then calculates its CRC (Cyclic Redundancy Check), which is concatenated to form a 2-byte packet:

```
+-----------+-----------+
| Message   | CRC       |
| (1 byte)  | (1 byte)  |
+-----------+-----------+
```

The client then sends the packet to the proxy, which modifies random bits in the first byte representing the message.

Note: We assume that the byte representing the CRC cannot be altered.

The proxy then forwards the modified packet to the server, which verifies its validity by performing a polynomial division in the binary field $F_2[X]$

![Architecture](https://i.imgur.com/zal9jgu.png)

Two cases can occur:

- If the remainder of the polynomial division is zero, there are no errors.
- If the remainder is non-zero, an error is detected and can be corrected by comparison with a software register.

If the server fails to correct the error, it sends a predefined value of `0xFFFF`, indicating that the packet could not be corrected. The client then resends the packet until the server manages to correct the error.

## Usage
In root directory:
```
make
```
you will need to execute in order:
### Server
```
./bin/project-rio --server --server-address=localhost --server-port=9876
```
### Proxy
```
./bin/project-rio --proxy --proxy-address=localhost --proxy-port=9800 --server-address=localhost--server-port=9876
```
### Client
```
./bin/project-rio --client --proxy-address=localhost --proxy-port=9800
```

**Note:** You can use the `--help` flag to see the available options for each program.


## Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/index.html)
- [Ross N. Williams. : A painless guide to crc error detection algorithms](https://www.ross.net/crc/download/crc_v3.txt)

## Authors
- [Alexander Yanovskyy](https://yanovskyy.com/)
- Roman Bonnet
- Jean-Raphäel Julien
- Romain Bourdain