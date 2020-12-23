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
	ss << width << "x" << height << "!";
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

void file_result(fstream& out, AffineTransform** table_best_match, int width, int height)
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

void Coding(Image image, fstream& compress_result)
{
	Image domain_full, domain, range;
	DomainImageList* all_domain_blocks = new DomainImageList;
	all_domain_blocks = nullptr;
	AffineTransform** table_best_match;
	AffineTransform* cur_affine_transform = new AffineTransform;
	int width, height, i_table, j_table, x, y, i, j;
	width = image.columns();
	height = image.rows();

	if ((width % 4 != 0) || (height % 4 != 0)) // проверка размеров изображения
	{
		width = width / 4 * 4;
		height = height / 4 * 4;
		image.resize(forResize(width, height));
	}

	table_best_match = new AffineTransform * [height / 4];
	for (int i = 0; i < height / 4; i++)
		table_best_match[i] = new AffineTransform[width / 4];

	domain_full = image;

	for (y = 0; y < height - 7; y++) // создание списка всевозможных доменных блоков
	{
		for (x = 0; x < width - 7; x++)
		{
			DomainImageList* newElem = new DomainImageList;
			newElem->next = nullptr;
			domain = domain_full;
			domain.crop(forCrop(x, y, 8)); // обрезание
			domain.resize("4x4!");
			newElem->domain = domain;
			newElem->position.x = x;
			newElem->position.y = y;
			if (all_domain_blocks == nullptr) all_domain_blocks = newElem;
			else DILappend(&all_domain_blocks, newElem);
		}
	}

	for (j_table = 0; j_table < height / 4; j_table++)
	{
		for (i_table = 0; i_table < width / 4; i_table++) //проход по ранговым (ячейкам таблицы)
		{
			range = image;
			range.crop(forCrop(i_table * 4, j_table * 4, 4));

			cur_affine_transform->status = 0;

			bestStatus(range, &all_domain_blocks, cur_affine_transform); //отсюда выходит лучшее положение текущего доменного блока с позицией

			table_best_match[j_table][i_table] = *cur_affine_transform;
		}
	}

	file_result(compress_result, table_best_match, width, height);
}