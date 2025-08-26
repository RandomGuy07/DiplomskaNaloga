#include <iostream>
#include <bitset>
#include <vector>
#include <cstdint>
#include <chrono>

/* Largest n for which we can calculate (size for bitset) */
const int MAX_N = 128;

/* Array of sizes of largest 3AP free subsets */
int largestSizes[212] = {
    0,1,2,2,3,4,4,4,4,5,5,6,6,7,8,8,8,8,8,8,9,9,9,9,10,10,11,11,11,11,12,12,13,13,13,13,14,14,14,14,15,16,16,16,16,16,16,16,16,16,16,17,17,17,18,18,18,18,19,19,19,19,19,20,20,20,20,20,20,20,20,21,21,21,22,22,22,22,22,22,22,22,23,23,24,24,24,24,24,24,24,24,25,25,25,26,26,26,26,26,27,27,27,27,28,28,28,28,28,28,28,29,29,29,30,30,30,30,30,30,30,31,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,33,33,33,33,33,33,33,33,34,34,34,34,34,35,35,35,35,35,35,35,36,36,36,36,36,36,37,37,38,38,38,38,39,39,39,39,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,41,41,41,41,41,41,41,41,41,41,42,42,42,42,42,43,43,43
};

int n;
int k;

std::bitset<MAX_N> minSet;
std::vector<int> minSetElements;
bool found;

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
bool isSatur(__uint128_t f, int alphaSize, std::bitset<MAX_N> &set, std::vector<int> &setElements){
    __uint128_t mask = __uint128_t(1) << (n-alphaSize);
    mask--;
    //std::cout << std::bitset<64>(mask).to_string() << std::endl;
    if((f & mask) != mask) return false;

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
    /* If set is found stop search */
    if(found) return;

    /* If the built set is too big, return */
    if(alphaSize > n) return;

    /* If the remaining elements to be added is more than the largest 3AP free set, return */
    if(largestSizes[n - alphaSize] < (k - onesInAlpha)) return;

    /* If this is not the cannonical version of the set, return */
    if(onesInAlpha > 0 && !isSmallerThanMirror(set)) return;

    /* If the size of subset == k, check for saturation, if found stop search, else return*/
    if(onesInAlpha == k){
        if(isSatur(f,alphaSize,set,setElements)){
            minSetElements = setElements;
            minSet = set;
            found = true;
        }
        return;
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

    
    n = 4;
    k = 2;

    auto startTime = std::chrono::high_resolution_clock::now();

    while(n <= in){
        
        /* Check if previous set also saturated */
        if(isSaturSimple(minSetElements,minSet)){
            std::cout << "n: " << n << " Set: ";
            for (int x : minSetElements) {
                std::cout << x+1 << " ";
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << " Time: " << duration.count() << " miliseconds";
            std::cout << std::endl;
            n++;
            continue;
        }

        found = false;
        std::vector<int> setElements;
        std::bitset<MAX_N> set;

        /* Search */
        minSatSet(0,0,0,0,set,setElements);

        /* If the set is found print set and increase n, else increase k */
        if(found){
            std::cout << "n: " << n << " Set: ";
            for (int x : minSetElements) {
                std::cout << x+1 << " ";
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << " Time: " << duration.count() << " miliseconds";
            std::cout << std::endl;
            n++;
        }else{
            //std::cout << "Not found.\n";
            //break;
            k++;
        }
    }
}
