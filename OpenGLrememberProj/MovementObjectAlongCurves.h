#pragma once
#include <chrono>

//���������� delta_time (������� ����� ������������, �������)
double SearchDeltaTime() {
	static auto EndRender = std::chrono::steady_clock::now();
	auto CurTime = std::chrono::steady_clock::now();
	auto DeltaTime = CurTime - EndRender;
	double Delta = 1.0 * std::chrono::duration_cast<std::chrono::microseconds>(DeltaTime).count() / 1000000;
	EndRender = CurTime;
	return Delta;
}
