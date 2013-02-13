/*
ColorGenerator.cpp
Written by Matthew Fisher
*/

Vector<RGBColor> ColorGenerator::Generate(UINT colorCount)
{
    Vector<RGBColor> result(colorCount);
    Generate(result, Vec3f(1.0f, 1.0f, 1.0f));
    return result;
}

void ColorGenerator::Generate(Vector<RGBColor> &result)
{
    Generate(result, Vec3f(1.0f, 1.0f, 1.0f));
}

void ColorGenerator::Generate(Vector<RGBColor> &result, const Vec3f &scale)
{
    const UINT colorCount = result.Length();
    
    Vector<Vec3f> allColors(colorCount * 25);
    const UINT allColorsCount = allColors.Length();
    for(UINT colorIndex = 0; colorIndex < allColorsCount; colorIndex++)
    {
        Vec3f curColor = Vec3f(rnd(), rnd(), rnd());
        while(curColor.x + curColor.y + curColor.z < 0.5f)
        {
            curColor = Vec3f(rnd(), rnd(), rnd());
        }
        allColors[colorIndex] = Vec3f(curColor.x * scale.x, curColor.y * scale.y, curColor.z * scale.z);
    }

    KMeansClustering<Vec3f, Vec3fKMeansMetric> clustering;
    clustering.Cluster(allColors, colorCount, 20, false);

    for(UINT colorIndex = 0; colorIndex < colorCount; colorIndex++)
    {
        result[colorIndex] = RGBColor(clustering.ClusterCenter(colorIndex));
    }
}
