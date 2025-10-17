#include "graphics.h"
#include "camera.h"  
#include <vector>
#include <algorithm> 









class Mesh3D
{
	public:
	
	std::vector<ws::Vec3d> vertices;
	std::vector<std::vector<int>> faces;
	
	
	
	void addVertex(double x,double y,double z)
	{
		vertices.push_back({x,y,z});
	}
	
	void addFace(std::vector<int> newface)
	{
		faces.push_back(newface);
	}
	
	Mesh3D()
	{
		
	}
	
	
	
	
	
	
	std::vector<ws::Poly> getPolygons()
	{
		
		std::vector<std::pair<float,int>> faceDepths; // A sort buffer that references indexes of the faces in a depth sorted manner.
		
		for(unsigned int a=0;a<faces.size();a++)
		{
			//get average depth for that face.
			bool skipface = false;
			float totalDepth = 0;
			for(int index : faces[a])
			{
				//check that the index is valid.
				if(index < 0 || index >= int(vertices.size()))
				{
					std::cerr << "Error: Invalidly Defined Face! An index to a vertex was out of range.\n";
					skipface = true;
					break;
				}				
				totalDepth += vertices[index].z;
				
				
				if(vertices[index].z <= camera.z)
				{
					skipface = true;
					break;
				}
				
				
			}
			
			float averageDepth = (float)totalDepth / (float)faces[a].size();
			
			
			if(!skipface)
				faceDepths.push_back({averageDepth,(int)a});
			
		}
		
		std::sort(faceDepths.begin(), faceDepths.end(), std::greater<>());
		
		
		
		
		
		std::vector<ws::Poly> polygons;
		
		for(auto& face : faceDepths) // gets the face vector from the faces vector.
		{
			
			int faceIndex = face.second;
			
			
			
			
			
			
			ws::Poly poly;
			poly.fillColor = fillColor;
			poly.borderColor = borderColor;
			poly.borderWidth = borderWidth;
			poly.filled = filled;
			poly.closed = closed;
			
			
			
			for(int vIndex : faces[faceIndex])
			{
			
				//check that the index is valid.
				if(vIndex < 0 || vIndex >= int(vertices.size()))
				{
					std::cerr << "Error: Invalidly Defined Face! An index to a vertex was out of range.\n";
					continue;
				}
				//
				
				float scale = 1;
				int screenX = 0,screenY = 0;
				
				camera.ToScreen(
				vertices[vIndex].x,
				vertices[vIndex].y,
				vertices[vIndex].z,
				screenX,
				screenY,
				scale);
				
				
				
				poly.addVertex(screenX,screenY);
				
				
			}
			
			polygons.push_back(poly);
			
		}
		return polygons;
	}
	
	
	
	
	
	
	void setFilled(bool isFilled = true)
	{
		filled = isFilled;
	}
	
	void setClosed(bool isClosed = true)
	{
		closed = isClosed;
	}
	
	void setFillColor(COLORREF color)
	{
		fillColor = color;
	}
	
	
	private:
		COLORREF fillColor = RGB(100,200,100);
		COLORREF borderColor = RGB(0,0,0);
		int borderWidth = 1;
		bool filled = true;
		bool closed = true;
		
	
		
};









