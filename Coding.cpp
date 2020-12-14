#include "Coding.h";

void DILappend(DomainImageList** head, DomainImageList* newElem)
{
	DomainImageList* cur = *head;
	while (cur->next != nullptr) { cur = cur->next; }
	cur->next = newElem;
}

void bestStatus(Image range, DomainImageList** all_domain_blocks, AffineTransform* best_affine_transform) //лучший доменный блок для посылаемого рангового (перебор всех доменных)
{
	double factorRMSEM, bestfactorRMSEM = 1;
	BrightnessShiftRGB temp_brightness_shift;
	DomainImageList* cur_domain_list = *all_domain_blocks;
	Image domain;

	while (cur_domain_list->next != nullptr)
	{
		domain = cur_domain_list->domain;
		brightnessShift(range, domain, &temp_brightness_shift);
		applyColorTransformation(&domain, temp_brightness_shift);
		for (int i = 0; i < 8; i++)
		{
			if (i == 4) domain.flop();
			factorRMSEM = range.compare(domain, Magick::RootMeanSquaredErrorMetric);
			if (bestfactorRMSEM > factorRMSEM)
			{
				bestfactorRMSEM = factorRMSEM;
				best_affine_transform->brightness_shift.red = temp_brightness_shift.red;
				best_affine_transform->brightness_shift.green = temp_brightness_shift.green;
				best_affine_transform->brightness_shift.blue = temp_brightness_shift.blue;
				best_affine_transform->position.x = cur_domain_list->position.x;
				best_affine_transform->position.y = cur_domain_list->position.y;
				best_affine_transform->status = i;
			}
			domain.rotate(90);
		}
		cur_domain_list = cur_domain_list->next;
	}
}

void applyColorTransformation(Image* domain, BrightnessShiftRGB shift)
{
	double coef_compress = 0.75;
	ColorRGB current;
	double current_red, current_green, current_blue;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			current = domain->pixelColor(i, j);
			current_red = current.red();
			current_green = current.green();
			current_blue = current.blue();
			current_red = current_red * coef_compress + shift.red;
			current_green = current_green * coef_compress + shift.green;
			current_blue = current_blue * coef_compress + shift.blue;
			domain->pixelColor(i, j, ColorRGB(current_red, current_green, current_blue));
		}
	}
}

void brightnessShift(Image range, Image domain, BrightnessShiftRGB* brightness_shift_RGB)
{
	double coef_compress = 0.75;
	int i, j;
	ColorRGB cursor_range, cursor_domain;

	brightness_shift_RGB->red = 0, brightness_shift_RGB->green = 0, brightness_shift_RGB->blue = 0;


	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			cursor_range = range.pixelColor(i, j);
			cursor_domain = domain.pixelColor(i, j);
			brightness_shift_RGB->red = brightness_shift_RGB->red + cursor_range.red() - coef_compress * cursor_domain.red();
			brightness_shift_RGB->green = brightness_shift_RGB->green + cursor_range.green() - coef_compress * cursor_domain.green();
			brightness_shift_RGB->blue = brightness_shift_RGB->blue + cursor_range.blue() - coef_compress * cursor_domain.blue();

		}

	}
	brightness_shift_RGB->red /= 16;
	brightness_shift_RGB->green /= 16;
	brightness_shift_RGB->blue /= 16;



}

string forResize(int width, int height)
{
	string str;
	stringstream ss;
	ss << width * 4 << "x" << height * 4 << "!";
	ss >> str;
	return str;
}

string forCrop(int width, int height, int k)
{
	string str;
	stringstream ss;
	ss << k << "x" << k << "+" << width << "+" << height;
	ss >> str;
	return str;
}

void file_result(ofstream& out, AffineTransform** table_best_match, int width, int height)
{
	int i, j;
	out << width << ' ' << height << '\n';
	for (j = 0; j < height / 4; j++)
	{
		for (i = 0; i < width / 4; i++)
		{
			out << table_best_match[j][i].position.x << ' ' << table_best_match[j][i].position.y << ' ';
			out << table_best_match[j][i].brightness_shift.red << ' ' << table_best_match[j][i].brightness_shift.green << ' ' << table_best_match[j][i].brightness_shift.blue << ' ';
			out << table_best_match[j][i].status << '\n';
		}
	}
}