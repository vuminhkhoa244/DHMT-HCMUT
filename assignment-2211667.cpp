#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>

#ifndef PI
#define PI 3.14159265358979323846
#endif

using namespace std;

//==============Support Class============================
class Point3
{
public:
    float x, y, z;
    void set(float dx, float dy, float dz)
    {
        x = dx;
        y = dy;
        z = dz;
    }
    void set(Point3 &p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
    }
    Point3() { x = y = z = 0; }
    Point3(float dx, float dy, float dz)
    {
        x = dx;
        y = dy;
        z = dz;
    }
};
class Color3
{
public:
    float r, g, b;
    void set(float red, float green, float blue)
    {
        r = red;
        g = green;
        b = blue;
    }
    void set(Color3 &c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
    }
    Color3() { r = g = b = 0; }
    Color3(float red, float green, float blue)
    {
        r = red;
        g = green;
        b = blue;
    }
};

class Vector3
{
public:
    float x, y, z;
    void set(float dx, float dy, float dz)
    {
        x = dx;
        y = dy;
        z = dz;
    }
    void set(Vector3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    void flip()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    void normalize()
    {
        float temp = sqrt(x * x + y * y + z * z);
        if (temp > 0.0)
        {
            x /= temp;
            y /= temp;
            z /= temp;
        }
    }

    Vector3() { x = y = z = 0; }
    Vector3(float dx, float dy, float dz)
    {
        x = dx;
        y = dy;
        z = dz;
    }
    Vector3(Vector3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3 cross(Vector3 b)
    {
        Vector3 c;
        c.x = y * b.z - z * b.y;
        c.y = z * b.x - x * b.z;
        c.z = x * b.y - y * b.x;
        return c;
    }

    float dot(Vector3 b)
    {
        return x * b.x + y * b.y + z * b.z;
    }
};

//==============Mesh Class===============================
class VertexID
{
public:
    int vertIndex;
    int colorIndex;
};

class Face
{
public:
    int nVerts;
    VertexID *vert;
    Vector3 facenorm;

    Face()
    {
        nVerts = 0;
        vert = NULL;
    }
    ~Face()
    {
        if (vert != NULL)
        {
            delete[] vert;
            vert = NULL;
        }
        nVerts = 0;
    }
};

class Mesh
{
public:
    int numVerts;
    Point3 *pt;
    int numFaces;
    Face *face;

    Mesh()
    {
        numVerts = 0;
        pt = NULL;
        numFaces = 0;
        face = NULL;
    }
    ~Mesh()
    {
        if (pt != NULL)
            delete[] pt;
        if (face != NULL)
            delete[] face;
        numVerts = 0;
        numFaces = 0;
    }

    void draw()
    {
        for (int f = 0; f < numFaces; f++)
        {
            glBegin(GL_POLYGON);
            for (int v = 0; v < face[f].nVerts; v++)
            {
                int iv = face[f].vert[v].vertIndex;

                if (v == 0 && face[f].nVerts >= 3)
                {
                    Point3 p1 = pt[face[f].vert[0].vertIndex];
                    Point3 p2 = pt[face[f].vert[1].vertIndex];
                    Point3 p3 = pt[face[f].vert[2].vertIndex];
                    Vector3 v1(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
                    Vector3 v2(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
                    Vector3 normal = v1.cross(v2);
                    normal.normalize();
                    glNormal3f(normal.x, normal.y, normal.z);
                }

                glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
            }
            glEnd();
        }
    }
    void CreateCylinder(int nSegment, float fHeight, float fRadius)
    {
        numVerts = nSegment * 2 + 2; 
        pt = new Point3[numVerts];

        int centerTop = numVerts - 2;
        int centerBottom = numVerts - 1;
        pt[centerTop].set(0, fHeight / 2, 0);
        pt[centerBottom].set(0, -fHeight / 2, 0);

        float segmentAngle = 2 * PI / nSegment;

        for (int i = 0; i < nSegment; i++)
        {
            float x = fRadius * cos(i * segmentAngle);
            float z = fRadius * sin(i * segmentAngle);
            pt[i].set(x, fHeight / 2, z);             // Vòng trên
            pt[i + nSegment].set(x, -fHeight / 2, z); // Vòng dưới
        }

        numFaces = nSegment * 3; // Thân + Nắp trên + Nắp dưới
        face = new Face[numFaces];

        for (int i = 0; i < nSegment; i++)
        {
            // Mặt bên (Hình chữ nhật -> vẽ bằng Polygon 4 đỉnh)
            face[i].nVerts = 4;
            face[i].vert = new VertexID[4];
            face[i].vert[0].vertIndex = i;
            face[i].vert[1].vertIndex = (i + 1) % nSegment;
            face[i].vert[2].vertIndex = (i + 1) % nSegment + nSegment;
            face[i].vert[3].vertIndex = i + nSegment;

            // Nắp trên (Tam giác nối với tâm)
            face[i + nSegment].nVerts = 3;
            face[i + nSegment].vert = new VertexID[3];
            face[i + nSegment].vert[0].vertIndex = i;
            face[i + nSegment].vert[1].vertIndex = centerTop;
            face[i + nSegment].vert[2].vertIndex = (i + 1) % nSegment;

            // Nắp dưới
            face[i + 2 * nSegment].nVerts = 3;
            face[i + 2 * nSegment].vert = new VertexID[3];
            face[i + 2 * nSegment].vert[0].vertIndex = i + nSegment;
            face[i + 2 * nSegment].vert[1].vertIndex = (i + 1) % nSegment + nSegment;
            face[i + 2 * nSegment].vert[2].vertIndex = centerBottom;
        }
    }
    void CreateCuboid(float fSizeX, float fSizeY, float fSizeZ);
    void CreateShape4(float fSizeX, float fSizeY, float fSizeZ, float inset);
    void CreateShape5(float width, float height, float depth, float slope);
    void CreateShape1(int nSegment, float fHeight, float fInnerRadius, float fOuterRadius, float fSectorAngle);
    void CreateShape3(float fWidth, float fDepth, float fHeight, float fChamfer);
    void CreateShape2(float fInnerR, float fOuterR, float fHeight, float fGap, float fSolidEnd, float fAngle);
};

// ==========================================================
// CÁC THAM SỐ TOÀN CỤC
// ==========================================================
int screenWidth = 800;
int screenHeight = 600;
float angle = 0.0f;
float cameraHeight = 12.0f;
float cameraDistance = 18.0f;
bool isLightingOn = true;
bool isWireframe = false;
float wheelRotation = 0.0f;
float slideY = 0.0f;
bool is2D = false;

void Mesh::CreateCuboid(float fSizeX, float fSizeY, float fSizeZ)
{
    numVerts = 8;
    pt = new Point3[numVerts];

    float hX = fSizeX / 2.0f;
    float hY = fSizeY / 2.0f;
    float hZ = fSizeZ / 2.0f;

    // Các đỉnh
    pt[0].set(-hX, -hY, hZ);
    pt[1].set(hX, -hY, hZ);
    pt[2].set(hX, -hY, -hZ);
    pt[3].set(-hX, -hY, -hZ);
    pt[4].set(-hX, hY, hZ);
    pt[5].set(hX, hY, hZ);
    pt[6].set(hX, hY, -hZ);
    pt[7].set(-hX, hY, -hZ);

    numFaces = 6;
    face = new Face[numFaces];

    static int faceVerts[6][4] = {
        {3, 2, 1, 0}, {4, 5, 6, 7}, // Bottom, Top
        {0, 1, 5, 4},
        {1, 2, 6, 5}, // Front, Right
        {2, 3, 7, 6},
        {3, 0, 4, 7} // Back, Left
    };

    for (int i = 0; i < numFaces; i++)
    {
        face[i].nVerts = 4;
        face[i].vert = new VertexID[4];
        for (int j = 0; j < 4; j++)
            face[i].vert[j].vertIndex = faceVerts[i][j];
    }
}

void Mesh::CreateShape1(int nSegment, float fHeight, float fInnerRadius, float fOuterRadius, float fSectorAngle)
{
    numVerts = nSegment * 2 + 2;
    pt = new Point3[numVerts];

    float halfHeight = fHeight / 2.0f;
    float angleStep = 2.0f * PI / nSegment;

    // Center points
    pt[0].set(0, halfHeight, 0);
    pt[numVerts - 1].set(0, -halfHeight, 0);

    // Perimeter vertices
    for (int i = 0; i < nSegment; i++)
    {
        float currentAngle = i * angleStep;
        float radius = (currentAngle < fSectorAngle) ? fOuterRadius : fInnerRadius;

        float x = radius * cos(currentAngle);
        float z = radius * sin(currentAngle);

        pt[i + 1].set(x, halfHeight, z);

        pt[i + 1 + nSegment].set(x, -halfHeight, z);
    }

    numFaces = nSegment * 3;
    face = new Face[numFaces];

    int faceIndex = 0;

    for (int i = 0; i < nSegment; i++)
    {
        int current_top_idx = i + 1;
        int next_top_idx = (i == nSegment - 1) ? 1 : i + 2;

        int current_bot_idx = current_top_idx + nSegment;
        int next_bot_idx = next_top_idx + nSegment;

        // Top face (Tam giác nối tâm trên)
        face[faceIndex].nVerts = 3;
        face[faceIndex].vert = new VertexID[3];
        face[faceIndex].vert[0].vertIndex = 0;
        face[faceIndex].vert[1].vertIndex = next_top_idx;
        face[faceIndex].vert[2].vertIndex = current_top_idx;
        faceIndex++;

        // Side face (Hình chữ nhật bên hông)
        face[faceIndex].nVerts = 4;
        face[faceIndex].vert = new VertexID[4];
        face[faceIndex].vert[0].vertIndex = current_top_idx;
        face[faceIndex].vert[1].vertIndex = next_top_idx;
        face[faceIndex].vert[2].vertIndex = next_bot_idx;
        face[faceIndex].vert[3].vertIndex = current_bot_idx;
        faceIndex++;

        // Bottom face (Tam giác nối tâm dưới)
        face[faceIndex].nVerts = 3;
        face[faceIndex].vert = new VertexID[3];
        face[faceIndex].vert[0].vertIndex = numVerts - 1;
        face[faceIndex].vert[1].vertIndex = current_bot_idx;
        face[faceIndex].vert[2].vertIndex = next_bot_idx;
        faceIndex++;
    }
}

// ==========================================================
void Mesh::CreateShape2(float fInnerR, float fOuterR, float fHeight, float fGap, float fSolidEnd, float fAngle)
{
    int nSolidSeg = 6;
    int nSlotSeg = 18;
    int slices1 = nSolidSeg + 1;
    int slices2 = nSlotSeg + 1;
    int slices3 = nSolidSeg + 1;

    numVerts = (slices1 + slices2 + slices3) * 8;
    pt = new Point3[numVerts];

    numFaces = (nSolidSeg * 4 + 2) + (nSlotSeg * 8 + 2) + (nSolidSeg * 4 + 2);
    face = new Face[numFaces];

    float halfH = fHeight / 2.0f;
    float totalW = fOuterR - fInnerR;
    float wallW = (totalW - fGap) / 2.0f;

    float r0 = fInnerR;
    float r1 = fInnerR + wallW;
    float r2 = fOuterR - wallW;
    float r3 = fOuterR;
    float r_mid = (r1 + r2) / 2.0f;

    float startAngle = -(fAngle / 2.0f);
    float solidAngle = fSolidEnd;
    float slotAngle = fAngle - 2 * fSolidEnd;

    float stepSolid = solidAngle / nSolidSeg;
    float stepSlot = slotAngle / nSlotSeg;

    int vIdx = 0;
    int fIdx = 0;

    int startIdx1 = vIdx;
    for (int i = 0; i < slices1; i++)
    {
        float theta = startAngle + i * stepSolid;
        float c = cos(theta);
        float s = sin(theta);
        // r1, r2 chập lại thành r_mid
        pt[vIdx++].set(r0 * s, r0 * c, halfH);        // 0
        pt[vIdx++].set(r_mid * s, r_mid * c, halfH);  // 1
        pt[vIdx++].set(r_mid * s, r_mid * c, halfH);  // 2
        pt[vIdx++].set(r3 * s, r3 * c, halfH);        // 3
        pt[vIdx++].set(r0 * s, r0 * c, -halfH);       // 4
        pt[vIdx++].set(r_mid * s, r_mid * c, -halfH); // 5
        pt[vIdx++].set(r_mid * s, r_mid * c, -halfH); // 6
        pt[vIdx++].set(r3 * s, r3 * c, -halfH);       // 7
    }

    for (int i = 0; i < nSolidSeg; i++)
    {
        int b = startIdx1 + i * 8;
        int n = b + 8;

        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = b + 3;
        face[fIdx].vert[2].vertIndex = n + 3;
        face[fIdx].vert[3].vertIndex = n + 0;
        fIdx++;
        // Bot
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 4;
        face[fIdx].vert[1].vertIndex = n + 4;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = b + 7;
        fIdx++;
        // In
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = n + 0;
        face[fIdx].vert[2].vertIndex = n + 4;
        face[fIdx].vert[3].vertIndex = b + 4;
        fIdx++;
        // Out
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 3;
        face[fIdx].vert[1].vertIndex = b + 7;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = n + 3;
        fIdx++;
    }

    face[fIdx].nVerts = 4;
    face[fIdx].vert = new VertexID[4];
    face[fIdx].vert[0].vertIndex = startIdx1 + 0;
    face[fIdx].vert[1].vertIndex = startIdx1 + 3;
    face[fIdx].vert[2].vertIndex = startIdx1 + 7;
    face[fIdx].vert[3].vertIndex = startIdx1 + 4;
    fIdx++;

    int end1 = startIdx1 + nSolidSeg * 8;
    face[fIdx].nVerts = 4;
    face[fIdx].vert = new VertexID[4];
    face[fIdx].vert[0].vertIndex = end1 + 3;
    face[fIdx].vert[1].vertIndex = end1 + 0;
    face[fIdx].vert[2].vertIndex = end1 + 4;
    face[fIdx].vert[3].vertIndex = end1 + 7;
    fIdx++;

    int startIdx2 = vIdx;
    float currentAngle = startAngle + solidAngle;
    for (int i = 0; i < slices2; i++)
    {
        float theta = currentAngle + i * stepSlot;
        float c = cos(theta);
        float s = sin(theta);
        pt[vIdx++].set(r0 * s, r0 * c, halfH);  // 0
        pt[vIdx++].set(r1 * s, r1 * c, halfH);  // 1
        pt[vIdx++].set(r2 * s, r2 * c, halfH);  // 2
        pt[vIdx++].set(r3 * s, r3 * c, halfH);  // 3
        pt[vIdx++].set(r0 * s, r0 * c, -halfH); // 4
        pt[vIdx++].set(r1 * s, r1 * c, -halfH); // 5
        pt[vIdx++].set(r2 * s, r2 * c, -halfH); // 6
        pt[vIdx++].set(r3 * s, r3 * c, -halfH); // 7
    }

    for (int i = 0; i < nSlotSeg; i++)
    {
        int b = startIdx2 + i * 8;
        int n = b + 8;

        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; 
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = b + 1;
        face[fIdx].vert[2].vertIndex = n + 1;
        face[fIdx].vert[3].vertIndex = n + 0;
        fIdx++;
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; 
        face[fIdx].vert[0].vertIndex = b + 4;
        face[fIdx].vert[1].vertIndex = n + 4;
        face[fIdx].vert[2].vertIndex = n + 5;
        face[fIdx].vert[3].vertIndex = b + 5;
        fIdx++;

        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; 
        face[fIdx].vert[0].vertIndex = b + 2;
        face[fIdx].vert[1].vertIndex = b + 3;
        face[fIdx].vert[2].vertIndex = n + 3;
        face[fIdx].vert[3].vertIndex = n + 2;
        fIdx++;
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; 
        face[fIdx].vert[0].vertIndex = b + 6;
        face[fIdx].vert[1].vertIndex = n + 6;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = b + 7;
        fIdx++;

        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; 
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = n + 0;
        face[fIdx].vert[2].vertIndex = n + 4;
        face[fIdx].vert[3].vertIndex = b + 4;
        fIdx++;
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; // SlotIn
        face[fIdx].vert[0].vertIndex = b + 1;
        face[fIdx].vert[1].vertIndex = b + 5;
        face[fIdx].vert[2].vertIndex = n + 5;
        face[fIdx].vert[3].vertIndex = n + 1;
        fIdx++;
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; // SlotOut
        face[fIdx].vert[0].vertIndex = b + 2;
        face[fIdx].vert[1].vertIndex = n + 2;
        face[fIdx].vert[2].vertIndex = n + 6;
        face[fIdx].vert[3].vertIndex = b + 6;
        fIdx++;
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4]; // Out
        face[fIdx].vert[0].vertIndex = b + 3;
        face[fIdx].vert[1].vertIndex = b + 7;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = n + 3;
        fIdx++;
    }

    int startIdx3 = vIdx;
    currentAngle += slotAngle;
    for (int i = 0; i < slices3; i++)
    {
        float theta = currentAngle + i * stepSolid;
        float c = cos(theta);
        float s = sin(theta);
        pt[vIdx++].set(r0 * s, r0 * c, halfH);
        pt[vIdx++].set(r_mid * s, r_mid * c, halfH);
        pt[vIdx++].set(r_mid * s, r_mid * c, halfH);
        pt[vIdx++].set(r3 * s, r3 * c, halfH);
        pt[vIdx++].set(r0 * s, r0 * c, -halfH);
        pt[vIdx++].set(r_mid * s, r_mid * c, -halfH);
        pt[vIdx++].set(r_mid * s, r_mid * c, -halfH);
        pt[vIdx++].set(r3 * s, r3 * c, -halfH);
    }

    face[fIdx].nVerts = 4;
    face[fIdx].vert = new VertexID[4];
    face[fIdx].vert[0].vertIndex = startIdx3 + 0;
    face[fIdx].vert[1].vertIndex = startIdx3 + 3;
    face[fIdx].vert[2].vertIndex = startIdx3 + 7;
    face[fIdx].vert[3].vertIndex = startIdx3 + 4;
    fIdx++;

    for (int i = 0; i < nSolidSeg; i++)
    {
        int b = startIdx3 + i * 8;
        int n = b + 8;
        // Top
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = b + 3;
        face[fIdx].vert[2].vertIndex = n + 3;
        face[fIdx].vert[3].vertIndex = n + 0;
        fIdx++;
        // Bot
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 4;
        face[fIdx].vert[1].vertIndex = n + 4;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = b + 7;
        fIdx++;
        // In
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 0;
        face[fIdx].vert[1].vertIndex = n + 0;
        face[fIdx].vert[2].vertIndex = n + 4;
        face[fIdx].vert[3].vertIndex = b + 4;
        fIdx++;
        // Out
        face[fIdx].nVerts = 4;
        face[fIdx].vert = new VertexID[4];
        face[fIdx].vert[0].vertIndex = b + 3;
        face[fIdx].vert[1].vertIndex = b + 7;
        face[fIdx].vert[2].vertIndex = n + 7;
        face[fIdx].vert[3].vertIndex = n + 3;
        fIdx++;
    }

    int end3 = startIdx3 + nSolidSeg * 8;
    face[fIdx].nVerts = 4;
    face[fIdx].vert = new VertexID[4];
    face[fIdx].vert[0].vertIndex = end3 + 3;
    face[fIdx].vert[1].vertIndex = end3 + 0;
    face[fIdx].vert[2].vertIndex = end3 + 4;
    face[fIdx].vert[3].vertIndex = end3 + 7;
    fIdx++;
}

void Mesh::CreateShape3(float fWidth, float fDepth, float fHeight, float fChamfer)
{
    numVerts = 16; // 8 đỉnh trên, 8 đỉnh dưới
    pt = new Point3[numVerts];

    float hw = fWidth / 2.0f;
    float hd = fDepth / 2.0f;
    float hh = fHeight / 2.0f;

    // Mặt trên (y = +hh) - Đi vòng ngược chiều kim đồng hồ
    pt[0].set(-hw + fChamfer, hh, hd);  // Front-Left
    pt[1].set(hw - fChamfer, hh, hd);   // Front-Right
    pt[2].set(hw, hh, hd - fChamfer);   // Right-Front
    pt[3].set(hw, hh, -hd + fChamfer);  // Right-Back
    pt[4].set(hw - fChamfer, hh, -hd);  // Back-Right
    pt[5].set(-hw + fChamfer, hh, -hd); // Back-Left
    pt[6].set(-hw, hh, -hd + fChamfer); // Left-Back
    pt[7].set(-hw, hh, hd - fChamfer);  // Left-Front

    // Mặt dưới (y = -hh) - Tương tự
    for (int i = 0; i < 8; i++)
    {
        pt[i + 8].set(pt[i].x, -hh, pt[i].z);
    }

    numFaces = 10; // 8 mặt bên + 1 nắp trên + 1 nắp dưới
    face = new Face[numFaces];
    int f = 0;

    // 1. Vẽ 8 mặt bên (Hình chữ nhật)
    for (int i = 0; i < 8; i++)
    {
        face[f].nVerts = 4;
        face[f].vert = new VertexID[4];

        int next = (i + 1) % 8; // Đỉnh tiếp theo

        // Nối đỉnh trên với đỉnh dưới tương ứng
        face[f].vert[0].vertIndex = i;        
        face[f].vert[1].vertIndex = i + 8;    
        face[f].vert[2].vertIndex = next + 8; 
        face[f].vert[3].vertIndex = next;     
        f++;
    }

    // 2. Vẽ Nắp trên (Hình bát giác)
    face[f].nVerts = 8;
    face[f].vert = new VertexID[8];
    for (int i = 0; i < 8; i++)
    {
        face[f].vert[i].vertIndex = i;
    }
    f++;

    // 3. Vẽ Nắp dưới (Hình bát giác)
    face[f].nVerts = 8;
    face[f].vert = new VertexID[8];
    for (int i = 0; i < 8; i++)
    {
        face[f].vert[i].vertIndex = 15 - i; 
    }
}

void Mesh::CreateShape4(float width, float height, float depth, float inset)
{
    numVerts = 16;
    pt = new Point3[numVerts];

    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;
    float hi = hw * inset; // Chiều rộng phần lõi (inner)
    float di = hd * inset; // Chiều sâu phần lõi (inner)

    // 8 Đỉnh ngoài (Outer Box)
    pt[0].set(-hw, +hh, +hd);
    pt[1].set(+hw, +hh, +hd);
    pt[2].set(+hw, +hh, -hd);
    pt[3].set(-hw, +hh, -hd);
    pt[4].set(-hw, -hh, +hd);
    pt[5].set(+hw, -hh, +hd);
    pt[6].set(+hw, -hh, -hd);
    pt[7].set(-hw, -hh, -hd);

    // 8 Đỉnh trong (Inner Box - phần thụt vào)
    pt[8].set(-hi, +hh, +di);
    pt[9].set(+hi, +hh, +di);
    pt[10].set(+hi, +hh, -di);
    pt[11].set(-hi, +hh, -di);
    pt[12].set(-hi, -hh, +di);
    pt[13].set(+hi, -hh, +di);
    pt[14].set(+hi, -hh, -di);
    pt[15].set(-hi, -hh, -di);

    numFaces = 12; 
    face = new Face[numFaces];
    int f = 0;

    // 1. Top 4 trapezium faces (4 mặt thang trên)
    for (int j = 0; j < 4; ++j)
    {
        face[f].nVerts = 4;
        face[f].vert = new VertexID[4];
        face[f].vert[0].vertIndex = j;
        face[f].vert[1].vertIndex = (j + 1) % 4;
        face[f].vert[2].vertIndex = 8 + ((j + 1) % 4);
        face[f].vert[3].vertIndex = 8 + j;
        f++;
    }

    // 2. Bottom 4 trapezium faces (4 mặt thang dưới)
    for (int j = 0; j < 4; ++j)
    {
        face[f].nVerts = 4;
        face[f].vert = new VertexID[4];
        face[f].vert[0].vertIndex = 4 + j;
        face[f].vert[1].vertIndex = 4 + ((j + 1) % 4);
        face[f].vert[2].vertIndex = 12 + ((j + 1) % 4);
        face[f].vert[3].vertIndex = 12 + j;
        f++;
    }

    // 3. 4 vertical sides (4 mặt bên thẳng đứng)
    for (int j = 0; j < 4; ++j)
    {
        face[f].nVerts = 4;
        face[f].vert = new VertexID[4];
        face[f].vert[0].vertIndex = j;
        face[f].vert[1].vertIndex = (j + 1) % 4;
        face[f].vert[2].vertIndex = 4 + ((j + 1) % 4);
        face[f].vert[3].vertIndex = 4 + j;
        f++;
    }
}

void Mesh::CreateShape5(float width, float height, float depth, float slope)
{
    numVerts = 10;
    pt = new Point3[numVerts];

    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float hd = depth * 0.5f;

    pt[0].set(-hw, -hh, +hd + slope); // bottom-front-left
    pt[1].set(+hw, -hh, +hd + slope); // bottom-front-right
    pt[2].set(+hw, -hh, -hd);         // bottom-back-right
    pt[3].set(-hw, -hh, -hd);         // bottom-back-left

    pt[4].set(-hw, +hh, -hd); // top-back-left
    pt[5].set(+hw, +hh, -hd); // top-back-right

    pt[6].set(-hw, +hh, +hd); // top-front-left (high)
    pt[7].set(+hw, +hh, +hd); // top-front-right (high)

    pt[8].set(-hw, +hh - slope, +hd + slope); // top-front-left (low)
    pt[9].set(+hw, +hh - slope, +hd + slope); // top-front-right (low)

    numFaces = 7;
    face = new Face[numFaces];

    // Face 0: Bottom
    face[0].nVerts = 4;
    face[0].vert = new VertexID[4];
    face[0].vert[0].vertIndex = 0;
    face[0].vert[1].vertIndex = 1;
    face[0].vert[2].vertIndex = 2;
    face[0].vert[3].vertIndex = 3;

    // Face 1: Back
    face[1].nVerts = 4;
    face[1].vert = new VertexID[4];
    face[1].vert[0].vertIndex = 2;
    face[1].vert[1].vertIndex = 3;
    face[1].vert[2].vertIndex = 4;
    face[1].vert[3].vertIndex = 5;

    // Face 2: Left
    face[2].nVerts = 5; 
    face[2].vert = new VertexID[5];
    face[2].vert[0].vertIndex = 0;
    face[2].vert[1].vertIndex = 3;
    face[2].vert[2].vertIndex = 4;
    face[2].vert[3].vertIndex = 6;
    face[2].vert[4].vertIndex = 8;

    // Face 3: Right
    face[3].nVerts = 5; 
    face[3].vert = new VertexID[5];
    face[3].vert[0].vertIndex = 1;
    face[3].vert[1].vertIndex = 2;
    face[3].vert[2].vertIndex = 5;
    face[3].vert[3].vertIndex = 7;
    face[3].vert[4].vertIndex = 9;

    // Face 4: Top
    face[4].nVerts = 4;
    face[4].vert = new VertexID[4];
    face[4].vert[0].vertIndex = 4;
    face[4].vert[1].vertIndex = 5;
    face[4].vert[2].vertIndex = 7;
    face[4].vert[3].vertIndex = 6;

    // Face 5: Front lower slanted
    face[5].nVerts = 4;
    face[5].vert = new VertexID[4];
    face[5].vert[0].vertIndex = 8;
    face[5].vert[1].vertIndex = 9;
    face[5].vert[2].vertIndex = 1;
    face[5].vert[3].vertIndex = 0;

    face[6].nVerts = 4;
    face[6].vert = new VertexID[4];
    face[6].vert[0].vertIndex = 6;
    face[6].vert[1].vertIndex = 7;
    face[6].vert[2].vertIndex = 9;
    face[6].vert[3].vertIndex = 8;
}

Mesh leftPart;
Mesh middlePart;
Mesh rightPart;
Mesh wheelMesh;
Mesh spacerBlock;
Mesh axleCyl;
Mesh sliderBar;
Mesh sliderArch;
Mesh sliderRoller;

void lightSetup()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE); 
    glShadeModel(GL_SMOOTH); 

    GLfloat light0_pos[] = { 10.0f, 10.0f, 10.0f, 0.0f }; 
    GLfloat ambient0[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
    GLfloat diffuse0[] = { 0.8f, 0.8f, 0.8f, 1.0f }; 
    GLfloat specular0[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glEnable(GL_LIGHT0);
    GLfloat light1_pos[] = { -10.0f, 5.0f, -10.0f, 0.0f };
    GLfloat ambient1[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse1[] = { 0.4f, 0.4f, 0.4f, 1.0f }; 
    GLfloat specular1[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 

    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    glEnable(GL_LIGHT1);
}

// ==========================================================
// HÀM VẼ Ô GẠCH
// ==========================================================
void drawTile(float fSize)
{
    float halfSize = fSize / 2.0f;
    float layerPattern = 0.02f;
    float layerBorder = 0.03f;

    // 1. NỀN GẠCH TRẮNG
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-halfSize, 0.0f, -halfSize);
    glVertex3f(halfSize, 0.0f, -halfSize);
    glVertex3f(halfSize, 0.0f, halfSize);
    glVertex3f(-halfSize, 0.0f, halfSize);
    glEnd();

    // 2. VIỀN ĐẬM (RON GẠCH)
    glLineWidth(2.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_LOOP);
    glNormal3f(0, 1, 0);
    glVertex3f(-halfSize, layerBorder, -halfSize);
    glVertex3f(halfSize, layerBorder, -halfSize);
    glVertex3f(halfSize, layerBorder, halfSize);
    glVertex3f(-halfSize, layerBorder, halfSize);
    glEnd();
    glLineWidth(1.0f);

    // 3. 4 GÓC MÀU XANH
    glColor3f(0.48f, 0.73f, 0.91f);
    float c = 0.707f * halfSize;

    glBegin(GL_TRIANGLES);
    glNormal3f(0, 1, 0);
    // Góc 1
    glVertex3f(-halfSize, layerPattern, -halfSize);
    glVertex3f(-halfSize, layerPattern, -c);
    glVertex3f(-c, layerPattern, -halfSize);
    // Góc 2
    glVertex3f(halfSize, layerPattern, -halfSize);
    glVertex3f(halfSize, layerPattern, -c);
    glVertex3f(c, layerPattern, -halfSize);
    // Góc 3
    glVertex3f(-halfSize, layerPattern, halfSize);
    glVertex3f(-c, layerPattern, halfSize);
    glVertex3f(-halfSize, layerPattern, c);
    // Góc 4
    glVertex3f(halfSize, layerPattern, halfSize);
    glVertex3f(halfSize, layerPattern, c);
    glVertex3f(c, layerPattern, halfSize);
    glEnd();

    glColor3f(0.48f, 0.73f, 0.91f);

    float innerR = 0.22f * halfSize;
    float midR = 0.55f * halfSize;
    float outerR = 0.90f * halfSize;

    for (int i = 0; i < 8; ++i)
    {
        float curAngle = i * PI / 4.0f;
        float nextAngle = curAngle + PI / 4.0f;

        float innerA = curAngle + PI / 16.0f;
        float innerB = nextAngle - PI / 16.0f;
        float midA = curAngle + PI / 32.0f;
        float midB = nextAngle - PI / 32.0f;

        glBegin(GL_POLYGON);
        glNormal3f(0, 1, 0);
        glVertex3f(innerR * cos(innerA), layerPattern, innerR * sin(innerA));
        glVertex3f(midR * cos(midA), layerPattern, midR * sin(midA));
        glVertex3f(outerR * cos(curAngle + PI / 8.0f), layerPattern, outerR * sin(curAngle + PI / 8.0f));
        glVertex3f(midR * cos(midB), layerPattern, midR * sin(midB));
        glVertex3f(innerR * cos(innerB), layerPattern, innerR * sin(innerB));
        glEnd();
    }
}

void drawGround()
{
    for (int row = -10; row < 10; row++)
    {
        for (int col = -10; col < 10; col++)
        {
            glPushMatrix();
            glTranslatef(col * 2.0f, 0.0f, row * 2.0f);
            drawTile(2.0f);
            glPopMatrix();
        }
    }
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = (float)w / h;
    
    screenWidth = w;
    screenHeight = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (is2D) {
        // --- CHẾ ĐỘ 2D (TRỰC GIAO) ---
        // Zoom: kích thước vùng nhìn thấy (càng lớn thì hình càng nhỏ)
        float zoom = 7.0f; 

        // Giữ tỷ lệ khung hình không bị méo
        if (w <= h)
            glOrtho(-zoom, zoom, -zoom / ratio, zoom / ratio, -100.0, 100.0);
        else
            glOrtho(-zoom * ratio, zoom * ratio, -zoom, zoom, -100.0, 100.0);
    } 
    else {
        // --- CHẾ ĐỘ 3D (PHỐI CẢNH) ---
        gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    }

    glMatrixMode(GL_MODELVIEW);
}

void drawAssembly()
{
    glPushMatrix();
    glTranslatef(0.0f, 5.0f, 0.0f); // Giữ độ cao 5.0

    // 1. KHỐI GIỮA (ĐỎ)
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    middlePart.draw();
    glPopMatrix();

    // 2. KHỐI TRÁI (XANH LÁ)
    glPushMatrix();
    glTranslatef(-0.56f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    leftPart.draw();
    glPopMatrix();

    glPushMatrix();
    float transX = 0.36f;

    float transY = -0.6f;

    glTranslatef(transX, transY, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); 

    glColor3f(0.0f, 1.0f, 1.0f);
    rightPart.draw();
    glPopMatrix();

    glPopMatrix();
}

void drawWheelSpacer()
{
    glPushMatrix();
    glTranslatef(-0.05f, 0.23f, 0.0f);

    glScalef(1.0f, 1.2f, 1.0f);

    glColor3f(1.0f, 0.6f, 0.0f); 
    axleCyl.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.05f, 0.46f, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f); 
    spacerBlock.draw();

    if (!isWireframe)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        spacerBlock.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glPopMatrix();
}

void drawWheel()
{
    glPushMatrix();
    glTranslatef(-0.1f, 2.95f, 0.0f);

    glRotatef(wheelRotation, 1.0f, 0.0f, 0.0f);

    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    wheelMesh.draw();

    if (!isWireframe)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wheelMesh.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    drawWheelSpacer();

    glPopMatrix();
}

void drawSliderSystem()
{
    float rad = wheelRotation * PI / 180.0f;
    float slideY = 0.4f * (1.0f + sin(rad));

    glPushMatrix();
    glTranslatef(-0.56f, 4.0f + slideY, 0.0f);

    // 1. THANH ĐỨNG 
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    sliderBar.draw();
    glPopMatrix();

    // 2. CỤM VÒM + CON LĂN
    glPushMatrix();
    glTranslatef(0.0f, -1.6f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Vẽ Vòm
    glColor3f(0.0f, 1.0f, 0.0f);
    sliderArch.draw();

    // Vẽ Con lăn
    float rRoller = 1.2f;
    float angRoller = 25.0f * PI / 180.0f;
    float xR = rRoller * sin(angRoller);
    float yR = rRoller * cos(angRoller);

    glColor3f(1.0f, 1.0f, 0.0f);

    glPushMatrix();
    glTranslatef(-xR, yR, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    sliderRoller.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(xR, yR, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    sliderRoller.draw();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

void drawLedSegment(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

void drawLedDigit(int number, float x, float y, float size) {
    float w = size * 0.15f; 
    float h = size * 0.45f; 
    float gap = size * 0.05f; 

    bool seg[7]; // A, B, C, D, E, F, G
    
    switch(number) {
        case 0: { bool s[]={1,1,1,1,1,1,0}; memcpy(seg,s,7); break; }
        case 1: { bool s[]={0,1,1,0,0,0,0}; memcpy(seg,s,7); break; }
        case 2: { bool s[]={1,1,0,1,1,0,1}; memcpy(seg,s,7); break; }
        case 3: { bool s[]={1,1,1,1,0,0,1}; memcpy(seg,s,7); break; }
        case 4: { bool s[]={0,1,1,0,0,1,1}; memcpy(seg,s,7); break; }
        case 5: { bool s[]={1,0,1,1,0,1,1}; memcpy(seg,s,7); break; }
        case 6: { bool s[]={1,0,1,1,1,1,1}; memcpy(seg,s,7); break; }
        case 7: { bool s[]={1,1,1,0,0,0,0}; memcpy(seg,s,7); break; }
        case 8: { bool s[]={1,1,1,1,1,1,1}; memcpy(seg,s,7); break; }
        case 9: { bool s[]={1,1,1,1,0,1,1}; memcpy(seg,s,7); break; }
    }

    // A (Top)
    if(seg[0]) drawLedSegment(x + w, y + 2*h + gap, h, w);
    // B (Top-Right)
    if(seg[1]) drawLedSegment(x + w + h + gap, y + h + gap, w, h);
    // C (Bot-Right)
    if(seg[2]) drawLedSegment(x + w + h + gap, y, w, h);
    // D (Bottom)
    if(seg[3]) drawLedSegment(x + w, y - w, h, w);
    // E (Bot-Left)
    if(seg[4]) drawLedSegment(x, y, w, h);
    // F (Top-Left)
    if(seg[5]) drawLedSegment(x, y + h + gap, w, h);
    // G (Middle)
    if(seg[6]) drawLedSegment(x + w, y + h, h, w);
}

void drawColon(float x, float y, float size) {
    float w = size * 0.15f;
    drawLedSegment(x + w, y + size*0.6f, w, w);
    drawLedSegment(x + w, y + size*0.3f, w, w);
}

void drawLEDClock() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int hour = ltm->tm_hour;
    int min = ltm->tm_min;
    int sec = ltm->tm_sec;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight); 
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float digitSize = 30.0f; 
    float startX = 20.0f;
    float startY = screenHeight - 50.0f;
    float spacing = digitSize * 0.8f; 

    glColor3f(1.0f, 0.0f, 0.0f); 

    drawLedDigit(hour / 10, startX, startY, digitSize);
    drawLedDigit(hour % 10, startX + spacing, startY, digitSize);

    drawColon(startX + spacing * 2, startY, digitSize);

    float minX = startX + spacing * 2.5f;
    drawLedDigit(min / 10, minX, startY, digitSize);
    drawLedDigit(min % 10, minX + spacing, startY, digitSize);

    drawColon(minX + spacing * 2, startY, digitSize);

    float secX = minX + spacing * 2.5f;
    drawLedDigit(sec / 10, secX, startY, digitSize);
    drawLedDigit(sec % 10, secX + spacing, startY, digitSize);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST); 

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float targetY = 3.0f; 

    if (is2D) {
        gluLookAt(20.0, targetY, 0.0,   
                  0.0, targetY, 0.0,    
                  0.0, 1.0, 0.0);       
        
        glDisable(GL_LIGHTING); 
    } 
    else {
        float camX = cameraDistance * sin(angle * PI / 180.0f);
        float camZ = cameraDistance * cos(angle * PI / 180.0f);
        
        gluLookAt(camX, cameraHeight, camZ, 
                  0.0, targetY, 0.0,    
                  0.0, 1.0, 0.0);

        // Bật/Tắt đèn 
        if (isLightingOn) {
            lightSetup();
        } else {
            glDisable(GL_LIGHTING);
        }
    }

    // Xử lý chế độ khung dây
    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Vẽ các đối tượng
    drawGround();
    drawAssembly();
    drawWheel();
    drawSliderSystem();
    drawLEDClock();

    glFlush();
    glutSwapBuffers();
}

void myInit()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // 1. Phần Trái (Shape 4):
    leftPart.CreateShape4(0.8f, 1.2f, 0.8f, 0.3f);

    // 2. Phần Giữa (Hộp):
    middlePart.CreateCuboid(0.32f, 1.2f, 0.8f);

    // 3. Phần Phải (Shape 5):
    rightPart.CreateShape5(0.8f, 2.4f, 0.4f, 0.24f);

    wheelMesh.CreateShape1(60, 0.32f, 0.9f, 1.4f, PI / 3.0f);
    spacerBlock.CreateShape3(0.5f, 0.5f, 0.4f, 0.15f);
    axleCyl.CreateCylinder(20, 0.8f, 0.15f);

    sliderBar.CreateCuboid(0.4f, 3.0f, 0.4f);

    sliderArch.CreateShape2(0.8f, 1.6f, 0.6f, 0.3f, 0.2f, PI / 2.0f);

    sliderRoller.CreateCylinder(20, 2.0f, 0.15f);
}

// ==========================================================
// INPUT
// ==========================================================
void mySpecialFunc(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        angle -= 5.0f;
    else if (key == GLUT_KEY_RIGHT)
        angle += 5.0f;
    else if (key == GLUT_KEY_UP)
        cameraHeight += 1.0f;
    else if (key == GLUT_KEY_DOWN)
        cameraHeight -= 1.0f;
    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
    if (key == '+')
        cameraDistance -= 1.0f;
    else if (key == '-')
        cameraDistance += 1.0f;
    else if (key == '1')
    {
        wheelRotation += 5.0f;
    }
    else if (key == '2')
    {
        wheelRotation -= 5.0f;
    }
    else if (key == 'l' || key == 'L')
    {
        isLightingOn = !isLightingOn;
        cout << "Lighting: " << (isLightingOn ? "ON" : "OFF") << endl;
    }
    else if (key == 'w' || key == 'W')
    {
        isWireframe = !isWireframe;
    }
    else if (key == 'v' || key == 'V') {
        is2D = !is2D;
        reshape(screenWidth, screenHeight);
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Vu Minh Khoa - 2211667");

    myInit();
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(reshape);
    glutSpecialFunc(mySpecialFunc);
    glutKeyboardFunc(myKeyboard);

    glutMainLoop();
    return 0;
}