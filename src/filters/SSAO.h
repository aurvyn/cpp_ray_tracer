#pragma once

#include "BasicEffect.h"
#include "RayGenerator.h"
#include <random>
#include <ctime>



class SSAO : public Effect {
    public:
    Buffer<Vector3> *depthmap;
    Buffer<Vector3> *normalmap;
    Buffer<Vector3> *positionmap;
    Camera camera;
    std::mt19937 randgen;
    std::uniform_real_distribution<> random_distribution;

    size_t sample_count = 20;
    float range = 0.01;
    RayGenerator rg;

    SSAO(Effect *child, Buffer<Vector3> *depthmap, Buffer<Vector3> *normalmap, Buffer<Vector3> *positionmap, Camera camera) : Effect(child) {
        
        this->depthmap = depthmap;
        this->normalmap = normalmap;
        this->positionmap = positionmap;
        this->camera = camera;
        this->rg = RayGenerator(camera, depthmap->getWidth(), depthmap->getHeight());

        // from: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
        std::random_device rd;  // Will be used to obtain a seed for the random number engine
        randgen = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
        random_distribution = std::uniform_real_distribution<> (0.0, 1.0);
    }

    SSAO* init(float range, size_t sample_count) {
        this->range = range;
        this->sample_count = sample_count;
        return this;
    }

    float random() {
        // returns float from 0 to 1
        return random_distribution(randgen);
    }

    float random(float min, float max) {
        return random_distribution(randgen)*(max - min) + min;
    }

    float fakest_fake(int x, int y, int xMax, int yMax) {
        float occlusion = 0.0f;
        float depth = this->depthmap->at(x, y).squaredLength();
        for (int i = 0; i < sample_count; i++) {
            int x_off = (int)(random(-1.0f, 1.0f)*range);
            int y_off = (int)(random(-1.0f, 1.0f)*range);
            if (x + x_off > 0 && x + x_off < xMax && y + y_off > 0 && y + y_off < yMax) {
                float got = this->depthmap->at(x + x_off, y + y_off).squaredLength();
                if (got > depth) occlusion += 1;
            }
        }
        return occlusion/sample_count;
    }

    Vector3 hemisphere_point(Vector3 point, Vector3 normal, float dist) {
        Vector3 off = Vector3(
            random(-1, 1),
            random(-1, 1),
            random(-1, 1)
        )*dist*random(0, 1);
        // flip if wrong side of hemisphere
        off = (normal.dot(off) < 0) ? -off : off;
        return point + off;
    }

    Vector3 get_pixel_dir(int x, int y) {
        return (
            camera.getPos() - 
            positionmap->at(
                std::max(0, std::min(x, (int)positionmap->getWidth())), 
                std::max(0, std::min(y, (int)positionmap->getHeight()))
            )
        ).normalize();
    }

    bool point_behind_position_buffer(size_t x, size_t y, Vector3 point) {
        // x and y are used to find the correct pixel to compare to
        // this is kind of backward that I have to calculate this but I lack the necessary ray information

        // start by finding where the closest x and y are 
        // Vector3 point_dir = (camera.getPos() - point).normalize();
        // int x_heading = (
        //     get_pixel_dir(x+1, y).dot(point_dir) > 
        //     get_pixel_dir(x, y).dot(point_dir)
        // ) ? 1 : -1;
        // int y_heading = (
        //     get_pixel_dir(x, y+1).dot(point_dir) > 
        //     get_pixel_dir(x, y).dot(point_dir)
        // ) ? 1 : -1;
        // while (get_pixel_dir(x+x_heading,y).dot(point_dir) > get_pixel_dir(x,y).dot(point_dir)) x += x_heading;
        // while (get_pixel_dir(x,y+y_heading).dot(point_dir) > get_pixel_dir(x,y).dot(point_dir)) y += y_heading;
        rg.getXY(x, y, Ray((point - camera.getPos()), camera.getPos()));


        // now that we have the correct pixel we can actually do our comparison
        float depthSquared = (positionmap->at(x, y) - camera.getPos()).squaredLength();
        float sampleDepthSquared = (point - camera.getPos()).squaredLength();
        // printf("%f %f\n", depthSquared, sampleDepthSquared);
        return depthSquared < sampleDepthSquared;
    }
    

    float ssao(int x, int y) {
        float occlusion = 0;
        // https://www.gamedev.net/tutorials/programming/graphics/a-simple-and-practical-approach-to-ssao-r2753/
        Vector3 hit_point = this->positionmap->at(x, y);
        Vector3 hit_norm = this->normalmap->at(x, y);

        for (int i = 0; i < sample_count; i++) {
            Vector3 sample_point = hemisphere_point(hit_point, hit_norm, this->range);
            if (point_behind_position_buffer(x, y, sample_point)) occlusion += 1;
        }
        return occlusion/sample_count;
    }

    void _apply() {
        size_t resX = this->imageBuffer->getWidth();
        size_t resY = this->imageBuffer->getHeight();
        this->colorSpace = RGB;
        
        for(int y=0; y<resY; y++) {
            for(int x=0; x<resX; x++) {
                // imageBuffer->at(x, y) = Vector3(
                //     this->positionmap->at(x,y)[0],
                //     this->positionmap->at(x,y)[1],
                //     this->positionmap->at(x,y)[2]
                // );
                imageBuffer->at(x, y) = (1 - ssao(x, y));
            }
        }

        // apply blur
        // (new BasicConvolution(NoOp(this->imageBuffer))).applyEffect();
    }
};