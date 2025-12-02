



namespace ws
{
	
	struct Vec2i
	{
		int x,y;
		Vec2i() = default;
        Vec2i(int x, int y) : x(x), y(y) {}       
        
		//POINT to Vec2i
		Vec2i(const POINT& p) : x(p.x), y(p.y) {}
		
		//Vec2i to POINT
        operator POINT() const {
            POINT p;
            p.x = x;
            p.y = y;
            return p;
        }		
        
        #ifdef SFML_SYSTEM_HPP
        
		Vec2i(sf::Vector2i& i) : x(i.x), y(i.y) {}
        
		operator sf::Vector2i() const {
			sf::Vector2i i;
			i.x = x;
        	i.y = y;
        	return i;
		}
        
		#endif       
        
	};

	struct Vec2f
	{
		float x,y;
        Vec2f() = default;
        Vec2f(float x, float y) : x(x), y(y) {}
        Vec2f(int x, int y) : x(x), y(y) {}
        Vec2f(Vec2i& f) : x(f.x), y(f.y) {}


        #ifdef SFML_SYSTEM_HPP
        Vec2f(sf::Vector2f& f) : x(f.x), y(f.y) {}
        
		operator sf::Vector2f() const {
			sf::Vector2f f;
			f.x = x;
        	f.y = y;
        	return f;
		}
        
		#endif
	};
		
	struct Vec2d
	{
		double x,y;	
        Vec2d() = default;
        Vec2d(double x, double y) : x(x), y(y) {}
        Vec2d(float x, float y) : x(x), y(y) {}
        Vec2d(int x, int y) : x(x), y(y) {}
        Vec2d(Vec2f& f) : x(f.x), y(f.y) {}
        Vec2d(Vec2i& f) : x(f.x), y(f.y) {}
        
        
        #ifdef SFML_SYSTEM_HPP
        Vec2d(sf::Vector2<double>& d) : x(d.x), y(d.y) {}
        
		operator sf::Vector2<double>() const {
			sf::Vector2<double> d;
			d.x = x;
        	d.y = y;
        	return d;
		}
        
		#endif
	};
		
	

	struct Vec3i
	{
		int x,y,z;
        Vec3i() = default;
        Vec3i(int x, int y,int z) : x(x), y(y), z(z) {}		

        #ifdef SFML_SYSTEM_HPP
        
		Vec3i(sf::Vector3i& i) : x(i.x), y(i.y), z(i.z) {}
        
		operator sf::Vector3i() const {
			sf::Vector3i i;
			i.x = x;
        	i.y = y;
        	return i;
		}
        
		#endif  		
		
	};

	
	struct Vec3f
	{
		float x,y,z;
        Vec3f() = default;
        Vec3f(float x, float y,float z) : x(x), y(y), z(z) {}
		Vec3f(int x, int y,int z) : x(x), y(y), z(z) {}
		Vec3f(Vec3i& i) : x(i.x), y(i.y), z(i.z) {}
        

        #ifdef SFML_SYSTEM_HPP
        
		Vec3f(sf::Vector3f& i) : x(i.x), y(i.y), z(i.z) {}
        
		operator sf::Vector3f() const {
			sf::Vector3f i;
			i.x = x;
        	i.y = y;
        	return i;
		}
        
		#endif         
        
	};

	struct Vec3d
	{
		double x,y,z;
		Vec3d() = default;
		Vec3d(double x,double y,double z) : x(x), y(y), z(z) {}
		Vec3d(float x,float y,float z) : x(x), y(y), z(z) {}
		Vec3d(int x,int y,int z) : x(x), y(y), z(z) {}
		Vec3d(Vec3f& i) : x(i.x), y(i.y), z(i.z) {}
        Vec3d(Vec3i& i) : x(i.x), y(i.y), z(i.z) {}
        
        
		#ifdef SFML_SYSTEM_HPP
        
		Vec3d(sf::Vector3<double>& i) : x(i.x), y(i.y), z(i.z) {}
        
		operator sf::Vector3<double>() const {
			sf::Vector3<double> i;
			i.x = x;
        	i.y = y;
        	return i;
		}
        
		#endif 
		
	};	
	
	struct IntRect
	{
		int left,top,width,height;
		IntRect() = default;
		IntRect(int left,int top,int width,int height) : left(left),top(top),width(width),height(height) {}
		IntRect(RECT &r) : left(r.left) , top(r.top),width(r.right - r.left),height(r.bottom - r.top) {}
		
		
		//IntRect to RECT
		operator RECT() const {
            RECT r;
            r.left = left;
            r.top = top;
            r.right = left + width;
            r.bottom = top + height;
            return r;
        }				
        
        #ifdef SFML_SYSTEM_HPP
        
		IntRect(sf::IntRect &r) : left(r.left) , top(r.top),width(r.width),height(r.height) {} 
        
        operator sf::IntRect() const {
        	sf::IntRect r;
        	r.left = left;
        	r.top = top;
        	r.width = width;
        	r.height = height;
        	return r;
		}
        
		#endif
	};
	
	struct FloatRect
	{
		float left,top,width,height;
		FloatRect() = default;
		FloatRect(float left,float top,float width,float height) : left(left),top(top),width(width),height(height) {}
		FloatRect(int left,int top,int width,int height) : left(left),top(top),width(width),height(height) {}
		FloatRect(ws::IntRect &r) : left(r.left), top(r.top),width(r.width),height(r.height) {} 
		
		
        #ifdef SFML_SYSTEM_HPP
        
		FloatRect(sf::FloatRect &r) : left(r.left) , top(r.top),width(r.width),height(r.height) {} 
        
        operator sf::FloatRect() const {
        	sf::FloatRect r;
        	r.left = left;
        	r.top = top;
        	r.width = width;
        	r.height = height;
        	return r;
		}
        
		#endif
	};
	

	struct DoubleRect
	{
		double left,top,width,height;
		DoubleRect() = default;
		DoubleRect(double left,double top,double width,double height) : left(left),top(top),width(width),height(height) {}
		DoubleRect(float left,float top,float width,float height) : left(left),top(top),width(width),height(height) {}
		DoubleRect(int left,int top,int width,int height) : left(left),top(top),width(width),height(height) {}
		
		
		DoubleRect(ws::FloatRect &r) : left(r.left), top(r.top),width(r.width),height(r.height) {} 
		DoubleRect(ws::IntRect &r) : left(r.left), top(r.top),width(r.width),height(r.height) {} 
		
		
        #ifdef SFML_SYSTEM_HPP
        
		DoubleRect(sf::Rect<double> &r) : left(r.left) , top(r.top),width(r.width),height(r.height) {} 
        
        operator sf::DoubleRect() const {
        	sf::Rect<double> r;
        	r.left = left;
        	r.top = top;
        	r.width = width;
        	r.height = height;
        	return r;
		}
        
		#endif
	};		
	
	
		
	
}