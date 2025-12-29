// Задача 4. Сортировка на GPU с использованием CUDA      // описание задачи
// Параллельная сортировка подмассивов на GPU             // пояснение

#include <iostream>                                      // ввод и вывод
#include <cuda_runtime.h>                                // CUDA runtime API
#include <algorithm>                                     // std::min

using namespace std;                                     // пространство имён std

// ---------------- CUDA-ядро сортировки внутри блока ----------------
__global__ void blockSort(int* data, int n, int blockSize) { // CUDA-ядро
    int start = blockIdx.x * blockSize;                  // начало подмассива
    int end = min(start + blockSize, n);                 // конец подмассива

    for (int i = start; i < end; i++) {                  // внешний цикл сортировки
        for (int j = i + 1; j < end; j++) {              // внутренний цикл
            if (data[j] < data[i]) {                     // сравнение элементов
                int tmp = data[i];                       // временная переменная
                data[i] = data[j];                       // обмен значений
                data[j] = tmp;                           // завершение обмена
            }
        }
    }
}

// ---------------- Главная функция ----------------
int main() {                                             // точка входа
    const int N = 10000;                                 // размер массива
    const int BLOCK_SIZE = 256;                          // размер подмассива

    int* h_data = new int[N];                            // массив в памяти CPU
    for (int i = 0; i < N; i++)                          // заполнение массива
        h_data[i] = rand() % 100000;                     // случайные числа

    int* d_data;                                         // указатель на GPU
    cudaMalloc(&d_data, N * sizeof(int));                // выделение памяти на GPU
    cudaMemcpy(d_data, h_data,                           // копирование данных
               N * sizeof(int),
               cudaMemcpyHostToDevice);

    int numBlocks = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;   // количество блоков

    cudaEvent_t start, stop;                             // CUDA-события
    cudaEventCreate(&start);                             // создание start
    cudaEventCreate(&stop);                              // создание stop

    cudaEventRecord(start);                              // старт таймера
    blockSort<<<numBlocks, 1>>>(d_data, N, BLOCK_SIZE);  // запуск CUDA-ядра
    cudaEventRecord(stop);                               // стоп таймера

    cudaEventSynchronize(stop);                          // ожидание завершения

    float ms = 0.0f;                                     // время выполнения
    cudaEventElapsedTime(&ms, start, stop);              // расчёт времени

    cudaMemcpy(h_data, d_data,                           // копирование назад
               N * sizeof(int),
               cudaMemcpyDeviceToHost);

    cout << "CUDA sorting time: "                         // вывод результата
         << ms << " ms" << endl;

    cudaFree(d_data);                                    // освобождение GPU памяти
    delete[] h_data;                                     // освобождение CPU памяти

    return 0;                                            // завершение программы
}
