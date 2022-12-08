#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <queue>
#include <map>
using namespace std;
#define MAXTIME 200

struct Job {
    char name;
    int serviceTime;
    int arrivalTime;
    bool operator==(char const& test) const {return name == test;}
};

void roundRobin(string[][MAXTIME], vector<Job>);
void shortestRemainingTime(string[][MAXTIME], vector<Job>);
void feedback(string[][MAXTIME], vector<Job>);
int* extractValues(char[]);
void clearMatrix(string[][MAXTIME], int);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Invalid number of arguments" << endl;
        return 0;
    }
    char file[32] = "\0";
    strcpy(file, argv[1]);
    string algorithm = argv[2];

    vector<Job> jobsList;

    // read in jobs
    FILE *jobsFile = fopen(file, "r");
    if(jobsFile == NULL){  
        cout << "File " << file << " not found" << endl;
        return 0;
    }
    char line[100] = {'\0'};
    while(fgets(line, 100, jobsFile)) {
        Job job;
        job.name = line[0];
        int* times = extractValues(line);
        job.arrivalTime = times[0]; 
        job.serviceTime = times[1];
        cout << "Job: " << job.name << " Arrival Time: " << job.arrivalTime << " Service Time: " << job.serviceTime << endl;
        jobsList.push_back(job);
    }
    
    int numJobs = jobsList.size();

    // define and clear output matrix
    string outputMatrix[numJobs][MAXTIME];
    // for(int i = 0; i < numJobs; i++) {
    //     for(int j = 0; j < MAXTIME; j++) {
    //         if(j == 0){
    //             outputMatrix[i][j] = (char)(i + 65); // set first column to letter
    //         }
    //         else{
    //             outputMatrix[i][j] = " ";
    //         }
    //     }
    // }
    clearMatrix(outputMatrix, numJobs);


    // run specificed algorithm(s)
    if(algorithm == "RR") {
        roundRobin(outputMatrix, jobsList);
    }
    else if(algorithm == "SRT") {
        shortestRemainingTime(outputMatrix, jobsList);
    }
    else if(algorithm == "FB") {
        feedback(outputMatrix, jobsList);
    }
    else if(algorithm == "ALL") {
        roundRobin(outputMatrix, jobsList);
        clearMatrix(outputMatrix, numJobs); // clear the matrix
        shortestRemainingTime(outputMatrix, jobsList);
        clearMatrix(outputMatrix, numJobs); // clear the matrix
        feedback(outputMatrix, jobsList);
    }
    else {
        cout << "Invalid scheduling algorithm";
        return 0;
    }

}

int* extractValues(char *line) {
    static int nums[2]; // times[0] is arrival time, times[1] is service time
    int lineIndex = 2;
    for(int i = 0; i < 2; i++) {
        char number[10] = {'\0'};   
        int numberIndex = 0;
        while(isdigit(line[lineIndex])) {
            number[numberIndex] = line[lineIndex];
            lineIndex++;
            numberIndex++;
        }
        nums[i] = atoi(number);
        lineIndex++;
    }
    return nums;
}

void clearMatrix(string matrix[][MAXTIME], int height) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < MAXTIME; j++) {
            if(j == 0){
                matrix[i][j] = (char)(i + 65); // set first column to letter
            }
            else{
                matrix[i][j] = " ";
            }
        }
    }
}

