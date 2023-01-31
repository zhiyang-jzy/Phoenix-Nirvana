#include "common.h"
#include "common_struct.h"
#include "discrete_pdf_class.h"
#include "texture_class.h"
namespace Phoenix{
    struct Vertex {
        Point3f position;
        Vector3f normal;
        Vector2f texcoord;
        Vector3f tangent;
        Vector3f b_tangent;
    };

    class Mesh{
    private:
        float area_;
        DiscretePdf dpdf_;
    public:
        vector<Vertex> vertices_;
        vector<uint> indices_;
        shared_ptr<Texture> texture_;
        Mesh(vector<Vertex> vertices,vector<uint> indices,shared_ptr<Texture> texture);
    public:
        void PostProcess();
        float area(){return area_;};
    };

    class Model{
    public:
        std::vector<shared_ptr<Mesh> > meshes_;
        map<uint,shared_ptr<Mesh>> mesh_dict_;
        float area_;
        DiscretePdf dpdf_;
    public:
        Model();
        void AddMesh(const shared_ptr<Mesh>& mesh);
        void PostProcess();
    };
}