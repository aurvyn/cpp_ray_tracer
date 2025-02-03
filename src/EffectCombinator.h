#pragma once

#include <functional>

#include "GenVector.h"
#include "Buffer.h"
#include <string>

// // Factory Singleton Factory 
// // void SinglePixelFilterBuilder(std::function<Vector3(std::array<Vector3, N>)> function) {
    
// // }

// // this typedef may be a footgun
// typedef Buffer<Vector3> Buf;

// // so that we don't have to write so much

// template <size_t N> 
// class Filter {
//     virtual Buf apply(const std::array<Buf, N> &buffer) = 0;
//     virtual std::string name() = 0;
// };

// // ((vec3[N], int, int) -> vec3) -> (buf -> buf)
// template <size_t N> 
// class CreateSinglePixelOperator : public Filter<N> {
//     private:
//     // pixel2pixel has signature (input_pixels: vec3[N], x, y)
//     std::function<Vector3(std::array<Vector3, N>, int, int)> pixel2pixel;
//     CreateSinglePixelOperator(std::function<Vector3(std::array<Vector3, N>, int, int)> pixel2pixel) {
//         this->pixel2pixel = pixel2pixel
//     }

//     Buf apply(const Buf &buffer) {
//         // size_t for correctness or smth
//         for (size_t i = 0; i < buffer.getWidth(); i++) {
//             for (size_t j = 0; j < buffer.getHeight(); j++) {
//                 // TODO apply for each
//             }
//         }
//         // TODO return
//     }
    
// };

// class PipelineStep {
//     public:
//     // buf[pixel] -> buf[pixel]
//     std::string step_name;
//     Filter filter;
//     std::array<PipelineStep*, N> inputs;

//     PipelineStep(
//         std::string step_name,
//         Filter filter;
//         std::array<PipelineStep*, N> inputs
//     ) {
//         this->step_name = step_name;
//         this->filter = filter;
//         this->inputs = inputs;
//         for (PipelineStep* ps : this->inputs) {
//             assert(ps != NULL);
//         }
//     }
// };



// PipelineStep<2> Multiply(PipelineStep* input1, PipelineStep* input2) {
//     return PipelineStep(std::array{input1, input2});
// }