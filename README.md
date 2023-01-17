# UDCrypt

A terminal-based file encryption tool.

## Description

<p align = "center">
    <img src = "https://github.com/de-upayan/udcrypt/blob/main/screenshots/Screenshot%20from%202023-01-17%2002-59-24.png" alt = "screenshot" width = "700">
</p>

Creates an encrypted '.ud' file from a given input file (of any type) using an ASCII string key of variable length.
The encrypted file can be decrypted using the correct key only. The original extension of the input file is also
encrypted and stored inside the file, and hence the type of file cannot be ascertained before decrypting.

## Getting Started

### Dependencies

#### Linux
* None
#### Windows
* A terminal emulator that supports the full range of ANSI escape codes (eg. [cmder](https://cmder.app/))

### Installing

* Download the code as ZIP
* Extract the files to a suitable location
#### or
* Clone the repository
```
git clone https://github.com/de-upayan/udcrypt.git
```

### Executing program

* Open terminal and navigate to the directory where the code files are extracted
* Run the executable
#### Linux
```
./udcrypt.out
```
#### Windows 32-bit
```
udcrypt_32bit.exe
```
#### Windows 64-bit
```
udcrypt_64bit.exe
```

## Examples

* A few examples of encrypted files (.jpg, .mp4) are present in the directory *root/examples*
* An additional file *guesswhat.ud* (an undisclosed file of unknown type) is left for you to decrypt and find out yourself! :-)

#### Key
```
de-upayan
```

## Help

* In case of '*permission denied*' error message on linux, make sure you have allowed executing udcrypt.out [as program](https://linuxhint.com/make-file-executable-linux/)

### Compiling from source

#### Linux
```
gcc -o udcrypt.out udcrypt.c
```
#### Windows
```
gcc -o udcrypt.exe udcrypt.c
```

## Author

[Upayan De](https://upayande.ml/)

## Version History

* 0.1
    * Initial Release

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE.md file for details
