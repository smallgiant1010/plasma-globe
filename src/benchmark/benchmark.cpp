#include "benchmark.hpp"

Benchmark::Benchmark(float benchmarkInterval) {
  this->benchmarkInterval = benchmarkInterval;
  glGenQueries(1, &timerQuery);
}

Benchmark::~Benchmark() { glDeleteQueries(1, &timerQuery); }

Benchmark::Benchmark(const Benchmark &other) {
  clearAllStatistics();
  this->benchmarkInterval = other.benchmarkInterval;
  glGenQueries(1, &timerQuery);
}

Benchmark &Benchmark::operator=(const Benchmark &other) {
  if (this != &other) {
    clearAllStatistics();
    this->benchmarkInterval = other.benchmarkInterval;
    glGenQueries(1, &timerQuery);
  }

  return *this;
}

void Benchmark::startCPUClock() { start = Clock::now(); }

void Benchmark::endCPUClock() {
  end = Clock::now();
  cpuSum += std::chrono::duration<double, std::milli>(end - start).count();
  CPUSampleCount++;
}

void Benchmark::startGPUClock() { glBeginQuery(GL_TIME_ELAPSED, timerQuery); }

void Benchmark::endGPUClock() {
  glEndQuery(GL_TIME_ELAPSED);
  glGetQueryObjectui64v(timerQuery, GL_QUERY_RESULT, &elapsedNanoSeconds);
  gpuSum += elapsedNanoSeconds / 1'000'000.0;
  GPUSampleCount++;
}

void Benchmark::calculateDrawTime(float currentFrame) {
	if (currentFrame - lastBenchmarkUpdate >= benchmarkInterval) {
		if (CPUSampleCount > 0) {
			cpuDrawTime = cpuSum / CPUSampleCount;
			CPUSampleCount = 0;
			cpuSum = 0.0;
		}
		if (GPUSampleCount > 0) {
			gpuDrawTime = gpuSum / GPUSampleCount;
			GPUSampleCount = 0;
			gpuSum = 0.0;
		}
		lastBenchmarkUpdate = currentFrame;
	}
}

float Benchmark::getGeometryGenerationTime() {
  geometryGenerationTime =
      std::chrono::duration<float, std::milli>(end - start).count();
  cpuSum = 0.0;
  CPUSampleCount = 0;
  return geometryGenerationTime;
}

void Benchmark::clearAllStatistics() {
  CPUSampleCount = GPUSampleCount = 0;
  geometryGenerationTime = cpuDrawTime = gpuDrawTime = lastBenchmarkUpdate =
      0.0f;
  cpuSum = gpuSum = 0.0;
  elapsedNanoSeconds = {};
  start = {};
  end = {};
}
