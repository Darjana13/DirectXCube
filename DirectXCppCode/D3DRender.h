#pragma once
//#include <D3D9.h>
#include <DXGI.h>
#include <D3D11.h>
#include <D2D1.h>
#include <Dwrite.h>
#include <directxmath.h>
#include <atlbase.h>
#include <memory>
#include <vector>
#include <array>
#include <string>
# define M_PI           3.14159265358979323846  /* pi */
namespace DX
{
	namespace Shaders
	{
	#include "Shaders/VertexShader.hvs"
	#include "Shaders/PixelShader.hps"
#include <DirectXMath.h>
	}
inline void CheckHR(HRESULT hr) 
{
	if(hr < 0)
	{
//		if(hr == D2DERR_RECREATE_TARGET)return;
		if(hr == E_OUTOFMEMORY)throw new std::exception("DirectX error Out of Memory");
		if (hr == E_INVALIDARG)throw new std::exception("DirectX error InvalidArgs");
		char buf[100];
		throw new std::exception(("DirectX error"+std::string(_itoa(hr,buf,10))).c_str());
	}
}

class Directx11
{
	struct RotateMartix
	{
		DirectX::XMMATRIX RotX, RotY;
		float AngleX = 0, AngleY = 0;
	};

	//double angleY = 0, angleX = 0;
	RotateMartix Rot;

public:
	struct RenderingUnit
	{
		CComPtr<ID3D11Buffer> vertexBuffer;
		CComPtr<ID3D11Buffer> indexBuffer;
		CComPtr<ID3D11ShaderResourceView> ColorTexRV;

		enum class UnitType{ Triangle, Square, Cube } uType;
		UINT IndexCount;
	};
protected:
	struct VertexForColorTriangle
		{
			DirectX::XMFLOAT3 Pos;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT4 Color;
			//DirectX::XMFLOAT2 Tex;   // Координаты текстуры tu, tv
		};
	struct ConstantBufferStruct
	{
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
		DirectX::XMMATRIX mWorld;
	};
	


	CComPtr<ID3D11Device> device;
	CComPtr<ID3D11DeviceContext> context;
	CComPtr<IDXGISwapChain> swapChain;

	CComPtr<ID3D11Texture2D> renderTarget;
	CComPtr<ID3D11Texture2D> depthStencil;
	CComPtr<ID3D11RenderTargetView> renderTargetView;
	CComPtr<ID3D11DepthStencilView> depthStencilView;


	CComPtr<ID3D11ShaderResourceView> textureView;


	CComPtr<ID2D1RenderTarget> renderTarget2D;
	CComPtr<ID2D1Factory> factory2D;

	CComPtr<ID3D11InputLayout> vertexLayout;
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;


	CComPtr<ID3D11Buffer> constantBufferForVertexShader;

	CComPtr<ID3D11RasterizerState> rasterizerState;

	std::vector<RenderingUnit> SavedScene;


	CComPtr<ID2D1Brush> current2DBrush;
	CComPtr<IDWriteFactory> writeFactory;
	CComPtr<IDWriteTextFormat> textFormat;

	D2D1::Matrix3x2F transformationMatrix2D;

