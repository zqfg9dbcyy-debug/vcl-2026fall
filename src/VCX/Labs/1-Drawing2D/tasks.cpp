#include <random>

#include <spdlog/spdlog.h>

#include "Labs/1-Drawing2D/tasks.h"

using VCX::Labs::Common::ImageRGB;

namespace VCX::Labs::Drawing2D {
    /******************* 1.Image Dithering *****************/
    void DitheringThreshold(
        ImageRGB &       output,
        ImageRGB const & input) {
        for (std::size_t x = 0; x < input.GetSizeX(); ++x)
            for (std::size_t y = 0; y < input.GetSizeY(); ++y) {
                glm::vec3 color = input.At(x, y);
                output.At(x, y) = {
                    color.r > 0.5 ? 1 : 0,
                    color.g > 0.5 ? 1 : 0,
                    color.b > 0.5 ? 1 : 0,
                };
            }
    }

    void DitheringRandomUniform(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        std::mt19937 generator(std::random_device{}());
        std::uniform_real_distribution<float> noise(-0.5f, 0.5f);
        ImageRGB noisy(input.GetSizeX(), input.GetSizeY());
        for(std::size_t x=0;x<input.GetSizeX();++x){
            for(std::size_t y=0;y<input.GetSizeY();++y){
                glm::vec3 color = input.At(x,y);
                float gray=color.r;
                gray+=noise(generator);
                noisy.At(x,y)=glm::vec3(gray);
            }
        }
        DitheringThreshold(output,noisy);
    }

    void DitheringRandomBlueNoise(
        ImageRGB &       output,
        ImageRGB const & input,
        ImageRGB const & noise) {
        // your code here:
        ImageRGB noisy(input.GetSizeX(), input.GetSizeY());
        for(std::size_t x=0;x<input.GetSizeX();++x){
            for(std::size_t y=0;y<input.GetSizeY();++y){
                glm::vec3 n = noise.At(
                x % noise.GetSizeX(),
                y % noise.GetSizeY()
                );
                noisy.At(x,y)=input.At(x,y)+n-glm::vec3(0.5f);
            }
        }
        DitheringThreshold(output,noisy);
    }

    void DitheringOrdered(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        output.Fill(glm::vec3(0.0f));
        for(std::size_t x=0;x<input.GetSizeX();++x){
            for(std::size_t y=0;y<input.GetSizeY();++y){
                glm::vec3 color=input.At(x,y);
                float gray=color.r;
                if(gray>0.1) output.At(3*x+1,3*y+1)=glm::vec3(1.0f);
                if(gray>0.2) output.At(3*x,3*y+1)=glm::vec3(1.0f);
                if(gray>0.3) output.At(3*x+1,3*y+2)=glm::vec3(1.0f);
                if(gray>0.4) output.At(3*x+2,3*y+1)=glm::vec3(1.0f);
                if(gray>0.5) output.At(3*x+2,3*y)=glm::vec3(1.0f);
                if(gray>0.6) output.At(3*x,3*y+2)=glm::vec3(1.0f);
                if(gray>0.7) output.At(3*x,3*y)=glm::vec3(1.0f);
                if(gray>0.8) output.At(3*x+2,3*y+2)=glm::vec3(1.0f);
                if(gray>0.9) output.At(3*x+1,3*y)=glm::vec3(1.0f);
            }
        }
    }

    void DitheringErrorDiffuse(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        int width=static_cast<int>(input.GetSizeX());
        int length=static_cast<int>(input.GetSizeY());
        std::vector<float> buffer(width*length,0.0f);
        for(int y=0;y<length;++y){
            for(int x=0;x<width;++x){
                buffer[y*width+x]=input.At(x,y).r;
            }
        }
        for(int y=0;y<length;++y){
            for(int x=0;x<width;++x){
                float color=0.0f;
                color=buffer[y*width+x]>0.5f ? 1.0f : 0.0f;
                output.At(x,y)=glm::vec3(color);
                float error=buffer[y*width+x]-color;
                if(x+1<width) buffer[y*width+x+1]+=(error*7/16);
                if(y+1<length){
                    if(x>0) buffer[(y+1)*width+x-1]+=(error*3/16);
                    buffer[(y+1)*width+x]+=(error*5/16);
                    if(x+1<width) buffer[(y+1)*width+x+1]+=(error*1/16);
                }
            }
        }
    }

    /******************* 2.Image Filtering *****************/
    void Blur(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        std::size_t W=input.GetSizeX();
        std::size_t H=input.GetSizeY();
        if (W==0||H==0) return;
        ImageRGB padded(W+2,H+2);
        for (std::size_t y=0;y<H;++y)
            for (std::size_t x=0; x<W;++x){
                padded.At(x+1,y+1)=input.At(x,y);
            }
        for (std::size_t y=0;y<H;++y) {
            padded.At(0,y+1)=input.At(0,y);
            padded.At(W+1,y+1)=input.At(W-1,y);
        }
        for (std::size_t x=0;x<W;++x) {
            padded.At(x+1,0)= input.At(x,0);
            padded.At(x+1,H+1)=input.At(x,H-1);
        }
        padded.At(0,0)=input.At(0,0);
        padded.At(W+1,0)=input.At(W-1,0);
        padded.At(0,H+1)=input.At(0,H-1);
        padded.At(W+1,H+1)=input.At(W-1,H-1);

        const float kernel[3][3] = {
            {1,2,1},{2,4,2},{1,2,1}
        };

        for (std::size_t y = 0; y < H; ++y) {
            for (std::size_t x = 0; x < W; ++x) {
                glm::vec3 result(0.0f);

                for (std::size_t dy = 0; dy < 3; ++dy)
                    for (std::size_t dx = 0; dx < 3; ++dx) {
                        glm::vec3 pixel = padded.At(x + dx, y + dy);
                        result += pixel * kernel[dy][dx];
                    }
                output.At(x,y) = result / 16.0f;
            }
        }
    }

