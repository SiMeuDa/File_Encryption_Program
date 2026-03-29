#pragma once

class IProgress{
public:
	virtual ~IProgress() = default;
	virtual void update(double) = 0;
};
