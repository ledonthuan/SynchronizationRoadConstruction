/*
 * README file for project 2
 */


We have one lane closed of a two-lane road, with traffic coming from the North and South. Because of traffic lights, the traffic on the road comes in bursts. When a car arrives, there is an 80% chance of another car following it, but once no car comes, there is a 20 second delay (use the provided pthread_sleep) before any new car will come.
During the times when no cars are at either end, the flag person will fall asleep. When a car arrives at either end, the flag person will wake up and allow traffic from that side to pass the construction area, until there are no more cars from that side, or until there are 10 cars or more lining up on the opposite side. If there are 10 cars or more on the opposite side, the flag person needs to allow the cars from the opposite side to pass.
Each car takes 1 second (use the provided pthread_sleep) to go through the construction area.
Your job is to construct a simulation of these events where under no conditions will a deadlock occur. A deadlock could either be that the flag person does not allow traffic through from either side, or let’s traffic through from both sides causing an accident.

///////// Team member's name and contributions //////////////////////
Team member #1: Sultan Al-icantspellmypartnersnamecoorectly-neif
Contributions: created main thread, car thread, writing to the file and formatting the time - both members participated in planning the semaphore and mutex locks
Percentage of contributions: 50%

Team member #2: Don Le
Contributions: created the producer and flagger consumer threads, and the structs to save the information of when the flagger sleeps and the car info. - both members participated in planning the semaphore and mutex locks
Percentage of contributions: 50%



/////// Thread information /////////////////////////////////////////
Total threads: 5

[Thread #1]
  --> Task: producer that produces cars from the north side
  --> Thread function name: produceSouth

[Thread #2]
  --> Task: producer that produces cars from the south side
  --> Thread function name: produceNorth

[Thread #3]
  --> Task: simulator of going through the construction site for the North side queue and writes to car.log file
  --> Thread function name: carThreadN

[Thread #4]
  --> Task: simulator of going through the construction site for the South side queue and writes to car.log file
  --> Thread function name: carThreadS

[Thread #5]
  --> Task: consumer that regulates North and the South queues passing through construction site and adds each car to its own carThread
  --> Thread function name: Flagger
..............



////// Semaphores ////////////////////////////////////////////////
Number of semaphores: 1

[Sempahore #1]
  --> Variable: carSema
  --> Initial value: 0
  --> Purpose: make flagger consumer blocked upon on events there are no cars in the queue

............



////// Mutex lock ///////////////////////////////////////////////
Number of mutex locks: 4

[Mutex lock #1]
  --> Variable: mutexFlaggerN
  --> Purpose: avoid race condition on shared data structure queue northSide

[Mutex lock #2]
  --> Variable: mutexFlaggerS
  --> Purpose: avoid race condition on shared data structure queue southSide

[Mutex lock #3]
  --> Variable: mutexCarWrite
  --> Purpose: avoid race condition on writing to the file


[Mutex lock #4]
  --> Variable: mutexCarCount
  --> Purpose: avoid race condition on shared car counter carCount
............


///// Strengths  ////////////////////////////////////////////////







//// Weaknesses ///////////////////////////////////////////////
One of the weaknesses of our program is the amount of code that was written. Much of the code could have been merged such as the carThread for the north and south. Another weakness is the logic in the code is crude. It is very step by step thinking and could also be made to look cleaner. A third weakness is that we also added an extra mutex lock to the carCount which tracks the car ID's but this could have been avoided if we had used local variables so save the global carCount and used the local variable to assign the carID. A fourth weakness is in the carThreadS and carThreadN, this is where we popped off the cars from the queues but this should have been done in the flagger thread because by having it in the car thread, we had to acquire the lock and extra time.








