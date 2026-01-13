# REVERT

**REVERT** is a C software develop for CS50 2026.

It is lightweight software that allows you to view **.bmp** files. It is designed for Harvard's CS50.

[Screenshot]("./screenshots/1.png")

## Install 
```bash
git clone https://github.com/Cr12dev/revert-cs50-2026.git
```
```bash
mkdir build
cd build
```
```bash
cmake ..
make
./sdl_image_renderer
```

## Utils `/utils`
**Require:** Python3 & PIL

You can generate image `.bmp` using `/utils/create_test_image.py`
```bash
python3 ./utils/create_test_image.py
```