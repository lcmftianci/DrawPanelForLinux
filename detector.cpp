#include "detector.h"

Detector::Detector():m_Type(nNone)
{

}

void Detector::PushPoint(QPoint pt)
{
    m_arrPoint.push_back(pt);
}

int Detector::GetPoints(std::vector<QPoint> &arrPoints)
{
    arrPoints = m_arrPoint;
    return m_arrPoint.size();
}

void Detector::PushPoint(const std::vector<QPoint> &arrPoints)
{
    m_arrPoint = arrPoints;
}

QColor Detector::GetColor()
{
    return m_cColor;
}

void Detector::SetColor(QColor color)
{
    m_cColor = color;
}

QPoint Detector::GetLastPoint()
{
    return m_arrPoint[m_arrPoint.size() - 1];
}

void Detector::DeletePoint()
{
    if(m_arrPoint.size()>0)
        m_arrPoint.erase(m_arrPoint.end() - 1);
}

void Detector::SetWidth(int nWidth)
{
    m_nPenWidth = nWidth;
}

int Detector::GetWidth()
{
    return m_nPenWidth;
}

int Detector::IsSamePoint(QPoint pt)
{
    int i = 0;
    foreach (QPoint mPt, m_arrPoint)
    {
        if(abs(mPt.x() - pt.x()) < 5 && abs(mPt.y() - pt.y()) < 5)
            return i;
        i++;
    }
    return -1;
}

void Detector::MovePoint(QPoint pt, int index)
{
    //int index = IsSamePoint(pt);
    if(index >= 0 && index < m_arrPoint.size())
        m_arrPoint[index] = pt;
}

void Detector::MoveView(int x, int y)
{
    int i = 0;
    foreach (QPoint pt, m_arrPoint)
    {
        m_arrPoint[i] = QPoint(pt.x()+x, pt.y()+y);
        i++;
    }
}

bool Detector::IsInArea(QPoint pt)
{
    int px = pt.x();
    int py = pt.y();
    bool bIn = false;
    for(int i = 0, l = m_arrPoint.size(), j = l-1; i < l; j = i,i++)
    {
        int sx = m_arrPoint[i].x();
        int sy = m_arrPoint[i].y();
        int tx = m_arrPoint[j].x();
        int ty = m_arrPoint[j].y();

        //点与边顶点重合
        if((sx == px && sy == py)||(tx==px&&ty==py))
        {
            return true;
        }
        //判断线段端点是否在射线两侧
        if((sy < py && ty >=py)||sy >=py && ty<py)
        {
            int x = sx + (py - sy)*(tx-sx)/(ty-sy);
            //多边形边上
            if(x == px)
            {
                return true;
            }
            //射线过多边形边
            if(x > px)
            {
                bIn = !bIn;
            }
        }
    }
    //是否在其内
    return bIn;
}
