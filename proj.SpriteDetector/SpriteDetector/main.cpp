/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "lodepng.h"
#include <queue>
#include <iostream>
#include <fstream>

struct Rect {
	int x, y, width, height;
};

struct Pixel {
	unsigned char red, green, blue, alpha;

	Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
};

int checkIn(std::vector<Rect> rectArray, int x, int y)
{
	for (int i = 0; i < rectArray.size(); i++) {
		if (x >= rectArray[i].x && y >= rectArray[i].y
			&& x  < rectArray[i].x + rectArray[i].width
			&& y  < rectArray[i].y + rectArray[i].height) {
			return i;
		}
	}
	return -1;
}

bool mySort(Rect a, Rect b) {
	if ((a.y >= b.y && b.y + b.height >= a.y)
		|| (b.y >= a.y && a.y + a.height >= b.y)) {
		return a.x < b.x;
	}
	return a.y < b.y;
}

void decodeOneStep(const char* filename, std::ofstream &file)
{
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	else std::cout << "decoder success" << std::endl;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...

	std::vector<Pixel> pixelArray;
	std::vector<Rect> rectArray;

	// parse to pixel array
	for (int i = 0; i < image.size() - 3; i += 4) {
		pixelArray.push_back(Pixel(image[i], image[i + 1], image[i + 2], image[i + 3]));
	}
	std::cout << "pixel count: " << pixelArray.size() << std::endl;

	for (int i = 0; i < pixelArray.size(); i++) {
		if (pixelArray[i].alpha) { // non-transparent pixel
			int pX = i%width;
			int pY = i / width;
			int id = checkIn(rectArray, pX, pY);
			if (id >= 0) { // expand the rect contain the pixel
				i = rectArray[id].x + rectArray[id].width - 1;
				i += pY * width;
				continue;
			}
			else {  // create new rect with the current pixel
				Rect rect;
				std::queue<std::pair<int, int>> pointQueue;

				pointQueue.push(std::pair<int, int>(pX, pY));
				rect.x = pX;
				rect.y = pY;
				rect.width = 1;
				rect.height = 1;

				while (!pointQueue.empty()) // collect all related pixels
				{
					int x = pointQueue.front().first;
					int y = pointQueue.front().second;
					pointQueue.pop();

					if (x < rect.x) {
						rect.width += rect.x - x;
						rect.x = x;
					}
					else {
						if (x - rect.x + 1 > rect.width)
							rect.width = x - rect.x + 1;
					}
					if (y < rect.y) {
						rect.height += rect.y - y;
						rect.y = y;
					}
					else {
						if (y - rect.y + 1 > rect.height)
							rect.height = y - rect.y + 1;
					}

					if (x > 0 && pixelArray[y*width + x - 1].alpha) { // left
						pixelArray[y*width + x - 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x - 1, y));
					}
					if (x < width - 1 && pixelArray[y*width + x + 1].alpha) { // right
						pixelArray[y*width + x + 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x + 1, y));
					}
					if (y > 0 && pixelArray[y*width + x - width].alpha) { // top
						pixelArray[y*width + x - width].alpha = 0;
						pointQueue.push(std::pair<int, int>(x, y - 1));
					}
					if (y < height - 1 && pixelArray[y*width + x + width].alpha) { // bottom
						pixelArray[y*width + x + width].alpha = 0;
						pointQueue.push(std::pair<int, int>(x, y + 1));
					}
					if (x > 0 && y > 0 && pixelArray[y*width + x - width - 1].alpha) { // left-top
						pixelArray[y*width + x - width - 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x - 1, y - 1));
					}
					if (x < width - 1 && y > 0 && pixelArray[y*width + x - width + 1].alpha) { // right-top
						pixelArray[y*width + x - width + 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x + 1, y - 1));
					}
					if (x > 0 && y < height - 1 && pixelArray[y*width + x + width - 1].alpha) { // left-bottom
						pixelArray[y*width + x + width - 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x - 1, y + 1));
					}
					if (x < width - 1 && y < height - 1 && pixelArray[y*width + x + width + 1].alpha) { // right-bottom
						pixelArray[y*width + x + width + 1].alpha = 0;
						pointQueue.push(std::pair<int, int>(x + 1, y + 1));
					}
				}

				std::cout << "Rect: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << std::endl;
				rectArray.push_back(rect); // dangerous!!!
			}
		}
	}
	
	// sort by y and then by x
	std::sort(rectArray.begin(), rectArray.end(), mySort);
	for (int i = 0; i < rectArray.size(); i++) {
		if (!i) file << "{";
		else file << ",";
		file << std::endl << "\"Rect" << i << "\":[" << rectArray[i].x << "," << rectArray[i].y << "," << rectArray[i].width << "," << rectArray[i].height << "]";
	}
	file << std::endl << "}";
}

int main(int argc, char *argv[])
{
	// testing
	const char* filename = argc > 1 ? argv[1] : "0.png";
	const char* data = argc > 2 ? argv[2] : "data.json";
	std::ofstream file;
	file.open(data);
	decodeOneStep(filename, file);
	file.close();
}