void CreateCube(Mesh3D &mesh, ws::Vec3d pos = {0,0,0}, int width = 100, int height = 100, int depth = 100)
{
    double x,y,z;
    x = pos.x - width/2.0;
    y = pos.y;  // Base sits at y position
    z = pos.z - depth/2.0;
    
    // In this coordinate system: -Y is UP, +Y is DOWN
    // So the "top" of the cube is actually at a more negative Y value
    
    // Bottom face (higher Y value = lower position)
    mesh.addVertex(x, y, z);                      // 0: bottom-front-left
    mesh.addVertex(x + width, y, z);              // 1: bottom-front-right  
    mesh.addVertex(x + width, y, z + depth);      // 2: bottom-back-right
    mesh.addVertex(x, y, z + depth);              // 3: bottom-back-left
    
    // Top face (lower Y value = higher position)
    mesh.addVertex(x, y - height, z);             // 4: top-front-left
    mesh.addVertex(x + width, y - height, z);     // 5: top-front-right
    mesh.addVertex(x + width, y - height, z + depth); // 6: top-back-right
    mesh.addVertex(x, y - height, z + depth);     // 7: top-back-left
    
    // Define faces with counter-clockwise winding (outward facing)
    mesh.faces = {
        {0, 1, 2, 3}, // Bottom face (facing downward)
        {4, 5, 6, 7}, // Top face (facing upward)  
        {0, 4, 7, 3}, // Left face (facing left)
        {1, 5, 6, 2}, // Right face (facing right)
        {0, 1, 5, 4}, // Front face (facing forward)
        {3, 2, 6, 7}  // Back face (facing backward)
    };
}

// Fixed pyramid with negative Y as up
void CreatePyramid(Mesh3D &mesh, ws::Vec3d pos = {0,0,0}, int baseWidth = 100, int baseDepth = 100, int height = 100)
{
    double x = pos.x;
    double y = pos.y;  // Base sits at y position
    double z = pos.z;
    
    double hw = baseWidth / 2.0;
    double hd = baseDepth / 2.0;
    
    // Base vertices (at ground level - higher Y value)
    mesh.addVertex(x - hw, y, z - hd); // 0: back-left
    mesh.addVertex(x + hw, y, z - hd); // 1: back-right
    mesh.addVertex(x + hw, y, z + hd); // 2: front-right  
    mesh.addVertex(x - hw, y, z + hd); // 3: front-left
    
    // Apex (lower Y value = higher position)
    mesh.addVertex(x, y - height, z); // 4: apex
    
    // Define faces with proper winding
    mesh.addFace({0, 3, 2, 1}); // Base (counter-clockwise from above)
    mesh.addFace({0, 4, 3});    // Left face
    mesh.addFace({3, 4, 2});    // Front face  
    mesh.addFace({2, 4, 1});    // Right face
    mesh.addFace({1, 4, 0});    // Back face
}

// Fixed sphere with negative Y as up
void CreateSmoothSphere(Mesh3D &mesh, ws::Vec3d center = {0,0,0}, double radius = 50, int segments = 20, int rings = 12)
{
    mesh.vertices.clear();
    mesh.faces.clear();
    
    // In this coordinate system: -Y is UP, +Y is DOWN
    // Adjust center so sphere sits on y=0 (with bottom at y=0)
    center.y -= radius;  // Subtract radius because -Y is up
    
    // Add top vertex (lowest Y value = highest position)
    mesh.addVertex(center.x, center.y - radius, center.z);
    
    // Generate middle vertices
    for (int ring = 1; ring < rings; ring++) {
        double phi = (M_PI * ring) / rings;
        double y = center.y - radius * cos(phi);  // Subtract because -Y is up
        double ringRadius = radius * sin(phi);
        
        for (int segment = 0; segment < segments; segment++) {
            double theta = (2 * M_PI * segment) / segments;
            double x = center.x + ringRadius * cos(theta);
            double z = center.z + ringRadius * sin(theta);
            
            mesh.addVertex(x, y, z);
        }
    }
    
    // Add bottom vertex (highest Y value = lowest position)
    mesh.addVertex(center.x, center.y + radius, center.z);
    
    int topIndex = 0;
    int bottomIndex = 1 + (rings - 1) * segments;
    
    // Create top cap faces with correct winding
    for (int segment = 0; segment < segments; segment++) {
        int nextSegment = (segment + 1) % segments;
        mesh.addFace({topIndex, 1 + segment, 1 + nextSegment});
    }
    
    // Create middle faces with correct winding
    for (int ring = 0; ring < rings - 2; ring++) {
        for (int segment = 0; segment < segments; segment++) {
            int current = 1 + ring * segments + segment;
            int next = 1 + ring * segments + (segment + 1) % segments;
            int below = current + segments;
            int belowNext = next + segments;
            
            mesh.addFace({current, next, below});
            mesh.addFace({next, belowNext, below});
        }
    }
    
    // Create bottom cap faces with correct winding
    int lastRingStart = 1 + (rings - 2) * segments;
    for (int segment = 0; segment < segments; segment++) {
        int current = lastRingStart + segment;
        int next = lastRingStart + (segment + 1) % segments;
        mesh.addFace({bottomIndex, next, current});
    }
}



