#include <iostream>
#include <bitset>
#include <vector>
#include <cstdint>
#include <chrono>
#include <cmath>

/* Largest n for which we can calculate (size for bitset) */
const int MAX_N = 128;

int n;
int k;

std::bitset<MAX_N> minSet;
std::vector<int> minSetElements;
int minSetSize;


/* Check element causes 3AP, used for checking previous set in new n */
bool hasArithSimple(const std::vector<int> &setElements, std::bitset<MAX_N> &set, int added){
    /* (a,added,c) of (c, added, a)*/
    for(int a : setElements){
        int c = 2*added - a;
        if(c >= 0 && c < n && set[c]) return true;

        int sum = a + added;
        if(sum % 2 == 0){
            int mid = sum / 2;
            if(set[mid]) return true;
        }
    }

    return false;
}

/* Check if set is saturated, used for checking previous set in new n */
bool isSaturSimple(const std::vector<int> &setElements, std::bitset<MAX_N> &set){
    for(int i = 0; i < n; i++){
        if(!set[i] && !hasArithSimple(setElements, set, i)) {
            return false;
        }
    }

    return true;
}


/* Check if adding element causes AP in elements < alphaSize*/
bool hasArith(std::bitset<MAX_N> &set, const std::vector<int> &setElements, int added, int alphaSize){
    /* (a,added,c) or (c, added, a)*/
    for(int a : setElements){
        int c = 2*added - a;
        if(c >= 0 && c < alphaSize && set[c]) return true;

        int sum = a + added;
        if(sum % 2 == 0){
            int mid = sum / 2;
            if(set[mid]) return true;
        }
    }

    return false;
}

/* Check if set is saturated, with help of forbiden bits */
bool isSatur(int alphaSize, std::bitset<MAX_N> &set, std::vector<int> &setElements){
    for(int i = 0; i < alphaSize; i++){
        if(!set[i]){
            if(!hasArith(set, setElements,i,alphaSize)) return false;
        }
    }

    return true;
}


/* Check if this set is the smallest between it and its mirror */
bool isSmallerThanMirror(const std::bitset<MAX_N> &set) {
    for (int el = n - 1; el >= 0; el--) {
        if (set[el] && !set[n - el - 1]) return false;
        if (!set[el] && set[n - el - 1]) return true;
    }
    return true;
}



void minSatSet(__uint128_t alpha, __uint128_t f, int alphaSize, int onesInAlpha, std::bitset<MAX_N> &set, std::vector<int> &setElements){

    /* If the built set is too big, return */
    if(onesInAlpha > minSetSize) return;
    if(alphaSize > n) return;
    

    /* If this is not the cannonical version of the set, return */
    if(onesInAlpha > 0 && !isSmallerThanMirror(set)) return;
    
    
    /* If the size of subset == k, check for saturation, if found stop search, else return*/
    __uint128_t mask = __uint128_t(1) << (n-alphaSize);
    mask--;
    if((f & mask) == mask){
        if(isSatur(alphaSize,set,setElements)){
            if(onesInAlpha <= minSetSize){
                minSetElements = setElements;
                minSet = set;
                minSetSize = setElements.size();
            }
            return;
        }
    } 
    
    /* Skip all elements from forbiden, that form 3AP*/
    while((f & 1) && alphaSize <= n){
        alpha = alpha << 1;
        f = f >> 1;
        alphaSize++;
    }
    if(alphaSize == n) return;

    /* Append 0 and 1 to alpha, and corresponding forbiden */
    __uint128_t alpha0 = alpha << 1;   
    __uint128_t alpha1 = alpha0 | 1;
    __uint128_t f0 = f >> 1;
    __uint128_t f1 = (f >> 1) | alpha;

    /* Recursively call for alpha0 */
    minSatSet(alpha0,f0,alphaSize+1, onesInAlpha,set,setElements);

    /* Recursively call for alpha1 */
    set[(alphaSize)] = true;
    setElements.push_back(alphaSize);
    minSatSet(alpha1,f1,alphaSize+1, onesInAlpha+1,set,setElements);
    setElements.pop_back();
    set[(alphaSize)] = false;
}

int main() {
    int in = 0;
    std::cin >> in;

    /* n is the current size of the set in which we search */
    n = 4;
    /* k is the maximum depth (number of elements in alpha) */
    k = 2;

    
    while(n <= in){
        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<int> setElements;
        std::bitset<MAX_N> set;
        /* Search */
        minSetSize = k;
        minSatSet(0,0,0,0,set,setElements);

        /* If the set is found print set and increase n and set k to minSetSize + 1 */
        std::cout << "n: " << n  << " k: " << k << " Set: ";
        for (int x : minSetElements) {
            std::cout << x+1 << " ";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << " Time: " << duration.count() << " milisecond";
        std::cout << std::endl;

        n++;
        k = minSetSize + 1;

    }
}
