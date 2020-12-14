#include"Coding.h"

void main(int argc, char** argv) {
	try {
		InitializeMagick(*argv);
		ofstream compress_result;
		compress_result.open("Compress Image.txt");
		Image image("lena512.bmp");
		image.resize("16x16!");
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
			image.resize(forResize(width / 4, height / 4));
			width = width / 4 * 4;
			height = height / 4 * 4;
		}

		table_best_match = new AffineTransform * [height / 4];
		for (int i = 0; i < height / 4; i++)
			table_best_match[i] = new AffineTransform[width / 4];

		domain_full = image;
		domain_full.resize(forResize(width / 2, height / 2)); // сжатие доменного изображения в 2 раза

		for (y = 0; y < height / 2 - 3; y++) // создание списка всевозможных доменных блоков
		{
			for (x = 0; x < width / 2 - 3; x++)
			{
				DomainImageList* newElem = new DomainImageList;
				newElem->next = nullptr;
				domain = domain_full;
				domain.crop(forCrop(x, y, 4)); // обрезание
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
		compress_result.close();
	}
	catch (Magick::Exception& error) {
		cerr << "Caught Magick++ exception: " << error.what() << endl;
	}
}