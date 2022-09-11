# WarheadImageMirror

## Build requirements

1. Ubuntu

```sh
sudo apt-get install git clang cmake make gcc g++ libboost-all-dev libjpeg-dev
```

2. Manjaro Linux

```sh
sudo pacman -S boost-libs git clang cmake make
```

## Build process

```sh
git clone https://github.com/Winfidonarleyan/WarheadImageMirror
cd WarheadImageMirror
mkdir build
cd build
cmake ../ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_INSTALL_PREFIX=../../wim
make -j 4 && make install
cd ..
```

A new folder should appear next to the repository `wim`

### Create config file
1. Move to `.../wim/etc`
2. Copy dist config file from - https://github.com/Winfidonarleyan/WarheadImageMirror/blob/master/src/app/WarheadImageMirror.conf.dist
3. Replace file name to `WarheadImageMirror.conf`
4. Change options if need

### Run server
1. Move to `../wim/bin`
2. ./WarheadImageMirror
