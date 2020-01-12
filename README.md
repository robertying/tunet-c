# tunet-c

C cli and library for Tsinghua University network authentication.

Support `auth4`, `auth6` and `net`, including wired and wireless `Tsinghua-5G`, `Tsinghua` and `Tsinghua-IPv4`.

You can use it as a command line tool or integrate the lib into any application.

Installation on `OpenWrt` is also available!

## Usage

```sh
# Create a config file
vi ~/.tunet_config
# Fill in your username and password on each line

# Set proper permissions
chmod 600 ~/.tunet_config

# Use it!
tunet -m auth4 -c ~/.tunet_config -r

# or
tunet -m auth4 -u yingr16 -p iampassword -r
```

### Notes

Entering your password in command line is extremely inappropriate.

Please use a config file and make sure you are the only one who can access it.

### Arguments

```
        -u       Tsinghua username
        -p       Tsinghua password (wrap it in single quotes if it contains special characters)
                    Caution: enter password in cli is not recommended
                    Please use config file instead
        -r       Logout first before logging in; useful when network left inactive for too long
        -m       Network choice: [net auth auth4 auth6]
        -c       Credential config file path
                    Username should be in the first line and password in the second
        -h       Usage help
```

## Installation

### OpenWrt

#### Prebuilt Binary

For OpenWrt users, you can download prebuilt binary from [releases](https://github.com/robertying/tunet-c/releases).

Upload them to your router and use `opkg` to install packages.

For example, if you are using a `mipsel_24kc` router and packages are uploaded to `/tmp`, run:

```sh
opkg install /tmp/libtidy_5.7.28-1_mipsel_24kc.ipk
opkg install /tmp/libtunet_3.0.0-1_mipsel_24kc.ipk
opkg install /tmp/tunet_3.0.0-1_mipsel_24kc.ipk
```

You could add an init script to run `tunet` on boot.

```sh
vi /etc/init.d/tunet
```

With the following content:

```sh
#!/bin/sh /etc/rc.common
# /etc/init.d/tunet

START=99
DELAY=10

start() {
        tunet -m auth4 -c /root/.tunet_config -r
}

boot() {
        [ $DELAY -gt 0 ] && sleep $DELAY
        start
}
```

```sh
chmod +x /etc/init.d/tunet
/etc/init.d/tunet enable
```

You can also add a cron job to run `/etc/init.d/tunet start` periodically in case the network somehow become logged out after inactivity.

#### Build From Source

Currently, `tunet` is tested on `ramips/mt7620` and should work consistently for all targets.

Other platforms will be added gradually and you can always file an issue to request pre-compiled binary for these platforms.

<details>
<summary>
<strong>Here are details for those who want to build the binary by themselves</strong>
</summary>

1. Get ready by following [Install buildsystem](https://openwrt.org/docs/guide-developer/build-system/install-buildsystem) and install prerequisites.

2. Download OpenWrt SDK for your platform. Follow [Using the SDK](https://openwrt.org/docs/guide-developer/using_the_sdk).

3. For example, for mt7620 and OpenWrt 19.07, download and unzip `openwrt-sdk-19.07.0-ramips-mt7620_gcc-7.5.0_musl.Linux-x86_64.tar.xz`:

   ```sh
   tar xvf openwrt-sdk-19.07.0-ramips-mt7620_gcc-7.5.0_musl.Linux-x86_64.tar.xz
   ```

4. Add `tunet` into SDK feeds `feeds.conf.default`:

   ```sh
   cd openwrt-sdk-19.07.0-ramips-mt7620_gcc-7.5.0_musl.Linux-x86_64
   ```

   Edit the file `feeds.conf.default`. Add the following line:

   ```
   src-git tunet https://github.com/robertying/tunet-c.git
   ```

5. Use `./scripts/feeds update -a` command to obtain and update package definitions.

6. After the definitions have been updated, run `./scripts/feeds install libtidy tunet` to prepare the package and its dependencies.

7. Run `make menuconfig` and go to `Global Build Settings` to exclude the following:

   ```
   [ ] Select all target specific packages by default
   [ ] Select all kernel module packages by default
   [ ] Select all userspace packages by default
   ```

8. Exit and go to `Network`, select `tunet` by pressing `m` and all the dependencies will be selected automatically.

9. Save the configuration and exit the menu.

10. Run `make package/tunet/compile -j5`

11. After the compilation is finished, the generated .ipk files are placed in the `bin/packages/$ARCH/tunet/` of the directory you extracted the SDK into.

</details>

### Linux & macOS

Now you can only build `tunet` from source for general Linux distributions or macOS.

You need prerequisites like `openssl`, `curl` and `tidy-html5` so that `libcrypto`, `libcurl` and `libtidy` exist in system PATH.

For example, macOS users should do:

```bash
brew install openssl curl tidy-html5
export LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/opt/curl/lib -L/usr/local/opt/tidy-html5/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/opt/curl/include -I/usr/local/opt/tidy-html5/include"
```

And Linux (Ubuntu) users should do:

```bash
sudo apt update
sudo apt install libssl-dev libcurl4-openssl-dev libtidy-dev
```

Then build and install:

```bash
git clone --recursive https://github.com/robertying/tunet-c.git
cd tunet-c
./autogen.sh
./configure --prefix=/usr/local/
make
sudo make install
```

By default, the binary `tunet` is installed in `$prefix/bin`. Change `--prefix=/usr/local/` to other directories if you want.

A shared library `libtunet` in `$prefix/lib` and a header file `tunet.h` in `$prefix/include` are provided as well if you want to integrate it yourself.

### Android

See [CampusNet-Android](https://github.com/robertying/CampusNet-Android) for details about NDK integration.

## Contributing

Please file an issue if you encounter any problem or want new features.

Pull requests are mostly welcomed but will be considered carefully.
