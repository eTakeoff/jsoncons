// Copyright 2022 Daniel Parker
// Distributed under Boost license

#include <catch/catch.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

//  Left run is A[iLeft :iRight-1].
// Right run is A[iRight:iEnd-1  ].
using std::chrono::high_resolution_clock;


template <class T,class Compare>
void BottomUpMerge(T* A, std::size_t iLeft, std::size_t iRight, std::size_t iEnd, T* B, Compare compare)
{
    std::size_t i = iLeft, j = iRight;
    // While there are elements in the left or right runs...

    for (std::size_t k = iLeft; k < iEnd; k++) 
    {
        if (i < iRight && (j >= iEnd || !compare(A[j],A[i]) ))
        {
            B[k] = std::move(A[i]);
            i = i + 1;
        }
        else
        {
            B[k] = std::move(A[j]);
            j = j + 1;    
        }
    }
}

template <class T,class Compare>
void BottomUpMergeSort(T& A, T& B, Compare compare)
{
    // Each 1-element run in A is already "sorted".
    // Make successively longer sorted runs of length 2, 4, 8, 16... until the whole array is sorted.
    const std::size_t n = A.size();


    typename T::pointer p = A.data();
    typename T::pointer q = B.data();
    for (std::size_t width = 1; width < n; width = 2 * width)
    {
        // Array A is full of runs of length width.
        for (std::size_t i = 0; i < n; i = i + 2 * width)
        {
            // Merge two runs: A[i:i+width-1] and A[i+width:i+2*width-1] to B[]
            // or copy A[i:n-1] to B[] ( if (i+width >= n) )
            BottomUpMerge(p, i, (std::min)(i+width, n), (std::min)(i+2*width, n), q, compare);
        }
        // Now work array B is full of runs of length 2*width.
        // Swap the roles of A and B.

        std::swap(p,q);
        // Now array A is full of runs of length 2*width.
    }
    if (p != A.data())
    {
        std::swap(A, B);
    }
}



TEST_CASE("jsoncons::detail::merge_sort tests")
{
    SECTION("test1")
    {
        std::vector<int> v = {3,  4,  2,  1,  7,  5,  8,  9,  0,  6};

        std::vector<int> work(v.size());

        BottomUpMergeSort(v, work, std::less<int>());
        for (auto i : v)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
    }
    SECTION("test2")
    {
        std::vector<int> u;
        for (int i = 0; i < 9999; ++i)
        {
            u.push_back(i);
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(u.begin(),u.end(),g);

        std::vector<int> v{u};

        std::stable_sort(u.begin(),u.end());

        std::vector<int> w(v.size());
        BottomUpMergeSort(v,w,std::less<int>());

        CHECK(u == v);
    }
    SECTION("test3")
    {
        std::vector<int> u;
        for (int i = 0; i < 99; ++i)
        {
            u.push_back(i);
            u.push_back(i);
            u.push_back(i);
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(u.begin(),u.end(),g);

        auto start1 = high_resolution_clock::now();
        std::vector<int> x;
        for (int i = 0; i < 100; ++i)
        {
            x = u;
            std::stable_sort(x.begin(),x.end());
        }
        auto end1 = high_resolution_clock::now();
        auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();;

        std::cout << "stable_sort: " << dur1 << "\n";

        std::vector<int> w(u.size());

        auto start = high_resolution_clock::now();
        std::vector<int> y;
        for (int i = 0; i < 100; ++i)
        {
            y = u;
            BottomUpMergeSort(y,w,std::less<int>());
        }
        auto end = high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();;

        std::cout << "stable_sort: " << dur1 << ", merge_sort: " << dur << "\n";

        CHECK(x == y);
    }
}

