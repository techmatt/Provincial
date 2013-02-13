/*
VecNf.inl
Written by Matthew Fisher

Inline file for a 4-dimensional vector of floats
*/

#pragma once

VecNf::VecNf()
{

}

VecNf::VecNf(UINT Dimension)
{
    v.Allocate(Dimension);
}

VecNf::VecNf(const VecNf &a)
{
    v = a.v;
}

VecNf& VecNf::operator = (const VecNf &a)
{
    v = a.v;
    return *this;
}

UINT VecNf::Dimension() const
{
    return v.Length();
}

float VecNf::Length() const
{
    float sum = 0.0f;
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        const float x = v[dimensionIndex];
        sum += x * x;
    }
    return sqrtf(sum);
}

float VecNf::LengthSq() const
{
    float sum = 0.0f;
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        const float x = v[dimensionIndex];
        sum += x * x;
    }
    return sum;
}

VecNf operator * (const VecNf &left, float right)
{
    const UINT dimensionCount = left.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = left.v[dimensionIndex] * right;
    }
    return result;
}

VecNf operator * (float right, const VecNf &left)
{
    const UINT dimensionCount = left.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = left.v[dimensionIndex] * right;
    }
    return result;
}

VecNf operator / (const VecNf &left, float right)
{
    float scale = 1.0f / right;
    const UINT dimensionCount = left.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = left.v[dimensionIndex] * scale;
    }
    return result;
}

VecNf operator + (const VecNf &left, const VecNf &right)
{
    const UINT dimensionCount = left.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = left.v[dimensionIndex] + right.v[dimensionIndex];
    }
    return result;
}

VecNf operator - (const VecNf &left, const VecNf &right)
{
    const UINT dimensionCount = left.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = left.v[dimensionIndex] - right.v[dimensionIndex];
    }
    return result;
}

VecNf& VecNf::operator *= (float right)
{
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        v[dimensionIndex] *= right;
    }
    return *this;
}

VecNf& VecNf::operator /= (float right)
{
    const float scale = 1.0f / right;
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        v[dimensionIndex] *= scale;
    }
    return *this;
}

VecNf& VecNf::operator += (const VecNf &right)
{
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        v[dimensionIndex] += right[dimensionIndex];
    }
    return *this;
}

VecNf& VecNf::operator -= (const VecNf &right)
{
    const UINT dimensionCount = v.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        v[dimensionIndex] -= right[dimensionIndex];
    }
    return *this;
}

__forceinline VecNf operator - (const VecNf &right)
{
    const UINT dimensionCount = right.v.Length();
    VecNf result(dimensionCount);
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        result.v[dimensionIndex] = -right.v[dimensionIndex];
    }
    return result;
}

__forceinline float VecNf::Dist(const VecNf &left, const VecNf &right)
{
    const UINT dimensionCount = left.v.Length();
    float sum = 0.0f;
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        float diff = left.v[dimensionIndex] - right.v[dimensionIndex];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

__forceinline float VecNf::DistSq(const VecNf &left, const VecNf &right)
{
    const UINT dimensionCount = left.v.Length();
    float sum = 0.0f;
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        float diff = left.v[dimensionIndex] - right.v[dimensionIndex];
        sum += diff * diff;
    }
    return sum;
}

__forceinline VecNf VecNf::Normalize(const VecNf &left)
{
    float length = left.Length();
    if(length == 0.0f)
    {
        return left;
    }
    else
    {
        float factor = 1.0f / length;
        const UINT dimensionCount = left.v.Length();
        VecNf result(dimensionCount);
        for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            result.v[dimensionIndex] = left.v[dimensionIndex] * factor;
        }
        return result;
    }
}
