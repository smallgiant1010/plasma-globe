#pragma once
#include <chrono>
#include <glad/glad.h>

using Clock = std::chrono::steady_clock;

class Benchmark {
public:
	Benchmark(float benchmarkInterval);
	~Benchmark();
	Benchmark(const Benchmark &other);
	Benchmark& operator=(const Benchmark &other);
	void startCPUClock();
	void endCPUClock();
	void startGPUClock();
	void endGPUClock();
	void calculateDrawTime(float currentFrame);
	float getGeometryGenerationTime();		
	float getCPUDrawTime() const { return cpuDrawTime; }	
	float getGPUDrawTime() const { return gpuDrawTime; }	
private:
	unsigned int CPUSampleCount{};
	unsigned int GPUSampleCount{};
	GLuint timerQuery{};
	float geometryGenerationTime{};
	float cpuDrawTime{};
	float gpuDrawTime{};
	float lastBenchmarkUpdate{};
	float benchmarkInterval;
	double cpuSum{};
	double gpuSum{};
	GLuint64 elapsedNanoSeconds{};
	Clock::time_point start;
	Clock::time_point end;
	void clearAllStatistics();
};

