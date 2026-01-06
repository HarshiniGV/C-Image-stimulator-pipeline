#include "image.h"
#include <cmath>
#include <fstream>
#include <algorithm>

// ---------------- Image Class ----------------
Image::Image(int w, int h, int c) : width(w), height(h), channels(c) {
    data.resize(w * h * c);
}

unsigned char& Image::at(int x, int y, int c) {
    return data[(y * width + x) * channels + c];
}

// ---------------- Image Processing ----------------

// RGB → Grayscale
void convertToGrayscale(Image& img) {
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            unsigned char r = img.at(x, y, 0);
            unsigned char g = img.at(x, y, 1);
            unsigned char b = img.at(x, y, 2);

            unsigned char gray = static_cast<unsigned char>(
                0.299 * r + 0.587 * g + 0.114 * b
            );

            img.at(x, y, 0) = gray;
            img.at(x, y, 1) = gray;
            img.at(x, y, 2) = gray;
        }
    }
}

// Gamma correction
void applyGammaCorrection(Image& img, float gamma) {
    float invGamma = 1.0f / gamma;

    for (auto& pixel : img.data) {
        float normalized = pixel / 255.0f;
        pixel = static_cast<unsigned char>(
            std::pow(normalized, invGamma) * 255.0f
        );
    }
}

// Sobel Edge Detection (expects grayscale)
void applySobelEdgeDetection(Image& img) {
    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    std::vector<unsigned char> output = img.data;

    for (int y = 1; y < img.height - 1; y++) {
        for (int x = 1; x < img.width - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    unsigned char pixel = img.at(x + kx, y + ky, 0);
                    sumX += pixel * gx[ky + 1][kx + 1];
                    sumY += pixel * gy[ky + 1][kx + 1];
                }
            }

            int magnitude = std::min(255, static_cast<int>(
                std::sqrt(sumX * sumX + sumY * sumY)
            ));

            for (int c = 0; c < 3; c++)
                output[(y * img.width + x) * 3 + c] = magnitude;
        }
    }

    img.data = output;
}

// ---------------- BMP I/O ----------------

#pragma pack(push,1)
struct BMPHeader {
    char signature[2];
    int fileSize;
    int reserved;
    int dataOffset;
    int headerSize;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    int compression;
    int imageSize;
    int xPixelsPerM;
    int yPixelsPerM;
    int colorsUsed;
    int importantColors;
};
#pragma pack(pop)

bool loadBMP(const std::string& filename, Image& img) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.bitsPerPixel != 24) return false;

    img = Image(header.width, header.height, 3);
    file.seekg(header.dataOffset);
    file.read(reinterpret_cast<char*>(img.data.data()), img.data.size());

    return true;
}

bool saveBMP(const std::string& filename, const Image& img) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    BMPHeader header = {};
    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.dataOffset = sizeof(BMPHeader);
    header.headerSize = 40;
    header.width = img.width;
    header.height = img.height;
    header.planes = 1;
    header.bitsPerPixel = 24;
    header.imageSize = img.data.size();
    header.fileSize = header.dataOffset + header.imageSize;

    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    file.write(reinterpret_cast<const char*>(img.data.data()), img.data.size());

    return true;
}
