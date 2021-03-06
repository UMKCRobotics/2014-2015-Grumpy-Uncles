#include "gcalc.h"

int stddev(int data[], int n)
{
	float mean=0.0, sum_deviation=0.0;
	int i;
	for(i=0; i<n;++i)
	{
		mean+=data[i];
	}
	mean=mean/n;
	for(i=0; i<n;++i)
		sum_deviation+=(data[i]-mean)*(data[i]-mean);

	return round(sqrt(sum_deviation / n));
}
