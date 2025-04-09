#ifndef STERLING_TEXTURES_H
#define STERLING_TEXTURES_H

struct ColourRGBA
{
public:
	char red;
	char green;
	char blue;
	char alpha;
};

class Texture2D
{
private:
	unsigned int ID;
	int width;
	int height;
	int channelCount;

public:
	Texture2D(const char* path);
	void use();
};

#endif