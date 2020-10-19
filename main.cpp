//  Sultan Alneif & Don-Thuan Le
//  CPSC 3500 Computing Systems
//  Hw4
//  main.cpp
//
//  Copyright © 2020 Don-Thuan Le. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <semaphore.h>
#include <fstream>
#include <queue>

using namespace std;

// semaphore
sem_t carSema;

// mutex locks
pthread_mutex_t mutexFlaggerN; // for north queue
pthread_mutex_t mutexFlaggerS; // for south queue
pthread_mutex_t mutexCarWrite; // for car.log file

pthread_mutex_t mutexCarCount; // so carID doesn't get mixed up.

// car ID counter
int maxCars;
int carCount = 0;
int flaggerCarCount = 0;

//Car.log and flagperson.log files
string car_file = "car.log";
string worker_file = "flagperson.log";


// which direction car is coming from, either N or S
char direction = 'N';

// structure for each car created
struct car{
    int carID;
    char dir;
    string arrival;
    string start;
    string finish;
};

// tracks when flagger sleeps and wakes up
struct flagger{
    string sleep;
    string wakeup;
    
};
struct flagger flaggerSleep;

// delcaration of queues
queue<car> northSide;
queue<car> southSide;

// time function, converts current time to string format for easy print
string getTime(){
    time_t current_time;
    struct tm * local_time;
    char arr[30];
    string converted_time;
    
    time(&current_time);
    local_time = localtime(&current_time);
    strftime(arr, sizeof(arr), "%H:%M:%S", local_time);
    converted_time = arr;
    return converted_time;
}

//given function that stalls a thread
int pthread_sleep (int seconds) //sleep function
{
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    struct timespec timetoexpire;
    if(pthread_mutex_init(&mutex,NULL))
    {
        return -1;
    }
    if(pthread_cond_init(&conditionvar,NULL))
    {
        return -1;
    }
    //When to expire is an absolute time, so get the current time and add
    //it to our delay time
    timetoexpire.tv_sec = (unsigned int)time(NULL) + seconds;
    timetoexpire.tv_nsec = 0;
    return pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
}

// PRODUCER thread for the north cars
void* produceNorth(void* args){
    struct car newCar;
    
    while(carCount <= maxCars){
        pthread_mutex_lock(&mutexFlaggerN);
        //80% chance of car being produced
        while(((rand() % 10 + 1) < 9) && (carCount <= maxCars)){ // 80% chance of another car
            pthread_mutex_lock(&mutexCarCount); // aquire lock so both producers don't create
            if (carCount >= maxCars){           // the car with the same ID
                pthread_mutex_unlock(&mutexFlaggerN);
                pthread_mutex_unlock(&mutexCarCount);
                return NULL;
            }
            carCount++;                          // increment car ID
            newCar.carID = carCount;
            pthread_mutex_unlock(&mutexCarCount);   // release lock for carID
            newCar.dir = 'N';
            newCar.arrival = getTime();
            northSide.push(newCar);
            sem_post(&carSema); // increment semaphore so flagger(consumer) knows there are cars
        }
        // no car wait 20 sec
        pthread_sleep(20);
        // release
        pthread_mutex_unlock(&mutexFlaggerN);
    }
    return NULL;
}

// PRODUCER thread for the south cars (same as thread for the north but for the south queue)
void* produceSouth(void* args){
    struct car newCar;
    while(carCount <= maxCars){
        pthread_mutex_lock(&mutexFlaggerS);
        
        while(((rand() % 10 + 1) < 9) && (carCount <= maxCars)){
            pthread_mutex_lock(&mutexCarCount);
            if (carCount >= maxCars){
                pthread_mutex_unlock(&mutexFlaggerS);
                pthread_mutex_unlock(&mutexCarCount);
                return NULL;
            }
            carCount++;
            newCar.carID = carCount;
            pthread_mutex_unlock(&mutexCarCount);
            newCar.dir = 'S';
            newCar.arrival = getTime();
            southSide.push(newCar);
            sem_post(&carSema);
        }
        
        pthread_sleep(20);
        
        pthread_mutex_unlock(&mutexFlaggerS);
    }
    return NULL;
}

// CAR thread, simulates 1 sec time to go through construction, specifically for the north
void* carThreadN(void* args){
    struct car carPass;
    
    pthread_mutex_lock(&mutexFlaggerN); // aquire lock to pop off of the queues
    carPass = northSide.front();
    northSide.pop();
    pthread_mutex_unlock(&mutexFlaggerN); // release lock on queues

    carPass.start = getTime();
    pthread_sleep(2);            // car takes one second to go through the construction zone
    carPass.finish = getTime();
    
    pthread_mutex_lock(&mutexCarWrite); // aquire the lock to write to the car file
    
    // WRITE CAR INFORMATION TO FILE (check)
    
    ofstream car_stream(car_file.c_str(), ios_base::out | ios_base::app);
    car_stream << carPass.carID << "\t" << carPass.dir << "\t" << carPass.arrival << "\t";
    car_stream << carPass.start << "\t" << carPass.finish << endl;
    car_stream.close();

    pthread_mutex_unlock(&mutexCarWrite); // release the lock to the car.log file
   // pthread_detach(pthread_self());       // detach thread, all cars just go off on their own
    return NULL;
}

