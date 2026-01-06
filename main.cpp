#include "image.h"
#include <iostream>

int main() {
    Image img;

    if (!loadBMP("input.bmp", img)) {
        std::cout << "Failed to load input.bmp\n";
        return 1;
    }

    convertToGrayscale(img);
    applyGammaCorrection(img, 2.2f);
    applySobelEdgeDetection(img);

    saveBMP("output.bmp", img);

    std::cout << "Image processed successfully. Output saved as output.bmp\n";
    return 0;
}
