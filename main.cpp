#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace std;

// Class to represent a pixel
class Pixel {
public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    // Constructor
    Pixel(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) : red(r), green(g), blue(b) {}
};

// Class to represent a BMP image
class BMPImage {
private:
    int width;
    int height;
    vector<Pixel> pixels; // Vector to store pixel data

public:
    BMPImage() : width(0), height(0) {}

    // Function to load BMP image from file
    bool loadImage(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Error: Could not open BMP file " << filename << endl;
            return false;
        }

        // Read BMP header to get image dimensions
        char header[54];
        file.read(header, 54);

        width = *(int*)&(header[18]);
        height = *(int*)&(header[22]);

        // Check if the image is 24-bit BMP (no compression)
        if (*(int*)&(header[28]) != 24 || *(int*)&(header[30]) != 0) {
            cerr << "Error: Unsupported BMP format. Only 24-bit BMP without compression is supported." << endl;
            file.close();
            return false;
        }

        // Read pixel data
        pixels.clear();
        int padding = (4 - (width * 3) % 4) % 4; // BMP files are padded to be multiple of 4 bytes
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint8_t blue, green, red;
                file.read(reinterpret_cast<char*>(&blue), 1);
                file.read(reinterpret_cast<char*>(&green), 1);
                file.read(reinterpret_cast<char*>(&red), 1);
                pixels.emplace_back(red, green, blue);
            }
            // Skip padding bytes
            file.seekg(padding, ios::cur);
        }

        file.close();
        return true;
    }

    // Function to get the width of the image
    int getWidth() const {
        return width;
    }

    // Function to get the height of the image
    int getHeight() const {
        return height;
    }

    // Function to get the pixels of the image
    const vector<Pixel>& getPixels() const {
        return pixels;
    }

    // Function to calculate the average color of the pixels
    Pixel calculateAverageColor() const {
        unsigned long sumR = 0, sumG = 0, sumB = 0;
        for (const auto& pixel : pixels) {
            sumR += pixel.red;
            sumG += pixel.green;
            sumB += pixel.blue;
        }
        int avgR = static_cast<int>(sumR / pixels.size());
        int avgG = static_cast<int>(sumG / pixels.size());
        int avgB = static_cast<int>(sumB / pixels.size());
        return Pixel(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
    }

    // Function to calculate the average color of a specific vertical part of the image
    Pixel calculateAverageColorOfVerticalPart(int startX, int endX) const {
        // Ensure startX and endX are within bounds
        startX = max(0, min(startX, width - 1));
        endX = max(0, min(endX, width - 1));

        unsigned long sumR = 0, sumG = 0, sumB = 0;
        int pixelCount = 0;

        // Iterate over rows
        for (int y = 0; y < height; y++) {
            // Iterate over pixels within the specified vertical range
            for (int x = startX; x <= endX; x++) {
                const Pixel& pixel = pixels[y * width + x]; // Access pixel at (x, y)
                sumR += pixel.red;
                sumG += pixel.green;
                sumB += pixel.blue;
                pixelCount++;
            }
        }

        // Calculate average color
        int avgR = static_cast<int>(sumR / pixelCount);
        int avgG = static_cast<int>(sumG / pixelCount);
        int avgB = static_cast<int>(sumB / pixelCount);
        return Pixel(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
    }

    // Function to calculate the average color of a specific horizontal part of the image
    Pixel calculateAverageColorOfHorizontalPart(int startY, int endY) const {
        // Ensure startY and endY are within bounds
        startY = max(0, min(startY, height - 1));
        endY = max(0, min(endY, height - 1));

        unsigned long sumR = 0, sumG = 0, sumB = 0;
        int pixelCount = 0;

        // Iterate over rows within the specified vertical range
        for (int y = startY; y <= endY; ++y) {
            // Iterate over pixels in the row
            for (int x = 0; x < width; ++x) {
                const Pixel& pixel = pixels[y * width + x]; // Access pixel at (x, y)
                sumR += pixel.red;
                sumG += pixel.green;
                sumB += pixel.blue;
                pixelCount++;
            }
        }

        // Calculate average color
        int avgR = static_cast<int>(sumR / pixelCount);
        int avgG = static_cast<int>(sumG / pixelCount);
        int avgB = static_cast<int>(sumB / pixelCount);
        return Pixel(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
    }

    // Function to calculate the average colors of the pixels in the top horizontal line
    vector<Pixel> calculateAverageColorsOfTopHorizontalLine(int numSections) const {
        vector<Pixel> avgColors;

        if (numSections <= 0) {
            cerr << "Error: Number of sections must be greater than zero." << endl;
            return avgColors; // Return an empty vector
        }

        int stepSize = width / numSections;

        for (int i = 0; i < numSections; ++i) {
            int startX = i * stepSize;
            int endX = (i + 1) * stepSize - 1;

            unsigned long sumR = 0, sumG = 0, sumB = 0;
            int pixelCount = 0;

            for (int x = startX; x <= endX; ++x) {
                const Pixel& pixel = pixels[x]; // Access pixel from BMPImage
                sumR += pixel.red;
                sumG += pixel.green;
                sumB += pixel.blue;
                pixelCount++;
            }

            int avgR = static_cast<int>(sumR / pixelCount);
            int avgG = static_cast<int>(sumG / pixelCount);
            int avgB = static_cast<int>(sumB / pixelCount);
            avgColors.emplace_back(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
        }

        return avgColors;
    }


// Function to calculate the average colors of the pixels in the right vertical line
    vector<Pixel> calculateAverageColorsOfRightVerticalLine(int numSections) const {
        vector<Pixel> avgColors;

        if (numSections <= 0) {
            cerr << "Error: Number of sections must be greater than zero." << endl;
            return avgColors; // Return an empty vector
        }

        int stepSize = height / numSections;

        for (int i = 0; i < numSections; ++i) {
            int startY = i * stepSize;
            int endY = (i + 1) * stepSize - 1;

            unsigned long sumR = 0, sumG = 0, sumB = 0;
            int pixelCount = 0;

            for (int y = startY; y <= endY; ++y) {
                const Pixel& pixel = pixels[y * width + width - 1]; // Access pixel from BMPImage
                sumR += pixel.red;
                sumG += pixel.green;
                sumB += pixel.blue;
                pixelCount++;
            }

            int avgR = static_cast<int>(sumR / pixelCount);
            int avgG = static_cast<int>(sumG / pixelCount);
            int avgB = static_cast<int>(sumB / pixelCount);
            avgColors.emplace_back(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
        }

        return avgColors;
    }

// Function to calculate the average colors of the pixels in the bottom horizontal line
    vector<Pixel> calculateAverageColorsOfBottomHorizontalLine(int numSections) const {
        vector<Pixel> avgColors;

        if (numSections <= 0) {
            cerr << "Error: Number of sections must be greater than zero." << endl;
            return avgColors; // Return an empty vector
        }

        int stepSize = width / numSections;

        for (int i = 0; i < numSections; ++i) {
            int startX = i * stepSize;
            int endX = (i + 1) * stepSize - 1;

            unsigned long sumR = 0, sumG = 0, sumB = 0;
            int pixelCount = 0;

            for (int x = startX; x <= endX; ++x) {
                const Pixel& pixel = pixels[(height - 1) * width + x]; // Access pixel from BMPImage
                sumR += pixel.red;
                sumG += pixel.green;
                sumB += pixel.blue;
                pixelCount++;
            }

            int avgR = static_cast<int>(sumR / pixelCount);
            int avgG = static_cast<int>(sumG / pixelCount);
            int avgB = static_cast<int>(sumB / pixelCount);
            avgColors.emplace_back(static_cast<uint8_t>(avgR), static_cast<uint8_t>(avgG), static_cast<uint8_t>(avgB));
        }

        return avgColors;
    }

// Function to calculate the average colors of the pixels in the left vertical line
    vector<Pixel> calculateAverageColorsOfVerticalPartWithSubsections(int startX, int endX, int numSubsections) const {
        vector<Pixel> avgColors;

        if (numSubsections <= 0) {
            cerr << "Error: Number of subsections must be greater than zero." << endl;
            return avgColors; // Return an empty vector
        }

        // Ensure startX and endX are within bounds
        startX = max(0, min(startX, width - 1));
        endX = max(0, min(endX, width - 1));

        // Calculate the width of the specified range
        int rangeWidth = endX - startX + 1;

        // Determine the width of each subsection
        int subsectionWidth = rangeWidth / numSubsections;

        for (int i = 0; i < numSubsections; ++i) {
            // Calculate the start and end X-coordinates for the current subsection
            int subsectionStartX = startX + i * subsectionWidth;
            int subsectionEndX = subsectionStartX + subsectionWidth - 1;

            // Ensure startX and endX are within bounds for the current subsection
            subsectionStartX = max(startX, min(subsectionStartX, endX));
            subsectionEndX = max(startX, min(subsectionEndX, endX));

            // Calculate the average color for the current subsection
            Pixel avgColor = calculateAverageColorOfVerticalPart(subsectionStartX, subsectionEndX);
            avgColors.push_back(avgColor);
        }

        return avgColors;
    }

};


int main() {
    // Example usage
    BMPImage bmpImage;
    if (!bmpImage.loadImage("Italian_Flag.bmp")) {
        return 1;
    }

    // Define LED strip organization parameters
    double sizeOfMonitor = 21.0;
    int numSections = static_cast<int>(sizeOfMonitor * 2.333) + 1;
    int topMonitor = static_cast<int>(numSections * 0.30) + 1;
    int bottomMonitor = static_cast<int>(numSections * 0.30) + 1;
    int leftMonitor = static_cast<int>(numSections * 0.20);
    int rightMonitor = static_cast<int>(numSections * 0.20);
    int counter = 0;

    // Test top horizontal line
    cout << "Num of LEDs in top: " << numSections << endl;
    vector<Pixel> topHorizontalLine = bmpImage.calculateAverageColorsOfTopHorizontalLine(topMonitor);
    cout << "Top Horizontal Line:" << endl;
    for (const auto& pixel : topHorizontalLine) {
        cout << "R=" << static_cast<int>(pixel.red) << ", G=" << static_cast<int>(pixel.green) << ", B="
             << static_cast<int>(pixel.blue) << endl;
        // Store pixel values for further processing if needed
        // pixels1[counter++] = make_tuple(pixel.red, pixel.green, pixel.blue);
        counter++;
    }

    // Test right vertical line
    int start_x = (bmpImage.getWidth() / 10) * 9; // starts last 1/10
    int precision = bmpImage.getWidth() - 6; // removes last 6 pixels
    int end_x = precision;
    vector<Pixel> rightVerticalLine = bmpImage.calculateAverageColorsOfVerticalPartWithSubsections(start_x, end_x, rightMonitor);
    cout << "Right Vertical Line:" << endl;
    for (const auto& pixel : rightVerticalLine) {
        cout << "R=" << static_cast<int>(pixel.red) << ", G=" << static_cast<int>(pixel.green) << ", B="
             << static_cast<int>(pixel.blue) << endl;
        // pixels1[counter++] = make_tuple(pixel.red, pixel.green, pixel.blue);
        counter++;
    }

    // Test bottom horizontal line
    vector<Pixel> bottomHorizontalLine = bmpImage.calculateAverageColorsOfBottomHorizontalLine(bottomMonitor);
    cout << "Bottom Horizontal Line:" << endl;
    for (auto it = bottomHorizontalLine.rbegin(); it != bottomHorizontalLine.rend(); ++it) {
        const Pixel& pixel = *it;
        cout << "R=" << static_cast<int>(pixel.red) << ", G=" << static_cast<int>(pixel.green) << ", B="
             << static_cast<int>(pixel.blue) << endl;
        // pixels1[counter++] = make_tuple(pixel.red, pixel.green, pixel.blue);
        counter++;
    }

    // Test left vertical line
    start_x = 28;
    precision = bmpImage.getWidth() / 10;
    end_x = precision;
    vector<Pixel> leftVerticalLine = bmpImage.calculateAverageColorsOfVerticalPartWithSubsections(start_x, end_x, leftMonitor);
    cout << "Left Vertical Line:" << endl;
    for (auto it = leftVerticalLine.rbegin(); it != leftVerticalLine.rend(); ++it) {
        const Pixel& pixel = *it;
        cout << "R=" << static_cast<int>(pixel.red) << ", G=" << static_cast<int>(pixel.green) << ", B="
             << static_cast<int>(pixel.blue) << endl;
        // pixels1[counter++] = make_tuple(pixel.red, pixel.green, pixel.blue);
        counter++;
    }


    return 0;
}
