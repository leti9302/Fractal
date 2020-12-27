#include "Decoding.h"

Image randomImg(int width, int height) {
	string size;
	stringstream ss;
	ss << width << "x" << height;
	ss >> size;
	Geometry size_geom(size);
	Image image(size_geom, "white");
	double rand_color;
	srand(time(NULL));
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			rand_color = rand() % 255;
			rand_color /= 255;
			image.pixelColor(i, j, ColorRGB(rand_color, rand_color, rand_color));
		}
	}
	return image;
}


void Decoding(fstream& in)
{
	int width, height, i_table, j_table;
	in >> skipws;
	in >> width >> height;
	AffineTransform** table_best_match;
	table_best_match = new AffineTransform * [height / 4];
	for (int i = 0; i < height / 4; i++)
		table_best_match[i] = new AffineTransform[width / 4];
	for (j_table = 0; j_table < height / 4; j_table++)
	{
		for (i_table = 0; i_table < width / 4; i_table++)
		{
			in >> table_best_match[j_table][i_table].position.x;
			in >> table_best_match[j_table][i_table].position.y;
			in >> table_best_match[j_table][i_table].brightness_shift.red;
			in >> table_best_match[j_table][i_table].brightness_shift.green;
			in >> table_best_match[j_table][i_table].brightness_shift.blue;
			in >> table_best_match[j_table][i_table].status;
		}
	}
	string size;
	stringstream ss;
	ss << width << "x" << height;
	ss >> size;
	Geometry size_geom(size);
	Image temp_img(size_geom, "black");
	Image img_iter = randomImg(width, height);
	Image block;
	BrightnessShiftRGB temp_brightness_shift;
	int i = 0;
	while (i<20)
	{
		for (j_table = 0; j_table < height / 4; j_table++)
		{
			for (i_table = 0; i_table < width / 4; i_table++)
			{
				block = img_iter;
				block.crop(forCrop(table_best_match[j_table][i_table].position.x, table_best_match[j_table][i_table].position.y, 8));
				block.resize("4x4!");
				if (table_best_match[j_table][i_table].status > 3) block.flop();
				block.rotate(90*(table_best_match[j_table][i_table].status % 4));
				temp_brightness_shift.red = table_best_match[j_table][i_table].brightness_shift.red;
				temp_brightness_shift.green = table_best_match[j_table][i_table].brightness_shift.green;
				temp_brightness_shift.blue = table_best_match[j_table][i_table].brightness_shift.blue;
				applyColorTransformation(&block, temp_brightness_shift);
				temp_img.draw(DrawableCompositeImage(i_table * 4, j_table * 4, block));
			}
		}
		if (img_iter.compare(temp_img, Magick::RootMeanSquaredErrorMetric)<0.001) break;
		img_iter = temp_img;
		i++;
	}
	img_iter.write("result.jpg");
}