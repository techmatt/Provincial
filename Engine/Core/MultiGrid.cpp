/*
MultiGrid.cpp
Written by Matthew Fisher
*/

#pragma once

#include "MultiGrid.h"

template <class type> MultiGrid<type>::MultiGrid()
{
    _data = NULL;
    _multipliers = NULL;
}

template <class type> MultiGrid<type>::MultiGrid(const Vector<unsigned int> &dimensions)
{
    InitInternal(dimensions);
}

template <class type> MultiGrid<type>::MultiGrid(const Vector<unsigned int> &dimensions, const type &clearValue)
{
    InitInternal(dimensions);
    Clear(clearValue);
}

template <class type> MultiGrid<type>::MultiGrid(const MultiGrid<type> &G)
{
    InitInternal(G._dimensions);
    
    const unsigned int totalEntries = TotalEntries();
    for(unsigned int entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        _data[entryIndex] = G._data[entryIndex];
    }
}

template <class type> void MultiGrid<type>::InitInternal(const Vector<unsigned int> &dimensions)
{
    _dimensions = dimensions;

    unsigned int totalEntries = 1;
    const unsigned int dimensionCount = dimensions.Length();
    const unsigned int* dimensionArray = dimensions.CArray();
    _multipliers = new unsigned int[dimensionCount];
    for(unsigned int dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        const unsigned int curDimension = dimensionArray[dimensionIndex];
        _multipliers[dimensionIndex] = totalEntries;
        totalEntries *= curDimension;
    }

    _data = new type[totalEntries];
}

template <class type> MultiGrid<type>::~MultiGrid()
{
    FreeMemory();
}

template <class type> void MultiGrid<type>::FreeMemory()
{
    _dimensions.FreeMemory();
    if(_multipliers != NULL)
    {
        delete[] _multipliers;
        _multipliers = NULL;
    }
    if(_data != NULL)
    {
        delete[] _data;
        _data = NULL;
    }
}

template <class type> void MultiGrid<type>::Allocate(const Vector<unsigned int> &dimensions)
{
    FreeMemory();
    InitInternal(dimensions);
}

template <class type> void MultiGrid<type>::Allocate(const Vector<unsigned int> &dimensions, const type &clearValue)
{
    FreeMemory();
    InitInternal(dimensions);
    Clear(clearValue);
}

template <class type> void MultiGrid<type>::Clear(const type &T)
{
    const unsigned int totalEntries = TotalEntries();
    for(unsigned int entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        _data[entryIndex] = T;
    }
}

template <class type> bool MultiGrid<type>::IncrementIndices(Vector<unsigned int> &indices) const
{
    const unsigned int dimensionCount = _dimensions.Length();
    for(UINT dimensionIndex = 0; dimensionIndex < dimensionCount; dimensionIndex++)
    {
        UINT &curDimension = indices[dimensionIndex];
        if(curDimension + 1 < _dimensions[dimensionIndex])
        {
            curDimension++;
            return true;
        }
        else
        {
            curDimension = 0;
        }
    }
    return false;
}

template <class type> MultiGrid<type>& MultiGrid<type>::operator = (const MultiGrid<type> &G)
{
    FreeMemory();
    InitInternal(G._dimensions);
    const unsigned int totalEntries = TotalEntries();
    for(unsigned int entryIndex = 0; entryIndex < totalEntries; entryIndex++)
    {
        _data[entryIndex] = G._data[entryIndex];
    }

    return *this;
}
