#pragma once

#include <assimp/scene.h>
#include "phoenix/utils/file_tool_class.h"
#include "phoenix/core/model_class.h"
namespace Phoenix{

    class ModelLoader{
    private:
        PathTool path_tool_;
    public:
        ModelLoader();
        ModelLoader(PathTool path_tool):path_tool_(path_tool){};
        shared_ptr<Model> Load(const string& filename);
    private:
        shared_ptr<Model> LoadModel(const aiScene* scene);
        void ProcessNode(const aiNode* node,const aiScene* scene,shared_ptr<Model> model);
        shared_ptr<Mesh> ProcessMesh(const aiMesh *mesh, const aiScene* scene);
        shared_ptr<Texture> LoadMaterialTexture(const aiMaterial *mat, const aiTextureType type, const aiScene *scene,
                                                const TextureType tex_type);


    };
}