#include <iostream>
#include "Thread.hpp"

void Routine(){
    int num = 0;
    while(true){
        cout << "thread is runing ... and the num is " << num++  << endl;
        sleep(1);
    }
}

int main(){
    Thread thread(Routine);
    thread.Run();

    thread.jojn();
    return 0;
}