#include<iostream>
#include<list>
#include<fstream>
#include<map>
#include<queue>
#include"myrandom.h"
using namespace std;
// !!!assumption data come and go use the same channel, path never change
// let us suppose each unit of time represent 0.1 seconds
class devicetype;
list<devicetype> mobiledevice, cloudlet;

int task_idfield = 1;
int device_idfield = 1;
myrand randomnumber;
map<int, bool> availability;
vector<vector<double>> reach;

class tasktype
{
public: int task_id;
		double cpu_consumption;
		double bandwidthup_consumption;
		double bandwidthdown_consumption;
		double latency_requirement;
		int task_genid;
};

class runningtasktype :public tasktype
{
public:double bandwidthup, bandwidthdown;
	vector<int> path;
	int state;
	/* state=0 just create;
	   state=1 uploading tasks;
	   state=2 running tasks;
	   state=3 downloading tasks	
	*/


	runningtasktype(tasktype a)
	{
		cpu_consumption = a.cpu_consumption;
		bandwidthdown_consumption = a.bandwidthdown_consumption;
		bandwidthup_consumption = a.bandwidthup_consumption;
		latency_requirement = a.latency_requirement;
		state = 0;
	}

	runningtasktype(){}

};

class devicetype
{
public:
	bool iscloudlet;
	int device_id;
	// denote the power for mobile device
	double device_power;
	// denote the leave probability for mobile device
	double leave_probability;
	//denote the cpu available for mobile device
	double cpu_available;
	double cpu_speed;
	double generate_taskrate;
	double bandwidthup;
	double bandwidthdown;
	// power consumption for per unit of calculation
	double cpu_parameter;
	// power consumption for per unit of transmission
	double bandwidthup_parameter;
	double bandwidthdown_parameter;
	//time to live for each mobile device
	double ttl;
	runningtasktype tasks;
	// denote the node that can reach in 1 step, the cost means the latency
	map<int, double> reach;
	// betweeness
	double importance;
	//the power decrease even there is no tasks running on the node
	double decrease;
    //the probability to be used in the future
	double future;

	// generate 3 kind of devices, type 1 is mobile phone, type 2 is laptop, type 3 is server.
	devicetype()
	{
		device_id = device_idfield;
		device_idfield++;
		availability[device_id] = true;
		tasks.task_genid = -1;
	}

};

void refreshbetweeness()
{
	// delete useless link in the map & put roads into a matrix
	int n = cloudlet.size();
	int ordernumber = 0;
	map<int, int> reorder;
	vector<vector<double>> road;
	double f[100][100];
	int importance[100];
	for (int i = 0; i < 100; i++)
	{
		importance[i] = 0;
		for (int j = 0; j < 100; j++)
		{
			f[i][j] = i;
		}

	}
	/*vector<int> importance;
	vector<vector<double>> road, f;
	for (int i = 0; i <=n; i++) importance.push_back(0);
	for (list<devicetype>::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
	{
		vector<double> temp1, temp2;
		for (int i = 0; i <= n; i++) temp1.push_back(10000);
		for (int i = 0; i <= n; i++) temp2.push_back(ordernumber);
		road.push_back(temp1);
		f.push_back(temp2);
		reorder[itr->device_id] = ordernumber;
		ordernumber++;
	}*/
	road = reach;

	for (int k = 1; k <= n; k++)
	{
		int val;
		int select;
		bool used[1000];

		for (int j = 1; j <= n; j++) used[j] = false;
		for (int i = 1; i <= n; i++)
		{
			val = 2000;
			select = -1;
			for (int j = 1; j <= n; j++)
			{
				if (val > road[k][j] && !used[j])
				{
					val = road[k][j];
					select = j;
				}
			}
			if (select == -1) break;
			used[select] = true;
			for (int j = 1; j <= n; j++)
			{
				if (road[k][select] + road[select][j] < road[k][j])
				{
					road[k][j] = road[k][select] + road[select][j];
					f[k][j] = select;
				}
			}
		}
	}
	for (int i = 1; i <= n; i++)
	{
		for (int j = 1; j <= n; j++)
		{
			int current = j;
			while (current != i)
			{
				//cout << current << " ";
				importance[current]++;
				current = f[i][current];
			}
			//cout << current << endl;
			importance[current]++;
		}
	}
	int i = 0;
	double sum=0;
	for (i = 1; i <= n; i++)
	{
		sum += importance[i];
	}
	sum = sum / n;
	i = 1;
	for (list<devicetype>::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++, i++)
	{
		if (sum != 0)
			itr->importance = importance[i] / sum;
	}
}