void CreateGround(Mesh3D &mesh, ws::Vec3d center = {0,0,0}, double totalWidth = 1000, double totalDepth = 1000, int segmentsWidth = 10, int segmentsDepth = 10)
{
    mesh.vertices.clear();
    mesh.faces.clear();
    
    double halfWidth = totalWidth / 2.0;
    double halfDepth = totalDepth / 2.0;
    double segmentWidth = totalWidth / segmentsWidth;
    double segmentDepth = totalDepth / segmentsDepth;
    
    // Create vertices in a grid pattern
    for (int z = 0; z <= segmentsDepth; z++) {
        for (int x = 0; x <= segmentsWidth; x++) {
            double xPos = center.x - halfWidth + (x * segmentWidth);
            double zPos = center.z - halfDepth + (z * segmentDepth);
            mesh.addVertex(xPos, center.y + camera.getHillHeight(xPos,zPos), zPos);
        }
    }
    
    // Create quad faces
    for (int z = 0; z < segmentsDepth; z++) {
        for (int x = 0; x < segmentsWidth; x++) {
            int topLeft = z * (segmentsWidth + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (segmentsWidth + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            mesh.addFace({topLeft, topRight, bottomRight, bottomLeft});
        }
    }
}




int main()
{
    ws::Window window(960, 540, "3D Scene");
    
    camera.x = 0;
    camera.y = -100; 
    camera.z = -400;
    camera.screenWidth = 960;
    camera.screenHeight = 540;
    camera.setHorizontalFOV(60);
				
    
    Mesh3D cube;
    
    CreateCube(cube,{0,0,0});
    
    Mesh3D pyramid;
    CreatePyramid(pyramid,{-200,0,0});
    
    Mesh3D sphere;
    CreateSmoothSphere(sphere,{-400,0,0});
    
    Mesh3D ground;
    CreateGround(ground, {0, 0, 0}, 2000, 2000, 20, 20);  // Create large ground
    ground.setFillColor(RGB(80, 160, 80));  // Green color for ground    
    
    
    while(window.isOpen())
    {
        
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_LEFT)
				{
					camera.x += 10;
				}
				if(m.wParam == VK_RIGHT)
				{
					camera.x -= 10;
				}
				if(m.wParam == VK_UP)
				{
					camera.y += 10;
				}
				if(m.wParam == VK_DOWN)
				{
					camera.y -= 10;
				}
				if(m.wParam == 'X')
				{
					camera.z -= 10;
				}
				if(m.wParam == 'Z')
				{
					camera.z += 10;
				}
				
			}
		}
		
		window.clear(RGB(30, 30, 30));
        
    
    	camera.y = camera.getHillHeight(camera.x,camera.z) - 100;
    
    
    	auto groundPoly = ground.getPolygons();
		for(auto& polys : groundPoly)
        {
        	window.draw(polys);
		}
        
        // Draw pyramid
        auto pyramidPolygons = pyramid.getPolygons();
        for(auto& polys : pyramidPolygons)
        {
        	window.draw(polys);
		}
		
		//draw cube
        auto polygons = cube.getPolygons();
        for(auto& polys : polygons)
        {
        	window.draw(polys);
		}		
		
		auto spherePolygons = sphere.getPolygons();
        for(auto& polys : spherePolygons)
        {
        	window.draw(polys);
		}
		
		
        window.display();
    }
    
    return 0;
}