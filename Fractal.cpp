#include <sstream>
#include <ctime>

using namespace Magick;
using namespace std;

struct Coordinates
{
	int x, y;
};

struct AffineTransform
{
	Coordinates position;
	int status;
};

struct DomainImageList
{
	Image domain;
	Coordinates position;
	DomainImageList* next;
};

void DILappend(DomainImageList** head, DomainImageList* newElem)
{
	DomainImageList* cur = *head;
	while (cur->next != nullptr) { cur = cur->next; }
	cur->next = newElem;
}

void bestStatus(Image range, DomainImageList** all_domain_blocks, AffineTransform* cur_affine_transform)
{
	double factorRMSEM, bestfactorRMSEM = 1;
	DomainImageList* cur = *all_domain_blocks;
	while (cur->next != nullptr)
	{
		Image domain = cur->domain;
		for (int i = 0; i < 8; i++)
		{
			if (i == 4) domain.flop();
			factorRMSEM = range.compare(domain, Magick::RootMeanSquaredErrorMetric);
			if (bestfactorRMSEM > factorRMSEM)
			{
				bestfactorRMSEM = factorRMSEM;
				cur_affine_transform->position.x = cur->position.x;
				cur_affine_transform->position.y = cur->position.y;
				cur_affine_transform->status = i;
			}
			domain.rotate(90);
		}
		cur = cur->next;
	}
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

int main(int argc, char** argv) {
	try {
		InitializeMagick(*argv);
		Image image("lena512.bmp");
		image.resize("32x32!");
		Image domain_full, domain, range;
		DomainImageList* all_domain_blocks = new DomainImageList;
		all_domain_blocks = nullptr;
		AffineTransform** table_best_match;
		AffineTransform* cur_affine_transform = new AffineTransform;
		int width, height, i_table, j_table, x, y;
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

		int start_clock, end_clock;
		start_clock = clock();
		for (j_table = 0; j_table < height / 4; j_table++) //j_table < height/4; j_table++)
		{
			for (i_table = 0; i_table < width / 4; i_table++) //width/4; i_table++) //проход по ранговым (ячейкам таблицы)
			{
				range = image;
				range.crop(forCrop(i_table * 4, j_table * 4, 4));

				cur_affine_transform->status = 0;

				bestStatus(range, &all_domain_blocks, cur_affine_transform); //отсюда выходит лучшее положение текущего доменного блока с позицией

				table_best_match[j_table][i_table] = *cur_affine_transform;

				cout << "(" << table_best_match[j_table][i_table].position.x << ',' << table_best_match[j_table][i_table].position.y << ") ";
				cout << table_best_match[j_table][i_table].status << ' ' << " (" << j_table * 4 << ',' << i_table * 4 << ")\n";
			}
		}
		end_clock = clock();
		cout << "_________________________________\n";
		for (int i = 0; i < 4; i++)
		{
			cout << "(" << table_best_match[0][i].position.x << ',' << table_best_match[0][i].position.y << ") ";
			cout << table_best_match[0][i].status << ' ';
			cout << "(" << 0 << ',' << i * 4 << ")\n";
		}
		cout << double(end_clock - start_clock) / double(CLOCKS_PER_SEC) << endl;
		system("pause");
	}
	catch (Magick::Exception& error) {
		cerr << "Caught Magick++ exception: " << error.what() << endl;
	}
	return 0;
}