#include "winsimple.hpp"



int main()
{
	std::vector<int> keys = ws::Key::GetAllKeys();
	bool released = true;
	while(true)
	{
		for(auto& k : keys)
		{
			
			if(ws::Global::getButton(k))
			{
				if(released)
				{
					std::cout << "";
				}
				released = false;
			}
			else
				released = true;
		}
	}
	return 0;
}