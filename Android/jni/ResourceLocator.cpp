#include "ResourceLocator.hpp"

// If you're developing the app locally, replace the path below
// with your local path to the resource files.
// When the app is executed on Android, any value you specify here
// has no effect: it's overriden by a SetResourceBasePath call to
// the correct location on the device.
    
static std::string g_resource_base_path = "../DeepBeliefSDK/data/";
    
void SetResourceBasePath(std::string path)
{
    g_resource_base_path = path;
}

std::string PathForResource(std::string name)
{
    return g_resource_base_path+"/"+name;
}