// CAR thread, simulates 1 sec time to go through construction, specifically for the south
// similar to carThreadN
void* carThreadS(void* args){
    struct car carPass;
    
    pthread_mutex_lock(&mutexFlaggerS);
    carPass = southSide.front();
    southSide.pop();
    pthread_mutex_unlock(&mutexFlaggerS);

    carPass.start = getTime();
    pthread_sleep(2);
    carPass.finish = getTime();
    
    pthread_mutex_lock(&mutexCarWrite);
    
    // WRITE CAR INFORMATION TO FILE (check)
    
    ofstream car_stream(car_file.c_str(), ios_base::out | ios_base::app);
    car_stream << carPass.carID << "\t" << carPass.dir << "\t" << carPass.arrival << "\t";
    car_stream << carPass.start << "\t" << carPass.finish << endl;
    car_stream.close();

    pthread_mutex_unlock(&mutexCarWrite);
    //pthread_detach(pthread_self());
    return NULL;
}

// writes flagger struct to flagperson.log file
void flaggerWrite(){
    ofstream worker_stream(worker_file.c_str(), ios_base::out | ios_base::app);
   // check semaphore and decide
   //if asleep
        worker_stream << "asleep";
        worker_stream << "\t" << flaggerSleep.sleep << endl;
    //if awake
        worker_stream << "wake up";
        worker_stream << "\t" << flaggerSleep.wakeup << endl;
}
        

void* Flagger(void* args){ // CONSUMER thread, pops cars off from the queues and starts carthread

    int i;
    pthread_t carPID;

    while(flaggerCarCount < maxCars){
        flaggerSleep.sleep = getTime();       // records when the flagger sleeps
        sem_wait(&carSema);                   // semaphore to tell when there are cars in queues
        flaggerSleep.wakeup = getTime();      // records when the flagger wakes to let cars go
        flaggerWrite();                       // write flagger info to .log file
        if (direction == 'N'){
            if (northSide.size() < 10 && southSide.size() >= 10){
                direction = 'S';
            } else if (northSide.empty()){          // If statement that decides whether the
                direction = 'S';                    // north or south side should be signaled
            } else if (!northSide.empty()){         // to go
                pthread_mutex_lock(&mutexFlaggerN);
                for (i = 0; i < northSide.size(); i++){
                    if(pthread_create(&carPID, NULL, &carThreadN, NULL)){
                        perror("car thread create error");
                        return NULL;
                    }
                    flaggerCarCount++;
                }
                pthread_mutex_unlock(&mutexFlaggerN);
                direction = 'S';
            }
        } else if (direction == 'S'){
            if (southSide.size() < 10 && northSide.size() >= 10){
                direction = 'N';
            } else if (southSide.empty()){
                direction = 'N';
            } else if (!southSide.empty()){
                pthread_mutex_lock(&mutexFlaggerS);
                for (i = 0; i < southSide.size(); i++){
                    if(pthread_create(&carPID, NULL, &carThreadS, NULL)){
                        perror("car thread create error");
                        return NULL;
                    }
                    pthread_detach((pthread_t)carPID);
                    flaggerCarCount++;
                }
                pthread_mutex_unlock(&mutexFlaggerS);
                direction = 'N';
            }
        }
    }
    return NULL;
}

// main function
int main(int argc, const char * argv[]) {
    int* p = new int;
    *p = atoi(argv[1]);  // take in command line to know how many cars to produce
    maxCars = *p;
    pthread_t northtid;                 //thread PID's
    pthread_t southtid;
    pthread_t flaggertid;
    
    int pshared = 1;                    //semaphore init values
    int value = 0;
    
    // header for the car.log file
    ofstream car_stream(car_file.c_str(), ios_base::out | ios_base::trunc);
    car_stream << "carID \t direction \t arrival-Time \t start-Time \t end-Time" << endl;
    car_stream.close();
    
    // header for the flagperson.log file
    ofstream worker_stream(worker_file.c_str(), ios_base::out | ios_base::trunc);
    worker_stream << "Time \t\t State" << endl;
    worker_stream.close();

    // initialize semaphore
    if (sem_init(&carSema, pshared, value) != 0){
        perror("sem_init error");
        return -1;
    }
    
    // create producer north thread
    if(pthread_create(&northtid, NULL, &produceNorth, NULL)){
        perror("Producer north thread creation error");
        return -1;
    }
    
    // create producer south thread
    if(pthread_create(&southtid, NULL, &produceSouth, NULL)){
        perror("Producer south thread creation error");
        return -1;
    }

    // create consumer flagger thread
    if(pthread_create(&flaggertid, NULL, &Flagger, NULL)){
        perror("Flagger thread creation error");
        return -1;
    }

    // join producer north thread
    if(pthread_join(northtid, NULL)){
        perror("joining error north");
        return -1;
    }
    
    // join producer south thread
    if(pthread_join(southtid, NULL)){
        perror("joining error south");
        return -1;
    }
    
    // join consumer flagger thread
    if(pthread_join(flaggertid, NULL)){
        perror("joining error flagger");
        return -1;
    }
    
    pthread_sleep(10);
    
    pthread_mutex_destroy(&mutexFlaggerN);      // destroy locks and semaphores
    pthread_mutex_destroy(&mutexFlaggerS);
    pthread_mutex_destroy(&mutexCarWrite);
    pthread_mutex_destroy(&mutexCarCount);

    sem_destroy(&carSema);
    
    delete p;
    cout << "ALL CARS HAVE PASSED THE CONSTRUCTION" << endl;
    return 0;
}
