#include "Misc.h"
#include "GpuResourceUtils.h"
#include "ShapeRenderer.h"

// �R���X�g���N�^
ShapeRenderer::ShapeRenderer(ID3D11Device* device)
{
	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ���_�V�F�[�_�[
	GpuResourceUtils::LoadVertexShader(
		device,
		"Data/Shader/ShapeRendererVS.cso",
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf());

	// �s�N�Z���V�F�[�_�[
	GpuResourceUtils::LoadPixelShader(
		device,
		"Data/Shader/ShapeRendererPS.cso",
		pixelShader.GetAddressOf());

	// �萔�o�b�t�@
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		constantBuffer.GetAddressOf());

	// �����b�V������
	CreateBoxMesh(device, 1.0f, 1.0f, 1.0f);

	// �����b�V������
	CreateSphereMesh(device, 1.0f, 32);

	// �������b�V������
	CreateHalfSphereMesh(device, 1.0f, 32);

	// �~�����b�V������
	CreateCylinderMesh(device, 1.0f, 1.0f, -0.5f, 1.0f, 32);
}

// ���`��
void ShapeRenderer::RenderBox(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& angle,
	const DirectX::XMFLOAT3& size,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(size.x, size.y, size.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * R * T);

	Render(rc, boxMesh, transform, color);
}

// ���`��
void ShapeRenderer::RenderSphere(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	float radius,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, radius, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * T);

	Render(rc, sphereMesh, transform, color);
}

// �~���`��
void ShapeRenderer::RenderCylinder(
	const RenderContext& rc,
	const DirectX::XMFLOAT3& position,
	float radius,
	float height,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(radius, height, radius);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y + height * 0.5f, position.z);
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, S * T);

	Render(rc, cylinderMesh, transform, color);
}

// �J�v�Z���`��
void ShapeRenderer::RenderCapsule(
	const RenderContext& rc,
	const DirectX::XMFLOAT4X4& transform,
	float radius,
	float height,
	const DirectX::XMFLOAT4& color) const
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	// �㔼��
	{
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, height * 0.5f, 0, 0), Transform);
		DirectX::XMMATRIX World = DirectX::XMMatrixScaling(radius, radius, radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, halfSphereMesh, world, color);
	}
	// �~��
	{
		DirectX::XMMATRIX World;
		World.r[0] = DirectX::XMVectorScale(Transform.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(Transform.r[1], height);
		World.r[2] = DirectX::XMVectorScale(Transform.r[2], radius);
		World.r[3] = Transform.r[3];
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, cylinderMesh, world, color);
	}
	// ������
	{
		DirectX::XMMATRIX World = DirectX::XMMatrixRotationX(DirectX::XM_PI);
		DirectX::XMVECTOR Position = DirectX::XMVector3Transform(DirectX::XMVectorSet(0, -height * 0.5f, 0, 0), Transform);
		Transform.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
		World = DirectX::XMMatrixMultiply(World, Transform);
		World.r[0] = DirectX::XMVectorScale(World.r[0], radius);
		World.r[1] = DirectX::XMVectorScale(World.r[1], radius);
		World.r[2] = DirectX::XMVectorScale(World.r[2], radius);
		World.r[3] = DirectX::XMVectorSetW(Position, 1.0f);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);
		Render(rc, halfSphereMesh, world, color);
	}
}

// ���b�V������
void ShapeRenderer::CreateMesh(ID3D11Device* device, const std::vector<DirectX::XMFLOAT3>& vertices, Mesh& mesh)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * vertices.size());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&desc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	mesh.vertexCount = static_cast<UINT>(vertices.size());
}

