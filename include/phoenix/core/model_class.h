#include "common.h"
#include "common_struct.h"
#include "discrete_pdf_class.h"
#include "texture_class.h"
namespace Phoenix{
    struct Vertex {
        Point3f position;
        Normal3f normal;
        Vector2f texcoord;
        Vector3f tangent;
        Vector3f b_tangent;
    };

    class Mesh{
    private:
        float area_;
        DiscretePdf dpdf_;
    public:
        vector<Vertex> vertexes_;
        vector<uint> indices_;
        vector<Point3f> vertices_;
        map<TextureType,shared_ptr<Texture> > textures_;
    public:
        Mesh(vector<Vertex> vertices,vector<uint> indices,map<TextureType,shared_ptr<Texture> > textures);
        void PostProcess();
        float area() const{return area_;};

    };
    struct PositionSampleRecord;
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
        void SamplePosition(PositionSampleRecord& pos_rec,Vector2f sample);
    };
}