#include "BSplineRandomizer.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <iostream>

BSplineRandomizer::BSplineRandomizer()
    : m_gen(m_rd()), m_uniformZeroToOne(0.0f, 0.1f)
{}

BSplineRandomizer::~BSplineRandomizer()
{}

BSplineRandomizer::BSplineRandomizer(
    const glm::vec3 p0, 
    const glm::vec3 p1, 
    const glm::vec3 p2, 
    const glm::vec3 p3,
    const glm::vec3 pMin,
    const glm::vec3 pMax
    )
    : 
    m_p0{p0}, m_p1{p1}, m_p2{p2}, m_p3{p3}, m_pMin{pMin}, m_pMax{pMax},
    m_gen(m_rd()), m_uniformZeroToOne(0.0f, 1.0f)
{}

glm::vec3 BSplineRandomizer::GetCurvePosition(float t)
{      
    m_deCasteljau[0] = m_p0;
    m_deCasteljau[1] = m_p1;
    m_deCasteljau[2] = m_p2;
    m_deCasteljau[3] = m_p3;

    DeCasteljau(m_deCasteljau, 3, t);
    return m_deCasteljau[0];
}

glm::vec3 BSplineRandomizer::GetCurveTangent(float t)
{
    m_deCasteljau[0] = m_p0;
    m_deCasteljau[1] = m_p1;
    m_deCasteljau[2] = m_p2;
    m_deCasteljau[3] = m_p3;

    // first derivative 
    for (int i = 0; i < 3; i++) {
        m_deCasteljau[i] = 3.0f * (m_deCasteljau[i + 1] - m_deCasteljau[i]);
    }

    // value of the derivative 
    DeCasteljau(m_deCasteljau, 2, t);
    return m_deCasteljau[0];
}

void BSplineRandomizer::GenerateSubsequentCurve()
{
    glm::vec3 d = m_p2 - m_p1;
    glm::vec3 n0 = RayBoxIntersection(m_p2, d);

    d = m_p3 - m_p2;
    glm::vec3 n1_int = RayBoxIntersection(m_p3, d);
    float t = m_uniformZeroToOne(m_gen);
    glm::vec3 n1 = t * m_p3 + (1 - t) * n1_int;
    
    d = n1 - n0;
    glm::vec3 n2_int = RayBoxIntersection(n1, d);
    glm::vec3 n2 = (n1 + n2_int) / 2.0f;

    glm::vec3 n3;
    float l = glm::length(n2 - n1);
    float currL, bestL;
    glm::vec3 currN3, bestN3;
    bool isFirst = true;

    for (int i = 0; true; i++) {
        if (i >= m_MaxInterCound) {
            n3 = bestN3;
            break;
        }
        
        currN3 = m_pMin + (m_pMax - m_pMin) * 
        glm::vec3(m_uniformZeroToOne(m_gen), 
                  m_uniformZeroToOne(m_gen), 
                  m_uniformZeroToOne(m_gen));
        currL = glm::length(currN3 - n2);

        if (currL < l) {
            n3 = currN3;
            break;
        }

        if (isFirst || currL < bestL) {
            bestL = currL;
            bestN3 = currN3;
            isFirst = false;
        }
    }

    m_p0 = m_p3;
    m_p1 = n1;
    m_p2 = n2;
    m_p3 = n3;
}

glm::vec3 BSplineRandomizer::RayBoxIntersection(glm::vec3 p, glm::vec3 d)
{
    glm::vec3 t1 = (m_pMax - p) / d;
    glm::vec3 t2 = (m_pMin - p) / d;

    glm::vec3 tm = glm::max(t1, t2);
    float t = glm::min(glm::min(tm.x, tm.y), tm.z);

    return p + t * d;
}

void BSplineRandomizer::DeCasteljau(glm::vec3 tab[], int n, float t)
{
    float one_t = 1.0f - t;

    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n - i; j++) {
            tab[j] = one_t * tab[j] + t * tab[j+1];
        }
    }
}