// �����b�V���쐬
void ShapeRenderer::CreateBoxMesh(ID3D11Device* device, float width, float height, float depth)
{
	DirectX::XMFLOAT3 positions[8] =
	{
		// top
		{ -width,  height, -depth},
		{  width,  height, -depth},
		{  width,  height,  depth},
		{ -width,  height,  depth},
		// bottom
		{ -width, -height, -depth},
		{  width, -height, -depth},
		{  width, -height,  depth},
		{ -width, -height,  depth},
	};

	std::vector<DirectX::XMFLOAT3> vertices;
	vertices.resize(32);

	// top
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[0]);
	// bottom
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[7]);
	vertices.emplace_back(positions[4]);
	// side
	vertices.emplace_back(positions[0]);
	vertices.emplace_back(positions[4]);
	vertices.emplace_back(positions[1]);
	vertices.emplace_back(positions[5]);
	vertices.emplace_back(positions[2]);
	vertices.emplace_back(positions[6]);
	vertices.emplace_back(positions[3]);
	vertices.emplace_back(positions[7]);

	// ���b�V������
	CreateMesh(device, vertices, boxMesh);
}

// �����b�V���쐬
void ShapeRenderer::CreateSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	float step = DirectX::XM_2PI / subdivisions;

	std::vector<DirectX::XMFLOAT3> vertices;

	// XZ����
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = 0.0f;
			p.z = cosf(theta) * radius;
		}
	}
	// XY����
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = sinf(theta) * radius;
			p.y = cosf(theta) * radius;
			p.z = 0.0f;
		}
	}
	// YZ����
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& p = vertices.emplace_back();
			p.x = 0.0f;
			p.y = sinf(theta) * radius;
			p.z = cosf(theta) * radius;
		}
	}

	// ���b�V������
	CreateMesh(device, vertices, sphereMesh);
}

// �������b�V���쐬
void ShapeRenderer::CreateHalfSphereMesh(ID3D11Device* device, float radius, int subdivisions)
{
	std::vector<DirectX::XMFLOAT3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ����
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = 0.0f;
			v.z = cosf(theta) * radius;
		}
	}
	// XY����
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions) - DirectX::XM_PIDIV2;

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius;
			v.y = cosf(theta) * radius;
			v.z = 0.0f;
		}
	}
	// YZ����
	for (int i = 0; i < subdivisions / 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = 0.0f;
			v.y = sinf(theta) * radius;
			v.z = cosf(theta) * radius;
		}
	}

	// ���b�V������
	CreateMesh(device, vertices, halfSphereMesh);
}

// �~��
void ShapeRenderer::CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int subdivisions)
{
	std::vector<DirectX::XMFLOAT3> vertices;

	float theta_step = DirectX::XM_2PI / subdivisions;

	// XZ����
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius1;
			v.y = start;
			v.z = cosf(theta) * radius1;
		}
	}
	for (int i = 0; i < subdivisions; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			float theta = theta_step * ((i + j) % subdivisions);

			DirectX::XMFLOAT3& v = vertices.emplace_back();

			v.x = sinf(theta) * radius2;
			v.y = start + height;
			v.z = cosf(theta) * radius2;
		}
	}
	// XY����
	{
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start, radius1));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start + height, radius2));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start, -radius1));
		vertices.emplace_back(DirectX::XMFLOAT3(0.0f, start + height, -radius2));
	}
	// YZ����
	{
		vertices.emplace_back(DirectX::XMFLOAT3(radius1, start, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(radius2, start + height, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(-radius1, start, 0.0f));
		vertices.emplace_back(DirectX::XMFLOAT3(-radius2, start + height, 0.0f));
	}

	// ���b�V������
	CreateMesh(device, vertices, cylinderMesh);
}

// �`����s
void ShapeRenderer::Render(const RenderContext& rc, const Mesh& mesh, const DirectX::XMFLOAT4X4& transform, const DirectX::XMFLOAT4& color) const
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// �V�F�[�_�[�ݒ�
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	// �萔�o�b�t�@�ݒ�
	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	// �r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX VP = V * P;

	// �v���~�e�B�u�ݒ�
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// ���_�o�b�t�@�ݒ�
	dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);

	// ���[���h�r���[�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX WVP = W * VP;

	// �萔�o�b�t�@�X�V
	CbMesh cbMesh;
	DirectX::XMStoreFloat4x4(&cbMesh.worldViewProjection, WVP);
	cbMesh.color = color;

	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

	// �`��
	dc->Draw(mesh.vertexCount, 0);
}