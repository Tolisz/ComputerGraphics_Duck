#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>

#include <random>

class BSplineRandomizer
{
private:

    // *=*=*=*=*=*=*=*=*=*=
    //   Random number gen
    // *=*=*=*=*=*=*=*=*=*=
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_uniformZeroToOne;

public:

    // current points creating a curve 
    glm::vec3 m_p0;
    glm::vec3 m_p1;
    glm::vec3 m_p2;
    glm::vec3 m_p3;

private:

    // min and max points creating a boundary
    // parallelepiped
    glm::vec3 m_pMin;
    glm::vec3 m_pMax;

private:

    glm::vec3 m_deCasteljau[4];
    int m_MaxInterCound = 100;

public:

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    BSplineRandomizer();
    ~BSplineRandomizer();

    BSplineRandomizer(const BSplineRandomizer&) = delete;
    BSplineRandomizer(BSplineRandomizer&&) = delete;
    BSplineRandomizer& operator=(const BSplineRandomizer&) = delete;
    BSplineRandomizer& operator=(BSplineRandomizer&&) = delete;

    BSplineRandomizer(
        const glm::vec3 p0, 
        const glm::vec3 p1, 
        const glm::vec3 p2, 
        const glm::vec3 p3,
        const glm::vec3 pMin,
        const glm::vec3 max
        );
    
    // *=*=*=*=*=*=*=*=*=*=
    //       Methods
    // *=*=*=*=*=*=*=*=*=*=

    glm::vec3 GetCurvePosition(float t);
    glm::vec3 GetCurveTangent(float t);
    void GenerateSubsequentCurve();

private: 

    glm::vec3 RayBoxIntersection(glm::vec3 p, glm::vec3 d);
    void DeCasteljau(glm::vec3 tab[], int n, float t);
};