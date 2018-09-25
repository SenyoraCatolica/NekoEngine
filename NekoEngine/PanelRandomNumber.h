#ifndef __PanelRandomNumber_H__
#define __PanelRandomNumber_H__

#include "Panel.h"
#include <vector>

#include "Pcg/pcg_variants.h"

class PanelRandomNumber : public Panel
{
public:
	PanelRandomNumber(char* name);
	virtual ~PanelRandomNumber();

	virtual bool Draw();

private:
	int rng = 0;
	pcg_state_setseq_64 rngBound;
	pcg32_random_t rngSeedFloat;

	std::vector<float> randomIntNumbers;
	std::vector<float> randomFloatNumbers;
	bool plotBoundedHistogram = false;
	bool plotFloatHistogram = false;
};

#endif

