#include "straightCalculation.h"

extern std::atomic<bool> stopRequested;

void straightCalculation(Result& calculationResult, unsigned long long minNumber, unsigned long long maxNumber, unsigned long long threshold)
{
    //Result calculationResult;
    //unsigned long long searchedNumber = 1;
    unsigned long long maxChainLength = 1;
    for (unsigned long long currentNumber = minNumber; currentNumber <= maxNumber; ++currentNumber)
    {
        unsigned long long currentNextNumber = currentNumber;
        unsigned long long currentChainLength = 1;
        while (currentNextNumber != 1 and !stopRequested)
        {
            //currentNextNumber = getNextNumber(currentNextNumber).result;


            if (currentNextNumber % 2 == 0)
            {
                currentNextNumber = currentNextNumber / 2;
            }
            else
            {
                if (currentNextNumber > threshold)
                {
                    //std::cout << "Overflow!";
                    calculationResult.isOverflow = true;
                    break;
                }
                currentNextNumber = 3 * currentNextNumber + 1;
            }

            ++currentChainLength;
            //if (currentNextNumber == 1)
            //{
            //    break;
            //}
        }

        //std::cout << "current number: " << currentNumber << "  |  Collatz chain length: " << currentChainLength << "\n";

        if (currentChainLength > maxChainLength)
        {
            maxChainLength = currentChainLength;
            calculationResult.number = currentNumber;
            calculationResult.chainLength = currentChainLength;
        }
    }

    //return calculationResult;
}



void atomicCalculation(std::atomic<std::size_t>*& values, Result& calculationResult, unsigned long long minNumber, unsigned long long maxNumber, unsigned long long threshold, unsigned long long globalMaxNumber)
{
    //Result calculationResult;
    //unsigned long long searchedNumber = 1;
    unsigned long long maxChainLength = 1;
    for (unsigned long long currentNumber = minNumber; currentNumber <= maxNumber; ++currentNumber)
    {
        unsigned long long currentNextNumber = currentNumber;
        unsigned long long currentChainLength = 1;

        while (currentNextNumber != 1)
        {
            //currentNextNumber = getNextNumber(currentNextNumber).result;

            if (currentNextNumber % 2 == 0)
            {
                currentNextNumber = currentNextNumber / 2;
            }
            else
            {
                if (currentNextNumber > threshold)
                {
                    //std::cout << "Overflow!";
                    calculationResult.isOverflow = true;
                    break;
                }
                currentNextNumber = 3 * currentNextNumber + 1;
            }

            {
                //std::size_t cached = values[currentNextNumber].load(std::memory_order_relaxed);
                //if (cached != 0)
                //{
                //    currentChainLength += cached;
                //    break;
                //}
                //else
                //{
                //    ++currentChainLength;
                //}

                if (currentNextNumber <= globalMaxNumber)
                {
                    std::size_t cached = values[currentNextNumber].load(std::memory_order_relaxed);
                    if (cached != 0)
                    {
                        currentChainLength += cached;
                        break;
                    }
                }
                ++currentChainLength;


            }
        }

        //std::cout << "current number: " << currentNumber << "  |  Collatz chain length: " << currentChainLength << "\n";

        if (currentChainLength > maxChainLength)
        {
            maxChainLength = currentChainLength;
            calculationResult.number = currentNumber;
            calculationResult.chainLength = currentChainLength;
        }


        std::size_t expected = 0;
        values[currentNumber].compare_exchange_strong(expected, currentChainLength, std::memory_order_relaxed);


        /*if (values[currentNumber] == 0)
        {
            values[currentNumber] = currentChainLength;
        }*/
    }
}
