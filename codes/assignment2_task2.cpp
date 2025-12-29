// Задача 2. Поиск минимума и максимума в массиве
// Последовательная и параллельная (OpenMP) версии (улучшенный вариант)

#include <iostream>                       // подключаем ввод/вывод
#include <vector>                         // подключаем std::vector
#include <random>                         // подключаем генераторы случайных чисел
#include <chrono>                         // подключаем таймеры
#include <limits>                         // подключаем numeric_limits (границы типов)
#include <omp.h>                          // подключаем OpenMP

using namespace std;                      // чтобы не писать std::
using namespace chrono;                   // чтобы не писать chrono::

int main() {                              // точка входа программы
    const int N = 10000;                  // размер массива
    vector<int> arr(N);                   // создаём массив N элементов

    // Генерация случайных чисел (фиксируем seed для воспроизводимости)
    mt19937 gen(12345);                   // генератор случайных чисел с фиксированным seed
    uniform_int_distribution<int> dis(1, 100000); // диапазон случайных значений

    for (int i = 0; i < N; ++i)           // заполняем массив
        arr[i] = dis(gen);                // записываем случайное число

    // -------------------- Прогрев (чтобы замеры были стабильнее) --------------------
    volatile int warm = 0;                // volatile, чтобы компилятор не выкинул вычисления
    for (int i = 0; i < N; ++i)           // простой проход по массиву
        warm += arr[i];                   // “трогаем” данные в памяти

    // -------------------- Последовательный вариант --------------------
    int min_seq = numeric_limits<int>::max(); // начальный минимум = очень большое
    int max_seq = numeric_limits<int>::min(); // начальный максимум = очень маленькое

    auto start_seq = high_resolution_clock::now(); // старт таймера для seq

    for (int i = 0; i < N; ++i) {         // проходим по массиву
        if (arr[i] < min_seq) min_seq = arr[i]; // обновляем минимум
        if (arr[i] > max_seq) max_seq = arr[i]; // обновляем максимум
    }

    auto end_seq = high_resolution_clock::now();  // конец таймера для seq
    auto time_seq = duration_cast<microseconds>(end_seq - start_seq).count(); // время в мкс

    // -------------------- Параллельный вариант (OpenMP) --------------------
    int min_par = numeric_limits<int>::max(); // начальный минимум для omp
    int max_par = numeric_limits<int>::min(); // начальный максимум для omp

    auto start_par = high_resolution_clock::now(); // старт таймера для omp

#pragma omp parallel for reduction(min:min_par) reduction(max:max_par)
    for (int i = 0; i < N; ++i) {         // параллельный проход
        min_par = min(min_par, arr[i]);   // редукция минимума
        max_par = max(max_par, arr[i]);   // редукция максимума
    }

    auto end_par = high_resolution_clock::now();  // конец таймера для omp
    auto time_par = duration_cast<microseconds>(end_par - start_par).count(); // время в мкс

    // -------------------- Вывод  --------------------
    cout << "Sequential Min: " << min_seq << ", Max: " << max_seq << endl; // вывод seq
    cout << "Parallel   Min: " << min_par << ", Max: " << max_par << endl; // вывод omp

    cout << "Sequential time: " << time_seq << " microseconds" << endl;    // время seq
    cout << "Parallel time:   " << time_par << " microseconds" << endl;    // время omp

    return 0;                           // успешное завершение
}
