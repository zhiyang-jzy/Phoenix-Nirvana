#pragma once
#include "phoenix/core/common.h"
#include <spdlog/spdlog.h>
#include <any>
#include <optional>
#include "ext/json.hpp"
using json = nlohmann::json;
namespace Phoenix{




    class PropertyList
    {
    private:
        std::map<string,json> properties_;

    public:
        void Set(string name,json value)
        {
            properties_[name] = value;
        }
        template<typename T>
        std::optional<T> Get(string name)
        {
            if(properties_.count(name))
                return properties_[name].get<T>();
            return std::nullopt;
        }
    };


    class PhoenixObject {
    public:
        enum class PClassType {
            PCamera,
            PEmitter,
            PIntegrator,
            PSampler,
            PScene,
            PShape,
            PFilm,
            PFilter,
            PBSDF
        };

        virtual void AddChild(shared_ptr<PhoenixObject> child) {}
        virtual void SetParent(shared_ptr<PhoenixObject> parent) {}
        virtual void Active() {}

        [[nodiscard]] virtual PClassType GetClassType() const = 0;

        virtual string ToString() const = 0;

    };


    class PhoenixObjectFactory {
    public:
        typedef std::function<shared_ptr<PhoenixObject>(const PropertyList&)> Constructor;

        static void MRegisterClass(const std::string& name, const Constructor& constr);

        static shared_ptr<PhoenixObject> CreateInstance(const std::string& name,
                                                        const PropertyList& propList) {
            //            if (!m_constructors || m_constructors->find(name) == m_constructors->end())
            //                throw NoriException("A constructor for class \"%s\" could not be found!", name);
            return (*constructors_)[name](propList);
        }
        static bool Exist(const string& name){return constructors_->count(name);}

    private:
        static std::map<std::string, Constructor>* constructors_;
    };
    inline void CheckFactoryExist(const string& key)
    {
        if(!PhoenixObjectFactory::Exist(key)){
            spdlog::error("No such object {}",key);
            exit(0);
        }
    }

#define PHOENIX_REGISTER_CLASS(cls, name) \
    shared_ptr<cls> cls ##_create(const PropertyList &list) { \
        return std::make_shared<cls>(list); \
    } \
    static struct cls ##_{ \
        cls ##_() { \
            PhoenixObjectFactory::MRegisterClass(name, cls ##_create); \
        } \
    } cls ##__PHOENIX_;\

}