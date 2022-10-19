// Copyright 2022 Daniel Parker
// Distributed under Boost license

#include <catch/catch.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

//  Left run is A[iLeft :iRight-1].
// Right run is A[iRight:iEnd-1  ].
template <class T,class Compare>
void BottomUpMerge(T* A, std::size_t iLeft, std::size_t iRight, std::size_t iEnd, T* B, Compare compare)
{
    std::size_t i = iLeft, j = iRight;
    // While there are elements in the left or right runs...
    for (std::size_t k = iLeft; k < iEnd; k++) {
        // If left run head exists and is <= existing right run head.
        /* if (i < iRight && (j >= iEnd || A[i] <= A[j]))
        {
            B[k] = std::move(A[i]);
            i = i + 1;
        } 
        else 
        {
            B[k] = std::move(A[j]);
            j = j + 1;    
        }*/
        if (i >= iRight || (j < iEnd && compare(A[j],A[i])))
        {
            B[k] = std::move(A[j]);
            j = j + 1;
        }
        else
        {
            B[k] = std::move(A[i]);
            i = i + 1;
        }
    }
}

template <class T,class Compare>
void BottomUpMergeSort(T* A, T* B, std::size_t n, Compare compare)
{
    // Each 1-element run in A is already "sorted".
    // Make successively longer sorted runs of length 2, 4, 8, 16... until the whole array is sorted.
    for (std::size_t width = 1; width < n; width = 2 * width)
    {
        // Array A is full of runs of length width.
        for (std::size_t i = 0; i < n; i = i + 2 * width)
        {
            // Merge two runs: A[i:i+width-1] and A[i+width:i+2*width-1] to B[]
            // or copy A[i:n-1] to B[] ( if (i+width >= n) )
            BottomUpMerge(A, i, (std::min)(i+width, n), (std::min)(i+2*width, n), B, compare);
        }
        // Now work array B is full of runs of length 2*width.
        // Copy array B to array A for the next iteration.
        // A more efficient implementation would swap the roles of A and B.
        // CopyArray(B, A, n);

        std::swap(A,B);
        // 
        // Now array A is full of runs of length 2*width.
    }
}

TEST_CASE("jsoncons::detail::merge_sort tests")
{
    SECTION("test1")
    {
        std::vector<int> v = {3,  4,  2,  1,  7,  5,  8,  9,  0,  6};

        std::vector<int> work(v.size());

        BottomUpMergeSort(v.data(), work.data(), v.size(), std::less<int>());
        for (auto i : v)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
    }
}

