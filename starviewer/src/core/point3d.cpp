#include "point3d.h"

#include "mathtools.h"

namespace udg {

Point3D::Point3D()
{
    for (int i = 0; i < 3; ++i)
    {
        m_point[i] = 0.0;
    }
}

Point3D::Point3D(double point[3])
{
    for (int i = 0; i < 3; ++i)
    {
        m_point[i] = point[i];
    }
}

Point3D::Point3D(double x, double y, double z)
{
    m_point[0] = x;
    m_point[1] = y;
    m_point[2] = z;
}

Point3D::~Point3D()
{
}

void Point3D::set(int i, double value)
{
    if (MathTools::isInsideRange(i, 0, 2))
    {
        m_point[i] = value;
    }
}

const double Point3D::at(int i) const
{
    if (MathTools::isInsideRange(i, 0, 2))
    {
        return m_point[i];
    }
    else
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double& Point3D::operator[](int index)
{
    return m_point[index];
}

const double& Point3D::operator[](int index) const
{
    return m_point[index];
}

double* Point3D::getAsDoubleArray()
{
    return m_point;
}

} // End namespace udg
