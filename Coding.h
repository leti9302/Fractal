#pragma once
#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace Magick;
using namespace std;

struct Coordinates
{
	int x, y;
};

struct BrightnessShiftRGB
{
	double red;
	double green;
	double blue;
};

struct AffineTransform
{
	Coordinates position;
	BrightnessShiftRGB brightness_shift;
	short int status;
};

struct DomainImageList
{
	Image domain;
	Coordinates position;
	DomainImageList* next;
};

void applyColorTransformation(Image*, BrightnessShiftRGB);
void DILappend(DomainImageList**, DomainImageList*);
void bestStatus(Image, DomainImageList**, AffineTransform*);
void brightnessShift(Image, Image, BrightnessShiftRGB*);
string forResize(int, int);
string forCrop(int, int, int);
void file_result(fstream&, AffineTransform**, int, int);