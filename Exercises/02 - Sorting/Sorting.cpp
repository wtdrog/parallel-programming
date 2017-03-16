////////////////////////////////////////////////////////////////////////////////
// Программа сортирует строки из входного файла и сохраняет результат в выходном
//      файле.
// Программа принимает на вход два необязательных аргумента:
//      - имя входного файле (по умолчанию "input.txt"),
//      - имя выходного файле (по умолчанию "output.txt")
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>

const size_t PROGRESS_UPDATE    = 250000;       // период обновления прогресса
const char*  INPUT_FILENAME     = "input.txt";  // имя входного файла
const char*  OUTPUT_FILENAME    = "output.txt"; // имя выходного файла
const char*  LOG_FILENAME       = "log.txt";    // имя файла журнала

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Журнал работы программы, по окончании работы записывается в файл журнала.
// Запись в файл во время работы основоного алгоритма не производится в целях
// производительности.
////////////////////////////////////////////////////////////////////////////////
vector<string> program_log;

// время запуска основной програмы
double t0 = 0;

////////////////////////////////////////////////////////////////////////////////
// Чтение исходного массива из файла
////////////////////////////////////////////////////////////////////////////////
void WriteLog(const string& message)
{
	std::ostringstream s;
	s << (omp_get_wtime() - t0) << ", "
		<< omp_get_thread_num() << ", "
		<< message;

	#pragma omp critical
	program_log.push_back(s.str());
}

////////////////////////////////////////////////////////////////////////////////
// Чтение исходного массива из файла
////////////////////////////////////////////////////////////////////////////////
void ReadFile(const string& filename, vector<string>& v)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		cout << "Can not open file \"" << filename << "\"" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Loading \"" << filename << "\" ";

	size_t N = 0;
	file >> N;
	v.reserve(N);

	string str;
	while (getline(file, str))
	{
		if (!str.empty())
			v.push_back(str);

		// обновляем прогресс
		if (v.size() % PROGRESS_UPDATE == 0)
			cout << ".";
	}

	file.close();
	cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
// Запись отсортированного массива в файл
////////////////////////////////////////////////////////////////////////////////
void WriteFile(const string& filename, const vector<string>& v)
{
	ofstream file(filename);
	if (!file.is_open())
	{
		cout << "Can not write to file \"" << filename << "\"" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "Saving \"" << filename << "\" ";

	file << v.size() << endl;
	for (size_t i = 0; i < v.size(); ++i)
	{
		file << v[i] << endl;

		// обновляем прогресс
		if (i % PROGRESS_UPDATE == 0)
			cout << ".";
	}

	file.close();
	cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
// Функция сортировки
////////////////////////////////////////////////////////////////////////////////
void MySort(vector<string>& v)
{
	WriteLog("start sorting");

	// стандартную сортировку надо заменить на свой вариант параллельной сортировки
	sort(v.begin(), v.end());

	WriteLog("end sorting");
}

////////////////////////////////////////////////////////////////////////////////
// Тестирование результатов сортировки
////////////////////////////////////////////////////////////////////////////////
void TestSort(const vector<string>& v)
{
	cout << "Testing - ";

	for (size_t i = 1; i < v.size(); ++i)
		if (v[i] < v[i-1])
		{
			cout << "failure" << endl;
			return;
		}

	cout << "passed" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// Основная программа
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	// время запуска основной програмы
	double t0 = omp_get_wtime();

	vector<string> v;

	// обработка параметров командной строки
	string input_filename(INPUT_FILENAME);
	string output_filename(OUTPUT_FILENAME);
	string log_filename(LOG_FILENAME);

	if (argc >= 2)
		input_filename = string(argv[1]);
	if (argc >= 3)
		output_filename = string(argv[2]);

	// чтение исходных данных из файла
	ReadFile(input_filename, v);

	// время начала сортировки
	cout << "Start sorting (" << v.size() << ")" << endl;
	double t1 = omp_get_wtime();

	// сортировка
	MySort(v);

	// время окончания сортировки
	double t2 = omp_get_wtime();
	cout << "Sorting time " << (t2-t1) << endl;

	// тестирование результатов сортировки
	TestSort(v);

	// запись результатов в файл
	WriteFile(output_filename, v);

	// запись журнала работы в файл
	WriteFile(log_filename, program_log);

	return 0;
}
