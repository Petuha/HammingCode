#include "../SignalGenerator/SignalGenerator.h"
#include <fstream>
int main() {
	std::string bits = "111";
	conversionMethod method = conversionMethod::NRZ;
	double dt = 1;
	double A = 1;
	int dotsPerBit = 4;
	bool polarity = 0;
	std::vector<Dot> signal = generateSignalFromBits(bits, method, dt, A, dotsPerBit, polarity);
	std::ofstream fout("plot.txt");
	for (auto& d : signal) fout << d.x << ' ' << d.y << '\n';
	fout.close();
	return 0;
}