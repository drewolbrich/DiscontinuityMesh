
#include <iostream>
#include <list>
#include <vector>

#include <boost/pool/pool_alloc.hpp>

#include <os/Time.h>

int
main(int, char **)
{
    std::cout << "list:" << std::endl;

    {
        std::list<int, boost::fast_pool_allocator<int> > mIntList;
        std::list<double, boost::fast_pool_allocator<double> > mDoubleList;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntList.push_back(index);
            mDoubleList.push_back(index);
        }
        mIntList.clear();
        mDoubleList.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "boost fast_pool_allocator = " << duration.asDouble() << std::endl;
    }

    {
        std::list<int, boost::pool_allocator<int> > mIntList;
        std::list<double, boost::pool_allocator<double> > mDoubleList;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntList.push_back(index);
            mDoubleList.push_back(index);
        }
        mIntList.clear();
        mDoubleList.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "boost pool_allocator = " << duration.asDouble() << std::endl;
    }


    {
        std::list<int> mIntList;
        std::list<double> mDoubleList;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntList.push_back(index);
            mDoubleList.push_back(index);
        }
        mIntList.clear();
        mDoubleList.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "default allocator = " << duration.asDouble() << std::endl;
    }


    std::cout << std::endl;
    std::cout << "vector:" << std::endl;

    {
        std::vector<int, boost::fast_pool_allocator<int> > mIntVector;
        std::vector<double, boost::fast_pool_allocator<double> > mDoubleVector;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntVector.push_back(index);
            mDoubleVector.push_back(index);
        }
        mIntVector.clear();
        mDoubleVector.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "boost fast_pool_allocator = " << duration.asDouble() << std::endl;
    }

    {
        std::vector<int, boost::pool_allocator<int> > mIntVector;
        std::vector<double, boost::pool_allocator<double> > mDoubleVector;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntVector.push_back(index);
            mDoubleVector.push_back(index);
        }
        mIntVector.clear();
        mDoubleVector.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "boost pool_allocator = " << duration.asDouble() << std::endl;
    }


    {
        std::vector<int> mIntVector;
        std::vector<double> mDoubleVector;

        os::TimeValue start = os::GetProcessUserTime();

        for (int index = 0; index < 100000; ++index) {
            mIntVector.push_back(index);
            mDoubleVector.push_back(index);
        }
        mIntVector.clear();
        mDoubleVector.clear();

        os::TimeValue stop = os::GetProcessUserTime();

        os::TimeValue duration = stop - start;
        std::cout << "default allocator = " << duration.asDouble() << std::endl;
    }

}

