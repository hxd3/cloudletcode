#include<random>
#include <time.h>
#include <functional>
using namespace std;

class myrand
{
public: myrand()
{
			srand(time(0));
}
// exponential distribution with parameter lambta
public: double exponential(double lambta)
{
			default_random_engine generator(rand());
			exponential_distribution<double> dis(lambta);
			auto dice = bind(dis, generator);
			double u = dice();
			return u;
}

//normal distribution with parameter N(miu,sigma)
public: double normal(double miu, double sigma)
{
			default_random_engine generator(rand());
			normal_distribution<double> dis(miu, sigma);
			auto dice = bind(dis, generator);
			double u = dice();
			return u;
}

//bernoulli distribution with parameter p
public:	double bernoulli(double p)
{
			default_random_engine generator(rand());
			bernoulli_distribution dis(p);
			auto dice = bind(dis, generator);
			double u = dice();
			return u;
}

//uniform distribution with left and right bound
public:	double uniform(double left, double right)
{
			default_random_engine generator(rand());
			uniform_real_distribution<double> dis(left, right);
			auto dice = bind(dis, generator);
			double u = dice();
			return u;
}

/*public: double poisson(double lambda)
{
			default_random_engine generator(rand());
			poisson_distribution<double> dis(lambda);
			auto dice = bind(dis, generator);
			double u = dice();
			return u;
}
*/
};