list<devicetype> ::iterator getcloudlet(int id)
{
	for (list<devicetype> ::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
	{
		if (itr->device_id == id)
		{
			return itr;
		}
	}
}

list<devicetype> ::iterator getmobiledevice(int id)
{
	for (list<devicetype> ::iterator itr = mobiledevice.begin(); itr != mobiledevice.end(); itr++)
	{
		if (itr->device_id == id)
		{
			return itr;
		}
	}
}

class forsearchtype :public devicetype,public tasktype
{
public:double cost;
	double upspeed, downspeed;
	vector<int> path;
	forsearchtype()
	{
	}

public:void updatedevice(devicetype sourcedevice)
	{
		iscloudlet = sourcedevice.iscloudlet;
		device_id = sourcedevice.device_id;
		device_power = sourcedevice.device_power;
		leave_probability = sourcedevice.leave_probability;
		cpu_available = sourcedevice.cpu_available;
		cpu_speed = sourcedevice.cpu_speed;
		generate_taskrate = sourcedevice.generate_taskrate;
		bandwidthup = sourcedevice.bandwidthup;
		bandwidthdown = sourcedevice.bandwidthdown;
		cpu_parameter = sourcedevice.cpu_parameter;
		bandwidthup_parameter = sourcedevice.bandwidthup_parameter;
		bandwidthdown_parameter = sourcedevice.bandwidthdown_parameter;
		ttl = sourcedevice.ttl;
		tasks = sourcedevice.tasks;
		reach = sourcedevice.reach;
		importance = sourcedevice.importance;
		decrease = sourcedevice.decrease;
		future = sourcedevice.future;
	}

	forsearchtype(devicetype sourcedevice,tasktype sourcetask)
	{
		iscloudlet = sourcedevice.iscloudlet;
		device_id = sourcedevice.device_id;
		device_power = sourcedevice.device_power;
		leave_probability = sourcedevice.leave_probability;
		cpu_available = sourcedevice.cpu_available;
		cpu_speed = sourcedevice.cpu_speed;
		generate_taskrate = sourcedevice.generate_taskrate;
		bandwidthup = sourcedevice.bandwidthup;
		bandwidthdown = sourcedevice.bandwidthdown;
		cpu_parameter=sourcedevice.cpu_parameter;
		bandwidthup_parameter=sourcedevice.bandwidthup_parameter;
		bandwidthdown_parameter=sourcedevice.bandwidthdown_parameter;
		ttl=sourcedevice.ttl;
		tasks=sourcedevice.tasks;
		reach=sourcedevice.reach;
		importance=sourcedevice.importance;
		decrease=sourcedevice.decrease;
		task_id=sourcetask.task_id;
		cpu_consumption=sourcetask.cpu_consumption;
		bandwidthup_consumption=sourcetask.bandwidthup_consumption;
		bandwidthdown_consumption=sourcetask.bandwidthdown_consumption;
		latency_requirement=sourcetask.latency_requirement;
		task_genid=sourcetask.task_genid;
		future = sourcedevice.future;
	}

};

bool operator<(forsearchtype a, forsearchtype b)
{
	return a.cost>b.cost;
}


int main()
{
	ifstream input("data.txt");
	if (!input)
	{
		cout << "input file fail";
		return 0;
	}
	
	int n,op,optime;
	input >> n;
	input >> op >> optime;
	for (int time = 0; time < n; time++)
	{
		for (list<devicetype>::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
		{
			itr->device_power = itr->device_power-itr->decrease;
			itr->ttl--;
			if (itr->tasks.task_genid!=-1)
			{
				if (itr->tasks.state == 0) // just create 
				{
					if (itr->tasks.bandwidthup_consumption == 0)
					{
						itr->tasks.state = 2;
						continue;
					}
					for (int i = 0; i < itr->tasks.path.size(); i++)
					{
						//itr->tasks.path[i].bandwidthup -= itr->tasks.bandwidthup;
						list<devicetype> ::iterator temp = getcloudlet(itr->tasks.path[i]);
						temp->bandwidthup -= itr->tasks.bandwidthup;
					}
					itr->tasks.state = 1;
					itr->tasks.bandwidthup_consumption -= itr->tasks.bandwidthup;
				}
				else if (itr->tasks.state == 1) // uploading
				{
					itr->tasks.bandwidthup_consumption -= itr->tasks.bandwidthup;
					if (itr->tasks.bandwidthup_consumption <= 0)
					{
						itr->tasks.state = 2;
						for (int i = 0; i < itr->tasks.path.size(); i++)
						{
							//itr->tasks.path[i].bandwidthup += itr->tasks.bandwidthup;
							list<devicetype> ::iterator temp = getcloudlet(itr->tasks.path[i]);
							temp->bandwidthup += itr->tasks.bandwidthup;
						}
					}
						
				}
				else if (itr->tasks.state == 2) //running
				{
					itr->tasks.cpu_consumption -= itr->cpu_speed;
					if (itr->tasks.cpu_consumption <= 0)
					{
						itr->tasks.state = 3;
						for (int i = 0; i < itr->tasks.path.size(); i++)
						{
							list<devicetype> ::iterator temp = getcloudlet(itr->tasks.path[i]);
							temp->bandwidthdown -= itr->tasks.bandwidthdown;
						}
					}
				}
				else if (itr->tasks.state == 3)
				{
					itr->tasks.bandwidthdown_consumption -= itr->tasks.bandwidthdown;
					if (itr->tasks.bandwidthdown_consumption<=0)
					{
						for (int i = 0; i < itr->tasks.path.size(); i++)
						{
							list<devicetype> ::iterator temp = getcloudlet(itr->tasks.path[i]);
							temp->bandwidthdown += itr->tasks.bandwidthdown;
						}
						itr->tasks.task_genid = -1;
						availability[itr->device_id] = true;
					}
				}

			}
		}
		if (optime == time)
		{
			/*
			In data.txt
			0 means insert a cloudlet node
			2 means insert a mobile device
			3 means a mobile device leave
			4 generate a task
			*/
			switch (op)
			{
				case 0:
				{
						  devicetype temp;
						  int nlinks;
						  input >> temp.device_id>> temp.device_power>>temp.leave_probability>>temp.cpu_speed>>temp.cpu_parameter>>temp.bandwidthup>>temp.bandwidthdown>>temp.bandwidthup_parameter>>temp.bandwidthdown_parameter>>temp.decrease;
						  input >> nlinks;
						  temp.future = 0;
						  while (reach.size() < temp.device_id + 1)
						  {
							  vector<double> temp2;
							  reach.push_back(temp2);
						  }
						  for (int i = 0; i < temp.device_id+1; i++)
						  {
							  while (reach[i].size() < temp.device_id+1) reach[i].push_back(0);
						  }
						  for (int i = 0; i < nlinks; i++)
						  {
							  int reachid;
							  input >> reachid;
							  reach[temp.device_id][reachid] = 1;
							  reach[reachid][temp.device_id] = 1;
							  temp.reach[reachid] = 1;
							  getcloudlet(reachid)->reach[temp.device_id] = 1;
						  }
						  temp.iscloudlet = true;
						  temp.ttl = temp.device_power / temp.decrease;
						  cloudlet.push_back(temp);
						  refreshbetweeness();
						  break;
				}
				case 2:
				{
						  devicetype temp;
						  int nlinks;
						  input >> temp.device_id>> temp.device_power >> temp.leave_probability >>temp.cpu_speed >>temp.cpu_parameter>>temp.bandwidthup>>temp.bandwidthdown>>temp.bandwidthup_parameter>>temp.bandwidthdown_parameter>>temp.decrease;
						  input >> nlinks;
						  for (int i = 0; i < nlinks; i++)
						  {
							  int reachid;
							  input >> reachid;
							  temp.reach[reachid] = 1;
							  list<devicetype> ::iterator itr = getcloudlet(reachid);
							  //itr->reach[temp.device_id] = 1;
							  itr->future += 1 / nlinks;
						  }
						  temp.iscloudlet = false;
						  temp.ttl = temp.device_power / temp.decrease;
						  mobiledevice.push_back(temp);
						  availability[temp.device_id] = true;
						  break;
				}
				case 3:
				{
						  int id;
						  input >> id;
						  for (list<devicetype> ::iterator itr = mobiledevice.begin(); itr != mobiledevice.end();itr++)
						  {
							  if (itr->device_id == id)
							  {
								  availability[itr->device_id] = false;
								  double reachsize = itr->reach.size();
								  for (map<int, double>::iterator itr2 = itr->reach.begin(); itr2 != itr->reach.end(); itr2++)
								  {
									  getcloudlet(itr2->first)->future -= 1 / reach.size();
								  }
								  mobiledevice.erase(itr);
								  break;
							  }								  
						  }
						  break;

				}
				case 4:
				{

						  int id;
						  tasktype task;
						  input >> task.task_genid >> task.cpu_consumption >> task.bandwidthup_consumption>>task.bandwidthdown_consumption >> task.latency_requirement;
						  priority_queue<forsearchtype> cloudletqueue;
						  map<int, bool> visited;
						  forsearchtype tempsearchnode(*getmobiledevice(task.task_genid), task);
						  tempsearchnode.cost = 0;
						  tempsearchnode.upspeed = tempsearchnode.bandwidthup;
						  tempsearchnode.downspeed = tempsearchnode.bandwidthdown;
						  cloudletqueue.push(tempsearchnode);
						  visited[task.task_genid] = true;
						  vector<forsearchtype> answer;
						  while (!cloudletqueue.empty())
						  {
							  //will be put into answer vector
							  forsearchtype tempqueuenode = cloudletqueue.top();
							  //will be used for next step
							  forsearchtype nodetask = tempqueuenode;
							  //ttl decrease=taskcpu/cpu_parameter/decrease
							  double cputtldecrease = task.cpu_consumption*tempqueuenode.cpu_parameter / tempqueuenode.decrease;
							  double bandttldecrease = task.bandwidthup_consumption*tempqueuenode.bandwidthup_parameter / tempqueuenode.bandwidthup
								  + task.bandwidthdown_consumption*tempqueuenode.bandwidthdown_parameter / tempqueuenode.bandwidthdown;
							  //int oo = cputtldecrease*(tempqueuenode.importance + tempqueuenode.future + 1 / (tempqueuenode.ttl - bandttldecrease)) / (1 - tempqueuenode.leave_probability);
							  tempqueuenode.cost += cputtldecrease*(tempqueuenode.importance + tempqueuenode.future + 1 / (tempqueuenode.ttl - bandttldecrease)) / (1 - tempqueuenode.leave_probability);
							  tasktype temptask = tempqueuenode; 
							  tempqueuenode.latency_requirement = tempqueuenode.latency_requirement - tempqueuenode.cpu_consumption / tempqueuenode.cpu_speed;							  
							  if (tempqueuenode.latency_requirement>0)
								  answer.push_back(tempqueuenode);
							  //nodetask.cost += bandttldecrease;
							  if (tempqueuenode.bandwidthup < tempqueuenode.upspeed)
								  nodetask.latency_requirement=nodetask.latency_requirement + nodetask.bandwidthup_consumption / tempqueuenode.upspeed-nodetask.bandwidthup_consumption/tempqueuenode.bandwidthup;
							  if (tempqueuenode.bandwidthdown < tempqueuenode.downspeed)
								  nodetask.latency_requirement = nodetask.latency_requirement + nodetask.bandwidthdown_consumption / tempqueuenode.downspeed - nodetask.bandwidthdown_consumption / nodetask.bandwidthdown;
							  cloudletqueue.pop();
							  for (map<int, double> ::iterator itr = tempqueuenode.reach.begin(); itr != tempqueuenode.reach.end(); itr++)
							  {
								  if (!visited[itr->first] && availability[itr->first])
								  {
									  devicetype queuenode = *getcloudlet(itr->first);
									  visited[itr->first] = true;
									  forsearchtype insertnode = nodetask;
									  insertnode.updatedevice(queuenode);
									  insertnode.path = tempqueuenode.path;
									  insertnode.path.push_back(queuenode.device_id);
									  insertnode.upspeed = min(nodetask.bandwidthup, nodetask.upspeed);
									  insertnode.downspeed = min(nodetask.bandwidthdown, nodetask.downspeed);
									  insertnode.cost += bandttldecrease*(queuenode.importance + queuenode.future/mobiledevice.size() + 1 / (queuenode.ttl - bandttldecrease))/(1-queuenode.leave_probability);
									  cloudletqueue.push(insertnode);
									  //insertnode.cost += cputtldecrease*(queuenode.importance + queuenode.future + 1 / (queuenode.ttl - bandttldecrease)) / (1-queuenode.leave_probability);
									  //answer.push_back(insertnode);
								  }
							  }
						  }
						  int finalcost = 10000000,target=0;
						  for (int i = 1; i < answer.size(); i++)
						  {
							  if (finalcost>answer[i].cost)
							  {
								  finalcost = answer[i].cost;
								  target = i;
							  }
						  }
						  for (int i = 0; i < answer[target].path.size(); i++)
						  {
							  list<devicetype> ::iterator myitr = getcloudlet(answer[target].path[i]);
							  double bandttldecrease = task.bandwidthup_consumption*answer[target].bandwidthup_parameter / answer[target].bandwidthup
								  + task.bandwidthdown_consumption*answer[target].bandwidthdown_parameter / answer[target].bandwidthdown;
							  myitr->ttl = myitr->ttl-bandttldecrease;
						  }
						  double cputtldecrease = task.cpu_consumption*answer[target].cpu_parameter / answer[target].decrease;
						  list<devicetype> ::iterator myitr = getcloudlet(answer[target].device_id);
						  myitr->ttl = myitr->ttl - cputtldecrease;
						  runningtasktype runningtask(task);
						  runningtask.path = answer[target].path;
						  runningtask.bandwidthup = answer[target].upspeed;
						  runningtask.bandwidthdown = answer[target].downspeed;
						  myitr->tasks=runningtask;
						  availability[myitr->device_id] = false;
						  for (list<devicetype> ::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
						  {
							  cout << itr->ttl << " ";
						  }
						  cout <<answer[target].device_id<<"  "<< endl;
						  // cpu and network busy state
				}
			}
			if (! ( input >> op >> optime ))
			{
				op = -1;
				optime = n + 1;
			}
		}

	}
	input.close();
	for (list<devicetype> ::iterator itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
	{
		  
	}
	system("pause");
	return 0;
}
