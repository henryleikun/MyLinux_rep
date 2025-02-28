#pragma once

class Nocopy{
public:
	Nocopy(){}
	Nocopy(const Nocopy& rhs) = delete;
	const Nocopy& operator=(const Nocopy& rhs) = delete;
};