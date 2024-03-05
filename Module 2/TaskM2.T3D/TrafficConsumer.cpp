#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

#define NUM_LIGHTS 12
#define MEASUREMENTS 13     // 12 per hour plus held avg value

// Function to print traffic details for each traffic light
void printDetails(int array[NUM_LIGHTS][MEASUREMENTS]){
    for(int i = 1; i <= NUM_LIGHTS ; i++){
        cout << to_string(i) + ":\t";
        for (int j = 0 ; j < sizeof(array[i])/4 ; j++){
            cout << to_string(array[i-1][j]) + ";";
        }
        cout << endl;
    }
}

// Function to calculate the average traffic volume for each traffic light
void caluclateTrafficVolume(int array[NUM_LIGHTS][MEASUREMENTS]){
    cout << "calculating...\t";
    for (int i = 0 ; i < NUM_LIGHTS ; i++){
        array[i][0] = 0;
        for (int j = 1 ; j < MEASUREMENTS ; j++){
            array[i][0] += array[i][j];
        }
        array[i][0] = array[i][0] / (MEASUREMENTS-1); // integer division for simplicity
    }
    cout << "Done" << endl;
}

int main(){
    int Array[NUM_LIGHTS][MEASUREMENTS];
    int arraySize = sizeof(Array[1])/4;      // 4bit
    long rawTime;

    int lightNum;
    int trafficVolume;

    fstream log("log.txt", ios::in);
    if(!log.is_open()){
        cout << "Error: Log file not found" << endl;
    } else {
        cout << "Retrieving initial data from log.txt...\t";
        for(int i = 1 ; i <= NUM_LIGHTS ; i++){
            for (int j = 1 ; j < arraySize ; j++){
                Array[i-1][j] = 0;
            }
        }
        
        // Read initial data from the log file
        log >> rawTime >> lightNum >> trafficVolume;

        for(int k = 0 ; k < 12 ; k++){
            for(int i = 0 ; i < NUM_LIGHTS ; i++){
                // Read timestamp, light number, and traffic volume from the log file
                log >> rawTime >> lightNum >> trafficVolume;

                // Shift the traffic volume data in the array
                for(int j = sizeof(Array[0]) / sizeof(Array[0][0]) - 1 ; j > 0; j--){
                    Array[lightNum-1][j] = Array[lightNum-1][j-1];
                }
                Array[lightNum-1][1] = trafficVolume;
            }
        }
        log.close();
        cout << "Complete" << endl;
    }

    // Calculate and display the average traffic volume for each traffic light
    caluclateTrafficVolume(Array);
    // printDetails(Array);
}
