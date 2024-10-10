#include <iostream>
#include <unordered_map>
using namespace std;

int main(){
    cout << "Hello Linux" << endl;
    
    // int arr[] {1,2,3,4,5,6};
    // for(auto e : arr){
    //     cout << e << endl;
    // }

    unordered_map<int,int> um {{1,1},{2,2}};
    um[2] = 3;
    for(auto e : um){
        cout << e.second << endl;
    }


    return 0;
}