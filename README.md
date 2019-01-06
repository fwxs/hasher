# hasher

A basic C++ program to hash a single file or all the files in a directory, using the OpenSSL library.

## Usage

### Single file

```
$ ./hasher <file> <hash function> -r

[*] Hashing <file size> bytes.
[*] Output: <hash digest result>
```

The ```-r``` switch, hashes all the files inside a directory.

### Directory
```
[*] Hashing <files in the directory> files in <directory name>
        [+] Hashing <file name>    size <file size> bytes
        [-] Output: <hash digest result>
...
 	...
	...
```
