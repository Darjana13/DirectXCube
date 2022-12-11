// OpenGlCppCode.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <exception>
#include <map>
#include <memory>

#include "D3DRender.h"

#pragma comment(lib, "D3d11.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")

#define CPP_EXPORTS_API
#ifdef CPP_EXPORTS_API
#define CPP_API extern "C" __declspec(dllexport)
#else
#define CPP_API __declspec(dllimport)
#endif
using namespace std;
extern map<HWND, shared_ptr<DX::Directx11>>* device = NULL;
CPP_API void InitDirectX(int handle)
{
	(*device)[(HWND)handle] = make_shared<DX::Directx11>((HWND)handle);
}

CPP_API void RotateByTime(int handle)
{
	auto finded = device->find((HWND)handle);
	if (finded != device->end())
	{
		finded->second->RotateByTime();
	}
}

CPP_API void PrepareScene(int handle, int w, int h)
{
	auto finded = device->find((HWND)handle);
	/*if (finded != device->end())
	{
		finded->second->CreateTarget(w, h);
		finded->second->ClearAll();
		vector<array<array<int, 3>, 12>> triangles;

		array<array<int, 3>, 12> triangle;
		triangle[0] = { 0, 2, 1 };
		triangle[1] = { 128, 0, 128 };
		triangles.push_back(triangle);

		vector<array<double, 3>> xyz;
		xyz.resize(3);
		xyz[0] = { 0, 0, 0.5 };
		xyz[1] = { 1, 0, 0.5 };
		xyz[2] = { 0, 1, 0.5 };

		vector<array<double, 3>> normals;
		normals.resize(3);
		normals[0] = { 0, 0, 1 };
		normals[1] = { 0, 0, 1 };
		normals[2] = { 0, 0, 1 };

		finded->second->RenderStart();

		auto unit = finded->second->CreateTriangleColorUnit(triangles, xyz, normals);
		finded->second->AddToSaved(unit);
		finded->second->RenderSavedData();
		finded->second->EndRender();
	}*/
	if (finded != device->end())
	{
		finded->second->CreateTarget(w, h);
		finded->second->InitMatrix(w, h);
		finded->second->ClearAll();

		vector<array<double, 3>> all_points;
		all_points.resize(8);
		all_points[0] = { -1, -1, -1 };
		all_points[1] = { 1, -1, -1 };
		all_points[2] = { -1, 1, -1 };
		all_points[3] = { 1, 1, -1 };
		all_points[4] = { -1, -1, 1 };
		all_points[5] = { 1, -1, 1 };
		all_points[6] = { -1, 1, 1 };
		all_points[7] = { 1, 1, 1 };
		/*all_points[0] = {-1, -1, -0.5};
		all_points[1] = { -0.5, -1, -0.5 };
		all_points[2] = { -1, 0.5, -0.5 };
		all_points[3] = { -0.5, 0.5, -0.5 };
		all_points[4] = { 0.5, -0.5, 0.5 };
		all_points[5] = { 1, -0.5, 0.5 };
		all_points[6] = { 0.5, 1, 0.5 };
		all_points[7] = { 1, 1, 0.5 };*/
		/*all_points[0] = { -0.5, -0.5, -0.5 };
		all_points[1] = { 0.5, -0.5, -0.5 };
		all_points[2] = { -0.5, 0.5, -0.5 };
		all_points[3] = { 0.5, 0.5, -0.5 };
		all_points[4] = { -0.5, -0.5, 0.5 };
		all_points[5] = { 0.5, -0.5, 0.5 };
		all_points[6] = { -0.5, 0.5, 0.5 };
		all_points[7] = { 0.5, 0.5, 0.5 };*/

		vector<array<double, 3>> all_normals;
		all_normals.resize(8);
		all_normals[0] = { -1, -1, +1 };
		all_normals[1] = { +1, -1, +1 };
		all_normals[2] = { -1, +1, +1 };
		all_normals[3] = { +1, +1, +1 };
		all_normals[4] = { -1, -1, -1 };
		all_normals[5] = { +1, -1, -1 };
		all_normals[6] = { -1, +1, -1 };
		all_normals[7] = { +1, +1, -1 };
		double norm = 0;
		for (int i = 0; i < all_normals.size(); i++)
		{
			norm = 0;
			for (int j = 0; j < 3; j++)
			{
				norm += all_normals[i][j] * all_normals[i][j];
			}
			norm = sqrt(norm);
			for (int j = 0; j < 3; j++)
			{
				all_normals[i][j] /= norm;
			}			
		}

		for (int i_unit = 0; i_unit < 12; i_unit++)
		{
			vector<array<array<int, 3>, 2>> triangles;

			array<array<int, 3>, 2> triangle;
			triangle[0] = { 0,1,2 };         // порядок вершин
			//triangle[1] = { 0, 0, 0 }; // цвет
			switch (i_unit)
			{
			case 0:
			case 1:
				triangle[1] = { 128, 0, 128 };
				break;
			case 2:
			case 3:
				triangle[1] = { 0, 128, 128 };
				break;
			case 4:
			case 5:
				triangle[1] = { 128, 128, 0 };
				break;
			case 6:
			case 7:
				triangle[1] = { 0, 0, 128 };
				break;
			case 8:
			case 9:
				triangle[1] = { 0, 128, 0};
				break;
			case 10:
			case 11:
				triangle[1] = { 128, 0, 0 };
				break;
			default:
				break;
			}
			/*switch (i_unit)
			{
			case 0:
			case 1:
				triangle[1] = { 128, 0, 128 };
				break;
			case 2:
			case 3:
				triangle[1] = { 0, 128, 128 };
				break;
			case 4:
			case 5:
				triangle[1] = { 128, 128, 0 };
				break;
			case 6:
			case 7:
				triangle[1] = { 0, 0, 128 };
				break;
			case 8:
			case 9:
				triangle[1] = { 0, 128, 0};
				break;
			case 10:
			case 11:
				triangle[1] = { 128, 0, 0 };
				break;
			default:
				break;
			}*/
			triangles.push_back(triangle);

			vector<array<double, 3>> xyz;
			xyz.resize(3);
			switch (i_unit)
			{
			case 0:
				xyz[0] = all_points[0];
				xyz[1] = all_points[2];
				xyz[2] = all_points[4];
				
				break;
			case 1:
				xyz[0] = all_points[4];
				xyz[1] = all_points[2];
				xyz[2] = all_points[6];
				break;
			case 2:
				xyz[0] = all_points[5];
				xyz[1] = all_points[7];
				xyz[2] = all_points[3];
				break;
			case 3:
				xyz[0] = all_points[3];
				xyz[1] = all_points[1];
				xyz[2] = all_points[5];
				break;
			case 4:
				xyz[0] = all_points[5];
				xyz[1] = all_points[1];
				xyz[2] = all_points[0];
				break;
			case 5:
				xyz[0] = all_points[0];
				xyz[1] = all_points[4];
				xyz[2] = all_points[5];
				break;
			case 6:
				xyz[0] = all_points[7];
				xyz[1] = all_points[6];
				xyz[2] = all_points[2];
				break;
			case 7:
				xyz[0] = all_points[2];
				xyz[1] = all_points[3];
				xyz[2] = all_points[7];
				break;
			case 8:
				xyz[0] = all_points[0];
				xyz[1] = all_points[2];
				xyz[2] = all_points[3];
				break;
			case 9:
				xyz[0] = all_points[3];
				xyz[1] = all_points[1];
				xyz[2] = all_points[0];
				break;
			case 10:
				xyz[0] = all_points[5];
				xyz[1] = all_points[7];
				xyz[2] = all_points[6];
				break;
			case 11:
				xyz[0] = all_points[6];
				xyz[1] = all_points[4];
				xyz[2] = all_points[5];
				break;
			default:
				break;
			}

			vector<array<double, 3>> normals;
			normals.resize(3);
			switch (i_unit)
			{
			case 0:
			case 1:
				normals[0] = { -1, 0, 0 };
				normals[1] = { -1, 0, 0 };
				normals[2] = { -1, 0, 0 };
				break;
			case 2:
			case 3:
				normals[0] = { 1, 0, 0 };
				normals[1] = { 1, 0, 0 };
				normals[2] = { 1, 0, 0 };
				break;
			case 4:
			case 5:
				normals[0] = { 0, -1, 0 };
				normals[1] = { 0, -1, 0 };
				normals[2] = { 0, -1, 0 };
				break;
			case 6:
			case 7:
				normals[0] = { 0, 1, 0 };
				normals[1] = { 0, 1, 0 };
				normals[2] = { 0, 1, 0 };
				break;
			case 8:
			case 9:
				normals[0] = { 0, 0, 1 };
				normals[1] = { 0, 0, 1 };
				normals[2] = { 0, 0, 1 };
				break;
			case 10:
			case 11:
				normals[0] = { 0, 0, -1 };
				normals[1] = { 0, 0, -1 };
				normals[2] = { 0, 0, -1 };
				break;
			default:
				break;
			}
			/*switch (i_unit)
			{
			case 0:
				normals[0] = all_normals[0];
				normals[1] = all_normals[2];
				normals[2] = all_normals[4];

				break;
			case 1:
				normals[0] = all_normals[4];
				normals[1] = all_normals[2];
				normals[2] = all_normals[6];
				break;
			case 2:
				normals[0] = all_normals[5];
				normals[1] = all_normals[7];
				normals[2] = all_normals[3];
				break;
			case 3:
				normals[0] = all_normals[3];
				normals[1] = all_normals[1];
				normals[2] = all_normals[5];
				break;
			case 4:
				normals[0] = all_normals[5];
				normals[1] = all_normals[1];
				normals[2] = all_normals[0];
				break;
			case 5:
				normals[0] = all_normals[0];
				normals[1] = all_normals[4];
				normals[2] = all_normals[5];
				break;
			case 6:
				normals[0] = all_normals[7];
				normals[1] = all_normals[6];
				normals[2] = all_normals[2];
				break;
			case 7:
				normals[0] = all_normals[2];
				normals[1] = all_normals[3];
				normals[2] = all_normals[7];
				break;
			case 8:
				normals[0] = all_normals[0];
				normals[1] = all_normals[2];
				normals[2] = all_normals[3];
				break;
			case 9:
				normals[0] = all_normals[3];
				normals[1] = all_normals[1];
				normals[2] = all_normals[0];
				break;
			case 10:
				normals[0] = all_normals[5];
				normals[1] = all_normals[7];
				normals[2] = all_normals[6];
				break;
			case 11:
				normals[0] = all_normals[6];
				normals[1] = all_normals[4];
				normals[2] = all_normals[5];
				break;
			default:
				break;
			}*/

			finded->second->RenderStart();

			auto unit = finded->second->CreateTriangleColorUnit(triangles, xyz, normals);
			finded->second->AddToSaved(unit);


		}

		finded->second->RenderSavedData();
		finded->second->EndRender();
	}
}
CPP_API void RenderScene(int handle)
{
	auto finded = device->find((HWND)handle);
	if (finded != device->end())
	{
		finded->second->RenderStart();

		//DirectX::XMStoreFloat4x4(&(finded->second->ModelViewMatrix), DirectX::XMMatrixIdentity());
		//DirectX::XMStoreFloat4x4(&(finded->second->ProjectionMatrix), DirectX::XMMatrixIdentity());

		finded->second->RenderSavedData();
		finded->second->EndRender();
	}
}
