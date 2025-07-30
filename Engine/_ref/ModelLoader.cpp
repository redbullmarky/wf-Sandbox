#include "pch.h"
#include "Core/Application.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/Model.h"
#include "Mesh/ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace wf::mesh
{
	Model* loadModel(const char* filename)
	{
		Assimp::Importer importer;
		auto& modelManager = Application::getModelManager();
		auto& meshManager = Application::getMeshManager();

		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_GenNormals |
			aiProcess_OptimizeMeshes |
			aiProcess_FlipUVs
		);

		if (!scene || !scene->HasMeshes()) {
			throw std::runtime_error("Failed to load model: " + std::string(importer.GetErrorString()));
		}

		// Create the model first to keep our stuffs in
		auto model = modelManager.create();

		///////////////
		// MATERIALS //
		///////////////
		if (scene->HasMaterials()) {
			auto& materials = model->materials;
			materials.resize(scene->mNumMaterials);

			printf("Scene has %u materials\n", scene->mNumMaterials);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
				const aiMaterial* importMat = scene->mMaterials[i];
				printf("-> %s\n", importMat->GetName().C_Str());
				materials[i] = Application::getMaterialManager().create();

				// DIFFUSE COLOUR
				aiColor4D diffuse{};
				importMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				materials[i]->diffuse.colour = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };

				// TEXTURE @todo
				//aiString texPath;
				//Texture* texture = nullptr;

				//if (importMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
				//{
				//	std::string resolvedPath = "resources/models/" + std::string(texPath.C_Str()); // adjust this
				//	texture = Application::getTextureManager().load(resolvedPath.c_str());
				//}

				//material->albedo.texture = texture;
			}
		}

		////////////
		// MESHES //
		////////////
		model->meshes.resize(scene->mNumMeshes);

		for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
			const aiMesh* importMesh = scene->mMeshes[meshIdx];
			if (!importMesh->mNumVertices) continue;

			// associate our material
			model->meshMaterials[meshIdx] = importMesh->mMaterialIndex;
			bool hasVertexColours = importMesh->HasVertexColors(0);
			auto diffuseColour = model->materials[model->meshMaterials[meshIdx]]->diffuse.colour;

			model->meshes[meshIdx] = meshManager.create();
			model->meshes[meshIdx]->vertices.resize(importMesh->mNumVertices);

			for (unsigned int vertIdx = 0; vertIdx < importMesh->mNumVertices; vertIdx++) {
				aiVector3D pos = importMesh->mVertices[vertIdx];
				aiVector3D normal = importMesh->HasNormals() ? importMesh->mNormals[vertIdx] : aiVector3D(0, 0, 0);
				aiVector3D uv = importMesh->HasTextureCoords(0) ? importMesh->mTextureCoords[0][vertIdx] : aiVector3D(0, 0, 0);
				aiVector3D tangent = importMesh->HasTangentsAndBitangents() ? importMesh->mTangents[vertIdx] : aiVector3D(0, 0, 0);

				Vertex v{};
				v.position = { pos.x, pos.y, pos.z };
				v.normal = { normal.x, normal.y, normal.z };
				v.texcoord = { uv.x, uv.y };
				v.tangent = { tangent.x, tangent.y, tangent.z, 1.f };

				v.colour = hasVertexColours && importMesh->mColors[0] ?
					Colour{
						importMesh->mColors[0][vertIdx].r,
						importMesh->mColors[0][vertIdx].g,
						importMesh->mColors[0][vertIdx].b,
						importMesh->mColors[0][vertIdx].a } :
						diffuseColour;

				//printf("colour: %.2f %.2f %.2f %.2f %s\n", v.colour.r, v.colour.g, v.colour.b, v.colour.a, hasVertexColours ? "vertcol" : "diff");

				model->meshes[meshIdx]->vertices[vertIdx] = v;
			}

			for (unsigned int i = 0; i < importMesh->mNumFaces; i++) {
				aiFace& face = importMesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					model->meshes[meshIdx]->indices.push_back(face.mIndices[j]);
				}
			}
		}

		return model;
	}
}