// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <CoreTest/AudioTest.h>
#include <CoreTest/TimeTest.h>

#if defined(TL_TESTS_HAS_FFMPEG)
#include <IOTest/AudioResampleTest.h>
#endif

#include <ftk/Core/Context.h>
#include <ftk/Core/String.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    int r = 0;
    try
    {
        auto context = ftk::Context::create();

        // Build the list of tests.
        std::vector<std::shared_ptr<ftk::test::ITest> > tests;
        tests.push_back(tl::core_test::TimeTest::create(context));
        tests.push_back(tl::core_test::AudioTest::create(context));
#if defined(TL_TESTS_HAS_FFMPEG)
        tests.push_back(tl::io_test::AudioResampleTest::create(context));
#endif

        // Optional command-line filter: any argument is matched
        // case-insensitively against the test name.
        std::vector<std::string> filters;
        for (int i = 1; i < argc; ++i)
        {
            filters.emplace_back(argv[i]);
        }

        std::vector<std::shared_ptr<ftk::test::ITest> > toRun;
        if (filters.empty())
        {
            toRun = tests;
        }
        else
        {
            for (const auto& f : filters)
            {
                for (const auto& t : tests)
                {
                    if (ftk::contains(
                            t->getName(),
                            f,
                            ftk::CaseCompare::Insensitive))
                    {
                        toRun.push_back(t);
                    }
                }
            }
        }

        const auto startTime = std::chrono::steady_clock::now();
        for (const auto& t : toRun)
        {
            std::cout << "Running test: " << t->getName() << std::endl;
            t->run();
        }
        const auto endTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = endTime - startTime;
        std::cout << "Seconds elapsed: " << elapsed.count() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        r = 1;
    }
    return r;
}