	void CreateShaders()
	{
		vertexShader.Release();
		pixelShader.Release();

		CheckHR(device->CreateVertexShader(Shaders::VertexShader, sizeof(Shaders::VertexShader), nullptr, &vertexShader));
		CheckHR(device->CreatePixelShader(Shaders::PixelShader, sizeof(Shaders::PixelShader), nullptr, &pixelShader));

		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		CheckHR(device->CreateInputLayout(layout, ARRAYSIZE(layout), Shaders::VertexShader, sizeof(Shaders::VertexShader), &vertexLayout)); 	
	}
public:
	Directx11(HWND hWnd)
	{
		D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };
		D3D_FEATURE_LEVEL pFeatureLevel;

		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		//CheckHR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels, 6,
		//		D3D11_SDK_VERSION, &sd, &swapChain, &device, &pFeatureLevel, &context));
		CheckHR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels, 6,
			D3D11_SDK_VERSION, &sd, &swapChain, &device, &pFeatureLevel, &context));

		CreateShaders();				

		D3D11_RASTERIZER_DESC rdesc;
		ZeroMemory(&rdesc,sizeof(rdesc));
		rdesc.CullMode=D3D11_CULL_MODE::D3D11_CULL_NONE;
		rdesc.FillMode = D3D11_FILL_SOLID;
		rdesc.AntialiasedLineEnable=false;
		rdesc.DepthClipEnable=false;
		rdesc.MultisampleEnable=false;

		CheckHR(device->CreateRasterizerState(&rdesc,&rasterizerState));

		factory2D.Release();
		CheckHR(D2D1CreateFactory(	D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,&factory2D));

		writeFactory.Release();
		CheckHR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,	__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(&writeFactory)));

		constantBufferForVertexShader.Release();

		int sz1=sizeof(ConstantBufferStruct);
		CD3D11_BUFFER_DESC bufferDesc(sz1, D3D11_BIND_CONSTANT_BUFFER);
		CheckHR(device->CreateBuffer(&bufferDesc, nullptr, &constantBufferForVertexShader));
		context->VSSetConstantBuffers(0, 1, &constantBufferForVertexShader.p);

	}
	HRESULT CreateDDSTextureFromFile(ID3D11Device* d3dDevice, const wchar_t* fileName, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, size_t maxsize = 0);
	void CreateTarget(int width, int height)
	{					
		renderTarget.Release();			
		depthStencil.Release();
		renderTargetView.Release();
		depthStencilView.Release();

		//CD3D11_TEXTURE2D_DESC renderDesc(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1,
		//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT,
		//	0, 1, 0, D3D11_RESOURCE_MISC_SHARED);
		//CheckHR(device->CreateTexture2D(&renderDesc, nullptr, &renderTarget));


		// Create a render target view
//		ID3D11Texture2D* pBackBuffer = NULL;
		CheckHR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &renderTarget));
		CheckHR(device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView));

		CComPtr<IDXGISurface> surface;
		CheckHR(renderTarget->QueryInterface(&surface));


		D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties =  D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
		
		renderTarget2D.Release();
		CheckHR(factory2D->CreateDxgiSurfaceRenderTarget(surface,&renderTargetProperties,&renderTarget2D));
		

		CD3D11_TEXTURE2D_DESC depthDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 1,
			D3D11_BIND_DEPTH_STENCIL);

		CheckHR(device->CreateTexture2D(&depthDesc, nullptr, &depthStencil));
		CheckHR(device->CreateDepthStencilView(depthStencil, nullptr, &depthStencilView));			

		CD3D11_VIEWPORT viewport(renderTarget, renderTargetView);
		context->OMSetRenderTargets(1, &renderTargetView.p, depthStencilView);
		context->RSSetViewports(1, &viewport);

		current2DBrush.Release();
		CComPtr<ID2D1SolidColorBrush> scbrush;
		CheckHR(renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),&scbrush));
		current2DBrush=scbrush;


		CheckHR(CreateDDSTextureFromFile(device, L"..\\TextureExample.dds", NULL, &textureView));
		//CheckHR(CreateDDSTextureFromFile(device, L"..\\free-metal-texture-vector.dds", NULL, &textureView));

		
	}

	void SetNewSolidBrush(D2D1::ColorF col)
	{
		current2DBrush.Release();
		CComPtr<ID2D1SolidColorBrush> scbrush;
		CheckHR(renderTarget2D->CreateSolidColorBrush(col,&scbrush));
		current2DBrush=scbrush;
	}
	void ClearLayer()
	{
		FLOAT colorRGBA[] = { 1.0f,1.0f,1.0f,1.0f };
		context->ClearRenderTargetView(renderTargetView, colorRGBA);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
	void ClearAll()
	{
	SavedScene.clear();
	FLOAT colorRGBA[]={1.0f,0.0f,1.0f,1.0f};
	context->ClearRenderTargetView(renderTargetView, colorRGBA);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	context->RSSetState(rasterizerState);
	}
	void RotateByTime()
	{
		Rot.AngleX += 0.01;
		if (Rot.AngleX >= 2 * M_PI)
			Rot.AngleX = 0;
		Rot.AngleY += 0.01;
		if (Rot.AngleY >= 2 * M_PI)
			Rot.AngleY = 0;

		Rot.RotX = DirectX::XMMatrixRotationX(Rot.AngleX);
		Rot.RotY = DirectX::XMMatrixRotationY(Rot.AngleY);
		WorldMatrix = Rot.RotX * Rot.RotY;
	}
	void UpdateProectionsAndLightingData()
	{
		ConstantBufferStruct cb;
		cb.mView = XMMatrixTranspose(ModelViewMatrix);
		cb.mWorld = XMMatrixTranspose(WorldMatrix);
		cb.mProjection = XMMatrixTranspose(ProjectionMatrix);
		context->UpdateSubresource(constantBufferForVertexShader, 0, nullptr, &cb, 0, 0);
	}

	void RenderStart()
	{
		FLOAT colorRGBA[]={1.0f,1.0f,1.0f,1.0f};
		context->ClearRenderTargetView(renderTargetView, colorRGBA);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
	void EndRender()
	{
		context->Flush();
		swapChain->Present(0, 0);
	}
	void RenderUnit(RenderingUnit& unit)
	{
		if (unit.IndexCount == 0)return;
		switch (unit.uType)
		{
		case RenderingUnit::UnitType::Triangle:
		{
			UINT stride = sizeof(VertexForColorTriangle);
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &unit.vertexBuffer.p, &stride, &offset);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetIndexBuffer(unit.indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			context->IASetInputLayout(vertexLayout);
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
			context->PSSetShaderResources(0, 1,&textureView.p);//передаем текстуру в шейдер

			context->DrawIndexed(unit.IndexCount, 0, 0);
		}
		break;
		case RenderingUnit::UnitType::Cube:
		{
			UINT stride = sizeof(VertexForColorTriangle);
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &unit.vertexBuffer.p, &stride, &offset);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetIndexBuffer(unit.indexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			context->IASetInputLayout(vertexLayout);
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
			context->PSSetShaderResources(0, 1, &textureView.p);//передаем текстуру в шейдер

			context->DrawIndexed(unit.IndexCount, 0, 0);
		}
		break;
		}
	}
	RenderingUnit CreateTriangleColorUnit(std::vector<std::array<std::array<int, 3>,2>>& Triangles, std::vector<std::array<double, 3>>& vertexes, std::vector<std::array<double, 3>>& normals) //в паре первое -- треугольник, второе -- rgb цвет
	{
		RenderingUnit unit;
		switch (vertexes.size())
		{
		case 3:
			unit.uType = RenderingUnit::UnitType::Triangle;
			break;
		case 8:
			unit.uType = RenderingUnit::UnitType::Cube;
			break;
		default:
			break;
		}
		unit.IndexCount = (int) Triangles.size() * 3;
		if(unit.IndexCount == 0)return unit;

		std::vector<VertexForColorTriangle> vertarray(vertexes.size());
		std::vector<DWORD> indexarray(Triangles.size()*3);
		for(int i=0;i<vertexes.size();i++)
		{
			vertarray[i].Pos.x=vertexes[i][0];
			vertarray[i].Pos.y=vertexes[i][1];
			vertarray[i].Pos.z=vertexes[i][2];
			if (normals.size() == vertexes.size())
			{
				vertarray[i].Normal.x = normals[i][0];
				vertarray[i].Normal.y = normals[i][1];
				vertarray[i].Normal.z = normals[i][2];
			}
			else
			{
				vertarray[i].Normal.x = 0;
				vertarray[i].Normal.y = 0;
				vertarray[i].Normal.z = 1;
			}
			vertarray[i].Color.x=-1;
			vertarray[i].Color.y=-1;
			vertarray[i].Color.z=-1;
			vertarray[i].Color.w=-1;
		}

		for(int i=0;i<Triangles.size();i++)
		{
			for(int j=0;j<3;j++)
			{
				int ver=Triangles[i][0][j];
				indexarray[i*3+j]=ver;
				DirectX::XMFLOAT4 Col;
				Col.x=Triangles[i][1][0]/255.;
				Col.y=Triangles[i][1][1]/255.;
				Col.z=Triangles[i][1][2]/255.;
				Col.w=1;

				if(vertarray[ver].Color.x == -1)
				{	
					vertarray[ver].Color=Col;
				}

				/*else
				{
					if (vertarray[ver].Color.x != Col.x || vertarray[ver].Color.y != Col.y || vertarray[ver].Color.z != Col.z || vertarray[ver].Color.w != Col.w)
					{
						int newver = (int) vertarray.size();
						vertarray.push_back(vertarray[ver]);
						vertarray[newver].Color=Col;
						indexarray[i*3+j]=newver;
					}
				}*/
			}
		}

		CD3D11_BUFFER_DESC bufferDesc((int) vertarray.size() * sizeof(VertexForColorTriangle), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA data;

		data.pSysMem = vertarray.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		CheckHR(device->CreateBuffer(&bufferDesc, &data, &unit.vertexBuffer));

		{				
			CD3D11_BUFFER_DESC bufferDesc((int) indexarray.size() * sizeof(DWORD), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = indexarray.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		CheckHR(device->CreateBuffer(&bufferDesc, &data, &unit.indexBuffer));
		}
		return unit;
	}
	
	RenderingUnit CreateCubeColorUnit(std::vector<std::array<double, 3>>& all_points) 
	{
		RenderingUnit unit;
		switch (all_points.size())
		{
		case 8:
			unit.uType = RenderingUnit::UnitType::Cube;
			break;
		default:
			break;
		}
		unit.IndexCount = 36;
		if (unit.IndexCount == 0)return unit;

		std::vector<VertexForColorTriangle> vertarray(24);
		/*DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT4 Color;*/
		std::vector<DWORD> indexarray(36);
		vertarray =
		{
			{DirectX::XMFLOAT3(all_points[0][0], all_points[0][1], all_points[0][2]), DirectX::XMFLOAT3(-1, +0, +0), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[2][0], all_points[2][1], all_points[2][2]), DirectX::XMFLOAT3(-1, +0, +0), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[4][0], all_points[4][1], all_points[4][2]), DirectX::XMFLOAT3(-1, +0, +0), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[6][0], all_points[6][1], all_points[6][2]), DirectX::XMFLOAT3(-1, +0, +0), DirectX::XMFLOAT4(1, 1, 0, 0)},

			{DirectX::XMFLOAT3(all_points[5][0], all_points[5][1], all_points[5][2]), DirectX::XMFLOAT3(+1, +0, +0), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[7][0], all_points[7][1], all_points[7][2]), DirectX::XMFLOAT3(+1, +0, +0), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[3][0], all_points[3][1], all_points[3][2]), DirectX::XMFLOAT3(+1, +0, +0), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[1][0], all_points[1][1], all_points[1][2]), DirectX::XMFLOAT3(+1, +0, +0), DirectX::XMFLOAT4(1, 1, 0, 0)},

			{DirectX::XMFLOAT3(all_points[5][0], all_points[5][1], all_points[5][2]), DirectX::XMFLOAT3(+0, -1, +0), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[1][0], all_points[1][1], all_points[1][2]), DirectX::XMFLOAT3(+0, -1, +0), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[0][0], all_points[0][1], all_points[0][2]), DirectX::XMFLOAT3(+0, -1, +0), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[4][0], all_points[4][1], all_points[4][2]), DirectX::XMFLOAT3(+0, -1, +0), DirectX::XMFLOAT4(1, 1, 0, 0)},

			{DirectX::XMFLOAT3(all_points[7][0], all_points[7][1], all_points[7][2]), DirectX::XMFLOAT3(+0, +1, +0), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[6][0], all_points[6][1], all_points[6][2]), DirectX::XMFLOAT3(+0, +1, +0), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[2][0], all_points[2][1], all_points[2][2]), DirectX::XMFLOAT3(+0, +1, +0), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[3][0], all_points[3][1], all_points[3][2]), DirectX::XMFLOAT3(+0, +1, +0), DirectX::XMFLOAT4(1, 1, 0, 0)},

			{DirectX::XMFLOAT3(all_points[0][0], all_points[0][1], all_points[0][2]), DirectX::XMFLOAT3(+0, +0, -1), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[2][0], all_points[2][1], all_points[2][2]), DirectX::XMFLOAT3(+0, +0, -1), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[3][0], all_points[3][1], all_points[3][2]), DirectX::XMFLOAT3(+0, +0, -1), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[1][0], all_points[1][1], all_points[1][2]), DirectX::XMFLOAT3(+0, +0, -1), DirectX::XMFLOAT4(1, 1, 0, 0)},

			{DirectX::XMFLOAT3(all_points[5][0], all_points[5][1], all_points[5][2]), DirectX::XMFLOAT3(+0, +0, +1), DirectX::XMFLOAT4(1, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[7][0], all_points[7][1], all_points[7][2]), DirectX::XMFLOAT3(+0, +0, +1), DirectX::XMFLOAT4(0, 0, 0, 0)},
			{DirectX::XMFLOAT3(all_points[6][0], all_points[6][1], all_points[6][2]), DirectX::XMFLOAT3(+0, +0, +1), DirectX::XMFLOAT4(0, 1, 0, 0)},
			{DirectX::XMFLOAT3(all_points[4][0], all_points[4][1], all_points[4][2]), DirectX::XMFLOAT3(+0, +0, +1), DirectX::XMFLOAT4(1, 1, 0, 0)},

		};
		
		indexarray =
		{
			0, 1, 2,
			2, 1, 3,

			4, 5, 6,
			6, 7, 4,

			8, 9, 10,
			10, 11, 8,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 21, 22,
			22, 23, 20
		};

		
		CD3D11_BUFFER_DESC bufferDesc((int)vertarray.size() * sizeof(VertexForColorTriangle), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA data;

		data.pSysMem = vertarray.data();
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		CheckHR(device->CreateBuffer(&bufferDesc, &data, &unit.vertexBuffer));

		{
			CD3D11_BUFFER_DESC bufferDesc((int)indexarray.size() * sizeof(DWORD), D3D11_BIND_INDEX_BUFFER);
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = indexarray.data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			CheckHR(device->CreateBuffer(&bufferDesc, &data, &unit.indexBuffer));
		}
		return unit;
	}


	void RenderScene(std::vector<RenderingUnit>& scene)
	{
		UpdateProectionsAndLightingData();
		for(auto& i:scene)RenderUnit(i);
	}
	void AddToSaved(RenderingUnit& unit)
	{
		SavedScene.push_back(unit);
	}

	void RenderSavedData()
	{
		RenderScene(SavedScene);
		context->Flush();

		/*renderTarget2D->BeginDraw();
		std::wstring txt = L"Hello, world";
		ResetTextFormat(L"Times New Roman", false, true, 18);
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING);
		CComPtr<IDWriteTextLayout> layout;
		auto sz = renderTarget2D->GetSize();
		writeFactory->CreateTextLayout(txt.c_str(), (UINT32) txt.length(), textFormat, sz.width, sz.height, &layout);
		D2D1_POINT_2F p = { 350, 200 };

		renderTarget2D->DrawTextLayout(p, layout, current2DBrush);

		CComPtr<ID2D1SolidColorBrush> scbrush;
		CheckHR(renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &scbrush));

		D2D1_ELLIPSE ellipse;
		ellipse.point = { 100, 100 };
		ellipse.radiusX = 100;
		ellipse.radiusY = 50;
		renderTarget2D->DrawEllipse(ellipse, scbrush, 3);
		CheckHR(renderTarget2D->EndDraw());*/

		swapChain->Present(0, 0);
	}

	void ResetTextFormat(std::wstring family,bool isBold, bool isItalic,double fontsize)
	{
		DWRITE_FONT_WEIGHT w=DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL;
		if(isBold)w=DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD;
		DWRITE_FONT_STYLE s=DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL;
		if(isItalic)s=DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_ITALIC;
		DWRITE_FONT_STRETCH fs=DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL;
		textFormat.Release();
		writeFactory->CreateTextFormat(family.c_str(), NULL, w, s, fs, (96. / 72)*fontsize, L"", &textFormat);
	}

	//bool IsLightingEnable;
	//DirectX::XMFLOAT4 AmbientLightColor;
	//DirectX::XMFLOAT4 DiffuseLightColor;
	//DirectX::XMFLOAT4 SpecularLightColor;
	//DirectX::XMFLOAT4 LightPosition;

	/*DirectX::XMFLOAT4X4 ProjectionMatrix;
	DirectX::XMFLOAT4X4 ModelViewMatrix;*/
	DirectX::XMMATRIX ProjectionMatrix;
	DirectX::XMMATRIX ModelViewMatrix;
	DirectX::XMMATRIX WorldMatrix;

	void InitMatrix(double width, double height)
	{
		// Инициализация матрицы вида
		DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);	// Откуда смотрим
		DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);	// Куда смотрим
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	// Направление верха
		ModelViewMatrix = DirectX::XMMatrixLookAtLH(Eye, At, Up);

		// Инициализация матрицы проекции
		// Параметры: 1) ширина угла объектива 2) "квадратность" пикселя
		// 3) самое ближнее видимое расстояние 4) самое дальнее видимое расстояние
		ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(M_PI/4, width / (FLOAT)height, 0.01f, 100.0f);

		// Инициализация матрицы мира
		WorldMatrix = DirectX::XMMatrixIdentity();
	}
};

}

