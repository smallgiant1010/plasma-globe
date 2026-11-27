#pragma once

class Blur {
public:
	Blur(int amount, int width, int height);
	~Blur();
	Blur(const Blur &other);
	Blur& operator=(const Blur &other);
	const unsigned int *getCBO() const { return pingPongCBO; }
	const unsigned int *getFBO() const { return pingPongFBO; }
	int getAmount() const { return amount; }
private:
	int screenWidth;
	int screenHeight;
	int amount;
	unsigned int pingPongFBO[2];
	unsigned int pingPongCBO[2];
	void cleanup();
	void setupObjects();
};
