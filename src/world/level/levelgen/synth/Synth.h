#pragma once

class Synth
{
public:
	virtual ~Synth() {}

	virtual double getValue(double x, double y) = 0;
};
