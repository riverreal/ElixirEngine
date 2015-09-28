#pragma once

//Releases COM Pointers
template<typename T>
void ReleaseCOM(T* p_COM)
{
	if (p_COM != NULL)
	{
		p_COM->Release();
		p_COM = 0;
	}
	
}