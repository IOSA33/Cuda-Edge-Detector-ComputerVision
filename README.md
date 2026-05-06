# Computer Vision Hand Detector
- Using modern C++
- GPU side AMD HIP/CUDA for quick optimization of the algorithm

## __CPU__ Test
- 2k Image input- 0.034s ~ 33 fps
- 8k Image input - 0.093s ~ 10 fps

## __GPU__ Test
- 2k Image input - 0.0018s ~ 550 fps
- 8k Image input - 0.0042s ~ 240 fps

## Image result
![Test](./hip/output.jpg)

# How to Start CPU side
- Put your image in the "photos" folder
- Then in the ```main.cpp``` change the path to the image file
- After, to run the code write in console
```
mkdir build
cmake ..
ninja
./app.exe 1
```
# How to Start GPU side
- Put your image in the "photos" folder
- Then go to the hip folder, after in the ```main.hip.cpp``` change the path to the file
- To run the code write in console, in hip folder
```
hipcc main.hip.cc -o app
./app.exe 1
```