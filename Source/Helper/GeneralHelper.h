#pragma once

//Releases COM Pointers
template<typename T>
inline void ReleaseCOM(T& p_COM)
{
	if (p_COM != 0)
	{
		p_COM->Release();
		p_COM = 0;
	}
	
}