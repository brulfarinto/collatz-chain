#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "straightCalculation.h"


//#include <iostream>
#include <chrono>
//#include <iomanip>
#include <thread>


//#include <thread>
#include <atomic>

std::atomic<bool> stopRequested = false;
std::thread calcThread;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::calcFunction, Qt::ConnectionType::DirectConnection);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopFunction, Qt::ConnectionType::DirectConnection);



    ui->lineEditThreads->setText(QString::number(ui->threadsSlider->value()));

    connect(ui->threadsSlider, &QSlider::valueChanged,
            this, [=](int value) {
                ui->lineEditThreads->setText(QString::number(value));
            });





}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::calcFunction()
{
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->outputField->setPlainText("Calculation is in progress...\nPlease wait. Press 'Stop' to abort the calculation process.");

    Method method;
    if(ui->radioNotCached->isChecked())
        method = NOT_CACHED;
    else
        method = CACHED;


    unsigned long long maxNumber = ui->spinBoxNumber->value();
    size_t numOfThreads = ui->threadsSlider->value();


    stopRequested = false;


    calcThread = std::thread([=, this]()
    {

        auto start = std::chrono::high_resolution_clock::now();
        constexpr unsigned long long max_value = std::numeric_limits<unsigned long long>::max();
        unsigned long long threshold = (max_value - 1) / 3;

        unsigned long long finalNumber = 0;
        unsigned long long finalChainLength = 0;
        bool finalIsOverflow = false;

        //const unsigned int numOfThreads = 20;

        const unsigned long long minNumber = 1;
        //unsigned long long maxNumber = 700'000'000ULL; //158000;

        std::atomic<std::size_t>* Lengths = new std::atomic<std::size_t>[maxNumber + 1] { 0 }; //new
        Lengths[1].store(1); //new




        std::thread threads[numOfThreads];
        unsigned long long startValues[numOfThreads];
        unsigned long long stopValues[numOfThreads];

        unsigned long long delta = (maxNumber - minNumber) / numOfThreads;

        for (size_t i = 0; i < numOfThreads; ++i)
        {
            startValues[i] = 0;
            if (i == 0)
            {
                startValues[i] = minNumber;
                stopValues[i] = startValues[i] + delta;
            }
            else
            {
                startValues[i] = stopValues[i - 1] + 1;
                if (i != numOfThreads - 1)
                    stopValues[i] = startValues[i] + delta;
                else
                    stopValues[i] = maxNumber;
            }
        }

        //Result res;


        //auto start = std::chrono::high_resolution_clock::now();


        //Result res = straightCalculation(minNumber, maxNumber, threshold);

        Result interThreadResults[numOfThreads];


        for (size_t i = 0; i < numOfThreads; ++i)
        {
            //threads[i] = std::thread(straightCalculation, std::ref(interThreadResults[i]), startValues[i], stopValues[i], threshold);
            //threads[i] = std::thread(atomicCalculation, std::ref(Lengths), std::ref(interThreadResults[i]), startValues[i], stopValues[i], threshold, maxNumber);
            if (method == CACHED)
                threads[i] = std::thread(atomicCalculation, std::ref(Lengths), std::ref(interThreadResults[i]), startValues[i], stopValues[i], threshold, maxNumber);
            else
                threads[i] = std::thread(straightCalculation, std::ref(interThreadResults[i]), startValues[i], stopValues[i], threshold);
        }



        for (size_t i = 0; i < numOfThreads; ++i)
        {
            threads[i].join();
        }

        for (size_t i = 0; i < numOfThreads; ++i)
        {
            if (interThreadResults[i].isOverflow)
            {
                finalIsOverflow = true;
                break;
            }
            if (interThreadResults[i].chainLength > finalChainLength)
            {
                finalNumber = interThreadResults[i].number;
                finalChainLength = interThreadResults[i].chainLength;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


        delete[] Lengths; //new

        //----------------------------------------------------------------------------------------
        //output

        QMetaObject::invokeMethod(this, [=, this]()
        {
            QString methodReport;
            if (method == CACHED)
                methodReport = "Calculation method: Cached";
            else if (method == NOT_CACHED)
                methodReport = "Calculation method: Not Cached";

            if(!finalIsOverflow)
            {
                ui->outputField->setPlainText("Maximum number: " + QString::number(maxNumber) + "\n" +
                                              "Number of threads: " + QString::number(numOfThreads) + "\n" +
                                              methodReport + "\n" +
                                              "The number having the longest Collatz chain: " + QString::number(finalNumber) + "\n" +
                                              "The length of the Collatz chain: " + QString::number(finalChainLength) + "\n" +
                                              "Execution time: " + QString::number(double(duration.count() / 1000)) + " ms" );
            }
            else
            {
                ui->outputField->setPlainText("Overflow has occurred! The calculation was stopped.");
            }


            ui->startButton->setEnabled(true);
            ui->stopButton->setEnabled(false);

        });

    });

    calcThread.detach();

}


void MainWindow::stopFunction()
{
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->outputField->setPlainText("Calculation was aborted.\nPress 'Start' to run the calculation.");
}
