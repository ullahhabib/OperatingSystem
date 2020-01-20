#include <iostream>
#include <queue> 
#include <unistd.h>
#include <sys/wait.h>

using namespace std;


struct  parentChild 
{
	int exectime = -1;
	int priority = -1;
	int PID = 1;
	int split_process = 0;

	parentChild(int execution, int thisPriority, int thepid, int split)
	{
		exectime = execution;
		priority = thisPriority;
		PID = thepid;
		split_process = split;
	}
	parentChild() {}
};

struct compare
{
	bool operator()(parentChild const& child1, parentChild const& child2)
	{
		return(child1.priority > child2.priority || ((child1.priority == child2.priority) && (child1.exectime < child1.exectime)));
	}
};
int main()
{
	pid_t pid;
	parentChild parentChild1;
	priority_queue< parentChild, vector< parentChild>, compare> waitQueue; 

	int quantum = 0;
	int PID = 0;
	int exectime = 0;
	int priority = 0;

	cin >> quantum;

	while (cin >> exectime >> priority) 
	{
		if (exectime > quantum) 
		{
			exectime -= quantum;
			waitQueue.push(parentChild(quantum, priority, PID, 0));
			waitQueue.push(parentChild(exectime, priority, PID, 1));
		}
		else {
			waitQueue.push(parentChild(exectime, priority, PID, 1));
		}
		PID++;
	}
	std::priority_queue< parentChild, vector< parentChild>, compare> wait_copy = waitQueue;

	cout << "Scheduling queue: " << endl;

	//
	while (!wait_copy.empty()) 
	{
		parentChild1 = wait_copy.top();
		cout << "(" << parentChild1.PID << "," << parentChild1.exectime << "," << parentChild1.priority << ") ";
		wait_copy.pop();
	}
	
	cout << endl;

	while (!waitQueue.empty()) 
	{
		parentChild1 = waitQueue.top();
		waitQueue.pop();
		pid = fork();

		if (pid == 0) 
		{ 

			cout << "Process: " << parentChild1.PID << " Execution time = " << parentChild1.exectime << " Priority = " << parentChild1.priority << endl;
			sleep(parentChild1.exectime);
			if (parentChild1.split_process == 1) 
			{ 
				cout << "Process: " << parentChild1.PID << " ends" << endl;
			}
			exit(0);
		}
		if (pid != 0) 
		{ 		
			wait(NULL);
		}
	}

	if (pid != 0) 
	{	
		return 0;
	}
	return -1;
}
