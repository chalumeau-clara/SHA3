# SHA3
Supported mode sha3-224, sha3-256, sha3-384, sha3-512

## How to use it

By default sha3-256

-m [mode] for mode 
mode 224 or 256 or 384 or 512

````shell
./sha3 -m 256 file1 file2 ...
````

For shake : 

shake mode 128 or 256
-s for shake and -o for output_lengh

````shell
./sha3 -s 256 -o [output_lengh] file1 file2 ...
````


### Based on the documentation
https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf

https://keccak.team/files/Keccak-reference-3.0.pdf

### Test online
https://emn178.github.io/online-tools/shake_256_checksum.html