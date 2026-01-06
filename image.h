#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>

class Image {
public:
    int width, height, channels;
    std::vector<unsigned char> data;

    Image(int w = 0, int h = 0, int c = 0);
    unsigned char& at(int x, int y, int c);
};

// Image processing functions
void convertToGrayscale(Image& img);
void applyGammaCorrection(Image& img, float gamma);
void applySobelEdgeDetection(Image& img);

// BMP I/O
bool loadBMP(const std::string& filename, Image& img);
bool saveBMP(const std::string& filename, const Image& img);

#endif
