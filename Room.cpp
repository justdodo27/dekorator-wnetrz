#include "Room.h"

void Room::loadModel() 
{
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(this->floorPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	std::string err = imp.GetErrorString();
	if(err.length() != 0){
		printf("Error While Loading Model %s : %s\n", this->floorPath.c_str(), err.c_str());
	}
	aiMesh* mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		this->floor_vertices.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		this->floor_normals.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoords = mesh->mTextureCoords[0][i];
		this->floor_uv.push_back(glm::vec2(texCoords.x, texCoords.y)*8.0f); // powielenie tekstury na podłodze
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			this->floor_indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    scene = imp.ReadFile(this->wallsPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	err = imp.GetErrorString();
	if(err.length() != 0){
		printf("Error While Loading Model %s : %s\n", this->wallsPath.c_str(), err.c_str());
	}
	
    mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		this->walls_vertices.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		this->walls_normals.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoords = mesh->mTextureCoords[0][i];
		this->walls_uv.push_back(glm::vec2(texCoords.x, texCoords.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			this->walls_indices.push_back(face.mIndices[j]);
		}
	}

	material = scene->mMaterials[mesh->mMaterialIndex];
}

Room::Room(std::string floor, GLuint floor_tex, GLuint floor_spec, std::string walls, GLuint walls_tex, GLuint walls_spec)
{
	this->M = glm::mat4(1.0f);
	this->M = glm::translate(this->M, glm::vec3(0.0f, 0.1f, 0.0f));
	this->floorPath = floor;
	this->floor_texture = floor_tex;
	this->floor_specular = floor_spec;
    this->wallsPath = walls;
    this->walls_texture = walls_tex;
	this->walls_specular = walls_spec;
	this->loadModel();
}

void Room::drawFloor(ShaderProgram *sp) // rysowanie podłogi
{
	sp->use();

	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, this->floor_vertices.data());

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, this->floor_normals.data()); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, this->floor_uv.data()); //Wskaż tablicę z danymi dla atrybutu texCoord0

	glUniform1i(sp->u("specular"), 1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->floor_texture);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->floor_specular);

	glDrawElements(GL_TRIANGLES, this->floor_indices.size(), GL_UNSIGNED_INT, this->floor_indices.data());

	glUniform1i(sp->u("specular"), 0);
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

void Room::drawWalls(ShaderProgram *sp) // rysowanie ścian
{
	sp->use();

	glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, this->walls_vertices.data());

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, this->walls_normals.data()); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord0
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, this->walls_uv.data()); //Wskaż tablicę z danymi dla atrybutu texCoord0

	glUniform1i(sp->u("specular"), 1);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->walls_texture);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->walls_specular);

	glDrawElements(GL_TRIANGLES, this->walls_indices.size(), GL_UNSIGNED_INT, this->walls_indices.data());

	glUniform1i(sp->u("specular"), 0);
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

void Room::drawRoom(ShaderProgram *sp){
	Room::drawFloor(sp);
	Room::drawWalls(sp);
}