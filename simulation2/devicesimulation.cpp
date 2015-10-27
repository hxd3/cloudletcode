/*
In data.txt 
0 means insert a cloudlet node
1 means insert a mobile device
2 means a mobile device leave
3 generate a task
*/

#include<iostream>
#include<math.h>
#include<list>
#include<fstream>
#include<time.h>
#include<random>
#include<functional>
#include<map>
#include<queue>
#include"myrandom.h"
using namespace std;

// let us suppose each unit of time represent 0.1 seconds
class devicetype;
list<devicetype> mobiledevice, cloudlet;
list<devicetype>::iterator deviceitr;
double taskrate = 100;
double devicecomerate = 0.01;

int task_idfield = 1;
int device_idfield = 1;
myrand randomnumber;
map<int, bool> availability;
map<int, bool> iscloudlet2;

class tasktype
{
public: int task_id;
		double cpu_consumption;
		double bandwidthup_consumption;
		double bandwidthdown_consumption;
		double latency_requirement;
		int task_genid;


		tasktype(int device_id)
		{
			task_id = task_idfield;
			task_idfield++;
			cpu_consumption = randomnumber.uniform(0, 100);
			cpu_consumption = 100;
			bandwidthup_consumption = randomnumber.uniform(0, 100);
			bandwidthup_consumption = 10;
			bandwidthdown_consumption = 1;
			latency_requirement = 30;
			task_genid = device_id;
		}
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
	list<tasktype> tasks = list<tasktype>();
	// denote the node that can reach in 1 step, the cost means the latency
	map<int, double> reach;
	// betweeness
	double importance;
	//the power decrease even there is no tasks running on the node
	double decrease;
	devicetype()
	{
		iscloudlet = false;
		device_id = 1;
		device_power = 1000;
		leave_probability = 0.0001;
		cpu_available = 100;
		cpu_speed = 10;
		cpu_parameter = 1;
		bandwidthup = 50;
		bandwidthdown = 50;
		bandwidthup_parameter = 150;
		bandwidthdown_parameter = 100;
		ttl = 10000;
		generate_taskrate = 0.001;
		availability[device_id] = true;
		iscloudlet2[device_id] = false;
		decrease = 0.1;
	}

	// generate 3 kind of devices, type 1 is mobile phone, type 2 is laptop, type 3 is server.
	devicetype(int type)
	{
		if (type == 1)
		{
			device_id = device_idfield;
			device_idfield++;
			iscloudlet = false;
			device_power = 10000;
			leave_probability = 0.0001;// stay 3 hours average
			cpu_available = 100;
			cpu_speed = 10;
			cpu_parameter = 1;
			bandwidthup = 50;
			bandwidthdown = 50;
			bandwidthup_parameter = 150;
			bandwidthdown_parameter = 100;
			ttl = 10000;                                            // method to calculate time to live
			generate_taskrate = 0.006;
			availability[device_id] = true;
			iscloudlet2[device_id] = false;
			decrease = 1;
		}
		else if (type == 2)
		{
			device_id = device_idfield;
			device_idfield++;
			iscloudlet = false;
			device_power = 1000000;
			leave_probability = 0.0001;// stay 3 hours
			cpu_available = 500;
			cpu_speed = 50;
			cpu_parameter = 1;
			bandwidthup = 200;
			bandwidthdown = 200;
			bandwidthup_parameter = 150;
			bandwidthdown_parameter = 100;
			ttl = 10000;                                          // method to calculate time to live
			generate_taskrate = 0.005;
			availability[device_id] = true;
			iscloudlet2[device_id] = false;
			decrease = 100;
		}
		else if (type == 3)
		{
			device_id = device_idfield;
			device_idfield++;
			iscloudlet = false;
			device_power = 100000000;
			leave_probability = 0.00001;// stay 3 years
			cpu_available = 1000;
			cpu_speed = 99;
			cpu_parameter = 1;
			bandwidthup = 400;
			bandwidthdown = 400;
			bandwidthup_parameter = 150;
			bandwidthdown_parameter = 100;
			ttl = 1000000;                                          // method to calculate time to live
			generate_taskrate = 0.001;
			availability[device_id] = true;
			iscloudlet2[device_id] = true;
			decrease = 500;
		}
	}

	// generate random number that comply exponent distribution
	// y=1-exp(-lambta*x) ==> x=ln(1-y)/(-lambta)
	/*double expdist(double lambta)
	{
	double u = rand() / (double)RAND_MAX;
	double x = -log(1 - u) / lambta;
	return x;
	}*/

	// low power device leave immediately
	//the probability of leave comply exponent distribution
	//first time happen probability before t is 1-exp(-lambta*t) which means never happens before
	//rand<1-exp(-lambta*t) means node will leave
	bool leave()
	{
		//if (device_power < 100) return true;
		int p = randomnumber.exponential(leave_probability);
		if (p <= 1)
		{
			return true;
		}
		else
			return false;
	}

	//not finish yet
	bool generate_task()
	{
		if (!iscloudlet && randomnumber.exponential(generate_taskrate) <= 1)
		{
			return true;
		}
		return false;
	}

	bool task_request()
	{
		tasktype task(device_id);
		//cout << "device id: " << device_id << " generate task id:" << task.task_id << "  cpu: " << task.cpu_consumption << " band: " << task.bandwidth_consumption << " late: " << task.latency_requirement << endl;
		cout << device_id << " " << task.cpu_consumption << " " << task.bandwidthup_consumption <<" "<<task.bandwidthdown_consumption<< " " << task.latency_requirement << " " << endl;
		return false;
	}

};


