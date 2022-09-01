#include <Client/ClientResources.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Utility/MaterialData.hpp>

ClientResources::ClientResources()
{
	std::shared_ptr<Nz::Material> boxMat = LoadMaterialFromPath("assets/box.png");
	std::shared_ptr<Nz::Material> floorMat = LoadMaterialFromPath("assets/dev_grey.png");
	std::shared_ptr<Nz::Material> stoneMat = LoadMaterialFromPath("assets/stone.png");

	Nz::Boxf boxAABB;
	auto cubeMesh = BuildMesh(Nz::Primitive::Box(Nz::Vector3f(1.5f, 1.5f, 1.5f)), boxAABB);

	boxModel = std::make_shared<Nz::Model>(cubeMesh, boxAABB);
	boxModel->SetMaterial(0, boxMat);

	floorModel = BuildModel(Nz::Primitive::Plane(Nz::Vector2f(1.f, 1.f), Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 4.f, 4.f)));
	floorModel->SetMaterial(0, floorMat);

	stoneModel = std::make_shared<Nz::Model>(cubeMesh, boxAABB);
	stoneModel->SetMaterial(0, stoneMat);

	LoadBombModel();
	LoadPlayerModel();
}

std::shared_ptr<Nz::GraphicalMesh> ClientResources::BuildMesh(const Nz::Primitive& primitive, Nz::Boxf& aabb)
{
	Nz::Mesh mesh;
	mesh.CreateStatic();
	mesh.BuildSubMesh(primitive);

	aabb = mesh.GetAABB();

	return Nz::GraphicalMesh::BuildFromMesh(mesh);
}

std::shared_ptr<Nz::Model> ClientResources::BuildModel(const Nz::Primitive& primitive)
{
	Nz::Boxf aabb;
	auto gfxMesh = BuildMesh(primitive, aabb);

	return std::make_shared<Nz::Model>(gfxMesh, aabb);
}

std::shared_ptr<Nz::Material> ClientResources::LoadMaterialFromPath(const std::filesystem::path& path)
{
	std::shared_ptr<Nz::MaterialPass> matPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
	matPass->EnableDepthBuffer(true);

	{
		Nz::TextureParams texParams;
		texParams.renderDevice = Nz::Graphics::Instance()->GetRenderDevice();

		std::shared_ptr<Nz::Texture> texture = Nz::Texture::LoadFromFile(path, texParams);

		Nz::TextureSamplerInfo sampler;
		sampler.anisotropyLevel = 8;
		sampler.wrapModeU = Nz::SamplerWrap::Repeat;
		sampler.wrapModeV = Nz::SamplerWrap::Repeat;

		Nz::BasicMaterial basicMat(*matPass);
		basicMat.SetBaseColorMap(texture);
		basicMat.SetBaseColorSampler(sampler);
	}

	std::shared_ptr<Nz::Material> mat = std::make_shared<Nz::Material>();
	mat->AddPass("ForwardPass", matPass);

	return mat;
}

void ClientResources::LoadBombModel()
{
	Nz::MeshParams meshParams;
	meshParams.animated = false;
	meshParams.vertexRotation = Nz::EulerAnglesf(-90.f, 90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(1.f / 5.f);

	std::shared_ptr<Nz::Mesh> bombMesh = Nz::Mesh::LoadFromFile("assets/cartoon_bomb/cartoon_bomb.fbx", meshParams);
	Nz::Boxf aabb = bombMesh->GetAABB();

	std::shared_ptr<Nz::GraphicalMesh> bombGfxModel = Nz::GraphicalMesh::BuildFromMesh(*bombMesh);

	bombModel = std::make_shared<Nz::Model>(bombGfxModel, aabb);
	bombModel->SetMaterial(0, LoadMaterialFromPath("assets/cartoon_bomb/DefaultMaterial_Base_Color.png"));
}

void ClientResources::LoadPlayerModel()
{
	Nz::MeshParams meshParams;
	meshParams.animated = false;
	//meshParams.center = true;
	meshParams.vertexScale = Nz::Vector3f(0.01f);

	std::shared_ptr<Nz::Mesh> playerMesh = Nz::Mesh::LoadFromFile("assets/player.fbx", meshParams);
	Nz::Boxf aabb = playerMesh->GetAABB();

	std::shared_ptr<Nz::GraphicalMesh> playerGfxMesh = Nz::GraphicalMesh::BuildFromMesh(*playerMesh);
	
	playerModel = std::make_shared<Nz::Model>(playerGfxMesh, aabb);

	std::string matPath;
	playerMesh->GetMaterialData(0).GetStringParameter(Nz::MaterialData::BaseColorTexturePath, &matPath);

	playerModel->SetMaterial(0, LoadMaterialFromPath(matPath));
}
