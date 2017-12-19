#include <Renderer\IRenderer.h>
#include "RenderD11.h"
#include "RenderD12.h"
D3DPROJECT::RENDERER::IRenderer::IRenderer()
{
	
}

D3DPROJECT::RENDERER::IRenderer::~IRenderer()
{
	
}


DECLDIR D3DPROJECT::RENDERER::IRenderer * D3DPROJECT::RENDERER::CreateNewRenderer(int api)
{
	if (api == 1)
	{
		return new D3DPROJECT::RENDERER::RenderD11;
	}
	else if (api == 2)
	{
		return new D3DPROJECT::RENDERER::RenderD12;
	}
	return nullptr;
}



