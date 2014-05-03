#ifndef __HAYAI_CONSOLEOUTPUTTER
#define __HAYAI_CONSOLEOUTPUTTER
#include "hayai_outputter.hpp"
#include "hayai_console.hpp"

namespace hayai
{
    /// Console outputter.

    /// Prints the result to standard output.
    class ConsoleOutputter
        :   public Outputter
    {
    public:
        virtual void Begin(const std::size_t& benchmarksCount)
        {
            std::cout << std::fixed;
            std::cout << Console::TextGreen << "[==========]"
                      << Console::TextDefault << " Running "
                      << benchmarksCount
                      << (benchmarksCount == 1 ?
                          " benchmark." :
                          " benchmarks.")
                      << std::endl;
        }


        virtual void End(const std::size_t& benchmarksCount)
        {
            std::cout << Console::TextGreen << "[==========]"
                      << Console::TextDefault << " Ran " << benchmarksCount
                      << (benchmarksCount == 1 ?
                          " benchmark." :
                          " benchmarks.")
                      << std::endl;
        }


        virtual void BeginTest(const std::string& fixtureName,
                               const std::string& testName,
                               const std::string& parameters,
                               const std::size_t& runsCount,
                               const std::size_t& iterationsCount)
        {
            std::cout << Console::TextGreen << "[ RUN      ]"
                      << Console::TextYellow << " "
                      << fixtureName << "."
                      << testName
                      << parameters
                      << Console::TextDefault
                      << " (" << runsCount
                      << (runsCount == 1 ? " run, " : " runs, ") 
                      << iterationsCount
                      << (iterationsCount == 1 ?
                          " iteration per run)" :
                          " iterations per run)")
                      << std::endl;
        }


        virtual void EndTest(const std::string& fixtureName,
                             const std::string& testName,
                             const std::string& parameters,
                             const TestResult& result)
        {
#define PAD(x) std::cout << std::setw(34) << x << std::endl;
#define PAD_DEVIATION(description,                                      \
                      deviated,                                         \
                      average,                                          \
                      unit)                                             \
            {                                                           \
                double _d_ =                                            \
                    double(deviated) - double(average);                 \
                                                                        \
                PAD(description <<                                      \
                    deviated << " " << unit << " (" <<                  \
                    (deviated < average ?                               \
                     Console::TextRed :                                 \
                     Console::TextGreen) <<                             \
                    (deviated > average ? "+" : "") <<                  \
                    _d_ << " " << unit << " / " <<                      \
                    (deviated > average ? "+" : "") <<                  \
                    (_d_ * 100.0 / average) << " %" <<                  \
                    Console::TextDefault << ")");                       \
            }
#define PAD_DEVIATION_INVERSE(description,                              \
                              deviated,                                 \
                              average,                                  \
                              unit)                                     \
            {                                                           \
                double _d_ =                                            \
                    double(deviated) - double(average);                 \
                                                                        \
                PAD(description <<                                      \
                    deviated << " " << unit << " (" <<                  \
                    (deviated > average ?                               \
                     Console::TextRed :                                 \
                     Console::TextGreen) <<                             \
                    (deviated > average ? "+" : "") <<                  \
                    _d_ << " " << unit << " / " <<                      \
                    (deviated > average ? "+" : "") <<                  \
                    (_d_ * 100.0 / average) << " %" <<                  \
                    Console::TextDefault << ")");                       \
            }
            
            std::cout << Console::TextGreen << "[     DONE ]"
                      << Console::TextYellow << " "
                      << fixtureName << "."
                      << testName
                      << parameters
                      << Console::TextDefault << " ("
                      << std::setprecision(6)
                      << (result.TimeTotal() / 1000000.0) << " ms)"
                      << std::endl;

            std::cout << Console::TextBlue << "[          ] "
                      << Console::TextDefault
                      << "Fastest time: "
                      << std::setprecision(3)
                      << result.RunTimeMinimum() / 1000.0 << " us"
                      << std::endl;


#undef PAD_DEVIATION_INVERSE
#undef PAD_DEVIATION
#undef PAD
        }
    };
}
#endif
