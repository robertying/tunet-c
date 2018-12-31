# tunet-c

C cli and library for Tsinghua University network login.

Support `auth4`, `auth6`, `net`.

**Installation on `OpenWrt` is also available**!

## Installation

### OpenWrt

#### Install

For OpenWrt users, you can download pre-compiled binary from [releases](https://github.com/robertying/tunet-c/releases).

Upload them to your router and use `opkg` to install packages.

For example, packages are uploaded to `/tmp`:

```shell
opkg install /tmp/libtunet_1.0.0-1_mipsel_24kc.ipk
opkg install /tmp/tunet_1.0.0-1_mipsel_24kc.ipk

# Create a config file
vim ~/.tunet_config
# Use it!
tunet -m auth -c .tunet_config
```

#### Compile for your own platform

Currently, `tunet` is tested on `ramips/mt7620` and should work consistently for all `mipsel_24kc`.

Other platforms will be added gradually and you can always file an issue to request pre-compiled binary for these platforms.

The following can serve as a simple instruction for those who want to build by themselves:

-[ ] Under construction...

1. Download OpenWrt SDK for your platform. Follow [Using the SDK](https://openwrt.org/docs/guide-developer/using_the_sdk).

2. Use [OpenWrt.mk](./OpenWrt.mk) as package `Makefile`.

3. Add your own feed into SDK feeds.

4. `./script feeds update -a`

5. `./script feeds install tunet`

6. `make menuconfig`

7. Exclude all items from `Global build settings` in case of unnecessary builds.

8. Modularize (select) `tunet` from `Network`. `libtunet` will be selected automatically.

9. Save and exit. Run `make -j8`.

10. Binary will be listed in `$OPENWRT_SDK/bin/packages/$ARCH/$FEED_NAME`.

### Linux

Now you can only build `tunet` from source for general Linux distributions.

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