// performs and prints round robin scheduling algorithm
void roundRobin(string schedule[][MAXTIME], vector<Job> jobs) {
    cout << endl << "Round Robin" << endl << endl;
    int finishTime = 0; // when the final matrix is output, only the first finishTime columns will be printed 
    int clock = 0;
    int finishedJobs = 0;
    queue<Job> readyQueue;
    Job currJob = {'x', 0, 0};
    while(finishedJobs < jobs.size()) {
        // check to see if new job(s) arrived
        for(Job job : jobs) {
            if(job.arrivalTime == clock) {
                readyQueue.push(job); // push new job queue
                // cout << job.name << " arrives and is pushed onto queue at time " << clock << endl;
            }
        }
        
        // if the running job still has time left push it back onto the end of the queue
        if(currJob.serviceTime > 0) {
            readyQueue.push(currJob);
        }

        // run the next job in line (if there is one in line)
        if(!readyQueue.empty()) {
            // get next job from queue
            currJob = readyQueue.front();
            readyQueue.pop();
            currJob.serviceTime--; // decrement the job's service time

            // update the schedule matrix
            schedule[(int)(currJob.name) - 65][clock+1] = "X";

            // check if job is finished
            if(currJob.serviceTime == 0) {
                finishedJobs++;
                // cout << currJob.name << " finished at time " << clock + 1 << endl;
            }
        }
        
        clock++; // clock tick
        if(clock > MAXTIME){
            cout << "Hit time limit" << endl; 
            /* this shouldn't ever happen, I just created a "max time" for 
            memory purposes, I needed to know how big to make the output matrix.
            It also lets me know if there is a problem with the algorithm 
            like jobs aren't finishing or something */
            break;
        }
        finishTime = clock;
    }

    // print results
    for(int i = 0; i < jobs.size(); i++) {
        for(int j = 0; j <= finishTime + 1; j++) {
            cout << schedule[i][j];
        }
        cout << endl;
    }
}

// performs and prints shortest remaining time scheduling algorithm
void shortestRemainingTime(string schedule[][MAXTIME], vector<Job> jobs) {
    cout << endl << "Shortest Remaining Time" << endl << endl;
    int finishTime = 0; // when the final matrix is output, only the first finishTime columns will be printed 
    int clock = 0;
    int finishedJobs = 0;
    map<char, Job> readyJobs;

    while(finishedJobs < jobs.size()) {
        // check if new job has arrived
        for(Job job : jobs) {
            if(job.arrivalTime == clock) {
                readyJobs[job.name] = job; // add job to map
                // cout << job.name << " added to readyJobs at time " << clock << endl;
            }
        }

        if(!readyJobs.empty()) {
            Job currJob = readyJobs.begin()->second;
            // cout << "currJob: " << currJob.name << " Service Time: " << currJob.serviceTime << endl;
            // select job with shortest remaining time
            for(auto element: readyJobs) {
                Job job = element.second;
                // cout << "job: " << job.name << " service time: " << job.serviceTime << endl;
                if(job.serviceTime < currJob.serviceTime or (job.serviceTime == currJob.serviceTime and (int)job.name < (int)currJob.name)) {
                    currJob = job;
                }
            }

            readyJobs[currJob.name].serviceTime--; // decrement service time

            // update the schedule matrix
            schedule[(int)(currJob.name) - 65][clock+1] = "X";

            // check if job is done
            if(readyJobs[currJob.name].serviceTime == 0) {
                finishedJobs++;
                readyJobs.erase(currJob.name); // erase this job from the ready jobs map
            }

        }

        clock++; // clock tick
        if(clock > MAXTIME){
            cout << "Hit time limit" << endl; 
            /* this shouldn't ever happen, I just created a "max time" for 
            memory purposes, I needed to know how big to make the output matrix.
            It also lets me know if there is a problem with the algorithm 
            like jobs aren't finishing or something */
            break;
        }
        finishTime = clock;
    }

    // print results
    for(int i = 0; i < jobs.size(); i++) {
        for(int j = 0; j <= finishTime + 1; j++) {
            cout << schedule[i][j];
        }
        cout << endl;
    }
}  

