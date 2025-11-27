#ifndef STRAIGHTCALCULATION_H
#define STRAIGHTCALCULATION_H

#include <atomic>

enum Method
{
    CACHED,
    NOT_CACHED
};

struct Result
{
    bool isOverflow = false;
    unsigned long long chainLength = 0;
    unsigned long long number = 0;
};

void straightCalculation(Result& calculationResult, unsigned long long minNumber, unsigned long long maxNumber, unsigned long long threshold);
void atomicCalculation(std::atomic<std::size_t>*& values, Result& calculationResult, unsigned long long minNumber, unsigned long long maxNumber, unsigned long long threshold, unsigned long long globalMaxNumber);

#endif // STRAIGHTCALCULATION_H
