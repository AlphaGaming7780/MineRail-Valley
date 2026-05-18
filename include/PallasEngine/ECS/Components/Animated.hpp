#pragma once
namespace pallas
{
	struct Animated {
		int m_FramesX = 1;       // nombre de frames en X
		int m_FramesY = 1;       // nombre de frames en 
		float m_Duration = 0.f;
		float m_CurrentTime = 0.f;
		bool m_SharedAnimation = true;
	};
}
