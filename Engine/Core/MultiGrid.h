/*
MultiGrid.h
Written by Matthew Fisher

Arbitrary-type grid structure.
*/

#pragma once

#define __MULTIGRID_H

template <class type> class MultiGrid
{
public:
    MultiGrid();
    MultiGrid(const Vector<unsigned int> &dimensions);
    MultiGrid(const Vector<unsigned int> &dimensions, const type &clearValue);
    MultiGrid(const MultiGrid<type> &G);

    ~MultiGrid();

    //
    // Memory
    //
    void FreeMemory();
    MultiGrid<type>& operator = (const MultiGrid<type> &G);
    void Allocate(const Vector<unsigned int> &dimensions);
    void Allocate(const Vector<unsigned int> &dimensions, const type &clearValue);
    
    //
    // Accessors
    //
    __forceinline type& operator() (const Vector<unsigned int> &indices)
    {
        const unsigned int dimensionCount = indices.Length();
        const unsigned int* indexArray = indices.CArray();
#ifdef VECTOR_DEBUG
        const unsigned int* dimensionArray = _dimensions.CArray();
        Assert(_dimensions.Length() == dimensionCount, "Invalid number of indices");
        for(unsigned int dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            Assert(indexArray[dimensionIndex] < dimensionArray[dimensionIndex], "Out-of-bounds MultiGrid access");
        }
#endif
        unsigned int offset = indexArray[0];
        for(unsigned int dimensionIndex = 1; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            offset += _multipliers[dimensionIndex] * indexArray[dimensionIndex];
        }
        return _data[offset];
    }
    __forceinline const type& operator() (const Vector<unsigned int> &indices) const
    {
        const unsigned int dimensionCount = indices.Length();
        const unsigned int* indexArray = indices.CArray();
#ifdef VECTOR_DEBUG
        const unsigned int* dimensionArray = _dimensions.CArray();
        Assert(_dimensions.Length() == dimensionCount, "Invalid number of indices");
        for(unsigned int dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            Assert(indexArray[dimensionIndex] < dimensionArray[dimensionIndex], "Out-of-bounds MultiGrid access");
        }
#endif
        unsigned int offset = indexArray[0];
        for(unsigned int dimensionIndex = 1; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            offset += _multipliers[dimensionIndex] * indexArray[dimensionIndex];
        }
        return _data[offset];
    }
    __forceinline const Vector<unsigned int>& Dimensions() const
    {
        return _dimensions;
    }
    __forceinline type* CArray()
    {
        return _data;
    }
    __forceinline const type* CArray() const
    {
        return _data;
    }
    __forceinline unsigned int TotalEntries() const
    {
        unsigned int result = 1;
        const unsigned int dimensionCount = _dimensions.Length();
        const unsigned int* dimensionArray = _dimensions.CArray();
        for(unsigned int dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
        {
            result *= dimensionArray[dimensionIndex];
        }
        return result;
    }

    //
    // Modifiers
    //
    void Clear(const type &T);

    bool IncrementIndices(Vector<unsigned int> &indices) const;

protected:
    void InitInternal(const Vector<unsigned int> &dimensions);

    type *_data;
    unsigned int *_multipliers;
    Vector<unsigned int> _dimensions;
};

#include "MultiGrid.cpp"