void device_come()
{
	int number = randomnumber.uniform(0, cloudlet.size());
	devicetype temp(1);
	cout << temp.device_id << std::fixed << " " << temp.device_power << " " << temp.leave_probability << " " << temp.cpu_speed << " " << temp.cpu_parameter << " " << temp.bandwidthup << " " << temp.bandwidthdown << " " << temp.bandwidthup_parameter << " " << temp.bandwidthdown_parameter <<" "<<temp.decrease<< endl;
	list<devicetype>::iterator itr;
	//cout << " deviceid :" << temp.device_id << " come " << endl;
	for (itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
	{
		if (randomnumber.uniform(0,cloudlet.size()) > number)
		{
			temp.reach[itr->device_id] = 1;
			itr->reach[temp.device_id] = 1;			
		}
	}
	cout << temp.reach.size() << " ";
	for (map<int, double>::iterator itr2 = temp.reach.begin(); itr2 != temp.reach.end(); itr2++)
	{
		cout << itr2->first << " ";
	}
	cout << endl;
	mobiledevice.push_back(temp);
	availability[temp.device_id] = true;
}

void cloudlet_come()
{
	int number = randomnumber.uniform(0, cloudlet.size());
	devicetype temp(3);
	list<devicetype>::iterator itr;
	cout << "cloudletid: " << temp.device_id << " come " << endl;
	for (itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
	{
		if (randomnumber.uniform(0, cloudlet.size()) > number)
		{
			//temp.reach[itr->device_id] = 1 / (itr->bandwidth + 1) + 1;
			//itr->reach[temp.device_id] = 1 / (itr->bandwidth + 1) + 1;
			//simplify the problem latency is 1
			temp.reach[itr->device_id] = 1;
			itr->reach[temp.device_id] = 1;
		}
	}
	number = randomnumber.uniform(0, mobiledevice.size());
	for (itr = mobiledevice.begin(); itr != mobiledevice.end(); itr++)
	{
		if (randomnumber.uniform(0, mobiledevice.size()) > number)
		{
			//temp.reach[itr->device_id] = 1 / (itr->bandwidth + 1) + 1;
			//itr->reach[temp.device_id] = 1 / (itr->bandwidth + 1) + 1;
			//simplify the problem latency is 1
			temp.reach[itr->device_id] = 1;
			itr->reach[temp.device_id] = 1;
		}
	}
	cloudlet.push_back(temp);
	availability[temp.device_id] = true;
}

// setup a fully connected cloudlet
void ncloudlet_come(int n)
{	
	list<devicetype>::iterator itr;
	for (int i = 0; i < n; i++)
	{
		devicetype temp(3);
		cout << "0 " << i << " " << i + 1 << " " << std::fixed << temp.device_power << " " << temp.leave_probability << " " << temp.cpu_speed << " " << temp.cpu_parameter << " " << temp.bandwidthup << " " << temp.bandwidthdown << " " << temp.bandwidthup_parameter << " " << temp.bandwidthdown_parameter<<" "<<temp.decrease<<endl;
		cout << i << " ";
		for (int j = 0; j < i; j++)
			cout << j+1 << " ";
		cout << endl;
		for (itr = cloudlet.begin(); itr != cloudlet.end(); itr++)
		{
			temp.reach[itr->device_id] = 1;
			itr->reach[temp.device_id] = 1;
		}
		cloudlet.push_back(temp);
		availability[temp.device_id] = true;
	}
}

//int generatedata()
int main()
{
	streambuf *backup;
	ofstream fout;
	fout.open("data.txt");
	backup = cout.rdbuf();   // back up cin's streambuf  
	cout.rdbuf(fout.rdbuf()); // assign file's streambuf to cin  
	// ... cin will read from file  
	cout << 20000 << endl;
	ncloudlet_come(10); 
	devicetype device;
	int n = 0;
	srand(time(0));
	//device.leave_probability = 1000;
	/*for (int i = 0; i < 100000; i++)
	{
	if (device.leave()) n++;
	}
	cout << n;*/
	for (int time = 10; time < 20000; time++)
	{
		//generate device & cloudlet		
		if (randomnumber.exponential(0.001) <= 1)
		{
			cout <<2<<" "<<time<< " ";
			device_come();
		}
		/*if (randomnumber.exponential(0.0001) <= 1)
		{
			cout << "time:" << time << " ";
			cloudlet_come();
		}*/
		//simulate mobile device leave
		for (deviceitr = mobiledevice.begin(); deviceitr != mobiledevice.end();)
		{
			deviceitr->device_power--;
			if (!mobiledevice.empty() && deviceitr->leave())
			{
				cout << 3 <<" "<< time <<" " << deviceitr->device_id << endl;
				availability[deviceitr->device_id] = false;
				deviceitr = mobiledevice.erase(deviceitr);
			}
			else
				deviceitr++;
		}

		//simulate cloudlet leave
		/*for (deviceitr = cloudlet.begin(); deviceitr != cloudlet.end();)
		{
			deviceitr->device_power--;
			if (!cloudlet.empty() && deviceitr->leave())
			{
				cout << "time: " << time << "cloudlet: " << deviceitr->device_id << " leave" << endl;
				availability[deviceitr->device_id] = false;
				deviceitr = cloudlet.erase(deviceitr);
				break;
			}
			else
				deviceitr++;
		}*/

		//simulate mobile device generate task
		for (deviceitr = mobiledevice.begin(); deviceitr != mobiledevice.end(); deviceitr++)
		{
			if (deviceitr->generate_task())
			{
				cout << "4 " << time << " ";
				deviceitr->task_request();
			}
		}

	}
	cout.rdbuf(backup);     // restore cin's original streambuf
	cout << "generate device and task data complete" << endl;
	system("pause");
	return 0;
}