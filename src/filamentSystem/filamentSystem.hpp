#pragma once

#include <vector>
#include <functional>
#include <random>
#include <memory>
#include "filament/filament.hpp"

class FilamentSystem {
public:
	FilamentSystem(unsigned int segments = 32);
	~FilamentSystem();
	FilamentSystem(const FilamentSystem& other);
	void addFilament(float innerRadius, float outerRadius);
	void drawFilaments() const;
	void removeFilament();
	unsigned int getFilamentLength() const { return filaments.size(); }
private:
	unsigned int VAO;
	unsigned int segments;
	std::vector<std::unique_ptr<Filament>> filaments;
	std::vector<std::function<float(float)>> basis;
	std::mt19937 generator{std::random_device{}()};
	void setupObjects();
	float randomFloat(float min, float max);
};
