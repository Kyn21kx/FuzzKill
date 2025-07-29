#pragma once


class Application {
public:
	void PreAllocate();

	void Init();
	
	void Run();
	
	void Dispose();

private:
	void LoadResources();
};
