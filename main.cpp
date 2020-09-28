#include <iostream>
#include <vector>
#include <fstream>
// Count inverses in array with merge split for O(nlogn)

void merge(std::vector <int> &A, std::vector <int> &memory, int left, int middle_idx, int right, long &inv_cnt){
    for (int i = left; i <= right; ++i) // update helper array from the last recursion step
        memory[i] = A[i];
    int i = left; // iterator, starts from first element of left side | [left; middle]
    int j = middle_idx + 1; // iterator, starts from the first element of right side | [middle+1; right]
    int k = left; // iterator through the helper memory array

    while (i <= middle_idx && j <= right){ // until at least one half is over
        if (memory[i] <= memory[j]) {
            A[k] = memory[i++]; // ok, it isn't inverse
        }
        else { // it is inverse: we have already sorted left part in
            // which we push element from the right, so for inverse counting
            // we need to add middle_index - i + 1. For ex, right=4[3], left=0[1], middle_idx=2[5], i=2[5], j=3[3]:
            // [1,2,#,5] [3,4] here we need to place '3' before '5' and after '2', to the i position.
            // Our left part have been sorted already, so there are inverses of j element with all >=i elements.
            // Because of recursion, we will count inverses for all parts -> elements.
            A[k] = memory[j++];
            inv_cnt += (middle_idx - i + 1);
        }
        k++; // iterates from left to right in original array
    }
    // The way we choosing indexes (right-(middle+1) > (middle-left) allow us to think that
    // j value will be more than i value or equals it when while ends, so we just need to
    // change our array with remaining left part' values (we have already pushed to left all elements from right
    // that lower than some elements from left, at this point we have elements from right
    // that more than all elements from left (and pushed elements from right),
    // but they are already in A (we didn't change their positions).
    while (i <= middle_idx)
        A[k++] = memory[i++];
}

void mergesort(std::vector <int> &A, std::vector <int> &memory, int left, int right, long &inv_cnt){
    // Merge sort main recursive function
    // [left; middle] - left part, [middle+1; right] - right part
    if (left < right) {
        int middle_idx = left + (right - left) / 2; // index of middle element
        mergesort(A, memory, left, middle_idx, inv_cnt); // sort the first part [left, middle_idx]
        mergesort(A, memory, middle_idx + 1, right, inv_cnt); // sort the second part (middle_idx, right]
        merge(A, memory, left, middle_idx, right, inv_cnt); // merge this parts into one
    }
}

int main()
{
    std::ifstream fin("inverse.in");
    std::ofstream fout("inverse.out");
    int n; fin >> n;
//    int n = 5;
    long inv_cnt = 0;
    std::vector <int> arr(n);
//    std::vector <int> arr = {3, 4, 5, 6, 7, 8};
    std::vector <int> memory = arr; // helper array: copy of an original array on the each recursion step
    for (int i = 0; i < n; ++i) fin >> arr[i];
    mergesort(arr, memory, 0, n-1, inv_cnt);
//    std::cout << inv_cnt;
    fout << inv_cnt; fin.close(); fout.close();
    return 0;
}