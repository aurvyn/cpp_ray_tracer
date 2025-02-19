#pragma once

#include "Effect.h"

enum class CVDType { // Color Vision Deficiency Type
    PROTANOPIA,
    DEUTERANOPIA,
    TRITANOPIA
};

class Daltonization : public Effect
{
    using Effect::Effect; // uses super constructor

    CVDType type;
    bool compensate;
    bool simulate;
public:
    /**
     * @param type The type of color blindness to compensate or simulate.
     * @param fix If true, the image will be recolored to compensate the color blindness.
     * @param simulate If true, the image will be recolored to simulate the color blindness.
     */
    Daltonization *init(CVDType type, bool compensate = true, bool simulate = false) {
        this->type = type;
        this->compensate = compensate;
        this->simulate = simulate;
        return this;
    }
    // Reference: https://www.researchgate.net/publication/322781694_Covisance_A_Real_Time_Mobile_Recolorization_Tool_for_Aiding_Color_Vision_Deficient_Users_Utilizing_D-15_Color_Arrangement_Test
    void _apply() override
    {
        unsigned int width = imageBuffer->getWidth();
        unsigned int height = imageBuffer->getHeight();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vector3 rgb = imageBuffer->at(x, y);
                Vector3 lms( // Long Medium Short
                    17.8824*rgb[0] + 43.5161*rgb[1] + 4.11935*rgb[2],
                    3.45565*rgb[0] + 27.1554*rgb[1] + 3.86714*rgb[2],
                    .0299566*rgb[0] + .184309*rgb[1] + 1.46709*rgb[2]
                );
                switch (type) {
                    case CVDType::PROTANOPIA:
                        lms[0] = 2.02344*lms[1] - 2.52581*lms[2];
                        break;
                    case CVDType::DEUTERANOPIA:
                        lms[1] = .494207*lms[0] + 1.24827*lms[2];
                        break;
                    case CVDType::TRITANOPIA:
                        lms[2] = -.395913*lms[0] + .801109*lms[1];
                        break;
                }
                Vector3 cvd_rgb( // Color Vision Deficient RGB
                    .0809444479*lms[0] - .130504409*lms[1] + .116721066*lms[2],
                    -.0102485335*lms[0] + .0540193266*lms[1] - .113614708*lms[2],
                    -.000365296938*lms[0] - .00412161469*lms[1] + .693511405*lms[2]
                );
                Vector3 final_rgb = simulate ? cvd_rgb : rgb;
                if (compensate) {
                    Vector3 error = rgb - cvd_rgb; // Invisible for people with the CVD type
                    Vector3 fix(0, .7*error[0]+error[1], .7*error[0]+error[2]);
                    final_rgb += fix;
                }
                final_rgb = Vector3(
                    clamp(0.0f, 255.0f, final_rgb[0]),
                    clamp(0.0f, 255.0f, final_rgb[1]),
                    clamp(0.0f, 255.0f, final_rgb[2])
                );
                imageBuffer->at(x, y) = final_rgb;
            }
        }
    }
};
