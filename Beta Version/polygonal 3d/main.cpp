#include "graphics.h"
#include "camera.h"  
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>








class Mesh3D
{
	public:
	
	std::vector<ws::Vec3d> vertices;
	std::vector<std::vector<int>> faces;
	std::vector<COLORREF> faceColors;
	
	
	void addVertex(double x,double y,double z)
	{
		vertices.push_back({x,y,z});
	}
	
	void addFace(std::vector<int> newface, COLORREF color = RGB(100,200,100))
	{
		faces.push_back(newface);
		faceColors.push_back(color);
	}
	
	Mesh3D()
	{
		
	}
	
	
    bool loadFromOBJ(const std::string& filename)
    {
        vertices.clear();
        faces.clear();
        faceColors.clear();
        
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open OBJ file: " << filename << std::endl;
            return false;
        }
        
        std::string line;
        std::string currentMaterial = "default";
        COLORREF currentColor = defaultFaceColor;
        
        // First pass: collect vertices and face information
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "v") // Vertex
            {
                double x, y, z;
                iss >> x >> y >> z;
                addVertex(x, y, z);
            }
            else if (prefix == "f") // Face
            {
                std::vector<int> face;
                std::string vertex;
                while (iss >> vertex)
                {
                    // Handle format: vertex/texture/normal or just vertex
                    size_t pos = vertex.find('/');
                    if (pos != std::string::npos)
                    {
                        vertex = vertex.substr(0, pos);
                    }
                    
                    try {
                        int index = std::stoi(vertex);
                        // OBJ uses 1-based indexing, convert to 0-based
                        face.push_back(index - 1);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Error parsing face vertex: " << vertex << std::endl;
                    }
                }
                
                if (!face.empty())
                {
                    addFace(face, currentColor);
                }
            }
            else if (prefix == "usemtl") // Material usage
            {
                std::string materialName;
                iss >> materialName;
                currentMaterial = materialName;
                currentColor = getColorForMaterial(materialName);
            }
            else if (prefix == "mtllib") // Material library
            {
                std::string mtlFile;
                iss >> mtlFile;
                loadMTL(mtlFile, filename);
            }
        }
        
        file.close();
        std::cout << "Loaded OBJ: " << vertices.size() << " vertices, " << faces.size() << " faces" << std::endl;
        return true;
    }
    
    // Load MTL material file
    void loadMTL(const std::string& mtlFile, const std::string& objPath)
    {
        // Extract directory from OBJ path
        std::string directory = objPath.substr(0, objPath.find_last_of("/\\") + 1);
        std::string fullPath = directory + mtlFile;
        
        std::ifstream file(fullPath);
        if (!file.is_open())
        {
            std::cerr << "Warning: Could not open MTL file: " << fullPath << std::endl;
            return;
        }
        
        std::string line;
        std::string currentMaterial;
        
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            
            if (prefix == "newmtl")
            {
                iss >> currentMaterial;
            }
            else if (prefix == "Kd") // Diffuse color
            {
                float r, g, b;
                iss >> r >> g >> b;
                
                // Convert from 0-1 range to 0-255 range
                COLORREF color = RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
                materialColors[currentMaterial] = color;
            }
        }
        
        file.close();
    }
    
    // Get color for material name
    COLORREF getColorForMaterial(const std::string& materialName)
    {
        auto it = materialColors.find(materialName);
        if (it != materialColors.end())
        {
            return it->second;
        }
        return defaultFaceColor;
    }	
	
	
	
	
	std::vector<ws::Poly> polygons;
	
	bool getPolygons()
	{
		polygons.clear();		
		std::vector<std::pair<float,int>> faceDepths; // A sort buffer that references indexes of the faces in a depth sorted manner.
		
	    polygons.reserve(faces.size()); // Pre-allocate
	    faceDepths.reserve(faces.size());
		
		for(unsigned int a=0;a<faces.size();a++)
		{
			if(faces[a].empty()) continue;
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
				
				
				const auto& vertex = vertices[index];
				
				
				double worldRelativeX = vertex.x - camera.x;
				double worldRelativeY = vertex.y - camera.y;  
				double worldRelativeZ = vertex.z - camera.z;
				
				double cameraRelativeZ = worldRelativeX * camera.forwardX + 
				                        worldRelativeY * camera.forwardY + 
				                        worldRelativeZ * camera.forwardZ;
				
				if(cameraRelativeZ < 0 || cameraRelativeZ > camera.visible)
				{
					skipface = true;
					break;
				}
	
				totalDepth += cameraRelativeZ;
			}
			
			float averageDepth = (float)totalDepth / (float)faces[a].size();
			
			
			if(!skipface)
				faceDepths.push_back({averageDepth,(int)a});
			
		}
		
		std::sort(faceDepths.begin(), faceDepths.end(), std::greater<>());
		
		
		
		
		
		
		
		for(auto& face : faceDepths) // gets the face vector from the faces vector.
		{
			int faceIndex = face.second;
			const auto& faceIndices = faces[faceIndex];
			
			
			
			ws::Poly poly;
			poly.fillColor = faceColors[faceIndex];
			poly.borderColor = borderColor;
			poly.borderWidth = borderWidth;
			poly.filled = filled;
			poly.closed = closed;
			
			
			
			
			bool failedFace = false;
			
			poly.vertices.reserve(faceIndices.size());
			
			for(unsigned int b = 0;b < faceIndices.size();b++)
			{
				int vIndex = faceIndices[b];
				//check that the index is valid.
				if(vIndex < 0 || vIndex >= int(vertices.size()))
				{
					std::cerr << "Error: Invalidly Defined Face! An index to a vertex was out of range.\n";
					continue;
				}
				//
				
				float scale = 1;
				int screenX = 0,screenY = 0;
				
				if(!camera.ToScreen(
				vertices[vIndex].x,
				vertices[vIndex].y,
				vertices[vIndex].z,
				screenX,
				screenY,
				scale))
				{
					failedFace = true;
					break;
				}
				
				
            	poly.addVertex(screenX, screenY);
				
				
			}
			
			if(!failedFace && poly.vertices.size() >= 3)
			{
				polygons.push_back(std::move(poly)); 
			}
		}
		return true;
	}
	

	




	unsigned int lastDraw = 10000000;
	HDC canvasDC;
	void updateDraw(ws::Window &window,int processAmount = 50)
	{
		if(lastDraw >= tree.polygons.size())
		{
			//empty canvasDC
			lastDraw = 0;
		}
		for(unsigned int a = lastDraw; a < tree.polygons.size() && a < processAmount; a++)
		{
			tree.polygons[a].draw(canvasDC,window.view);
		}
		lastDraw += processAmount;
		
		
	}


	
	void draw(ws::Window &window)
	{
		//add canvasDC to window buffer DC without replacing pixels below canvasDC's non-transparent pixels.
	}
	
    // Set color for a specific face
    void setFaceColor(int faceIndex, COLORREF color)
    {
        if(faceIndex >= 0 && faceIndex < (int)faceColors.size())
        {
            faceColors[faceIndex] = color;
        }
    }
    
    // Set color for all faces
    void setAllFaceColors(COLORREF color)
    {
        for(auto& faceColor : faceColors)
        {
            faceColor = color;
        }
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
        defaultFaceColor = color;
        // Update all existing faces
        for(auto& faceColor : faceColors)
        {
            faceColor = color;
        }
    }
    
    void setBorderColor(COLORREF color)
    {
        borderColor = color;
    }
    
    void setBorderWidth(int width)
    {
        borderWidth = width;
    }
    
    
    
    void move(float deltaX,float deltaY,float deltaZ)
    {
    	for(unsigned int a=0;a<vertices.size();a++)
    	{
			vertices[a].x += deltaX;
    		vertices[a].y += deltaY;
    		vertices[a].z += deltaZ;
    	}
	}
    
    
    void scale(float deltaScale)
    {
    	for(unsigned int a=0;a<vertices.size();a++)
    	{
			vertices[a].x *= deltaScale;
    		vertices[a].y *= deltaScale;
    		vertices[a].z *= deltaScale;
    	}    	
	}
	
	
	
	void updateBounds()
	{
		left = 1000000000;
		right = 0;
		top = 0;
		bottom = 1000000000;
		back = 0;
		front = 100000000;
		
		
		for(unsigned int a=0;a<vertices.size();a++)
		{
			if(left > vertices[a].x)
				left = vertices[a].x;
			if(right < vertices[a].x)
				right = vertices[a].x;
			
			if(top > vertices[a].y)
				top = vertices[a].y;
			if(bottom < vertices[a].y)
				bottom = vertices[a].y;
			
			
			if(front > vertices[a].z)
				front = vertices[a].z;
			if(back < vertices[a].z)
				back = vertices[a].z;
			
			
			
				
		}
		
		width = right - left;
		height = bottom - top;
		depth = back - front;
		
		
		
		
	}
	
	
	
	
	
	
	
	void moveToMeshY(Mesh3D &clamp,int leniency = 200)
	{
		
		
		//get center of this mesh
		updateBounds();
		
		int originIndex = -1;
		
    	for(unsigned int a=0;a<vertices.size();a++)
    	{
    		
	        double relX = vertices[a].x - left;
	        double relZ = vertices[a].z - front;
	        
	        // Check if vertex is near center in X and Z
	        if(relX >= width/2 - leniency && relX <= width/2 + leniency &&
	           relZ >= depth/2 - leniency && relZ <= depth/2 + leniency)
	        {
	            originIndex = a;
	            break;
	        }
    	}
    	if(originIndex == -1)
    	{
    		std::cerr << "Failed to MoveToMeshY! Could not find a center vertex for the mesh that should move. Try increasing the leniency range parameter in the function.\n";
			return;
		}
    	for(unsigned int a=0;a<vertices.size();a++)
    	{
			vertices[a].y = clamp.vertices[originIndex].y + vertices[a].y;  
		}
	}
	
	
	unsigned int width = 0,height = 0,depth = 0,left = 0,right = 0,top = 0,bottom = 0,back = 0,front = 0;
	
	
	private:
		COLORREF defaultFaceColor = RGB(100, 200, 100);
		COLORREF borderColor = RGB(0,0,0);
		int borderWidth = 1;
		bool filled = true;
		bool closed = true;
		
	
		std::map<std::string, COLORREF> materialColors;
};