    void Edge(
        ImageRGB &       output,
        ImageRGB const & input) {
        // your code here:
        std::size_t W=input.GetSizeX();
        std::size_t H=input.GetSizeY();
        if (W==0||H==0) return;
        ImageRGB padded(W+2,H+2);
        for (std::size_t y=0;y<H;++y)
            for (std::size_t x=0; x<W;++x){
                padded.At(x+1,y+1)=input.At(x,y);
            }
        for (std::size_t y=0;y<H;++y) {
            padded.At(0,y+1)=input.At(0,y);
            padded.At(W+1,y+1)=input.At(W-1,y);
        }
        for (std::size_t x=0;x<W;++x) {
            padded.At(x+1,0)= input.At(x,0);
            padded.At(x+1,H+1)=input.At(x,H-1);
        }
        padded.At(0,0)=input.At(0,0);
        padded.At(W+1,0)=input.At(W-1,0);
        padded.At(0,H+1)=input.At(0,H-1);
        padded.At(W+1,H+1)=input.At(W-1,H-1);
        const float kernelx[3][3]={
            {-1,0,1},{-2,0,2},{-1,0,1}
        };
        const float kernely[3][3]={
            {1,2,1},{0,0,0},{-1,-2,-1}
        };
        for(std::size_t y=0;y<H;++y){
            for(std::size_t x=0;x<W;++x){
                glm::vec3 resultx(0.0f);
                glm::vec3 resulty(0.0f);
                for(int dy=0;dy<3;dy++){
                    for(int dx=0;dx<3;dx++){
                        glm::vec3 pixel=padded.At(x+dx,y+dy);
                        resultx+=pixel*kernelx[dy][dx];
                        resulty+=pixel*kernely[dy][dx];
                    }
                }
                float r=resultx.r*resultx.r+resulty.r*resulty.r;
                float g=resultx.g*resultx.g+resulty.g*resulty.g;
                float b=resultx.b*resultx.b+resulty.b*resulty.b;
                output.At(x,y)=glm::vec3(std::sqrt(r),std::sqrt(g),std::sqrt(b));
            }
        }
    }

    /******************* 3. Image Inpainting *****************/
    void Inpainting(
        ImageRGB &         output,
        ImageRGB const &   inputBack,
        ImageRGB const &   inputFront,
        const glm::ivec2 & offset) {
        output             = inputBack;
        std::size_t width  = inputFront.GetSizeX();
        std::size_t height = inputFront.GetSizeY();
        glm::vec3 * g      = new glm::vec3[width * height];
        memset(g, 0, sizeof(glm::vec3) * width * height);
        // set boundary condition
        for (std::size_t y = 0; y < height; ++y) {
            // set boundary for (0, y), your code: g[y * width] = ?
            // set boundary for (width - 1, y), your code: g[y * width + width - 1] = ?
        }
        for (std::size_t x = 0; x < width; ++x) {
            // set boundary for (x, 0), your code: g[x] = ?
            // set boundary for (x, height - 1), your code: g[(height - 1) * width + x] = ?
        }

        // Jacobi iteration, solve Ag = b
        for (int iter = 0; iter < 8000; ++iter) {
            for (std::size_t y = 1; y < height - 1; ++y)
                for (std::size_t x = 1; x < width - 1; ++x) {
                    g[y * width + x] = (g[(y - 1) * width + x] + g[(y + 1) * width + x] + g[y * width + x - 1] + g[y * width + x + 1]);
                    g[y * width + x] = g[y * width + x] * glm::vec3(0.25);
                }
        }

        for (std::size_t y = 0; y < inputFront.GetSizeY(); ++y)
            for (std::size_t x = 0; x < inputFront.GetSizeX(); ++x) {
                glm::vec3 color = g[y * width + x] + inputFront.At(x, y);
                output.At(x + offset.x, y + offset.y) = color;
            }
        delete[] g;
    }

    /******************* 4. Line Drawing *****************/
    void DrawLine(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1) {
        // your code here:
    }

    /******************* 5. Triangle Drawing *****************/
    void DrawTriangleFilled(
        ImageRGB &       canvas,
        glm::vec3 const  color,
        glm::ivec2 const p0,
        glm::ivec2 const p1,
        glm::ivec2 const p2) {
        // your code here:
    }

    /******************* 6. Image Supersampling *****************/
    void Supersample(
        ImageRGB &       output,
        ImageRGB const & input,
        int              rate) {
        // your code here:
    }

    /******************* 7. Bezier Curve *****************/
    // Note: Please finish the function [DrawLine] before trying this part.
    glm::vec2 CalculateBezierPoint(
        std::span<glm::vec2> points,
        float const          t) {
        // your code here:
        return glm::vec2 {0, 0};
    }
} // namespace VCX::Labs::Drawing2D