// performs and prints feedback scheduling algorithm
void feedback(string schedule[][MAXTIME], vector<Job> jobs) {
    cout << endl << "Feedback" << endl << endl;
    int finishTime = 0; // when the final matrix is output, only the first finishTime columns will be printed 
    int clock = 0;
    int finishedJobs = 0;
    Job currJob = {'x', 0, 0};
    queue<Job> priority1;
    queue<Job> priority2;
    queue<Job> priority3;
    queue<Job> cpu;
    int runningJobs = 0;
    int currJobPriority = 1;
    

    while(finishedJobs < jobs.size()) {
        // check for any new job arrivals
        for(Job job : jobs) {
            if(job.arrivalTime == clock) {
                priority1.push(job); // push job onto priority queue 1
                runningJobs++;
                // cout << job.name << " arrived at time " << clock << ", " << runningJobs << " are running" << endl;
            }
        }
    
        // schedule next highest priority job
        if(!priority1.empty()) {
            // check if there is a job in the cpu
            if(!cpu.empty()) {
                // pop job from cpu and push job onto lower priority queue
                if(currJobPriority < 3) {
                    currJobPriority++;
                }

                if(currJobPriority == 2) {
                    priority2.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority2" << endl;
                }
                else if(currJobPriority == 3) {
                    priority3.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority3" << endl;
                }
                cpu.pop();

            }

            cpu.push(priority1.front()); // send job from front of priority 1 queue to cpu
            // cout << "next job in priority1: " << priority1.front().name << endl;
            // cout << cpu.front().name << " pushed to cpu" << endl;
            priority1.pop();
            currJobPriority = 1;
        }
        else if(!priority2.empty()) {
             // check if there is a job in the cpu
            if(!cpu.empty()) {
                // pop job from cpu and push job onto lower priority queue
                if(currJobPriority < 3) {
                    currJobPriority++;
                }

                if(currJobPriority == 2) {
                    priority2.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority2" << endl;
                }
                else if(currJobPriority == 3) {
                    priority3.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority3" << endl;
                }
                cpu.pop();
            }

            cpu.push(priority2.front()); // send job from front of priority 2 queue to cpu
            // cout << "next job in priority2: " << priority2.front().name << endl;
            // cout << cpu.front().name << " pushed to cpu" << endl;
            priority2.pop();
            currJobPriority = 2;
        }
        else if(!priority3.empty()) {
             // check if there is a job in the cpu
            if(!cpu.empty()) {
                // pop job from cpu and push job onto lower priority queue
                if(currJobPriority < 3) {
                    currJobPriority++;
                }

                if(currJobPriority == 2) {
                    priority2.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority2" << endl;
                }
                else if(currJobPriority == 3) {
                    priority3.push(cpu.front());
                    // cout << cpu.front().name << " pushed to priority3" << endl;
                }
                cpu.pop();
            }

            cpu.push(priority3.front()); // send job from front of priority 1 queue to cpu
            // cout << "next job in priority3: " << priority3.front().name << endl;
            // cout << cpu.front().name << " pushed to cpu" << endl;
            priority3.pop();
            currJobPriority = 3;
        }

        if(!cpu.empty()) {
            currJob = cpu.front();
            cpu.pop();
            // cout << "currJob is " << currJob.name << endl;
            currJob.serviceTime--;
            // cout << "currJob has " << currJob.serviceTime << " time remaining" << endl;

            //update schedule matrix
            schedule[(int)(currJob.name) - 65][clock+1] = "X";

            // check if job is done
            if(currJob.serviceTime == 0) {
                // cout << currJob.name << " is finished at time " << clock;
                finishedJobs++;
                runningJobs--;
                // cout << cpu.front().name << " popped from cpu" << endl;
                cpu.pop();
            }
            cpu.push(currJob);
        }

        clock++; // clock tick
        if(clock > MAXTIME){
            cout << "Hit time limit" << endl; 
            /* this shouldn't ever happen, I just created a "max time" for 
            memory purposes, I needed to know how big to make the output matrix.
            It also lets me know if there is a problem with the algorithm 
            like jobs aren't finishing or something */
            break;
        }
        finishTime = clock;
    }


    // print results
    for(int i = 0; i < jobs.size(); i++) {
        for(int j = 0; j <= finishTime + 1; j++) {
            cout << schedule[i][j];
        }
        cout << endl;
    }
}

