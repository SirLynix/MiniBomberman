#pragma once

#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <filesystem>
#include <memory>

class ClientResources
{
	public:
		ClientResources();

		std::shared_ptr<Nz::Model> bombModel;
		std::shared_ptr<Nz::Model> boxModel;
		std::shared_ptr<Nz::Model> floorModel;
		std::shared_ptr<Nz::Model> playerModel;
		std::shared_ptr<Nz::Model> stoneModel;

		static std::shared_ptr<Nz::GraphicalMesh> BuildMesh(const Nz::Primitive& primitive, Nz::Boxf& aabb);
		static std::shared_ptr<Nz::Model> BuildModel(const Nz::Primitive& primitive);
		static std::shared_ptr<Nz::Material> LoadMaterialFromPath(const std::filesystem::path& path);

	private:
		void LoadBombModel();
		void LoadPlayerModel();

};