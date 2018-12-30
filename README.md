# tunet-c

C cli and library for Tsinghua University network login.

Support `auth4`, `auth6`, `net`.

## Installation

### Build from source

#### Prerequisites

Make sure common build tools are installed.

For example, on Ubuntu please run

```shell
sudo apt install build-essential dh-autoreconf libssl-dev libcurl4-openssl-dev
```

`openssl` and `curl` should be installed properly. `libssl`, `libcrypto` and `libcurl` should be present as a result.

#### Build and install

Run the following:

```shell
git clone --recursive https://github.com/robertying/tunet-c.git
cd tunet-c
./autogen.sh && ./configure
make
make install
```

By default, the binary `tunet` is installed in `/usr/local/bin`.

A shared library `libtunet` in `/usr/local/lib` and a header file `tunet.h` in `/usr/local/include` are provided as well if you want to integrate it yourself.

## Usage

```
        -u       Tsinghua username
        -p       Tsinghua password
                    Caution: enter password in cli is not recommended
                    Please use config file instead
        -m       Network choice: [net auth auth4 auth6]
        -c       Credential config file path
                    Username should be in the first line and password in the second
        -h       Usage help
```

### Example

```shell
tunet -m auth4 -u yingr16 -p iampassword
tunet -m auth4 -c ~/mycredential
```

### Note

Entering your password in command line is extremely inappropriate.

Please use a config file and make sure you are the only one who can access it.

## Contributing

Please file an issue if you encounter any problem or want new features.

Pull requests are mostly welcomed but will be considered carefully.

## License

MIT License.

Please refer to [LICENSE](./LICENSE) or their GitHub repos for third party libraries's licenses.