int height = 20;
int width = 200;

ws::PixelArray map;

void MakeMap(Mesh3D &mesh)
{
    mesh.vertices.clear();
    mesh.faces.clear();
		
    for(int z=0; z < map.height; z++)  // Using z for depth to match 3D convention
    {
        for(int x=0; x < map.width; x++)
        {
            COLORREF c = map.getPixel(x, z);  // x,z coordinates for the map
            int h = GetRValue(c);
            
            mesh.addVertex(x * width, h * height, z * width);
        }
    }
	
    for(int z = 0; z < map.height - 1; z++)
    {
        for(int x = 0; x < map.width - 1; x++)
        {
            int currentRow = z * map.width;
            int nextRow = (z + 1) * map.width;
            
            int topLeft = currentRow + x;
            int topRight = currentRow + (x + 1);
            int bottomLeft = nextRow + x;
            int bottomRight = nextRow + (x + 1);
            
            // Create two triangles for better rendering
            mesh.addFace({topLeft, topRight, bottomRight});
            mesh.addFace({topLeft, bottomRight, bottomLeft});
        }
    }	
	
	
	
}


















int main()
{
	ws::Texture tmap;
	tmap.load("reliefs/relief02.bmp");
	map.convertToPixel(tmap);
	
	
    ws::Window window(960, 540, "3D Scene");
    
    camera.x = 500;
    camera.y = -100; 
    camera.z = 400;
    camera.screenWidth = 960;
    camera.screenHeight = 540;
    camera.setHorizontalFOV(60);
    camera.visible = 5000;
				


    Mesh3D ground;
    
	MakeMap(ground);
	ground.setFillColor(RGB(0,140,0));   
    
    ws::Timer timer;
    double timesince = 0;
    double timeperframe = 1.f/60.f;
    
    
    // FPS-style mouse look variables
    POINT mouse;
    GetCursorPos(&mouse);
    
    
    
    
    Mesh3D tree;
    tree.loadFromOBJ("brown_gradient.obj");
    

	tree.move(0,500,500);

//    tree.scale(500);
//    tree.moveToMeshY(ground);
	
	    
    while(window.isOpen())
    {
    	
    	timesince += timer.getSeconds();
    	timer.restart();
    	
        while(timesince > timeperframe)
        {
			timesince -= timeperframe;
			MSG m;
			while(window.pollEvent(m))
			{
				
				
				
				
				if(m.message == WM_KEYDOWN)
                {
 
				    if(m.wParam == 'W' || m.wParam == VK_UP)
                    {
                        camera.moveForward(100);
                    }
                    if(m.wParam == 'S' || m.wParam == VK_DOWN)
                    {
                        camera.moveForward(-100);
                    }
                    if(m.wParam == 'A' || m.wParam == VK_LEFT)
                    {
                        camera.moveRight(-100);
                    }
                    if(m.wParam == 'D' || m.wParam == VK_RIGHT)
                    {
                        camera.moveRight(100);
                    }
                    if(m.wParam == VK_SPACE)
                    {
                        camera.moveUp(100);
                    }
                    if(m.wParam == VK_SHIFT)
                    {
                        camera.moveUp(-100);
                    }
                }
                
			}

			POINT newcursorpos;
            GetCursorPos(&newcursorpos);
            
        	int dx = mouse.x - newcursorpos.x;
        	int dy = mouse.y - newcursorpos.y;
        	
        	camera.rotateYaw(dx * 0.25);
        	
			camera.rotatePitch(dy * 0.25);
        	
			mouse = newcursorpos;
        	
        	

		}
		window.clear(RGB(30, 30, 30));
        
        
        
        
        
        
		// Convert camera world position to map coordinates
		int mapX = camera.x / width;
		int mapZ = camera.z / width;
		
		// Clamp to map bounds to avoid out-of-range errors
		mapX = std::max(0, std::min(map.width - 1, mapX));
		mapZ = std::max(0, std::min(map.height - 1, mapZ));
		
		COLORREF camColor = map.getPixel(mapX, mapZ);
		camera.y = (GetRValue(camColor) * height) + 450; // Multiply by height to match terrain scale
    
    
    
//    
//    	auto groundPoly = ground.getPolygons();
//		for(auto& polys : groundPoly)
//        {
//        	window.draw(polys);
//		}
        
        
        tree.getPolygons(); 
		ws::Timer frameRate;
		
		updateDraw(window);
		
		std::cerr << "FPS:" << frameRate.getSeconds() << "\n";
    
		
		
		
		
        window.display();
    }
    
    return 0;
}