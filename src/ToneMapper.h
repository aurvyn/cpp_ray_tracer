#pragma once

#include <GenVector.h>
#include <Buffer.h>

class ToneMapper {
private:
    virtual void prepare(Buffer<Vector3> &colors) {
    };
    
    virtual Vector3 map(Vector3 &color) {
        return color;
    };

public:
    virtual void apply(Buffer<Vector3> &colors) {
        this->prepare(colors);
        for (int y = 0; y < colors.getHeight(); y++)
            for (int x = 0; x < colors.getWidth(); x++)
                colors.at(x, y) = this->map(colors.at(x, y));
    }
};

class ConstantToneMapper: public ToneMapper {
protected:
    float _scale;
public:
    ConstantToneMapper(float scale) : _scale(scale) { }
private:
    Vector3 map(Vector3 &color) override {
        return color * _scale;
    }
};

class GlobalToneMapper: public ConstantToneMapper {
public:
    GlobalToneMapper() : ConstantToneMapper(0) { }
private:
    void prepare(Buffer<Vector3> &colors) override {
		size_t resX = colors.getWidth();
		size_t resY = colors.getHeight();
		float maxValue = 0.0f;

		for (int y = 0; y < resY; y++) {
			for (int x = 0; x < resX; x++) {
				Vector3 floatColor = colors.at(x, y);
				float maxComp = floatColor[floatColor.maxComponent()];
				if (maxComp > maxValue)
					maxValue = maxComp;
			}
		}

		if (maxValue <= 0.0f)
			maxValue = 1.0f;

        _scale = 1.0f / maxValue;
    }
};

class ReinhardToneMapper: public ToneMapper {
private:
    Vector3 map(Vector3 &color) override {
        return color / (color + 1);
    }
};

class ExtendedReinhardToneMapper: public ToneMapper {
protected:
    float _whitePointSquared;
public:
    ExtendedReinhardToneMapper(float whitePoint) : _whitePointSquared(whitePoint * whitePoint) { }
private:
    Vector3 map(Vector3 &color) override {
        return (color * ((color / _whitePointSquared) + 1)) / (color + 1);
    }
};