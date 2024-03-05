/* TrafficProducer.cpp
 * Author: Lovleen Kala
 * Date: 04 March 2023
 * 
 * Description:
 * This file is designed to be run by a scheduler to log traffic information to the log.txt file.
 * It simulates traffic data for a set number of traffic lights and logs the information in a structured format.
 */

#include <iostream>
#include <random>
#include <ctime>
#include <fstream>
#include <time.h>

using namespace std;

#define NUM_LIGHTS 12       // Number of simulated traffic lights

int main(){

    string output;
    
    time_t rawTime;
    time(&rawTime);

    srand (time(NULL));

    for (int i = 1 ; i <= NUM_LIGHTS ; i++){
        
        output += to_string(rawTime) + " "; // Timestamp
        output+= to_string(i) + " ";        // Traffic light number
        int cars = rand() % ((10 - 1) + 1) + 1;
        output+= to_string(cars) + " \n";   // Number of cars (random) for this timestamp and light
    }

    fstream log("log.txt", ios::app);                       // Append to the log file
    if(!log.is_open()){
        cout<<"Error: Log file not found"<<endl;
    } else {
        log<<output;
        cout<<output;
        cout<<"log.txt has been updated at "<<ctime(&rawTime)<<endl;
    }
    return 0;
}
