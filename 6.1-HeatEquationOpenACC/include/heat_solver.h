// heat_solver.h
#pragma once
#include <vector>

// Парсинг параметров командной строки
struct Options {
    int N;               // размер сетки
    double tol;          // точность
    long maxIter;        // макс. число итераций
};
Options parseOptions(int argc, char** argv);

// Установка граничных условий (линейная интерполяция)
void initBoundary(std::vector<double>& A, int N);

// Инициализация внутренней области нулями
void initInterior(std::vector<double>& A, int N);

// Одна итерация схемы Якоби + вычисление maxError
void jacobiIteration(const std::vector<double>& A,
                     std::vector<double>& Anew,
                     int N,
                     double& maxError);

// Копирование Anew → A после итерации
void swapGrids(std::vector<double>& A,
               std::vector<double>& Anew);

// Вывод результатов (количество итераций и ошибка)
void printSummary(long iter, double maxError);